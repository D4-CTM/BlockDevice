#ifndef __COMMAND_LINE_HPP__
#define __COMMAND_LINE_HPP__
#include "Partition.hpp"
#include "TextColor.h"
#include <filesystem>
#include <iostream>
#include <vector>

class Command_Line {
private:
    //Folder that includes each partition made by the user
    const std::string ROOT = "./Partitions";
    //Common use methods
    const std::string KILL = "kill";
    const std::string CLEAR = "clear";
    const std::string HELP_ME = "--help";
    const std::string LIST_ELEMENTS = "ls";
    //Mathods to manage partitions
    const std::string CREATE_PARTITION = "mkpar";
    const std::string SELECT_PARTITION = "sp";

    Disk_Partitioner* partitioner;
public:
    Command_Line();

    const void doCommand(const std::string& methods);
    constexpr inline bool isRunning() { return run; };
    const inline std::string getPartitionName() { return partitionName == "" ? "" : "/" + partitionName + ".pt"; }

    ~Command_Line() {
        if (partitioner) delete partitioner;
    }

private:
    const std::vector<std::string> split(const std::string& command);
    const void helpMe();
    const void listElements();
    const void createPartition(const std::string& fileName);

    inline void clearScreen() { system("clear"); }

//Variables
private:
    std::string partitionName;
    bool run;
};

#endif //__COMMAND_LINE_HPP__
