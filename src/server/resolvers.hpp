#pragma once
#include <string>
#include <map>
#include <functional>
#include "server.hpp"
#include "../commands.hpp"

typedef std::map<std::string, std::function<void(int sock, BufferView buffer, Command command)>> CommandResolverMap;

inline ServerResolver getCommandResolver(CommandResolverMap mp)
{
    // Копируем map внутрь лямбды, чтобы он жил долго
    return [mp](int sock, BufferView buffer) mutable
    {
        auto str = std::string(buffer);

        // Удаляем пробелы и переносы строк в конце
        auto end = str.find_last_not_of(" \t\n\r\f\v");
        if (end != std::string::npos)
        {
            str = str.substr(0, end + 1);
        }
        else
        {
            str = "";
        }

        if (str.empty())
            return;

        auto cmd = Command(str);
        std::string basis = cmd.makeCommandBasis(); // Например, "r2d2:backward"

        // Ищем resolver по БАЗОВОМУ имени команды
        auto it = mp.find(basis);

        if (it != mp.end())
        {
            // Вызываем найденный resolver
            // Передаем ему оригинальный буфер и распарсенный объект Command (в котором уже есть args)
            try
            {
                it->second(sock, buffer, cmd);
            }
            catch (const std::exception &e)
            {
                // На случай ошибок внутри самого обработчика
                std::cerr << "Error in resolver for " << basis << ": " << e.what() << std::endl;
            }
        }
        else
        {
            // Команда не найдена
            std::cerr << "Unknown command basis: " << basis << std::endl;
        }
    };
}