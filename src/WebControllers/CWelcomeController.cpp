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
#include "../stdafx.hpp"

#include "CWelcomeController.hpp"
#include "../utils/CConfig.hpp"
#include <sstream>

void CWelcomeController::index_route(const Pistache::Rest::Request & /*unused*/,
                                     Pistache::Http::ResponseWriter response) {
    Poco::JSON::Object::Ptr result(new Poco::JSON::Object);
    result->set("sucesso", true);

    returnPocoJson(Pistache::Http::Code::Ok, result, response);
}

void CWelcomeController::register_routes(const std::string &base,
                                         Pistache::Rest::Router &router) {
    Pistache::Rest::Routes::Get(
        router, base + "/",
        Pistache::Rest::Routes::bind(CWelcomeController::index_route));
}
