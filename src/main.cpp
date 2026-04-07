#include <iostream>
#include <memory>
#include "commands.hpp"
#include "logger/logger.hpp"
#include "server/server.hpp"
#include "server/resolvers.hpp"
#include "arduino_connector/arduino_connector.hpp"

int main()
{
    // BaseLogger serverLogger = BaseLogger(LogLevel::DEBUG);
    // serverLogger.setName("Server");
    // std::string host = "0.0.0.0";

    // Server server = Server(std::make_unique<BaseLogger>(serverLogger), &host);

    std::cout << "Hello";
    ArduinoConnector ac = ArduinoConnector();

    // server.setResolver(ac.getResolver(server));
    // server.run();
    return 0;
}