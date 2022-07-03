#include "api.hpp"
#include "../WebControllers/CWelcomeController.hpp"

void routes::api::registerRoutes(Pistache::Rest::Router &router) {
    using namespace Pistache::Rest::Routes;
    Get(router, "/", bind(CWelcomeController::index_route));
    NotFound(router, bind(CWelcomeController::not_found));
}
