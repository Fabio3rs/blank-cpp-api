#include "../src/utils/CConfig.hpp"
#include <gtest/gtest.h>

TEST(CCONFIG, CheckSet) {
    auto &config = CConfig::config();

    config.set("PORT", "8080");
    EXPECT_EQ(config.at("PORT"), "8080");    
}
