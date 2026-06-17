#include "me/parser.hpp"

#include <sstream>
#include <string>

namespace me {

namespace {

// Converts a string like "BUY" or "SELL" into our Side enum.
std::optional<Side> parse_side(const std::string& text) {
    if (text == "BUY") {
        return Side::Buy;
    }

    if (text == "SELL") {
        return Side::Sell;
    }

    return std::nullopt;
}

} // namespace

std::optional<OrderCommand> parse_command(const std::string& line) {
    std::istringstream input(line);

    std::string command_type;
    input >> command_type;

    if (command_type == "ADD") {
        std::uint64_t order_id{};
        std::string side_text;
        std::int64_t price{};
        std::int64_t quantity{};

        input >> order_id >> side_text >> price >> quantity;

        if (!input) {
            return std::nullopt;
        }

        auto side = parse_side(side_text);

        if (!side.has_value()) {
            return std::nullopt;
        }

        if (price <= 0 || quantity <= 0) {
            return std::nullopt;
        }

        OrderCommand command{};
        command.type = CommandType::Add;
        command.order_id = order_id;
        command.side = side.value();
        command.price = price;
        command.quantity = quantity;

        return command;
    }

    if (command_type == "CANCEL") {
        std::uint64_t order_id{};

        input >> order_id;

        if (!input) {
            return std::nullopt;
        }

        OrderCommand command{};
        command.type = CommandType::Cancel;
        command.order_id = order_id;

        return command;
    }

    if (command_type == "MODIFY") {
        std::uint64_t order_id{};
        std::int64_t price{};
        std::int64_t quantity{};

        input >> order_id >> price >> quantity;

        if (!input) {
            return std::nullopt;
        }

        if (price <= 0 || quantity <= 0) {
            return std::nullopt;
        }

        OrderCommand command{};
        command.type = CommandType::Modify;
        command.order_id = order_id;
        command.price = price;
        command.quantity = quantity;

        return command;
    }

    return std::nullopt;
}

} // namespace me
