/**
 *@file DBMigrationList.cpp
 * @author Fabio Rossini Sluzala ()
 * @brief Define todas as migrações (comandos sql e seus nomes) de banco de
 *dados para a API e roda o procedimento da classe DBMigrate
 * @version 0.1
 * @date 2021-08-21
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "DBMigrationList.hpp"
#include "migrations/2019_12_14_000001_create_personal_access_tokens_table.hpp"
#include "migrations/2021_08_18_220325_create_log_authentications_table.hpp"
#include "stdafx.hpp"

#include "../vendor/cppapiframework/src/Database/DBMigrate.hpp"
#include <memory>

static void push_all_migrations(Database::DBMigrate &dbmig) {
    /**
     *@brief API Access Key
     *
     */
    dbmig.push_migration(
        "2019_12_14_000001_create_personal_access_tokens_table",
        std::make_shared<CreatePersonalAccessTokens>());

    /**
     *@brief Log de autenticação geral
     *
     */
    dbmig.push_migration("2021_08_18_220325_create_log_authentications_table",
                         std::make_shared<CreateLogAuthentication>());
}

void prepare_and_run_all_migrations(bool /*unimplemented*/) {
    auto &conf = CConfig::config();
    auto &dbmig = Database::DBMigrate::singleton();

    Database::DatabaseAddress dbaddr;
    dbaddr.host = conf["MYSQL_HOST"];
    dbaddr.user = conf["MYSQL_USER"];
    dbaddr.pwd = conf["MYSQL_PASSWORD"];
    dbaddr.db = conf["MYSQL_DATABASE"];

    dbmig.set_connection_info(dbaddr);
    dbmig.init_migration_table();

    push_all_migrations(dbmig);

    dbmig.run();
}
