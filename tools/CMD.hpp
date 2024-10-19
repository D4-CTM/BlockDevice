#ifndef __CONSOLE_COMAND__
#define __CONSOLE_COMAND__
#include "Partition.hpp"
#include "TextColor.h"
#include <iostream>
#include <vector>
#include <string>

class CMD {
//Command lines
private:
    //Partition commands
    const std::string CREATE_PARTITION = "MkPar";
    const std::string DELETE_PARTITION = "rm";
    const std::string LIST_PARTITION = "ls";
    //General commands
    const std::string CLOSE_TERMINAL = "kill";
    const std::string CLEAR_SCREEN = "clear";
    const std::string HELP_ME = "--help";
//Public functions to use the software
public:
    CMD() : run(true), partitionName("") {}

    const void readLine(const std::string& command);
    const bool createPartition(const std::string& path);
    const bool removePartition(const std::string& path);
    const void listPartition(const std::string& path);

    inline void clearScreen() {
        std::cout << "\033[2J"; // Erase in Display (clear screen)
        std::cout << "\033[H"; // Move cursor to (0,0)
    }

    constexpr inline bool isRunning() { return run; }

//Private functions
private:
    std::vector<std::string> split(const std::string& command);
    constexpr inline void killTerminal() { run = false; }
    const void goBack();

//Private variables
private:
    std::string partitionName;
    bool run;
};

#endif  //__CONSOLE_COMAND__