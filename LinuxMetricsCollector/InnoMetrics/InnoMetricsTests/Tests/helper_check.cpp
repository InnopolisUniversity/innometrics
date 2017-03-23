//
// Created by alon on 22.03.17.
//

#include "gtest/gtest.h"
#include "Sources/Helpers/Helper.h"

TEST(helper_check, test_escape_json_basic) {
    std::string str = "Hello\"INNO\"";
    std::string proceed = Helper::escape_json(str);
    EXPECT_EQ(proceed, "Hello\\\"INNO\\\"");
}

TEST(helper_check, test_escape_json_empty) {
    std::string str = "";
    std::string proceed = Helper::escape_json(str);
    EXPECT_EQ(proceed, "");
}