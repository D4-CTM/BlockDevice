#include <iostream>
#include "./tools/CMD.hpp"

int main() {
    CMD commandLine = CMD();
    std::string command;

    while(commandLine.isRunning()) {
        try {
            std::cout << AnsiCodes::YELLOW << "sudo@Partitions$ " << AnsiCodes::DEFAULT;
            std::getline(std::cin, command);
            if (!command.empty()) {
                commandLine.readLine(command);
            }
        } catch (const std::exception& e) {
            std::cout << AnsiCodes::RED << e.what() << '\n';
        }
    }

    return 0;
}