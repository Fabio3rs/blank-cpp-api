#include "PersonalAccessTokens.hpp"
#include "../Database/CSql.hpp"

auto PersonalAccessTokens::findByID(uint64_t fid) -> bool {
    auto sqlconn = std::reinterpret_pointer_cast<sql::Connection>(usingconn);

    auto select_statement = unique_prepstatement_t(sqlconn->prepareStatement(
        "SELECT * FROM personal_access_tokens WHERE id = ?;"));

    select_statement->setUInt64(1, fid);

    auto res = unique_resultset_t(select_statement->executeQuery());

    if (!res->next()) {
        return false;
    }

    id = res->getUInt64("id");
    tokenable_id = res->getUInt64("tokenable_id");
    tokenable_name = res->getString("tokenable_name");
    name = res->getString("name");
    token = res->getString("token");
    abilities = res->isNull("abilities")
                    ? std::nullopt
                    : std::optional<std::string>(res->getString("abilities"));
    last_used_at =
        res->isNull("last_used_at")
            ? std::nullopt
            : std::optional<std::chrono::system_clock::time_point>(
                  CSql::string_to_system_clock(res->getString("last_used_at")));
    created_at =
        res->isNull("created_at")
            ? std::nullopt
            : std::optional<std::chrono::system_clock::time_point>(
                  CSql::string_to_system_clock(res->getString("created_at")));
    updated_at =
        res->isNull("updated_at")
            ? std::nullopt
            : std::optional<std::chrono::system_clock::time_point>(
                  CSql::string_to_system_clock(res->getString("updated_at")));

    return true;
}

static void
fill_prepared_statement_base_data(const PersonalAccessTokens &val,
                                  unique_prepstatement_t &prepstmt) {
    prepstmt->setUInt64(1, val.tokenable_id);
    prepstmt->setUInt64(1, val.tokenable_id);
    prepstmt->setString(2, val.tokenable_name);
    prepstmt->setString(3, val.name);
    prepstmt->setString(4, val.token);
    val.abilities ? prepstmt->setString(5, val.abilities.value())
                  : prepstmt->setNull(5, 0);
    val.last_used_at
        ? prepstmt->setString(6, CSql::system_time_to_str(val.last_used_at))
        : prepstmt->setNull(6, 0);
    val.created_at
        ? prepstmt->setString(7, CSql::system_time_to_str(val.created_at))
        : prepstmt->setNull(7, 0);
    val.updated_at
        ? prepstmt->setString(8, CSql::system_time_to_str(val.updated_at))
        : prepstmt->setNull(8, 0);
}

void PersonalAccessTokens::create() {
    auto sqlconn = std::reinterpret_pointer_cast<sql::Connection>(usingconn);

    auto create_statement = unique_prepstatement_t(sqlconn->prepareStatement(
        "INSERT INTO personal_access_tokens (tokenable_id, tokenable_name, "
        "name, token, abilities, last_used_at, created_at, updated_at) VALUES "
        "(?, ?, ?, ?, ?, ?, ?, ?)"));

    created_at = std::chrono::system_clock::now();
    updated_at = created_at;
    fill_prepared_statement_base_data(*this, create_statement);

    create_statement->execute();

    auto stmt = unique_statement_t(sqlconn->createStatement());
    auto res = unique_resultset_t(
        stmt->executeQuery("SELECT LAST_INSERT_ID() AS id;"));

    if (!res->next()) {
        throw std::runtime_error("Fail to create register");
    }

    id = res->getUInt64("id");
}

auto PersonalAccessTokens::update() -> int {
    auto sqlconn = std::reinterpret_pointer_cast<sql::Connection>(usingconn);

    updated_at = std::chrono::system_clock::now();

    auto update_statement = unique_prepstatement_t(sqlconn->prepareStatement(
        "UPDATE personal_access_tokens SET tokenable_id = ?, tokenable_name = "
        "?, name = ?, token = ?, abilities = ?, last_used_at = ?, created_at = "
        "?, updated_at = ? "
        "WHERE id = ?;"));

    fill_prepared_statement_base_data(*this, update_statement);
    update_statement->setUInt64(9, id);

    return update_statement->executeUpdate();
}

void PersonalAccessTokens::save() {
    if (id == 0) {
        create();
    } else {
        update();
    }
}
