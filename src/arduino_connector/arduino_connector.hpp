#pragma once
#include "../server/resolvers.hpp"
#include "../server/server.hpp"
#include "../soundpad/sound_player.hpp"
#include <thread>
#include <vector>

class ArduinoConnector
{
private:
    std::thread reader_thread;
    std::unique_ptr<SoundPlayer> sound_player;

    static ArduinoConnector *instance;

    const char *SERIAL_PORT = "/dev/ttyUSB0";
    bool keepRunning = true;
    int baudRate = 9600;
    int serialFd;

    char buffer[256];
    char serialBuffer[256];
    char fifoBuffer[256];
    std::string serialLine = "";

    int setupSerial(const char *port, int baud);
    static void signalHandler(int sig);
    bool sendCommand(const char &cmd);
    void readerLoop();
    int getArg(const std::vector<std::string> &args, size_t index) const;

public:
    ~ArduinoConnector();
    ArduinoConnector();
    void play(const std::string &file)
    {
        sound_player->play(file);
    }
    ServerResolver getResolver(Server &server)
    {
        CommandResolverMap mp{
            {"r2d2:forward", [this, &server](int sock, BufferView buffer, Command command)
             {
                 forward(std::stoi(command.args[0]));
                 server.sendTo(sock, "forward" + command.args[0]);
             }},
            {"r2d2:stop", [this, &server](int sock, BufferView buffer, Command command)
             {
                 stop();
                 server.sendTo(sock, "stop");
             }},
            {"r2d2:backward", [this, &server](int sock, BufferView buffer, Command command)
             {
                 backward(std::stoi(command.args[0]));
                 server.sendTo(sock, "backward" + command.args[0]);
             }},
            {"r2d2:left", [this, &server](int sock, BufferView buffer, Command command)
             {
                 left(std::stoi(command.args[0]));
                 server.sendTo(sock, "left" + command.args[0]);
             }},
            {"r2d2:right", [this, &server](int sock, BufferView buffer, Command command)
             {
                 right(std::stoi(command.args[0]));
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

    void forward(const char &speed);
    void backward(const char &speed);
    void left(const char &speed);
    void right(const char &speed);
    void stop();
    void chaos();
    void greeting();
    void move_head();
    void onArduinoByteResponse(uint8_t byte);
};
