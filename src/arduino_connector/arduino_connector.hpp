#pragma once
#include "../server/resolvers.hpp"
#include "../server/server.hpp"
class ArduinoConnector
{
private:
public:
    ArduinoConnector();
    void greeting();
    ServerResolver getResolver(Server &server)
    {
        CommandResolverMap mp{
            {"r2d2:forward", [&server](int sock, BufferView buffer, Command command) {}},
            {"r2d2:backward", [this, &server](int sock, BufferView buffer, Command command) {

             }},
            {"r2d2:left", [this, &server](int sock, BufferView buffer, Command command) {

             }},
            {"r2d2:right", [this, &server](int sock, BufferView buffer, Command command) {

             }},
            {"r2d2:chaos", [this, &server](int sock, BufferView buffer, Command command) {

             }},
            {"r2d2:greeting", [this, &server](int sock, BufferView buffer, Command command)
             {
                 this->greeting();
                 server.sendTo(sock, "Greetings");
             }},
            {"r2d2:move_head", [this, &server](int sock, BufferView buffer, Command command) {

             }}};
        return getCommandResolver(mp);
    }
};
