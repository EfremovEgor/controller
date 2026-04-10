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
    sound_player = std::make_unique<SoundPlayer>();
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
        reader_thread = std::thread(&ArduinoConnector::readerLoop, this);
        syslog(LOG_INFO, "Поток readerLoop запущен");
    }
}

ArduinoConnector::~ArduinoConnector()
{

    keepRunning = false;

    if (reader_thread.joinable())
    {
        syslog(LOG_INFO, "Ожидание завершения readerLoop...");
        reader_thread.join();
        syslog(LOG_INFO, "readerLoop завершён");
    }

    if (sound_player)
    {
        sound_player->stop();
    }

    if (serialFd >= 0)
    {
        close(serialFd);
        serialFd = -1;
    }
    instance = nullptr;

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

void ArduinoConnector::signalHandler(int sig)
{
    if (instance)
    {
        syslog(LOG_INFO, "Получен сигнал %d. Завершение...", sig);
        instance->keepRunning = false;
    }
}

bool ArduinoConnector::sendCommand(const char &cmd)
{
    if (serialFd < 0)
    {
        syslog(LOG_ERR, "Порт не открыт");
        return false;
    }

    ssize_t sent = write(serialFd, &cmd, 1);
    if (sent < 0)
    {
        return false;
    }

    tcdrain(serialFd);
    return true;
}

void ArduinoConnector::readerLoop()
{
    syslog(LOG_INFO, "Binary readerLoop started");
    uint8_t byte;

    while (keepRunning && serialFd >= 0)
    {
        ssize_t n = read(serialFd, &byte, 1);

        if (n > 0)
        {
            onArduinoByteResponse(byte);
        }
        else if (n < 0)
        {
            usleep(1000);
        }
        else
        {
            syslog(LOG_WARNING, "UART disconnected");
            break;
        }
    }
    syslog(LOG_INFO, "Binary readerLoop stopped");
}

void ArduinoConnector::onArduinoByteResponse(uint8_t byte)
{
    std::cout << "Byte: " << (int)byte << std::endl;
    std::cout.flush();

    syslog(LOG_DEBUG, "Received byte: %d", byte);

    switch (byte)
    {
    case 0:
        // syslog(LOG_INFO, "Command OK");
        break;

    case 255:
        syslog(LOG_ERR, "Command Error from Arduino");
        break;
    case 0xE0:
        syslog(LOG_WARNING, "Obsracle");
        if (sound_player)
        {
            sound_player->play("stop.mp3"); // <- Указать абсолютный путь к файлу
        }
        break;
    default:
        break;
    }
}

// Конкретные комманды

void ArduinoConnector::forward(const char &speed)
{
    sound_player->play("/home/nikita/MyProjects/R2D2/controller/src/soundpad/sounds/stop.mp3");
    char res = 0b00010000 | (speed & 0x0F);
    sendCommand(res);
}

void ArduinoConnector::backward(const char &speed)
{
    sound_player->play("/home/nikita/MyProjects/R2D2/controller/src/soundpad/sounds/stop.mp3");
    char res = 0b00100000 | (speed & 0x0F);
    sendCommand(res);
}

void ArduinoConnector::left(const char &speed)
{
    char res = 0b00110000 | (speed & 0x0F);
    sendCommand(res);
}

void ArduinoConnector::right(const char &speed)
{
    char res = 0b01000000 | (speed & 0x0F);
    sendCommand(res);
}

void ArduinoConnector::stop()
{
    char res = 0b01010000;
    sendCommand(res);
}

void ArduinoConnector::chaos()
{
    char res = 0b01110000;
    sendCommand(res);
}

void ArduinoConnector::greeting()
{
    char res = 0b10000000;
    sendCommand(res);
}

void ArduinoConnector::move_head()
{
    char res = 0b10010000;
    sendCommand(res);
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