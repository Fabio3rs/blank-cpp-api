#pragma once

#include "../stdafx.hpp"
#include <chrono>
#include <memory>
#include <optional>

class LogAuthentication {
    std::shared_ptr<GenericDBConnection> usingconn;

    auto update() -> int;
    void create();

  public:
    /* Model fields */
    uint64_t id{0};
    std::optional<uint64_t> partner_id;
    std::optional<uint64_t> access_id;
    std::optional<std::string> route_path;
    std::string sessao_uuid;
    std::string ip;
    std::string host;
    std::optional<std::chrono::system_clock::time_point> created_at;
    std::optional<std::chrono::system_clock::time_point> updated_at;

    /* Functions */
    auto empty() const -> bool { return id == 0; }

    auto findByPartner_id(uint64_t value) -> bool;
    auto findByAccess_id(uint64_t value) -> bool;
    auto findBySessao_uuid(const std::string &value) -> bool;
    auto findByIp(const std::string &value) -> bool;
    auto findByHost(const std::string &value) -> bool;

    auto findByID(uint64_t fid) -> bool;
    void save();

    LogAuthentication(std::shared_ptr<GenericDBConnection> conn)
        : usingconn(conn) {}
};
