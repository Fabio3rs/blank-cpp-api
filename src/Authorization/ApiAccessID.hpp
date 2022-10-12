/**
 *@file ApiAccessID.hpp
 * @author Fabio Rossini Sluzala ()
 * @brief Consulta chaves de acesso a API (API Access ID) no banco  de dados
 * @version 0.1
 * @date 2021-07-26
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once
#ifndef API_ACCESSID_HPP

#include "../stdafx.hpp"
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <string>
#include <utility>

struct PersonalAccess {
    uint64_t id;
    uint64_t tokenable_id;
    std::string token;
    std::string tokenable_type;
    Poco::JSON::Array::Ptr abilities;
};

class ApiAccessID {
    std::shared_ptr<sql::Connection> conn;

  public:
    struct nullOk {};

    using result_t = utils::Result<Poco::JSON::Object::Ptr, std::string>;
    using parse_t =
        utils::Result<std::pair<uint64_t, std::string>, std::string>;
    using compare_t = utils::Result<nullOk, std::string>;
    using authres_t = utils::Result<PersonalAccess, std::string>;

    auto safe_hash_cmp(const std::string &hash_a, const std::string &hash_b)
        -> compare_t;

    /**
     *@brief Consulta a API Access ID diretamente do objeto request
     *
     * @param request objeto de requisição
     * @return Poco::JSON::Object::Ptr resultado em json exemplo
     *{
     *   "id": 1,
     *   "token": "chave hasheada",
     *   "tokenable_id": 10,
     *   "tokenable_type": "User",
     *   "abilities": "[\"admin\"]"
     *}
     */
    auto getAPIAuthorization(const Pistache::Rest::Request &request)
        -> authres_t;

    /**
     *@brief Consulta a API Access ID pelo conteúdo da header Authorization para
     *uma chave do tipo "Bearer" algo similar a "Bearer 1|conteudorandômico"
     *
     * @param userAccessID Chave de autenticação "Bearer 1|conteudorandômico"
     * @return Poco::JSON::Object::Ptr resultado em json exemplo
     *{
     *   "id": 1,
     *   "token": "chave hasheada",
     *   "tokenable_id": 10,
     *   "tokenable_type": "User",
     *   "abilities": "[\"admin\"]"
     *}
     */
    auto getAPIAccessFromBearerAuth(const std::string &userAccessID)
        -> authres_t;

    void makeAccessIdSqlSelect(
        std::pair<unsigned long, std::basic_string<char>> &AccessKPairHashed,
        std::string &query);

    authres_t getApiAccessIdFromIdHash(
        std::pair<unsigned long, std::basic_string<char>> &AccessKPairHashed);

    auto getAPIAccessPlainID(const std::string &AccessID) -> authres_t;

    /**
     *@brief Get the Plain Access ID From Bearer Auth object
     *
     * @param UserAccessID Chave de autenticação completa "Bearer
     *1|conteudorandômico"
     * @return std::string resultado "1|conteudorandômico"
     */
    static auto getPlainAccessIDFromBearerAuth(const std::string &UserAccessID)
        -> utils::Result<std::string, std::string>;

    /**
     *@brief Separa o id da chave do conteúdo secreto da chave
     *
     * @param AccessID "1|conteudorandômico"
     * @return std::pair<uint64_t, std::string> { 1, "conteudorandômico" }
     */
    static auto parseAccessID(const std::string &AccessID) -> parse_t;

    /**
     *@brief hash chave secreta [atualmente usando SHA256]
     *
     * @param PlainAccessID "conteudorandômico"
     * @return std::string hash
     *"3987e286f3364d6e631ce7651052c5e7b7264663305909407527f884c25475e1"
     */
    static auto hashPlainAccessID(const std::string &PlainAccessID)
        -> std::string;

    /**
     *@brief Separa o id da chave do conteúdo secreto da chave e hasheia
     *
     * @param PlainAccessID "1|conteudorandômico"
     * @return std::pair<uint64_t, std::string> { 1,
     *"3987e286f3364d6e631ce7651052c5e7b7264663305909407527f884c25475e1" }
     */
    static auto parseAndHashPlainAccessID(const std::string &PlainAccessID)
        -> parse_t;

    /**
     *@brief Create a table mysql sql query
     *
     * @return std::string sql
     */
    static auto create_table_mysql() -> std::string;

    /**
     *@brief Construct a new Api Access ID object
     *
     * @param nconn conexão ao banco de dados
     */
    explicit ApiAccessID(std::shared_ptr<sql::Connection> nconn)
        : conn(std::move(nconn)) {}
};

#endif
