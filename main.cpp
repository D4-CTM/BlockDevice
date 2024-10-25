#include <iostream>
#include "./tools/Command_Line.hpp"

int main() {
    Command_Line commandLine;
    std::string line;

    while(commandLine.isRunning()) {
        try {
            std::cout << AnsiCodes::YELLOW << "sudo@FileManager: ~" << commandLine.getPartitionName() << AnsiCodes::DEFAULT << " $ ";
            getline(std::cin, line);
            commandLine.doCommand(line);
        } catch (const std::invalid_argument& e) {
            std::cerr << AnsiCodes::RED << e.what() << '\n';
        }
    }

    return 0;
}