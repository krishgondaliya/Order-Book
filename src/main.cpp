#include <iostream>
#include <string>
#include <map>
#include <set>
#include <list>
#include <cmath>
#include <deque>
#include <stack>
#include <limits>
#include <vector>
#include <numeric>
#include <algorithm>
#include <memory>
#include <variant>
#include <unordered_map>
#include <optional>
#include <tuple>
#include <format>
#include <ctime>
#include <cstdint>
#include <stdexcept>

enum class OrderType
{
    GoodTillCancel,
    FillAndKill
};

enum class Side
{
    Buy,
    Sell
};

using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;

struct LevelInfo
{
    Price price_;
    Quantity quantity_;
};

using LevelInfos = std::vector<LevelInfo>;

class OrderbookLevelInfos
{
public:
    OrderbookLevelInfos(const LevelInfos& bids, const LevelInfos& asks)
        : bids_{ bids }
        , asks_{ asks }
    { }

    const LevelInfos& GetBids() const { return bids_; }
    const LevelInfos& GetAsks() const { return asks_; }

private:
    LevelInfos bids_;
    LevelInfos asks_;
};

class Order
{
public:
    Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
        : orderType_{ orderType }
        , orderId_{ orderId }
        , side_{ side }
        , price_{ price }
        , initialQuantity_{ quantity }
        , remainingQuantity_{ quantity }
    { }

    OrderId GetOrderId() const { return orderId_; }
    Side GetSide() const { return side_; }
    Price GetPrice() const { return price_; }
    OrderType GetOrderType() const { return orderType_; }
    Quantity GetInitialQuantity() const{ return initialQuantity_; }
    Quantity GetRemainingQuantity() const{ return remainingQuantity_; }
    bool IsFilled() const { return GetRemainingQuantity() == 0; }
    void Fill(Quantity quantity)
    {
        if (quantity > GetRemainingQuantity())
            throw std::logic_error(std::format("Order ({}) cannot be filled for more than its remaining quantity.", GetOrderId()));

        remainingQuantity_ -= quantity;
    }

private:
    OrderType orderType_;
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity initialQuantity_;
    Quantity remainingQuantity_;
};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;

class OrderModify
{
public:
    OrderModify(OrderId orderId, Side side, Price price, Quantity quantity)
        : orderId_{ orderId }
        , price_{ price }
        , side_{ side }
        , quantity_{ quantity }
    { }

    OrderId GetOrderId() const { return orderId_; }
    Price GetPrice() const { return price_; }
    Side GetSide() const { return side_; }
    Quantity GetQuantity() const { return quantity_; }

    OrderPointer ToOrderPointer(OrderType type) const
    {
        return std::make_shared<Order>(type, GetOrderId(), GetSide(), GetPrice(), GetQuantity());
    }

private:
    OrderId orderId_;
    Price price_;
    Side side_;
    Quantity quantity_;
};

struct TradeInfo
{
    OrderId orderId_;
    Price price_;
    Quantity quantity_;
};

class Trade
{
public:
    Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade)
        : bidTrade_{ bidTrade }
        , askTrade_{ askTrade }
    { }

    const TradeInfo& GetBidTrade() const { return bidTrade_; }
    const TradeInfo& GetAskTrade() const { return askTrade_; }

private:
    TradeInfo bidTrade_;
    TradeInfo askTrade_;
};

using Trades = std::vector<Trade>;

class Orderbook
{
private:

    struct OrderEntry
    {
        OrderPointer order_{ nullptr };
        OrderPointers::iterator location_;
    };

    std::map<Price, OrderPointers, std::greater<Price>> bids_;
    std::map<Price, OrderPointers, std::less<Price>> asks_;
    std::unordered_map<OrderId, OrderEntry> orders_;

    bool CanMatch(Side side, Price price) const
    {
        if (side == Side::Buy)
        {
            if (asks_.empty())
                return false;
            
            const auto& [bestAsk, _] = *asks_.begin();
            return price >= bestAsk;
        }
        else
        {
            if(bids_.empty())
                return false;

            const auto& [bestBid, _] = *bids_.begin();
            return price <= bestBid;
        }
    }

    Trades MatchOrders()
    {
        Trades trades;
        trades.reserve(orders_.size());

        while(true)
        {
            if(bids_.empty() || asks_.empty())
                break;
            auto& [bidPrice, bids] = *bids_.begin();
            auto& [askPrice, asks] = *asks_.begin();

            if(bidPrice < askPrice)
                break;
            
            while (bids.size() && asks.size())
            {
                auto& bid = bids.front();
                auto& ask = asks.front();

                Quantity quantity = std::min(bid->GetRemainingQuantity(), ask->GetRemainingQuantity());

                bid->Fill(quantity);
                ask->Fill(quantity);

                if(bid->IsFilled())
                {
                    bids.pop_front();
                    orders_.erase(bid->GetOrderId());
                }

                if(ask->IsFilled())
                {
                    asks.pop_front();
                    orders_.erase(ask->GetOrderId());
                }

                if(bids.empty())
                    bids_.erase(bidPrice);
                
                if(asks.empty())
                    asks_.erase(askPrice);

                trades.push_back(Trade{
                    TradeInfo{ bid->GetOrderId(), bid->GetPrice(), quantity },
                    TradeInfo{ ask->GetOrderId(), ask->GetPrice(), quantity }
                });
            }
        }

        if (!bids_.empty())
        {
            auto& [_, bids] = *bids_.begin();
            auto& order = bids.front();
            if(order->GetOrderType() == OrderType::FillAndKill)
                CancelOrder(order->GetOrderId());
        }

        if(!asks_.empty())
        {
            auto& [_, asks] = *asks_.begin();
            auto& order = asks.front();
            if (order->GetOrderType() == OrderType::FillAndKill)
                CancelOrder(order->GetOrderId());
        }

        return trades;     
    }


public: 

    void CancelOrder(OrderId orderId)
    {
        auto orderEntry = orders_.find(orderId);
        if (orderEntry == orders_.end())
            return;

        const auto& [order, location] = orderEntry->second;

        if (order->GetSide() == Side::Buy)
        {
            auto priceLevel = bids_.find(order->GetPrice());
            if (priceLevel != bids_.end())
            {
                priceLevel->second.erase(location);
                if (priceLevel->second.empty())
                    bids_.erase(priceLevel);
            }
        }
        else
        {
            auto priceLevel = asks_.find(order->GetPrice());
            if (priceLevel != asks_.end())
            {
                priceLevel->second.erase(location);
                if (priceLevel->second.empty())
                    asks_.erase(priceLevel);
            }
        }

        orders_.erase(orderEntry);
    }

    Trades AddOrder(OrderPointer order)
    {
        if (orders_.find(order->GetOrderId()) != orders_.end())
            return { };
            
        if (order->GetOrderType() == OrderType::FillAndKill && !CanMatch(order->GetSide(), order->GetPrice()))
            return { };

        OrderPointers::iterator iterator;

        if (order->GetSide() == Side::Buy)
        {
            auto& orders = bids_[order->GetPrice()];
            orders.push_back(order);
            iterator = std::next(orders.begin(), orders.size() - 1);
        }
        else
        {
            auto& orders =  asks_[order->GetPrice()];
            orders.push_back(order);
            iterator = std::next(orders.begin(), orders.size() - 1);
        }

        orders_.insert({ order->GetOrderId(), OrderEntry{ order, iterator } });
        return MatchOrders();
    }

};


int main() {

    return 0;
}
