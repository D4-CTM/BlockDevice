#include <iostream>
#include "./tools/Command_Line.hpp"

int main() {
    Command_Line commandLine;
    std::string line;

    commandLine.doCommand("clear");
    commandLine.doCommand("--help");
    while(commandLine.isRunning()) {
        try {
            std::cout << AnsiCodes::YELLOW << "sudo@FileManager: ~" << commandLine.getPartitionName() << AnsiCodes::DEFAULT << " $ ";
            getline(std::cin, line);
            commandLine.doCommand(line);
        } catch (const std::exception& e) {
            std::cerr << AnsiCodes::RED << "ERROR: an error has occurred, please input correctly the data!" << '\n';
        }
    }

    return 0;
}