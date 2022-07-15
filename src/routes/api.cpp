#include "api.hpp"
#include "../WebControllers/CWelcomeController.hpp"

void routes::api::registerRoutes(Pistache::Rest::Router &router) {
    using namespace Pistache::Rest::Routes;
    Get(router, "/", bind(CWelcomeController::index_route));
    Post(router, "/start_some_job", bind(CWelcomeController::start_some_job));
    Get(router, "/get_some_job", bind(CWelcomeController::get_some_job));
    NotFound(router, bind(CWelcomeController::not_found));
}
