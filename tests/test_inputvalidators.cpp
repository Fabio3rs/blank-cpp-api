/**
 *@file test_inputvalidators.cpp
 * @author Fabio Rossini Sluzala ()
 * @brief teste do validador de email
 * @version 0.1
 * @date 2021-07-26
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "../src/utils/ControllerInputValidator.hpp"
#include <gtest/gtest.h>

const std::string_view defaultfname = "teste";

/**
 *@brief Checagem com diversos tipos de email
 *@todo Usar mais tipos de emails
 *
 */
TEST(InputValidatorTest, CheckEmail) {
    EmailValidator emailval;

    EXPECT_TRUE(emailval.validate(defaultfname, "").has_value());
    EXPECT_TRUE(emailval.validate(defaultfname, "aaaaa").has_value());
    EXPECT_TRUE(emailval.validate(defaultfname, "aaaaa@bbbb").has_value());
    EXPECT_FALSE(emailval.validate(defaultfname, "aaaaa@bbbb.com").has_value());
    EXPECT_FALSE(
        emailval.validate(defaultfname, "aaa.aa@bbbb.com").has_value());
    EXPECT_FALSE(
        emailval.validate(defaultfname, "aaa_aa@bbbb.com").has_value());
}
