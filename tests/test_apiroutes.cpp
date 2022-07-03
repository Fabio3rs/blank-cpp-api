#include "../src/InitWebService.hpp"
#include "../src/WebRequest/PocoRequest.hpp"
#include "../src/routes/api.hpp"
#include <Poco/Exception.h>
#include <gtest/gtest.h>

namespace {
static auto generateEndpoint() {
    std::shared_ptr<CPistacheEndpoint> endp(
        std::make_shared<CPistacheEndpoint>());

    routes::api::registerRoutes(endp->get_router());
    initPistacheWebService(*endp.get());

    return endp;
}

static auto getEndPoint() {
    static std::shared_ptr<CPistacheEndpoint> endpoint(generateEndpoint());

    return endpoint;
}

class TestAPIRoutes : public ::testing::Test {

  protected:
    void SetUp() override {
        auto &config = CConfig::config();
        config.set("API_LISTEN_PORT", "0");
        endpoint = getEndPoint();
        ASSERT_TRUE(endpoint != nullptr);
        port = endpoint->getPort();
        ASSERT_NE(port, 0);
    }

    TestAPIRoutes() = default;
    ~TestAPIRoutes() override;

    std::shared_ptr<CPistacheEndpoint> endpoint;
    PocoRequest req;
    int port{};
};
} // namespace

TestAPIRoutes::~TestAPIRoutes() = default;

TEST_F(TestAPIRoutes, CheckAPIUp) {
    auto json =
        req.http_get_json("http://127.0.0.1:" + std::to_string(port) + "/", {});

    EXPECT_FALSE(json.isNull());

    try {
        if (json) {
            EXPECT_EQ(json->getValue<bool>("success"), true);
        }
    } catch (const Poco::Exception &ex) {
        std::cout << ex.displayText() << std::endl;
        std::cout << ex.message() << std::endl;
        EXPECT_FALSE(true);
    }

    endpoint->stop();
}

TEST_F(TestAPIRoutes, NotFound) {
    auto json = req.http_get_json(
        "http://127.0.0.1:" + std::to_string(port) + "/something", {});

    EXPECT_FALSE(json.isNull());

    try {
        if (json) {
            EXPECT_EQ(json->getValue<bool>("success"), false);
        }
    } catch (const Poco::Exception &ex) {
        std::cout << ex.displayText() << std::endl;
        std::cout << ex.message() << std::endl;
        EXPECT_FALSE(true);
    }

    endpoint->stop();
}
