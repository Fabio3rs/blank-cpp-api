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
#include "RegisterControllers.hpp"
#include "stdafx.hpp"
#include "utils/CLog.hpp"
#include <fstream>
#include <iostream>
#include <signal.h>

#include "DBMigrationList.hpp"
#include "SeedDB.hpp"

#include "utils/DocAPI.hpp"

static std::shared_ptr<CPistacheEndpoint> endpoint;
static std::atomic<bool> keepRunning = true;
static std::mutex cmdmtx;

/**
 *@brief Handles program's shutdown sequence
 *
 */
static void start_shutdown_seq() {
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

static void signal_callback(int signal) {
    CLog::log().multiRegister("Signal received %0", signal);
    start_shutdown_seq();
}

static void wait_command(CPistacheEndpoint &endp) {
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

#ifndef PROJECT_NAME
#define PROJECT_NAME "unkownapi"
#endif

auto main(int argc, const char *argv[], const char *envp[]) -> int {
    /**
     * Testing environment variables
     */
    auto &conf = CConfig::config();
    conf.load_from_envp(envp);

    CLog &log = CLog::log(conf.at("LOG_PATH", PROJECT_NAME ".log").c_str());

    log << "API Starting";
    signal(SIGTERM, signal_callback);

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
        APITeste::runAllDatabaseSeeders();
    }

#ifdef DOCAPI_ENABLED
    auto &docapi = DocAPI::singleton();

    docapi.set_api_info_basic(
        "API Desconhecida",
        "Descrição API");
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

        registerPistacheWebControllers(endp);
        initPistacheWebService(endp);

        wait_command(endp);
    }

    log << "Exiting";
    endpoint.reset();

    bool dumpdocsalways = false;

#ifndef NDEBUG
    dumpdocsalways = true;
#endif

    {
        auto *it =
            std::find(argv, argvend, std::string_view("dumpdocsbeforeexit"));

        if (it != argvend || dumpdocsalways) {
            DocAPI::singleton().dump();
        }
    }

    return 0;
}
