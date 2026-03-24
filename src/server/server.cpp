#include "server.hpp"

void Server::init()
{
    this->listener = socket(AF_INET, SOCK_STREAM, 0);
    if (this->listener < 0)
    {
        this->logger->error("socket create failed");
        exit(1);
    }

    int opt = 1;
    if (setsockopt(this->listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        this->logger->error("setsockopt SO_REUSEADDR failed");
        exit(1);
    }

    this->addr.sin_family = AF_INET;
    this->addr.sin_port = htons(this->port);
    addr.sin_addr.s_addr = inet_addr(this->host.c_str());
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        this->logger->error("bind failed");
        exit(1);
    }
    listen(listener, 1);
}

Server::Server(std::unique_ptr<BaseLogger> logger, const std::string *host, unsigned short *port)
    : logger(std::move(logger)),
      host(host ? *host : "127.0.0.1"),
      port(port ? *port : 8080),
      resolver(echoResolver) {}

Server::~Server()
{
    close(this->sock);
    close(this->listener);
}

void Server::sendTo(int sock, BufferView buffer)
{
    send(sock, buffer.data(), buffer.size(), 0);
}

void Server::setResolver(ServerResolver resolver)
{
    this->resolver = resolver;
}

void Server::run()
{
    this->logger->info("server starting");
    this->init();

    this->logger->info(std::format("listening on {}:{}", this->host, this->port));
    while (true)
    {
        this->sock = accept(listener, NULL, NULL);
        if (this->sock < 0)
        {
            this->logger->error("failed on accept new connection");
            exit(1);
        }

        switch (fork())
        {
        case -1:
            this->logger->error("fork failed");
            break;
        case 0:
            close(listener);
            while (true)
            {
                Buffer buffer;
                int BytesRead;
                BytesRead = recv(this->sock, buffer, 2048, 0);
                if (BytesRead <= 0)
                    break;
                buffer[BytesRead] = '\0';
                this->logger->debug(std::format("received {} bytes", BytesRead));
                this->logger->debug(std::format("message: {}", buffer));
                this->resolver(this->sock, buffer);
            }

            close(sock);
            _exit(0);
        default:
            close(sock);
        }
    }
}