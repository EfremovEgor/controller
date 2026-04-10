#include <iostream>
#include <memory>
#include "commands.hpp"
#include "logger/logger.hpp"
#include "server/server.hpp"
#include "server/resolvers.hpp"
#include "arduino_connector/arduino_connector.hpp"

int main()
{
    BaseLogger serverLogger = BaseLogger(LogLevel::DEBUG);
    serverLogger.setName("Server");
    std::string host = "127.0.0.1";

    Server server = Server(std::make_unique<BaseLogger>(serverLogger), &host);

    std::cout << "Hello" << std::endl;
    ArduinoConnector ac = ArduinoConnector();
    // ac.forward(12);
    server.setResolver(ac.getResolver(server));
    server.run();
    // ac.play("/home/nikita/MyProjects/R2D2/controller/src/soundpad/sounds/stop.mp3");
    // sleep(5);
    // sleep(2);
    // std::cout << "Forward" << std::endl;
    // ac.forward(15);
    // sleep(2);
    // std::cout << "Left" << std::endl;
    // ac.left(15);
    // sleep(2);
    // std::cout << "Right" << std::endl;
    // ac.right(15);
    // sleep(2);
    // std::cout << "Backward" << std::endl;
    // ac.backward(15);
    // sleep(2);
    // server.run();
    return 0;
}