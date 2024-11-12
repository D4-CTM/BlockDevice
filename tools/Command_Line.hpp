#ifndef __COMMAND_LINE_HPP__
#define __COMMAND_LINE_HPP__
#include "BlockDevice.hpp"
#include <filesystem>

class Command_Line {
private:
    //Common use methods
    const std::string KILL = "kill";
    const std::string CLEAR = "clear";
    const std::string HELP_ME = "--help";
    const std::string LIST_ELEMENTS = "lp";
    //Mathods to manage partitions
    const std::string CREATE_BLOCK_DEVICE = "create";
    const std::string SELECT_BLOCK_DEVICE = "select";
    const std::string BLOCK_DEVICE_INFO = "info";
    const std::string WRITE_INFO = "write";
    const std::string READ_INFO = "read";

    BlockDevice* blockDevice;
public:
    Command_Line();

    const void doCommand(const std::string& methods);
    const std::string getPartitionName() { return blockDevice->getPartitionName().empty() ? "" : "/" + blockDevice->getPartitionName(); }
    constexpr inline bool isRunning() { return run; }

    ~Command_Line() {
        if (blockDevice) delete blockDevice;
    }

private:
    const std::vector<std::string> split(const std::string& command);
    const void helpMe();
    const void listElements();
    const void writeInformation(const std::vector<std::string>& partitionInfo, const std::string& originalCommand);
    const void createPartition(const std::vector<std::string>& partitionInfo);
    const void readInformation(const std::vector<std::string>& blockInfo);
    const void selectPartition(const std::string& fileName);

    const inline void clearScreen() { system("clear"); }

private:
    bool run;
};

#endif //__COMMAND_LINE_HPP__
