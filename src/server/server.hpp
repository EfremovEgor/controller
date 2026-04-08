#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <functional>
#include <string_view>
#include <map>
#include <sys/socket.h>

#include "../logger/logger.hpp"

typedef char Buffer[2048];
typedef std::string_view BufferView;
typedef std::function<void(int sock, BufferView buffer)> ServerResolver;

inline void echoResolver(int sock, BufferView buffer)
{
    send(sock, buffer.data(), buffer.size(), 0);
}

class Server
{
private:
    void init();
    std::unique_ptr<BaseLogger> logger;
    unsigned short port;
    std::string host;
    int sock, listener;
    struct sockaddr_in addr;
    ServerResolver resolver;

public:
    Server(std::unique_ptr<BaseLogger> logger, std::optional<std::string> host = std::nullopt,
           std::optional<unsigned short> port = std::nullopt);
    ~Server();
    void sendTo(int sock, BufferView buffer);
    void setResolver(ServerResolver resolver);
    void run();
};