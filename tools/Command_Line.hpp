#ifndef __COMMAND_LINE_HPP__
#define __COMMAND_LINE_HPP__
#include "Partition.hpp"
#include "TextColor.h"
#include <filesystem>
#include <iostream>
#include <vector>

class Command_Line {
private:
    //Common use methods
    const std::string KILL = "kill";
    const std::string CLEAR = "clear";
    const std::string HELP_ME = "--help";
    const std::string LIST_ELEMENTS = "lp";
    //Mathods to manage partitions
    const std::string CREATE_PARTITION = "create";
    const std::string SELECT_PARTITION = "select";
    const std::string PARTITION_INFO = "info";
    const std::string WRITE_INFO = "write";
    const std::string READ_INFO = "read";

    Disk_Partitioner* partitioner;
public:
    Command_Line();

    const void doCommand(const std::string& methods);
    const std::string getPartitionName() { return partitioner->getPartitionName().empty() ? "" : "/" + partitioner->getPartitionName(); }
    constexpr inline bool isRunning() { return run; }

    ~Command_Line() {
        if (partitioner) delete partitioner;
    }

private:
    const std::vector<std::string> split(const std::string& command);
    const void helpMe();
    const void listElements();
    const void createPartition(const std::vector<std::string>& partitionInfo);
    const void selectPartition(const std::string& fileName);

    inline void clearScreen() { system("clear"); }

private:
    bool run;
};

#endif //__COMMAND_LINE_HPP__
