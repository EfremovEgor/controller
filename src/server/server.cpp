#include <thread>
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
        // Принимаем соединение
        int client_sock = accept(listener, NULL, NULL);
        if (client_sock < 0)
        {
            this->logger->error("failed on accept new connection");
            continue; // Не выходим из цикла при ошибке accept
        }

        // Запускаем обработку в отдельном ПОТОКЕ
        std::thread([this, client_sock]()
                    {
            // Важно: закрываем listener в потоке, если он был наследован (хотя тут он не передан, но на всякий случай)
            // В твоем случае listener остается только в главном потоке
            
            Buffer buffer;
            int bytesRead;
            
            // Цикл чтения из сокета
            while ((bytesRead = recv(client_sock, buffer, sizeof(buffer) - 1, 0)) > 0)
            {
                buffer[bytesRead] = '\0';
                
                this->logger->debug(std::format("received {} bytes", bytesRead));
                this->logger->debug(std::format("message: {}", std::string(buffer)));
                
                // Вызываем резолвер. 
                // Так как это поток, а не форк, у нас есть доступ к тем же объектам,
                // что и в main (через захват this->resolver, который ссылается на общие данные)
                this->resolver(client_sock, BufferView(buffer, bytesRead));
            }

            close(client_sock);
            this->logger->info("Client disconnected"); })
            .detach(); // Отсоединяем поток, чтобы он жил самостоятельно

        // ВАЖНО: В продакшене лучше использовать pool потоков, а не detach,
        // чтобы не создать миллион потоков при DDOS-атаке.
        // Но для учебного проекта R2D2 это допустимо.
    }
}