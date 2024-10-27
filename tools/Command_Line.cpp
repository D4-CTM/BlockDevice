#include "Command_Line.hpp"

Command_Line::Command_Line()
: partitioner(new Disk_Partitioner), partitionName(""), run(true)
{
    if (!std::filesystem::exists(ROOT)) {
        std::filesystem::create_directory(ROOT);
    }
}

const void defineBlockSize(size_t& blocks_Size) {
    std::cout << AnsiCodes::GREEN << "Digit the size (bytes) of each block in the partition: " << AnsiCodes::DEFAULT;
    std::cin >> blocks_Size;
    if (blocks_Size < 10) {
        std::cout << AnsiCodes::RED << '\n' << "The size of the blocks should be, at minimum, 10 bytes!" << '\n';
        defineBlockSize(blocks_Size);
    }
}

const void defineBlockCant(size_t& block_cant) {
    std::cout << AnsiCodes::GREEN << "Digit how many blocks you'll like to have: " << AnsiCodes::DEFAULT;
    std::cin >> block_cant;
    if (block_cant < 5) {
        std::cout << AnsiCodes::RED << '\n' << "There should be at least 5 blocks" << '\n';
        defineBlockSize(block_cant);
    }
}

const void partitionInfo(const std::string& fileName, const size_t& blocks_size, const size_t& block_cant) {
    std::cout << AnsiCodes::GREEN;
    std::cout << "|----------------------------------|" << '\n';
    std::cout << "| Partition name: " << fileName << '\n';
    std::cout << "| Partition size: " << (2 * sizeof(size_t)) + (block_cant * (blocks_size)) << '\n';
    std::cout << "| Block quantity: " << block_cant << '\n';
    std::cout << "| Blocks size: " << blocks_size << '\n';
    std::cout << "|----------------------------------|" << '\n';

}

const bool partitionConfirmation(const std::string& fileName, const size_t& blocks_size, const size_t& block_cant) {
    std::string ans;

    partitionInfo(fileName, blocks_size, block_cant);
    std::cout << "Are you sure about this configuration [Y/N]: " << AnsiCodes::DEFAULT;

    std::cin >> ans;
    return ans[0] == 'Y' || ans[0] == 'y';
}

const void Command_Line::doCommand(const std::string &methods)
{
    if (methods.empty()) return ;
    auto lines = split(methods);
    if (lines[0].empty()) throw std::runtime_error("ERROR: please don't leave extra empty spaces");

    if (lines[0] == KILL) {
        run = false;
    } else if (lines[0] == CLEAR) {
        clearScreen();
    } else if (lines[0] == LIST_ELEMENTS) {
        listElements();
    } else if (lines[0] == HELP_ME) {
        helpMe();
    } else if (lines[0] == CREATE_PARTITION) {
        if (lines.size() > 1 && !lines[1].empty()) {
            createPartition(lines[1]);
        } else std::cout << AnsiCodes::RED << "ERROR: Please input the partition name!\n" << AnsiCodes::DEFAULT << "For reference you could use the \'--help\' command" << '\n';
    } else if (lines[0] == SELECT_PARTITION) {
        if (lines.size() > 1 && !lines[1].empty()) {
            selectPartition(lines[1]);
        } else std::cout << AnsiCodes::RED << "ERROR: Please input something the partition\'s name!\n" << AnsiCodes::DEFAULT << "For reference you could use the \'--help\' command" << '\n';
    } else if (lines[0] == PARTITION_INFO) {
        if (!partitionName.empty()) {
            partitionInfo(partitionName, partitioner->getBlockSize(), partitioner->getBlockCant());
        } else std::cout << AnsiCodes::RED << "ERROR: Please select a partition first!\n" << AnsiCodes::DEFAULT << "For reference you could use the \'--help\' command" << '\n';
    }

}

const void Command_Line::createPartition(const std::string& fileName) {
    size_t blocks_size = 10;
    size_t block_cant = 5;
    std::string ans = " ";
    
    clearScreen();
    while (ans[0] != '4') {
        std::cout << AnsiCodes::GREEN;
        std::cout << "| Partition name: " << fileName << '\n';
        std::cout << "|----------------------------------|" << '\n';
        std::cout << "| 1. Blocks size                   |" << '\n';
        std::cout << "| 2. blocks quantity               |" << '\n';
        std::cout << "| 3. Create partition              |" << '\n';
        std::cout << "| 4. Cancel operation              |" << '\n';
        std::cout << "|----------------------------------|" << '\n';
        std::cout << "| digit an option: " << AnsiCodes::DEFAULT;
        getline(std::cin, ans);

        clearScreen();
        switch (ans[0]) {
            case '1':
                defineBlockSize(blocks_size);
            break;

            case '2':
                defineBlockCant(block_cant);
            break;

            case '3':
                if (partitionConfirmation(fileName, blocks_size, block_cant)) {                    
                    partitioner->createPartition((ROOT + "/" + fileName), block_cant, blocks_size);
                }
            break;
        }
    }
    std::cout << AnsiCodes::DEFAULT;
}

const void Command_Line::selectPartition(const std::string &fileName)
{
    if (fileName == "..") {
        partitionName = "";
        return ;
    }

    partitionName = fileName;
    if (!std::filesystem::exists(ROOT + getPartitionName())) {
        std::cout << AnsiCodes::RED << "ERROR: the partition doesn't exists!\n" << AnsiCodes::DEFAULT << "For reference you can use the command \'--help\'" << '\n';
        partitionName = "";
        return ;
    }
    partitioner->selectPartition(ROOT+ getPartitionName());
}

const void Command_Line::listElements()
{
    if (partitionName.empty()) {
        for (const auto& element : std::filesystem::directory_iterator(ROOT)) {
            if (element.is_regular_file()) {
                std::cout << AnsiCodes::BLUE << std::filesystem::path(element).filename().string() << AnsiCodes::DEFAULT << '\t';
            }
        }
    }
    std::cout << '\n';
}

const std::vector<std::string> Command_Line::split(const std::string& command)
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

const void Command_Line::helpMe()
{
    std::cout << AnsiCodes::BLUE;
    std::cout << HELP_ME << ": display a list of all commands" << '\n';
    std::cout << "Commonly use commands" << '\n';
    std::cout << '\t' << LIST_ELEMENTS << ": display's a list of all partitions" << '\n';
    std::cout << '\t' << KILL << ": exit's this program" << '\n';
    std::cout << '\t' << CLEAR << ": remove's every element on the screen: for a cleaner terminal." << '\n';
    std::cout << "\nPartition tools:" << '\n';
    std::cout << '\t' << CREATE_PARTITION << " <partition name>: creates a partition with the specified name" << '\n';
    std::cout << '\t' << SELECT_PARTITION << " <partition name> | <..>: selects the partition with the specified name." << '\n';
    std::cout << "\t\t\t\t  " << "| go back to the root file \'~\'." << '\n';
    std::cout << '\t' << PARTITION_INFO << ": displays some basic information about the selected partition" << '\n'; 
//    std::cout << "\t" << '\n';
    std::cout << AnsiCodes::DEFAULT << '\n';
}
