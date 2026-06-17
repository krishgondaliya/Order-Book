#pragma once

#include <optional>
#include <string>
#include "me/message.hpp"

namespace me {

// Takes one line of text and tries to parse it into an OrderCommand.
//
// Examples:
//   ADD 101 BUY 100 50
//   CANCEL 101
//   MODIFY 101 105 25
//
// If parsing succeeds, returns an OrderCommand.
// If parsing fails, returns std::nullopt.
std::optional<OrderCommand> parse_command(const std::string& line);

} // namespace me
