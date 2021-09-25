#include "ApiAccessID.hpp"
#include <Poco/Crypto/DigestEngine.h>
#include <string>
#include <string_view>

auto ApiAccessID::getAPIAuthorization(const Pistache::Rest::Request &request)
    -> Poco::JSON::Object::Ptr {
    const auto auth_header =
        request.headers().tryGet<Pistache::Http::Header::Authorization>();

    if (!auth_header) {
        throw std::invalid_argument("No Authorization header found");
    }

    const std::string userAccessID = auth_header->value();

    if (userAccessID.empty()) {
        throw std::invalid_argument("AccessID is empty");
    }

    std::string plainAccessID = getPlainAccessIDFromBearerAuth(userAccessID);

    return getAPIAccessPlainID(plainAccessID);
}

auto ApiAccessID::getAPIAccessFromBearerAuth(const std::string &userAccessID)
    -> Poco::JSON::Object::Ptr {
    if (userAccessID.empty()) {
        throw std::invalid_argument("AccessID is empty");
    }

    std::string plainAccessID = getPlainAccessIDFromBearerAuth(userAccessID);

    return getAPIAccessPlainID(plainAccessID);
}

auto ApiAccessID::getAPIAccessPlainID(const std::string &AccessID)
    -> Poco::JSON::Object::Ptr {
    if (!conn) {
        throw std::invalid_argument("No connection to the database!");
    }

    std::pair<uint64_t, std::string> AccessKPairHashed =
        parseAndHashPlainAccessID(AccessID);

    std::string query;
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

    unique_statement_t ssmt(conn->createStatement());

    unique_resultset_t results(ssmt->executeQuery(query));

    if (results->rowsCount() == 0) {
        throw std::invalid_argument("Access ID invalido");
    }

    if (!results->next()) {
        throw std::invalid_argument("Access ID invalido");
    }

    std::string accesskey = results->getString("token");

    if (accesskey != AccessKPairHashed.second) {
        throw std::invalid_argument("Access ID invalido");
    }

    Poco::JSON::Object::Ptr result = new Poco::JSON::Object;

    result->set("id", results->getUInt64("id"));
    result->set("token", accesskey);
    result->set("tokenable_id", results->getUInt64("tokenable_id"));
    // SQLString estava sendo convertido para alguma outra coisa pela Poco.
    // Necessário cast manual
    result->set("tokenable_type",
                std::string(results->getString("tokenable_type")));

    Poco::JSON::Array::Ptr abilities;
    try {
        Poco::JSON::Parser parser;
        abilities = parser.parse(std::string(results->getString("abilities")))
                        .extract<Poco::JSON::Array::Ptr>();
    } catch (const std::exception &e) {
        abilities = new Poco::JSON::Array;
        std::cerr << e.what() << '\n';
    }

    result->set("abilities", abilities);

    return result;
}

auto ApiAccessID::getPlainAccessIDFromBearerAuth(
    const std::string &UserAccessID) -> std::string {
    const std::string Bearer = "Bearer ";
    if (UserAccessID.size() < Bearer.size() ||
        UserAccessID.find_first_of(Bearer) != 0) {
        throw std::invalid_argument("AccessID is invalid");
    }

    std::string plainAccessID = UserAccessID.substr(Bearer.size());
    return plainAccessID;
}

auto ApiAccessID::parseAccessID(const std::string &AccessID)
    -> std::pair<uint64_t, std::string> {
    if (AccessID.empty()) {
        throw std::invalid_argument("API Access ID is empty");
    }

    std::size_t IdEnd = AccessID.find_first_of('|');

    std::string idPlainText;
    uint64_t iddb = 0;

    if (IdEnd == std::string::npos) {
        idPlainText = AccessID;
    } else {
        iddb = std::stoull(AccessID.substr(0, IdEnd));
        idPlainText = AccessID.substr(IdEnd + 1);
    }

    if (AccessID.empty()) {
        throw std::invalid_argument("API Access ID is empty");
    }

    return {iddb, idPlainText};
}

auto ApiAccessID::hashPlainAccessID(const std::string &PlainAccessID)
    -> std::string {
    Poco::Crypto::DigestEngine sha256("SHA256");

    sha256.update(PlainAccessID);

    return Poco::Crypto::DigestEngine::digestToHex(sha256.digest());
}

auto ApiAccessID::parseAndHashPlainAccessID(const std::string &PlainAccessID)
    -> std::pair<uint64_t, std::string> {
    std::pair<uint64_t, std::string> AccessKPair = parseAccessID(PlainAccessID);
    AccessKPair.second = hashPlainAccessID(AccessKPair.second);

    return AccessKPair;
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
