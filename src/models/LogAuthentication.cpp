#include "../Database/CSql.hpp"
#include "LogAuthentication.hpp"

static void get_this_model_data(LogAuthentication &val,
                                unique_resultset_t &res) {
    val.partner_id =
        res->isNull("partner_id")
            ? std::nullopt
            : std::optional<uint64_t>(res->getUInt64("partner_id"));
    val.access_id = res->isNull("access_id")
                        ? std::nullopt
                        : std::optional<uint64_t>(res->getUInt64("access_id"));
    val.route_path =
        res->isNull("route_path")
            ? std::nullopt
            : std::optional<std::string>(res->getString("route_path"));
    val.sessao_uuid = res->getString("sessao_uuid");
    val.ip = res->getString("ip");
    val.host = res->getString("host");
    val.created_at =
        res->isNull("created_at")
            ? std::nullopt
            : std::optional<std::chrono::system_clock::time_point>(
                  CSql::string_to_system_clock(res->getString("created_at")));
    val.updated_at =
        res->isNull("updated_at")
            ? std::nullopt
            : std::optional<std::chrono::system_clock::time_point>(
                  CSql::string_to_system_clock(res->getString("updated_at")));
}

auto LogAuthentication::findByID(uint64_t fid) -> bool {
    auto sqlconn = std::reinterpret_pointer_cast<sql::Connection>(usingconn);

    auto select_statement = unique_prepstatement_t(sqlconn->prepareStatement(
        "SELECT * FROM log_authentications WHERE id = ?;"));

    select_statement->setUInt64(1, fid);

    auto res = unique_resultset_t(select_statement->executeQuery());

    if (!res->next()) {
        return false;
    }

    id = res->getUInt64("id");
    get_this_model_data(*this, res);

    return true;
}

auto LogAuthentication::findByPartner_id(uint64_t value) -> bool {
    auto sqlconn = std::reinterpret_pointer_cast<sql::Connection>(usingconn);

    auto select_statement = unique_prepstatement_t(sqlconn->prepareStatement(
        "SELECT * FROM log_authentications WHERE partner_id = ?;"));

    select_statement->setUInt64(1, value);

    auto res = unique_resultset_t(select_statement->executeQuery());

    if (!res->next()) {
        return false;
    }

    id = res->getUInt64("id");
    get_this_model_data(*this, res);

    return true;
}
auto LogAuthentication::findByAccess_id(uint64_t value) -> bool {
    auto sqlconn = std::reinterpret_pointer_cast<sql::Connection>(usingconn);

    auto select_statement = unique_prepstatement_t(sqlconn->prepareStatement(
        "SELECT * FROM log_authentications WHERE access_id = ?;"));

    select_statement->setUInt64(1, value);

    auto res = unique_resultset_t(select_statement->executeQuery());

    if (!res->next()) {
        return false;
    }

    id = res->getUInt64("id");
    get_this_model_data(*this, res);

    return true;
}
auto LogAuthentication::findBySessao_uuid(const std::string &value) -> bool {
    auto sqlconn = std::reinterpret_pointer_cast<sql::Connection>(usingconn);

    auto select_statement = unique_prepstatement_t(sqlconn->prepareStatement(
        "SELECT * FROM log_authentications WHERE sessao_uuid = ?;"));

    select_statement->setString(1, value);

    auto res = unique_resultset_t(select_statement->executeQuery());

    if (!res->next()) {
        return false;
    }

    id = res->getUInt64("id");
    get_this_model_data(*this, res);

    return true;
}
auto LogAuthentication::findByIp(const std::string &value) -> bool {
    auto sqlconn = std::reinterpret_pointer_cast<sql::Connection>(usingconn);

    auto select_statement = unique_prepstatement_t(sqlconn->prepareStatement(
        "SELECT * FROM log_authentications WHERE ip = ?;"));

    select_statement->setString(1, value);

    auto res = unique_resultset_t(select_statement->executeQuery());

    if (!res->next()) {
        return false;
    }

    id = res->getUInt64("id");
    get_this_model_data(*this, res);

    return true;
}
auto LogAuthentication::findByHost(const std::string &value) -> bool {
    auto sqlconn = std::reinterpret_pointer_cast<sql::Connection>(usingconn);

    auto select_statement = unique_prepstatement_t(sqlconn->prepareStatement(
        "SELECT * FROM log_authentications WHERE host = ?;"));

    select_statement->setString(1, value);

    auto res = unique_resultset_t(select_statement->executeQuery());

    if (!res->next()) {
        return false;
    }

    id = res->getUInt64("id");
    get_this_model_data(*this, res);

    return true;
}

static void
fill_prepared_statement_base_data(const LogAuthentication &val,
                                  unique_prepstatement_t &prepstmt) {
    val.partner_id ? prepstmt->setUInt64(1, val.partner_id.value())
                   : prepstmt->setNull(1, 0);
    val.access_id ? prepstmt->setUInt64(2, val.access_id.value())
                  : prepstmt->setNull(2, 0);
    val.route_path ? prepstmt->setString(3, val.route_path.value())
                   : prepstmt->setNull(3, 0);
    prepstmt->setString(4, val.sessao_uuid);
    prepstmt->setString(5, val.ip);
    prepstmt->setString(6, val.host);
    val.created_at
        ? prepstmt->setString(7, CSql::system_time_to_str(val.created_at))
        : prepstmt->setNull(7, 0);
    val.updated_at
        ? prepstmt->setString(8, CSql::system_time_to_str(val.updated_at))
        : prepstmt->setNull(8, 0);
}

void LogAuthentication::create() {
    auto sqlconn = std::reinterpret_pointer_cast<sql::Connection>(usingconn);

    auto create_statement = unique_prepstatement_t(sqlconn->prepareStatement(
        "INSERT INTO log_authentications (partner_id, access_id, route_path, "
        "sessao_uuid, ip, host, created_at, updated_at) VALUES "
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

auto LogAuthentication::update() -> int {
    auto sqlconn = std::reinterpret_pointer_cast<sql::Connection>(usingconn);

    updated_at = std::chrono::system_clock::now();

    auto update_statement = unique_prepstatement_t(sqlconn->prepareStatement(
        "UPDATE log_authentications SET partner_id = ?, access_id = ?, "
        "route_path = ?, sessao_uuid = ?, ip = ?, host = ?, created_at = ?, "
        "updated_at = ? "
        "WHERE id = ?;"));

    fill_prepared_statement_base_data(*this, update_statement);
    update_statement->setUInt64(9, id);

    return update_statement->executeUpdate();
}

void LogAuthentication::save() {
    if (id == 0) {
        create();
    } else {
        update();
    }
}
