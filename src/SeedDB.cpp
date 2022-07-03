#include "stdafx.hpp"
#include "SeedDB.hpp"
#include <Poco/StreamCopier.h>
#include <fstream>
#include <streambuf>

static void seedPartnersTable(unique_conn_t &conn) {
    std::fstream sqlfile("partners.sql", std::ios::in);

    if (!sqlfile.is_open()) {
        throw std::runtime_error("Partners file not open");
    }

    std::string sqlstring;

    sqlfile.seekg(0, std::ios::end);
    sqlstring.reserve(static_cast<size_t>(sqlfile.tellg()));
    sqlfile.seekg(0, std::ios::beg);

    Poco::StreamCopier::copyToString64(sqlfile, sqlstring);

    auto stmt = unique_statement_t(conn->createStatement());

    stmt->execute(sqlstring);
}

void SeedDB::runAllDatabaseSeeders() {
    unique_conn_t conn(CSql::instance().make_connection_cfg());

    conn->setAutoCommit(false);
    try {
        seedPartnersTable(conn);
        conn->commit();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        conn->rollback();
    }
}
