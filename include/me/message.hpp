#pragma once

#include <cstdint>
#include <string>

namespace me {

// Side tells us whether the order is a buy order or sell order.
enum class Side {
    Buy,
    Sell
};

// CommandType tells us what kind of message the user entered.
enum class CommandType {
    Add,
    Cancel,
    Modify
};

// This struct represents a parsed trading command.
//
// For ADD:
//   order_id, side, price, quantity are used.
//
// For CANCEL:
//   only order_id is used.
//
// For MODIFY:
//   order_id, price, quantity are used.
struct OrderCommand {
    CommandType type{};
    std::uint64_t order_id{};
    Side side{};
    std::int64_t price{};
    std::int64_t quantity{};
};

} // namespace me
