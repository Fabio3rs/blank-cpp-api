#include "RegisterControllers.hpp"
#include "WebControllers/CWelcomeController.hpp"
#include "utils/CConfig.hpp"

/**
 *@brief Instalação dos web controllers no gerenciador do endpoint HTTP
 *
 * @param endp objeto endpoint
 */
void registerPistacheWebControllers(CPistacheEndpoint &endp) {
    auto &conf = CConfig::config();
    bool enable_route_input_hash_check =
        (std::stoi(conf.at("VERIFICAR_HASH_ROTAS", "1")) != 0);

    // API Controllers setup
    std::unique_ptr<CWelcomeController> control(
        std::make_unique<CWelcomeController>());

    control->enableInputHashCheck(enable_route_input_hash_check);

    endp.register_controller(std::move(control));
}

/**
 *@brief Inicia o webservice do endpoint HTTP em uma porta definida no CConfig
 *ou 9000
 *
 * @param endp objeto endpoint
 */
void initPistacheWebService(CPistacheEndpoint &endp) {
    auto &conf = CConfig::config();

    int config_port = 9000;
    size_t server_threads = 2;

    {
        std::string API_LISTEN_PORT = conf.at("API_LISTEN_PORT", "9000");
        try {
            config_port = std::stoi(API_LISTEN_PORT, nullptr, 0);
        } catch (const std::exception &e) {
            std::cerr << e.what() << '\n';
        }
    }

    std::cout << "Starting server on port " << config_port << " with "
              << server_threads << " threads" << std::endl;

    Pistache::Port port(static_cast<uint16_t>(config_port));
    Pistache::Address addr(Pistache::Ipv4::any(), port);
    endp.init(addr, server_threads);
    endp.start();
}
