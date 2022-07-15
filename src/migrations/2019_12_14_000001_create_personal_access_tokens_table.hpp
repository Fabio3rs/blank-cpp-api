#pragma once

#include "../../vendor/cppapiframework/src/Database/Migration.hpp"

class CreatePersonalAccessTokens : public Database::Migration {

  public:
    void migrate() override {
        statement->execute("CREATE TABLE `personal_access_tokens` (   `id` bigint(20) "
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
    }

    void rollback() override {}

    ~CreatePersonalAccessTokens() override;
};
