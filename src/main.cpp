#include <iostream>
#include <string>

#include "me/parser.hpp"

int main() {
    std::cout << "Trading Message Parser\n";
    std::cout << "Examples:\n";
    std::cout << "  ADD 101 BUY 100 50\n";
    std::cout << "  CANCEL 101\n";
    std::cout << "  MODIFY 101 105 25\n";
    std::cout << "Type EXIT to quit.\n\n";

    std::string line;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);

        if (line == "EXIT") {
            break;
        }

        auto command = me::parse_command(line);

        if (!command.has_value()) {
            std::cout << "Invalid command\n";
            continue;
        }

        if (command->type == me::CommandType::Add) {
            std::cout << "Parsed ADD order: ";
            std::cout << "id=" << command->order_id;
            std::cout << " price=" << command->price;
            std::cout << " quantity=" << command->quantity;

            if (command->side == me::Side::Buy) {
                std::cout << " side=BUY\n";
            } else {
                std::cout << " side=SELL\n";
            }
        }

        if (command->type == me::CommandType::Cancel) {
            std::cout << "Parsed CANCEL order: ";
            std::cout << "id=" << command->order_id << "\n";
        }

        if (command->type == me::CommandType::Modify) {
            std::cout << "Parsed MODIFY order: ";
            std::cout << "id=" << command->order_id;
            std::cout << " new_price=" << command->price;
            std::cout << " new_quantity=" << command->quantity << "\n";
        }
    }

    return 0;
}
