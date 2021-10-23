#include "../src/RegisterControllers.hpp"
#include <gtest/gtest.h>
#include "../src/WebRequest/PocoRequest.hpp"

static auto generateEndpoint() {
    std::shared_ptr<CPistacheEndpoint> endp(
        std::make_shared<CPistacheEndpoint>());

    registerPistacheWebControllers(*endp.get());
    initPistacheWebService(*endp.get());

    return endp;
}

static auto getEndPoint() {
    static std::shared_ptr<CPistacheEndpoint> endpoint(generateEndpoint());

    return endpoint;
}

TEST(TestAPIRoutes, CheckAPIUp) {
    auto &config = CConfig::config();

    config.set("API_LISTEN_PORT", "0");

    auto endpoint = getEndPoint();
    auto httpendpoint = endpoint->getHttpEndpoint();

    ASSERT_TRUE(httpendpoint != nullptr);

    auto port = httpendpoint->getPort();

    ASSERT_NE(port, 0);

    PocoRequest req;

    auto json = req.http_get_json("http://127.0.0.1:" + std::to_string(port) + "/", {});

    EXPECT_FALSE(json.isNull());

    if (json)
    {
        EXPECT_EQ(json->getValue<bool>("sucesso"), true);
    }

    endpoint->stop();
}
