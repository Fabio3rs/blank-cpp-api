/**
 *@file CWelcomeController.hpp
 * @author Fabio Rossini Sluzala ()
 * @brief Este controller apenas serve para fornecer a rota / da API
 * @version 0.1
 * @date 2021-08-04
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once
#ifndef CWelcomeController_hpp
#define CWelcomeController_hpp

#include "../WebInterface/webstdafx.hpp"

class CWelcomeController : public CController {
    /**
     *@brief Rota / fornece informações sobre se a API está no ar ou
     *"sucesso":false em caso de algum problema
     *
     * @param request objeto da requisição do pistache
     * @param response objeto de resposta do pistache
     */
    static void index_route(const Pistache::Rest::Request &request,
                            Pistache::Http::ResponseWriter response);

  public:
    void register_routes(const std::string &base,
                         Pistache::Rest::Router &router) override;

    CWelcomeController() = default;
};

#endif
