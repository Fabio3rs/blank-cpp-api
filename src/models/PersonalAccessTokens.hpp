#pragma once

#include "../stdafx.hpp"
#include <chrono>
#include <memory>
#include <optional>

class PersonalAccessTokens {
    std::shared_ptr<GenericDBConnection> usingconn;

    auto update() -> int;
    void create();

  public:
    /* Model fields */
    uint64_t id{0};
    uint64_t tokenable_id{0};
    std::string tokenable_name;
    std::string name;
    std::string token;
    std::optional<std::string> abilities;
    std::optional<std::chrono::system_clock::time_point> last_used_at;
    std::optional<std::chrono::system_clock::time_point> created_at;
    std::optional<std::chrono::system_clock::time_point> updated_at;

    /* Functions */
    auto empty() const -> bool { return id == 0; }

    auto findByID(uint64_t fid) -> bool;
    void save();

    PersonalAccessTokens(std::shared_ptr<GenericDBConnection> conn)
        : usingconn(conn) {}
};
