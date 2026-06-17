#include <gtest/gtest.h>

#include "me/parser.hpp"

TEST(ParserTests, ParsesAddBuyCommand) {
    auto command = me::parse_command("ADD 101 BUY 100 50");

    ASSERT_TRUE(command.has_value());

    EXPECT_EQ(command->type, me::CommandType::Add);
    EXPECT_EQ(command->order_id, 101);
    EXPECT_EQ(command->side, me::Side::Buy);
    EXPECT_EQ(command->price, 100);
    EXPECT_EQ(command->quantity, 50);
}

TEST(ParserTests, ParsesAddSellCommand) {
    auto command = me::parse_command("ADD 102 SELL 105 20");

    ASSERT_TRUE(command.has_value());

    EXPECT_EQ(command->type, me::CommandType::Add);
    EXPECT_EQ(command->order_id, 102);
    EXPECT_EQ(command->side, me::Side::Sell);
    EXPECT_EQ(command->price, 105);
    EXPECT_EQ(command->quantity, 20);
}

TEST(ParserTests, ParsesCancelCommand) {
    auto command = me::parse_command("CANCEL 101");

    ASSERT_TRUE(command.has_value());

    EXPECT_EQ(command->type, me::CommandType::Cancel);
    EXPECT_EQ(command->order_id, 101);
}

TEST(ParserTests, ParsesModifyCommand) {
    auto command = me::parse_command("MODIFY 101 110 25");

    ASSERT_TRUE(command.has_value());

    EXPECT_EQ(command->type, me::CommandType::Modify);
    EXPECT_EQ(command->order_id, 101);
    EXPECT_EQ(command->price, 110);
    EXPECT_EQ(command->quantity, 25);
}

TEST(ParserTests, RejectsInvalidSide) {
    auto command = me::parse_command("ADD 101 HOLD 100 50");

    EXPECT_FALSE(command.has_value());
}

TEST(ParserTests, RejectsNegativePrice) {
    auto command = me::parse_command("ADD 101 BUY -100 50");

    EXPECT_FALSE(command.has_value());
}

TEST(ParserTests, RejectsNegativeQuantity) {
    auto command = me::parse_command("ADD 101 BUY 100 -50");

    EXPECT_FALSE(command.has_value());
}

TEST(ParserTests, RejectsUnknownCommand) {
    auto command = me::parse_command("HELLO 101 BUY 100 50");

    EXPECT_FALSE(command.has_value());
}
