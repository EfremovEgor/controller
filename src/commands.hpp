#pragma once
#include <iostream>
#include <vector>

class Command
{
public:
    std::string domain;
    std::string command;
    std::vector<std::string> args;
    const std::string makeCommandBasis()
    {
        return this->domain + ":" + this->command;
    }
    Command(std::string rawCommand)
    {
        size_t posColon = rawCommand.find(':');
        size_t posPipe = rawCommand.find('|');

        this->domain = (posColon != std::string::npos)
                           ? rawCommand.substr(0, posColon)
                           : rawCommand;

        size_t startCmd = (posColon != std::string::npos) ? posColon + 1 : 0;
        size_t endCmd = (posPipe != std::string::npos) ? posPipe : rawCommand.length();
        this->command = rawCommand.substr(startCmd, endCmd - startCmd);

        if (posPipe != std::string::npos)
        {
            std::string rest = rawCommand.substr(posPipe + 1);

            size_t posPrev = 0;
            size_t posComma;
            while ((posComma = rest.find(',', posPrev)) != std::string::npos)
            {
                std::string arg = rest.substr(posPrev, posComma - posPrev);
                arg.erase(0, arg.find_first_not_of(" \t"));
                arg.erase(arg.find_last_not_of(" \t") + 1);
                this->args.push_back(arg);
                posPrev = posComma + 1;
            }
            std::string lastArg = rest.substr(posPrev);
            lastArg.erase(0, lastArg.find_first_not_of(" \t"));
            lastArg.erase(lastArg.find_last_not_of(" \t") + 1);
            this->args.push_back(lastArg);
        }
    }
};
