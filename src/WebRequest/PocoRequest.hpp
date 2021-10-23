/**
 *@file PocoRequest.hpp
 * @author Fabio Rossini Sluzala ()
 * @brief Helper para usar o cliente HTTP da biblioteca poco
 * @version 0.1
 * @date 2021-08-04
 * @todo funciona porém não está padronizado
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once
#ifndef POCOREQUEST_HPP
#define POCOREQUEST_HPP
#include "../stdafx.hpp"

#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <string>
#include <string_view>
#include <vector>

class PocoRequest {
  public:
    Poco::Net::HTTPResponse responseraw;

    typedef std::vector<std::pair<std::string, std::string>> formdata_t;

    static auto http_get_json(const std::string &fullurl,
                              const formdata_t &queryparams = {})
        -> Poco::JSON::Object::Ptr;
    auto http_get_rawstream(const std::string &fullurl,
                            const formdata_t &queryparams = {})
        -> std::stringstream;
    static auto http_post_json(const std::string &fullurl,
                               const formdata_t &formdata)
        -> Poco::JSON::Object::Ptr;
};

#endif
