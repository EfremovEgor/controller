#pragma once

#include <string>
#include <map>
#include <functional>
#include "server.hpp"

#include "../commands.hpp"

typedef std::map<std::string, std::function<void(int sock, BufferView buffer, Command command)>> CommandResolverMap;

inline ServerResolver getCommandResolver(CommandResolverMap mp)
{
    return [mp = std::move(mp)](int sock, BufferView buffer) mutable
    {
        auto str = std::string_view(buffer);
        auto end = str.find_last_not_of(" \t\n\r\f\v");
        auto key = (end == std::string_view::npos) ? std::string{} : std::string(str.substr(0, end + 1));

        auto cmd = Command(key);
        const std::string basis = cmd.makeCommandBasis();

        auto it = mp.find(basis);
        if (it == mp.end())
            return;

        it->second(sock, buffer, cmd);
    };
}