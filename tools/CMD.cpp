#include "CMD.hpp"

const void CMD::readLine(const std::string& command)
{
    auto lines = split(command);
    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].empty()) {
            throw std::invalid_argument("ERROR: please don't use extra white spaces!");
        }

        if (lines[i] == CLEAR_SCREEN) {
            clearScreen();
        } else if (lines[i] == CLOSE_TERMINAL) {
            goBack();
        }

    }
}

const void CMD::goBack()
{
    if (partitionName.empty()) {
        killTerminal();
    }
}


const bool CMD::createPartition(const std::string& path)
{
    if (path.empty()) return false;

    return true;
}

const bool CMD::removePartition(const std::string& path)
{
    if (path.empty()) return false;

    return true;
}

const void CMD::listPartition(const std::string& path)
{
}

std::vector<std::string> CMD::split(const std::string& command)
{
    std::vector<std::string> lines;
    int lastPoint = 0;
    for (int i = 0; i < command.length(); i++) {
        if (command[i] == ' ') {
            lines.push_back(command.substr(lastPoint, i - lastPoint));
            lastPoint = i + 1;
        }
    }
    lines.push_back(command.substr(lastPoint, command.length() - lastPoint));
    return lines;
}
