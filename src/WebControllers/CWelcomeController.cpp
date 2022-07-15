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
#include "../config/Instances.hpp"
#include "jobs/HelloWorldHerokuJob.hpp"
#include <Poco/JSON/Parser.h>
#include <chrono>
#include <exception>
#include <sstream>

using namespace Pistache::Http;

void CWelcomeController::index_route(const Pistache::Rest::Request & /*unused*/,
                                     ResponseWriter response) {
    using namespace Poco::JSON;
    Object::Ptr result(new Object);
    result->set("success", true);

    returnPocoJson(Code::Ok, result, response);
}

void CWelcomeController::start_some_job(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
    using namespace Poco::JSON;

    Object::Ptr jobdata(new Object);
    jobdata->set("query", request.query().as_str());
    try {
        auto body = Poco::JSON::Parser().parse(request.body());
        jobdata->set("data", body);
    } catch (const std::exception &ex) {
        LOGERR("BODY INVÁLIDO %0", ex.what());
        jobdata->set("data", {});
    }

    job::HelloWorldHerokuJob job(jobdata);

    std::string uuid = Instances::singleton().dispatch_job(
        job, std::chrono::system_clock::now() + std::chrono::seconds(10));

    Object::Ptr result(new Object);
    result->set("success", true);
    result->set("uuid", uuid);

    returnPocoJson(Code::Ok, result, response);
}

void CWelcomeController::get_some_job(const Pistache::Rest::Request &request,
                                      Pistache::Http::ResponseWriter response) {
    using namespace Poco::JSON;

    auto uuid = request.query().get("uuid");

    if (!uuid) {
        returnPocoJson(Code::Bad_Gateway,
                       default_json_return(false, "falta uma query do uui"),
                       response);
        return;
    }

    auto jobdata = Instances::singleton().queueSystem->getPersistentData(
        "job_instance:" + uuid.value());

    Object::Ptr result(new Object);
    result->set("success", true);
    result->set("data", Poco::JSON::Parser().parse(jobdata["payload"]));
    result->set("STDOUT", jobdata["JobStdout"]);
    result->set("STDERR", jobdata["JobStderr"]);

    returnPocoJson(Code::Ok, result, response);
}

void CWelcomeController::not_found(const Pistache::Rest::Request & /*request*/,
                                   ResponseWriter response) {
    using namespace Poco::JSON;
    Object::Ptr result(new Object);
    result->set("success", false);

    returnPocoJson(Code::Not_Found, result, response);
}
