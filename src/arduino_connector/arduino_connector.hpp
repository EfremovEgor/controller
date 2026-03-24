#pragma once
#include "../server/resolvers.hpp"
#include "../server/server.hpp"

class ArduinoConnector
{
private:
    static ArduinoConnector *instance;

    const char *SERIAL_PORT = "/dev/ttyUSB0";
    bool keepRunning = true;
    int baudRate = B9600;
    int serialFd;

    char buffer[256];
    char serialBuffer[256];
    char fifoBuffer[256];
    std::string serialLine = "";

    int setupSerial(const char *port, int baud);
    static void signalHandler(int sig);
    bool sendCommand(std::string cmd);
    void readerLoop();
    void ArduinoConnector::onArduinoResponse(const std::string &response);
    int getArg(const std::vector<std::string> &args, size_t index) const;

public:
    ~ArduinoConnector();
    ArduinoConnector();
    void greeting();
    ServerResolver getResolver(Server &server)
    {
        CommandResolverMap mp{
            {"r2d2:forward", [this, &server](int sock, BufferView buffer, Command command)
             {
                 forward(command.args[0]);
                 server.sendTo(sock, "forward" + command.args[0]);
             }},
            {"r2d2:backward", [this, &server](int sock, BufferView buffer, Command command)
             {
                 backward(command.args[0]);
                 server.sendTo(sock, "backward" + command.args[0]);
             }},
            {"r2d2:left", [this, &server](int sock, BufferView buffer, Command command)
             {
                 sendCommand(command.args[0]);
                 server.sendTo(sock, "left" + command.args[0]);
             }},
            {"r2d2:right", [this, &server](int sock, BufferView buffer, Command command)
             {
                 sendCommand(command.args[0]);
                 server.sendTo(sock, "right" + command.args[0]);
             }},
            {"r2d2:chaos", [this, &server](int sock, BufferView buffer, Command command)
             {
                 chaos();
                 server.sendTo(sock, "chaos");
             }},
            {"r2d2:greeting", [this, &server](int sock, BufferView buffer, Command command)
             {
                 greeting();
                 server.sendTo(sock, "Greetings");
             }},
            {"r2d2:move_head", [this, &server](int sock, BufferView buffer, Command command)
             {
                 move_head();
                 server.sendTo(sock, "move_head");
             }}};
        return getCommandResolver(mp);
    }

    void forward(const std::string &speed);
    void backward(const std::string &speed);
    void left(const std::string &speed);
    void right(const std::string &speed);
    void move_head();
    void chaos();
};
