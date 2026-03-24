#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <syslog.h>

#include "arduino_connector.hpp"

ArduinoConnector *ArduinoConnector::instance = nullptr;

ArduinoConnector::ArduinoConnector()
{
    instance = this;
    serialFd = setupSerial(SERIAL_PORT, baudRate);
    if (serialFd < 0)
    {
        syslog(LOG_ERR, "Ошибка инициализации UART");
        closelog();
    }
    else
    {
        openlog("arduino_daemon", LOG_PID, LOG_DAEMON);
        syslog(LOG_INFO, "Запуск демона управления Arduino");

        signal(SIGINT, signalHandler);  // Ctrl+C
        signal(SIGTERM, signalHandler); // kill
    }
}

ArduinoConnector::~ArduinoConnector()
{
    instance = nullptr;
    close(serialFd);
    syslog(LOG_INFO, "Демон остановлен");
    closelog();
}

int ArduinoConnector::setupSerial(const char *port, int baud)
{
    int fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        syslog(LOG_ERR, "Не удалось открыть UART-порт %s", port);
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);

    cfsetspeed(&options, baud);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag |= CREAD | CLOCAL;

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;

    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 5;

    tcsetattr(fd, TCSANOW, &options);
    tcflush(fd, TCIOFLUSH);
    return fd;
}

void ArduinoConnector::greeting()
{
}

void ArduinoConnector::signalHandler(int sig)
{
    if (instance)
    {
        syslog(LOG_INFO, "Получен сигнал %d. Завершение...", sig);
        instance->keepRunning = false;
    }
}

bool ArduinoConnector::sendCommand(std::string cmd)
{
    if (serialFd < 0)
    {
        syslog(LOG_ERR, "Порт не открыт");
        return false;
    }

    if (!cmd.empty() && cmd.back() != '\n')
    {
        cmd += '\n';
    }

    ssize_t sent = write(serialFd, &cmd, cmd.size());
    tcdrain(serialFd);

    return true;
}

// Получение и воспроизведение сигналов от ардуино

void ArduinoConnector::readerLoop()
{
    std::string buffer;
    char ch;

    while (keepRunning && serialFd >= 0)
    {
        ssize_t n = read(serialFd, &ch, 1);
        if (n > 0)
        {
            if (ch == '\n' || ch == '\r')
            {
                if (!buffer.empty())
                {
                    onArduinoResponse(buffer);
                    buffer.clear();
                }
            }
            else
            {
                buffer += ch;
            }
        }
        else
        {
            usleep(10000);
        }
    }
}

void ArduinoConnector::onArduinoResponse(const std::string &response)
{
    syslog(LOG_INFO, "Arduino: %s", response.c_str());
    std::cout << "Arduino: " << response << std::endl;
    std::cout.flush();

    if (response == "7")
    {
        int ret = system("./venv/bin/python3 sound.py stop.mp3 &");
        if (ret == 0)
            syslog(LOG_INFO, "Звук объезда воспроизведён");
    }
}

// Конкретные комманды

void ArduinoConnector::forward(const std::string &speed)
{
    sendCommand("f" + speed);
}

void ArduinoConnector::backward(const std::string &speed)
{
    sendCommand("b" + speed);
}

void ArduinoConnector::left(const std::string &speed)
{
    sendCommand("l" + speed);
}

void ArduinoConnector::right(const std::string &speed)
{
    sendCommand("r" + speed);
}

void ArduinoConnector::move_head()
{
    sendCommand("move_head");
}

void ArduinoConnector::chaos()
{
    sendCommand("chaos");
}

int ArduinoConnector::getArg(const std::vector<std::string> &args, size_t index) const
{
    if (index > args.size())
    {
        syslog(LOG_ERR, "Выход за пределы массива");
        return 0;
    }
    return std::stoi(args[index]);
}