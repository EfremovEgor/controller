#include <iostream>
#include <memory>
#include "commands.hpp"
#include "logger/logger.hpp"
#include "server/server.hpp"
#include "server/resolvers.hpp"
int main()
{
    BaseLogger serverLogger = BaseLogger(LogLevel::DEBUG);
    serverLogger.setName("Server");
    std::string host = "0.0.0.0";

    Server server = Server(std::make_unique<BaseLogger>(serverLogger), &host);
    CommandResolverMap mp{
        {"r2d2:test", [&server](int sock, BufferView buffer, Command command)
         {
             server.sendTo(sock, "GLOBAL COMMAND TEST");
         }}};

    auto resolver = getCommandResolver(mp);
    server.setResolver(resolver);
    server.run();
    return 0;
}