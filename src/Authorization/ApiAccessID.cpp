#include "ApiAccessID.hpp"
#include "../../vendor/cppapiframework/src/utils/ResultMacros.hpp"
#include "models/PersonalAccessTokens.hpp"
#include <Poco/Crypto/DigestEngine.h>
#include <exception>
#include <openssl/crypto.h>
#include <pistache/http_defs.h>
#include <pistache/http_header.h>
#include <string>
#include <string_view>

auto ApiAccessID::safe_hash_cmp(const std::string &hash_a,
                                const std::string &hash_b) -> compare_t {
    if (hash_a.size() != hash_b.size() ||
        CRYPTO_memcmp(hash_a.c_str(), hash_b.c_str(), hash_b.size()) != 0) {
        return compare_t::error_t{"Access ID invalido"};
    }

    return compare_t::ok_t{};
}

auto ApiAccessID::getAPIAuthorization(const Pistache::Rest::Request &request)
    -> authres_t {
    const auto auth_header =
        request.headers().tryGet<Pistache::Http::Header::Authorization>();

    if (!auth_header) {
        return result_t::error_t{"No Authorization header found"};
    }

    const std::string userAccessID = auth_header->value();

    if (userAccessID.empty()) {
        return result_t::error_t{"AccessID is empty"};
    }

    if (auth_header->getMethod() !=
        Pistache::Http::Header::Authorization::Method::Bearer) {
        return result_t::error_t{"AccessID should be a Bearer Token"};
    }

    std::string plainAccessID =
        EXPECT_RESULT(getPlainAccessIDFromBearerAuth(userAccessID));

    return getAPIAccessPlainID(plainAccessID);
}

auto ApiAccessID::getAPIAccessFromBearerAuth(const std::string &userAccessID)
    -> authres_t {
    if (userAccessID.empty()) {
        return result_t::error_t{"AccessID is empty"};
    }

    std::string plainAccessID =
        EXPECT_RESULT(getPlainAccessIDFromBearerAuth(userAccessID));

    return getAPIAccessPlainID(plainAccessID);
}

void ApiAccessID::makeAccessIdSqlSelect(
    std::pair<unsigned long, std::basic_string<char>> &AccessKPairHashed,
    std::string &query) {
    query.reserve(256);

    if (AccessKPairHashed.first != 0) {
        query = "SELECT id, token, tokenable_id, tokenable_type, abilities "
                "FROM personal_access_tokens WHERE id = ";
        query += std::to_string(AccessKPairHashed.first);
        query += " LIMIT 1;";
    } else {
        query = "SELECT id, token, tokenable_id, tokenable_type, abilities "
                "FROM personal_access_tokens WHERE token = '";
        query += AccessKPairHashed.second; // Sanitizado pela hash sha256
        query += "' LIMIT 1;";
    }
}

ApiAccessID::authres_t ApiAccessID::getApiAccessIdFromIdHash(
    std::pair<unsigned long, std::basic_string<char>> &AccessKPairHashed) {
    std::string query;
    makeAccessIdSqlSelect(AccessKPairHashed, query);

    auto ssmt = unique_statement_t(conn->createStatement());

    auto results = unique_resultset_t(ssmt->executeQuery(query));

    if (results->rowsCount() == 0 || !results->next()) {
        return authres_t::error_t{"Access ID invalido"};
    }

    std::string accesskey = results->getString("token");

    EXPECT_RESULT(safe_hash_cmp(accesskey, AccessKPairHashed.second));

    PersonalAccess access;

    access.id = results->getUInt64("id");
    access.token = accesskey;
    access.tokenable_id = results->getUInt64("tokenable_id");
    access.tokenable_type = results->getString("tokenable_type");

    try {
        Poco::JSON::Parser parser;
        access.abilities =
            parser.parse(std::string(results->getString("abilities")))
                .extract<Poco::JSON::Array::Ptr>();
    } catch (const std::exception &e) {
        if (access.abilities.isNull()) {
            access.abilities = new Poco::JSON::Array;
        }
        std::cerr << e.what() << '\n';
    }

    return authres_t::ok_t{access};
}

auto ApiAccessID::getAPIAccessPlainID(const std::string &AccessID)
    -> authres_t {
    if (!conn) {
        return authres_t::error_t{"No connection to the database!"};
    }

    auto AccessKPairHashed = EXPECT_RESULT(parseAndHashPlainAccessID(AccessID));
    return getApiAccessIdFromIdHash(AccessKPairHashed);
}

auto ApiAccessID::getPlainAccessIDFromBearerAuth(
    const std::string &UserAccessID)
    -> utils::Result<std::string, std::string> {
    const std::string Bearer = "Bearer ";
    if (UserAccessID.size() < Bearer.size() ||
        UserAccessID.find_first_of(Bearer) != 0) {
        return utils::Err<std::string>{"AccessID is invalid"};
    }

    std::string plainAccessID = UserAccessID.substr(Bearer.size());
    return utils::Ok<std::string>{plainAccessID};
}

auto ApiAccessID::parseAccessID(const std::string &AccessID) -> parse_t {
    if (AccessID.empty()) {
        return parse_t::error_t{"API Access ID is empty"};
    }

    std::size_t IdEnd = AccessID.find_first_of('|');

    if (IdEnd == std::string::npos) {
        return parse_t::ok_t{{0, AccessID}};
    }

    uint64_t iddb = 0;

    try {
        iddb = std::stoull(AccessID.substr(0, IdEnd));
    } catch (const std::exception &e) {
        using namespace std::string_literals;
        return parse_t::error_t{"API Access ID Parse error: "s + e.what()};
    }

    std::string idPlainText = AccessID.substr(IdEnd + 1);

    if (idPlainText.empty()) {
        return parse_t::error_t{"Plain API Access ID is empty"};
    }

    return parse_t::ok_t{{iddb, idPlainText}};
}

auto ApiAccessID::hashPlainAccessID(const std::string &PlainAccessID)
    -> std::string {
    Poco::Crypto::DigestEngine sha256("SHA256");

    sha256.update(PlainAccessID);

    return Poco::Crypto::DigestEngine::digestToHex(sha256.digest());
}

auto ApiAccessID::parseAndHashPlainAccessID(const std::string &PlainAccessID)
    -> parse_t {
    auto AccessKPair = EXPECT_RESULT(parseAccessID(PlainAccessID));
    AccessKPair.second = hashPlainAccessID(AccessKPair.second);

    return parse_t::ok_t{AccessKPair};
}

auto ApiAccessID::create_table_mysql() -> std::string {
    std::string create =
        "CREATE TABLE `personal_access_tokens` (\n"
        "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,\n"
        "  `tokenable_type` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,\n"
        "  `tokenable_id` bigint(20) unsigned NOT NULL,\n"
        "  `name` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,\n"
        "  `token` varchar(64) COLLATE utf8mb4_unicode_ci NOT NULL,\n"
        "  `abilities` text COLLATE utf8mb4_unicode_ci DEFAULT NULL,\n"
        "  `last_used_at` timestamp NULL DEFAULT NULL,\n"
        "  `created_at` timestamp NULL DEFAULT NULL,\n"
        "  `updated_at` timestamp NULL DEFAULT NULL,\n"
        "  PRIMARY KEY (`id`),\n"
        "  UNIQUE KEY `personal_access_tokens_token_unique` (`token`),\n"
        "  KEY `personal_access_tokens_tokenable_type_tokenable_id_index` "
        "(`tokenable_type`,`tokenable_id`)\n"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;\n";
    return create;
}
