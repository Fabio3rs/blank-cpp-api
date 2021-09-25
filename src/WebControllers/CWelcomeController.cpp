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
#include "../Database/CSql.hpp"
#include "../utils/CConfig.hpp"
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <sstream>

void CWelcomeController::index_route(const Pistache::Rest::Request & /*unused*/,
                                     Pistache::Http::ResponseWriter response) {
    auto &conf = CConfig::config();
    Poco::JSON::Object::Ptr result = new Poco::JSON::Object;
    result->set("sucesso", true);

    unique_conn_t con;
    try {
        con = CSql::instance().make_connection_cfg();
        con->setSchema(conf["MYSQL_DATABASE"]);
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';

        result->set("sucesso", false);
        result->set("mensagem", "Entre em contato com o suporte");

        returnPocoJson(Pistache::Http::Code::Internal_Server_Error, result,
                       response);
        return;
    }

    returnPocoJson(Pistache::Http::Code::Ok, result, response);
}

void CWelcomeController::register_routes(const std::string &base,
                                         Pistache::Rest::Router &router) {
    Pistache::Rest::Routes::Get(
        router, base + "/",
        Pistache::Rest::Routes::bind(CWelcomeController::index_route));
}
