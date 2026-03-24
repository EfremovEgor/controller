#pragma once
#include <string>
#include <map>
#include <functional>
#include "server.hpp"
#include "../commands.hpp"

typedef std::map<std::string, std::function<void(int sock, BufferView buffer, Command command)>> CommandResolverMap;

inline ServerResolver getCommandResolver(CommandResolverMap mp)
{
    return [&mp](int sock, BufferView buffer)
    {
        auto str = std::string(buffer);
        auto end = str.find_last_not_of(" \t\n\r\f\v");

        auto key = (end == std::string::npos) ? "" : str.substr(0, end + 1);
        auto cmd = Command(key);

        if (!mp.contains(cmd.makeCommandBasis()))
            return;
        mp[key](sock, buffer, cmd);
    };
}
