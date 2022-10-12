#include "AuthMiddleware.hpp"
#include "../../vendor/cppapiframework/src/utils/ResultMacros.hpp"

namespace {

auto responseMiddlwareUnauth(Pistache::Http::ResponseWriter &response) {
    response.send(Pistache::Http::Code::Unauthorized);
    return false;
}

auto responseMiddlwareInternalError(Pistache::Http::ResponseWriter &response) {
    response.send(Pistache::Http::Code::Internal_Server_Error);
    return false;
}

} // namespace

bool AuthMiddleware::auth(Pistache::Http::Request &request,
                          Pistache::Http::ResponseWriter &response) {
    auto auth =
        request.headers().tryGet<Pistache::Http::Header::Authorization>();

    auto peer = response.getPeer();

    auto sessao =
        std::static_pointer_cast<APISession>(peer->tryGetData(AUTH_DATA_NAME));

    if (!auth) {
        if (sessao) {
            sessao->logout();
        }

        return true;
    }

    try {
        if (!sessao) {
            sessao = std::make_shared<APISession>();
            peer->putData(AUTH_DATA_NAME, sessao);
        }

        shared_conn_t connection = CSql::instance().make_shr_connection_cfg();

        auto conn = std::reinterpret_pointer_cast<sql::Connection>(connection);
        ApiAccessID accessid(conn);

        sessao->user =
            EXPECT_RESULT_E(accessid.getAPIAccessFromBearerAuth(auth->value()),
                            responseMiddlwareUnauth(response));

    } catch (...) {
        return responseMiddlwareInternalError(response);
    }

    return true;
}
