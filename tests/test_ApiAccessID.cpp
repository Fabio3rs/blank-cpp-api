/**
 *@file test_ApiAccessID.cpp
 * @author Fabio Rossini Sluzala ()
 * @brief Testa a classe ApiAccessID
 * @todo Mockar database para testes
 * @version 0.1
 * @date 2021-07-26
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "../src/Authorization/ApiAccessID.hpp"
#include <gtest/gtest.h>
#include <iostream>

/**
 *@brief ID do ApiAccessID no banco de dados
 *
 */
static const uint64_t API_TEST_ID = 10;

/**
 *@brief Secret do ApiAccessID
 *
 */
static const std::string SECRET_EXAMPLE = "qwertyuioprandomico";

/**
 *@brief Hash do secret acima; é guardado no banco de dados
 *
 */
static const std::string SECRET_EXAMPLE_HASH =
    "bad5b8751b2b69e7743a64e19ae1344e056ce765af587c760793d583d982d8f6";

/**
 *@brief testa função ApiAccessID::parseAccessID
 *
 */
TEST(ApiAccessIDTest, TestParsePlainText) {
    auto pair = ApiAccessID::parseAccessID(std::to_string(API_TEST_ID) + "|" +
                                           SECRET_EXAMPLE)
                    .unwrap();

    EXPECT_EQ(pair.first, API_TEST_ID);
    EXPECT_EQ(pair.second, SECRET_EXAMPLE);
}

/**
 *@brief Testa função ApiAccessID::hashPlainAccessID
 *
 */
TEST(ApiAccessIDTest, TestHashPlainAccessID) {
    auto hashedplainaccessid = ApiAccessID::hashPlainAccessID(SECRET_EXAMPLE);

    EXPECT_EQ(hashedplainaccessid, SECRET_EXAMPLE_HASH);
}

/**
 *@brief Testa função ApiAccessID::getPlainAccessIDFromBearerAuth
 *
 */
TEST(ApiAccessIDTest, TestParsePlainTextBearer) {
    const auto TEST_PLAIN_ID =
        std::to_string(API_TEST_ID) + "|" + SECRET_EXAMPLE;
    auto plainaccessid =
        ApiAccessID::getPlainAccessIDFromBearerAuth("Bearer " + TEST_PLAIN_ID);

    EXPECT_EQ(plainaccessid.unwrap(), TEST_PLAIN_ID);
}

/**
 *@brief Testa função ApiAccessID::parseAndHashPlainAccessID
 *
 */
TEST(ApiAccessIDTest, TestParseAndHashPlainAccessID) {
    const auto TEST_PLAIN_ID =
        std::to_string(API_TEST_ID) + "|" + SECRET_EXAMPLE;
    auto pair = ApiAccessID::parseAndHashPlainAccessID(TEST_PLAIN_ID).unwrap();

    EXPECT_EQ(pair.first, API_TEST_ID);
    EXPECT_EQ(pair.second, SECRET_EXAMPLE_HASH);
}
