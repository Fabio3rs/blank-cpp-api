/**
 *@file CWelcomeController.cpp
 * @author Fabio Rossini Sluzala ()
 * @brief Implementação do controller de boas vindas da API
 * @version 0.1
 * @date 2021-08-04
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "CWelcomeController.hpp"
#include <sstream>

using namespace Pistache::Http;

void CWelcomeController::index_route(const Pistache::Rest::Request & /*unused*/,
                                     ResponseWriter response) {
    using namespace Poco::JSON;
    Object::Ptr result(new Object);
    result->set("success", true);

    returnPocoJson(Code::Ok, result, response);
}

void CWelcomeController::not_found(const Pistache::Rest::Request & /*request*/,
                                   ResponseWriter response) {
    using namespace Poco::JSON;
    Object::Ptr result(new Object);
    result->set("success", false);

    returnPocoJson(Code::Not_Found, result, response);
}
