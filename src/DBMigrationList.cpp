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
#include "stdafx.hpp"

#include "Database/DBMigrate.hpp"

static void push_all_migrations(DBMigrate &dbmig) {
    /**
     *@brief API Access Key
     *
     */
    dbmig.push_migration(
        "2019_12_14_000001_create_personal_access_tokens_table",
        "CREATE TABLE `personal_access_tokens` (   `id` bigint(20) "
        "unsigned NOT NULL AUTO_INCREMENT,   `tokenable_type` varchar(255) "
        "COLLATE utf8mb4_unicode_ci NOT NULL,   `tokenable_id` bigint(20) "
        "unsigned NOT NULL,   `name` varchar(255) COLLATE "
        "utf8mb4_unicode_ci NOT NULL,   `token` varchar(128) COLLATE "
        "utf8mb4_unicode_ci NOT NULL,   `abilities` text COLLATE "
        "utf8mb4_unicode_ci DEFAULT NULL,   `last_used_at` timestamp NULL "
        "DEFAULT NULL,   `created_at` timestamp NULL DEFAULT NULL,   "
        "`updated_at` timestamp NULL DEFAULT NULL,   PRIMARY KEY (`id`),   "
        "UNIQUE KEY `personal_access_tokens_token_unique` (`token`),   KEY "
        "`personal_access_tokens_tokenable_type_tokenable_id_index` "
        "(`tokenable_type`,`tokenable_id`) ) ENGINE=InnoDB DEFAULT "
        "CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;");

    /**
     *@brief Log de autenticação geral
     *
     */
    dbmig.push_migration(
        "2021_08_18_220325_create_log_authentications_table",
        "CREATE TABLE `log_authentications` (   `id` bigint(20) unsigned NOT "
        "NULL AUTO_INCREMENT,   `partner_id` bigint(20) unsigned DEFAULT NULL, "
        "  `access_id` bigint(20) unsigned DEFAULT NULL,   `route_path` text "
        "COLLATE utf8mb4_unicode_ci DEFAULT NULL,   `sessao_uuid` char(36) "
        "COLLATE utf8mb4_unicode_ci NOT NULL,   `ip` varchar(255) COLLATE "
        "utf8mb4_unicode_ci NOT NULL,   `host` varchar(255) COLLATE "
        "utf8mb4_unicode_ci NOT NULL,   `created_at` timestamp NULL DEFAULT "
        "NULL,   `updated_at` timestamp NULL DEFAULT NULL,   PRIMARY KEY "
        "(`id`),   KEY `log_authentications_partner_id_foreign` "
        "(`partner_id`),   KEY `log_authentications_access_id_foreign` "
        "(`access_id`),   KEY `log_authentications_sessao_uuid_index` "
        "(`sessao_uuid`),   KEY `log_authentications_ip_index` (`ip`),   KEY "
        "`log_authentications_host_index` (`host`),   CONSTRAINT "
        "`log_authentications_access_id_foreign` FOREIGN KEY (`access_id`) "
        "REFERENCES `personal_access_tokens` (`id`),   CONSTRAINT "
        "`log_authentications_partner_id_foreign` FOREIGN KEY (`partner_id`) "
        "REFERENCES `partners` (`id`) ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 "
        "COLLATE=utf8mb4_unicode_ci;");
}

void prepare_and_run_all_migrations(bool /*unimplemented*/) {
    auto &conf = CConfig::config();
    auto &dbmig = DBMigrate::singleton();

    DatabaseAddress dbaddr;
    dbaddr.host = conf["MYSQL_HOST"];
    dbaddr.user = conf["MYSQL_USER"];
    dbaddr.pwd = conf["MYSQL_PASSWORD"];
    dbaddr.db = conf["MYSQL_DATABASE"];

    dbmig.set_connection_info(dbaddr);
    dbmig.init_migration_table();

    push_all_migrations(dbmig);

    dbmig.run();
}
