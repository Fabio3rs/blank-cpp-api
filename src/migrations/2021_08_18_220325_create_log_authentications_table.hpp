#pragma once

#include "../../vendor/cppapiframework/src/Database/Migration.hpp"

class CreateLogAuthentication : public Database::Migration {

  public:
    void migrate() override {
        statement->execute(
            "CREATE TABLE `log_authentications` (   `id` bigint(20) unsigned "
            "NOT NULL AUTO_INCREMENT,   `partner_id` bigint(20) unsigned "
            "DEFAULT "
            "NULL,   `access_id` bigint(20) unsigned DEFAULT NULL,   "
            "`route_path` "
            "text COLLATE utf8mb4_unicode_ci DEFAULT NULL,   `sessao_uuid` "
            "char(36) "
            "COLLATE utf8mb4_unicode_ci NOT NULL,   `ip` varchar(255) COLLATE "
            "utf8mb4_unicode_ci NOT NULL,   `host` varchar(255) COLLATE "
            "utf8mb4_unicode_ci NOT NULL,   `created_at` timestamp NULL "
            "DEFAULT NULL,   `updated_at` timestamp NULL DEFAULT NULL,   "
            "PRIMARY KEY "
            "(`id`),   KEY `log_authentications_partner_id_foreign` "
            "(`partner_id`),   KEY `log_authentications_access_id_foreign` "
            "(`access_id`),   KEY `log_authentications_sessao_uuid_index` "
            "(`sessao_uuid`),   KEY `log_authentications_ip_index` (`ip`),   "
            "KEY "
            "`log_authentications_host_index` (`host`),   CONSTRAINT "
            "`log_authentications_access_id_foreign` FOREIGN KEY (`access_id`) "
            "REFERENCES `personal_access_tokens` (`id`),   CONSTRAINT "
            "`log_authentications_partner_id_foreign` FOREIGN KEY "
            "(`partner_id`) "
            "REFERENCES `partners` (`id`) ) ENGINE=InnoDB DEFAULT "
            "CHARSET=utf8mb4 "
            "COLLATE=utf8mb4_unicode_ci;");
    }

    void rollback() override {}

    ~CreateLogAuthentication() override;
};
