/**
 *@file main.cpp
 * @author Fabio Rossini Sluzala ()
 * @brief Main do projeto API
 * @version 0.1
 * @date 2021-07-26
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "../vendor/cppapiframework/src/SSLUtils/InitClient.hpp"
#include "../vendor/cppapiframework/src/utils/DocAPI.hpp"
#include "DBMigrationList.hpp"
#include "InitWebService.hpp"
#include "SeedDB.hpp"
#include "autogen/jobslist.hpp"
#include "config/Instances.hpp"
#include "routes/api.hpp"
#include "stdafx.hpp"
#include "JSON/StructParserMacros.hpp"
#include <Poco/Crypto/EVPPKey.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/Dynamic/VarHolder.h>
#include <Poco/JSON/Parser.h>
#include <Poco/SharedPtr.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ossl_typ.h>
#include <openssl/rsa.h>
#include <signal.h>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace {

std::shared_ptr<CPistacheEndpoint> endpoint;
std::atomic<bool> keepRunning = true;
std::mutex cmdmtx;

/**
 *@brief Handles program's shutdown sequence
 *
 */
void start_shutdown_seq() {
    std::lock_guard<std::mutex> lck(cmdmtx);

    auto &log = CLog::log();

    log << "Starting signal sequence";
    keepRunning = false;

    if (endpoint) {
        log << "Will send API endpoint stop message";
        endpoint->stop();
    } else {
        log << "API endpoint is nullptr";
    }

    log << "std::cin will be closed to stop command reading";
    log.multiRegister("close(0) -> %0", close(0));
}

void signal_callback(int signal) {
    CLog::log().multiRegister("Signal received %0", signal);
    start_shutdown_seq();
}

void wait_command(CPistacheEndpoint &endp) {
    std::cout << "Type stop to exit\n";
    std::string str;
    while (keepRunning) {
        str.clear();
        std::cin >> str;

        /**
         * Iniciando pelo nohup entra em loop infinito por causa do stdin sendo
         * direcionado para /dev/null Pequeno ajuste para evitar isso
         */
        if (std::cin.fail() || str.empty()) {
            if (!keepRunning) {
                return;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        std::lock_guard<std::mutex> lck(cmdmtx);

        if (str == "stop") {
            std::cout << "Exiting..." << std::endl;
            endp.stop();
            break;
        }
        if (str == "dump") {
            DocAPI::singleton().dump();
        } else {
            std::cout << "Unknown command " << str << std::endl;
        }
    }
}

void job_handler() {
    const std::string queue_name = Instances::singleton().queue_name;

    LOGINF("Listeing for jobs on (%0)", queue_name);

    while (keepRunning) {
        if (!Instances::singleton().queueWorker->do_one(queue_name)) {
            LOGDBG("No jobs available on (%0)", queue_name);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void setup_instances() {
    Instances &defInstances = Instances::singleton();
    defInstances.queue_name =
        CConfig::config().at("MYSQL_DATABASE", "apicheck") + ":queue:default";
    defInstances.queueSystem = std::make_shared<RedisQueue>();

    {
        auto worker = std::make_shared<job::QueueWorker>(
            job::JobsHandler::default_instance(), defInstances.queueSystem);

        worker->setForkToHandleJob(true);
        worker->setCleanSuccessfulJobsLogs(false);
        defInstances.queueWorker = worker;
    }

    autogen::registerJobs(*job::JobsHandler::default_instance());
}

struct AnotherStruct {
    std::string name;
    std::string lastname;
};

struct TestJs {
    std::string val;
    std::string anotherval;

    std::vector<std::string> dataarr;

    int64_t intvalue;

    AnotherStruct userdata;

    std::vector<AnotherStruct> otherUsers;

    bool algumacoisa{};
};

} // namespace

namespace JSONStructParser {
MAKE_DISABLE_SET_STRUCT(AnotherStruct);
MAKE_START_OBJECT_SPECIALIZATION(AnotherStruct);
MAKE_FIELD_LIST_JS(AnotherStruct, name, lastname);
MAKE_FIELD_LIST_JS(TestJs, anotherval, val, dataarr, intvalue, userdata,
                   otherUsers, algumacoisa);
} // namespace JSONStructParser

auto main(int argc, const char *argv[], const char *envp[]) -> int {
    /**
     * Testing environment variables
     */

    CLog::initSingleton({{}, &std::cout});

    auto &conf = CConfig::config();
    conf.load_from_envp(envp);

    CLog &log = CLog::log();

    {
        TestJs data;

        Poco::SharedPtr<JSONStructParser::StructParser> sparser =
            new JSONStructParser::StructParser;
        sparser->current =
            std::make_unique<JSONStructParser::TemplateStructFiller<TestJs>>(
                data);

        Poco::JSON::Parser parser(sparser);
        parser.parse(R"json({
            "anotherval" : 0,
            "val" : "AAAAA TESTE",
            "dataarr": [
                "a",
                "b",
                "c",
                "d"
            ],
            "intvalue": 10,
            "userdata" : {
                "name" : "Fulano",
                "lastname" : "De Tal"
            },
            "otherUsers" : [
                {
                    "name" : "Fulano",
                    "lastname" : "De Tal 2"
                },
                {
                    "name" : "Fulano",
                    "lastname" : "De Tal 3"
                },
                {
                    "name" : "Fulano",
                    "lastname" : "De Tal 4"
                },
                {
                    "name" : "Fulano",
                    "lastname" : "De Tal 5"
                }
            ],
            "algumacoisa": true
        })json");

        std::cout << "data.anotherval " << data.anotherval << std::endl;
        std::cout << "data.val " << data.val << std::endl;
        std::cout << "data.intvalue " << data.intvalue << std::endl;
        std::cout << "data.userdata.name " << data.userdata.name << std::endl;
        std::cout << "data.userdata.lastname " << data.userdata.lastname
                  << std::endl;

        for (const auto &d : data.dataarr) {
            std::cout << " dataarr val " << d << std::endl;
        }
        for (const auto &d : data.otherUsers) {
            std::cout << "d.name " << d.name << std::endl;
            std::cout << "d.lastname " << d.lastname << std::endl;
        }

        std::cout << "data.algumacoisa " << data.algumacoisa << std::endl;
    }

    SSLUtils::initClient();

    log << "API Starting";
    signal(SIGTERM, signal_callback);

    setup_instances();

    auto *argvend = argv + argc;

    bool migrate_fresh =
        argvend != std::find(argv, argvend, std::string_view("migrate:fresh"));
    bool migrate_exit =
        argvend != std::find(argv, argvend, std::string_view("migrate:exit"));
    bool migrate =
        migrate_exit ||
        argvend != std::find(argv, argvend, std::string_view("migrate"));

    bool seeddb =
        argvend != std::find(argv, argvend, std::string_view("seeddb"));

    if (migrate || migrate_fresh) {
        try {
            prepare_and_run_all_migrations(migrate_fresh);
        } catch (const std::exception &e) {
            std::cerr << CLOG_EXCEPTION_LOG(e) << '\n';
            return 1;
        }

        if (migrate_exit) {
            return 0;
        }
    }

    if (seeddb) {
        SeedDB::runAllDatabaseSeeders();
    }

    bool queue_work =
        argvend != std::find(argv, argvend, std::string_view("queue:work"));

    if (queue_work) {
        job_handler();
        return 0;
    }

#ifdef DOCAPI_ENABLED
    auto &docapi = DocAPI::singleton();

    docapi.set_api_info_basic("API Desconhecida", "Descrição API");
    docapi.set_contact("Desenvolvimento Teste", "dev@www.exemplo.com.br",
                       "http://www.exemplo.com.br/");
    docapi.add_server("Homologação", "https://sandbox.example.com/");
    docapi.add_server("Produção", "https://production.example.com/");
    docapi.set_security_schema("ApiAccessID",
                               {"http", "bearer", "unique_string"});
#endif

    endpoint = std::make_shared<CPistacheEndpoint>();

    {
        CPistacheEndpoint &endp = *endpoint;

        routes::api::registerRoutes(endp.get_router());
        initPistacheWebService(endp);
    }
    wait_command(*endpoint);

    log << "Exiting API";
    endpoint.reset();

    bool DumpDocs = false;

#ifndef NDEBUG
    DumpDocs = true;
#endif

    {
        auto *it =
            std::find(argv, argvend, std::string_view("dumpdocsbeforeexit"));

        if (it != argvend || DumpDocs) {
            DocAPI::singleton().dump();
        }
    }

    return 0;
}
