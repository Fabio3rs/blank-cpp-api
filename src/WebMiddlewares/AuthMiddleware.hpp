#pragma once

#include "../stdafx.hpp"
#include "Authorization/APISession.hpp"
#include "Authorization/ApiAccessID.hpp"
#include <memory>
#include <pistache/http_defs.h>
#include <pistache/http_header.h>
#include <pistache/peer.h>
#include <pistache/router.h>

class AuthMiddleware {

  public:
    static constexpr auto AUTH_DATA_NAME = "AuthDataInfo";

    static bool auth(Pistache::Http::Request &request,
                     Pistache::Http::ResponseWriter &response);
};
