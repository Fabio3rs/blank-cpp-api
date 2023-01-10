#include "api.hpp"
#include "../../vendor/cppapiframework/src/WebControllers/CStorageController.hpp"
#include "../WebControllers/CWelcomeController.hpp"
#include "../WebMiddlewares/AuthMiddleware.hpp"
#include <memory>
#include <utility>
#include <vector>

namespace {

std::vector<std::shared_ptr<CController>> controllers;

}

void routes::api::registerRoutes(Pistache::Rest::Router &router) {
    using namespace Pistache::Rest::Routes;
    router.addMiddleware(middleware(AuthMiddleware::auth));

    auto storageController = std::make_shared<CStorageController>();
    storageController->register_routes("", router);
    controllers.emplace_back(std::move(storageController));

    Get(router, "/", bind(CWelcomeController::index_route));
    Post(router, "/start_some_job", bind(CWelcomeController::start_some_job));
    Get(router, "/get_some_job", bind(CWelcomeController::get_some_job));
    NotFound(router, bind(CWelcomeController::not_found));
}
