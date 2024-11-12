#include "Command_Line.hpp"

Command_Line::Command_Line()
: blockDevice(new BlockDevice()), run(true)
{
    if (!std::filesystem::exists(blockDevice->getRoot())) {
        std::filesystem::create_directory(blockDevice->getRoot());
    }
}

const void Command_Line::doCommand(const std::string &methods)
{
    if (methods.empty()) return ;
    const auto lines = split(methods);
    
    if (lines[0].empty()) throw std::runtime_error("ERROR: please input first the command!");

    if (lines[0] == KILL) {
    
        run = false;
    
    } else if (lines[0] == CLEAR) {
    
        clearScreen();
    
    } else if (lines[0] == LIST_ELEMENTS) {
    
        listElements();
    
    } else if (lines[0] == HELP_ME) {
    
        helpMe();

    } else if (lines[0] == CREATE_BLOCK_DEVICE) {
        
        createPartition(lines);
    
    } else if (lines[0] == SELECT_BLOCK_DEVICE) {
    
        if (lines.size() > 1 && !lines[1].empty()) {
    
            selectPartition(lines[1]);
    
        } else std::cerr << AnsiCodes::RED << "ERROR: Please input something for the block\'s name!\n" << AnsiCodes::DEFAULT << "For reference you could use the \'--help\' command" << '\n';
    
    } else if (lines[0] == BLOCK_DEVICE_INFO) {
    
        if (!blockDevice->getPartitionName().empty()) {
            std::cout << AnsiCodes::GREEN;

            blockDevice->info();

            std::cout << AnsiCodes::DEFAULT;
        } else std::cerr << AnsiCodes::RED << "ERROR: Please select a block first!\n" << AnsiCodes::DEFAULT << "For reference you could use the \'--help\' command" << '\n';
    
    } else if (lines[0] == WRITE_INFO) {

        writeInformation(lines, methods);

    } else if (lines[0] == READ_INFO) {

        readInformation(lines);

    }

}

const bool isNumeric(const std::string& str) {
    int i;
    for (auto& c : str) {
        for (i = 0; i < 10; i++) {
            if (c == ('0' + i)) {
                break;
            }
            if (i == 9) return false;
        }
    }
    return true;
}

const void Command_Line::createPartition(const std::vector<std::string>& partitionInfo) {
    if (!getPartitionName().empty()) {
        std::cerr << AnsiCodes::RED << "ERROR: Please close the current block device!" << '\n';
        return ;
    }

    const int size = partitionInfo.size();
    if (size <= 1) {
        std::cerr << AnsiCodes::RED << "ERROR: please input the block name!" << '\n';
        return ;
    }
    std::string fileName = partitionInfo[1];

    if (size <= 2) {
        std::cerr << AnsiCodes::RED << "ERROR: please digit the size of each block!" << '\n';
        return ;
    }

    if (!isNumeric(partitionInfo[2])) {
        std::cerr << AnsiCodes::RED << "ERROR: the input for the block size must be a possitive integer!" << '\n';
        return ;
    }
    size_t blocks_size = std::stoi(partitionInfo[2]);

    if (blocks_size == 0) {
        std::cerr << AnsiCodes::RED << "ERROR: the input for the block size must be a greater than 0!" << '\n';
        return ;
    }

    if (size <= 3) {
        std::cerr << AnsiCodes::RED << "ERROR: please digit the quantity of blocks on the block!" << '\n';
        return ;
    }

    if (!isNumeric(partitionInfo[3])) {
        std::cerr << AnsiCodes::RED << "ERROR: the input for the blocks quantity must be a possitive integer!" << '\n';
        return ;
    }
    size_t block_cant = std::stoi(partitionInfo[3]);

    if (block_cant == 0) {
        std::cerr << AnsiCodes::RED << "ERROR: the input for the blocks quantity must be a greater than 0!" << '\n';
        return ;
    }

    blockDevice->create(fileName, block_cant, blocks_size);
}

const void Command_Line::readInformation(const std::vector<std::string> &blockInfo)
{
    const int size = blockInfo.size();

    if (size <= 1) {
        std::cerr << AnsiCodes::RED << "ERROR: please input the block in which you'll like to read!" << '\n';
        return;
    }

    if (!isNumeric(blockInfo[1])) {
        std::cerr << AnsiCodes::RED << "ERROR: please digit a positive numeric value to indicate correctly the block you'll like to read." << '\n';
        return;
    }
    int blockPos = std::stoi(blockInfo[1]);
    auto& text = blockDevice->readBlock(blockPos);

    int offset = 0;
    int totalChars = text.size();

    if (size > 2 ) {
        
        if (!isNumeric(blockInfo[2])) {
            std::cerr << AnsiCodes::RED << "ERROR: please input a positive numeric value for the starting position of the text!" << '\n';
            return;
        }
        offset = std::stoi(blockInfo[2]);

        if (offset > totalChars) {
            std::cerr << AnsiCodes::RED << "ERROR: the starting position for the block #" << blockPos << " should not be greater than " << totalChars << "!" << '\n';            
            return;
        }

    }

    if (size > 3) {

        if (!isNumeric(blockInfo[3])) {
            std::cerr << AnsiCodes::RED << "ERROR: please input a positive numeric as for how many characters you'll like to read!" << '\n';
            return;
        }
        totalChars = std::stoi(blockInfo[3]);

        if (totalChars > text.size()) {
            std::cerr << AnsiCodes::RED << "ERROR: the ammount of chars to read exceeds the size of the text itself" << '\n';
            return;
        }

        if ((offset + totalChars) >= text.size()) {
            std::cerr << AnsiCodes::RED << "ERROR: based on the offset of " << offset << " the ammount of chars to read exceeds the limit!" << '\n';
            return ;
        }

    }

    std::cout << AnsiCodes::DEFAULT;
    for (int i = 0; i < totalChars; i++) {
        std::cout << std::hex << (int) text[offset + i] << ' ';
    }
    std::cout << '\n';
}

const void Command_Line::selectPartition(const std::string &fileName)
{
    if (fileName == "..") {
        blockDevice->clearSuperblock();
        return ;
    }

    if (!blockDevice->getPartitionName().empty()) {
        //change added to make sense of the 'close()' function asked.
        std::cerr << AnsiCodes::RED << "ERROR: please close the current block before attempting to change into a new one" << '\n';
        return ;
    }

    if (!std::filesystem::exists(blockDevice->getRoot() + "/" + blockDevice->getPartitionName())) {
        std::cerr << AnsiCodes::RED << "ERROR: the block doesn't exists!\n" << AnsiCodes::DEFAULT << "For reference you can use the command \'--help\'" << '\n';
        return ;
    }
    blockDevice->select(fileName);
}

const void Command_Line::listElements()
{
    for (const auto& element : std::filesystem::directory_iterator(blockDevice->getRoot())) {
        if (element.is_regular_file()) {
            std::cout << AnsiCodes::BLUE << std::filesystem::path(element).filename().string() << AnsiCodes::DEFAULT << '\t';
        }
    }
    std::cout << '\n';
}

std::vector<unsigned char> getInnerString(std::string str) {
    int initialPos = 0;
    int finalPos = 0;

    for (int i = 0; i < str.length(); i++) {

        if (str[i] == '\"') {

            if (initialPos == 0) {
                initialPos = i + 1;
            } else {
                finalPos = i;
            }

        }

    }

    if (finalPos <= initialPos) return {};

    std::vector<unsigned char> text;

    for (int i = initialPos; i < finalPos; i++) {
        text.push_back(str[i]);
    }

    return text;
}

const void Command_Line::writeInformation(const std::vector<std::string>& partitionInfo, const std::string& originalCommand)
{
    const int size = partitionInfo.size();

    if (size <= 1) {
        std::cerr << AnsiCodes::RED << "ERROR: please input the block in which you'll like to write something!" << '\n';
        return;
    }

    if (!isNumeric(partitionInfo[1])) {
        std::cerr << AnsiCodes::RED << "ERROR: please digit a numeric value for the position to which you'll write." << '\n';
        return;
    }
    const int writtingPos = std::stoi(partitionInfo[1]);

    if (size <= 2) {
        std::cerr << AnsiCodes::RED << "ERROR: please input the data you'll store on this block!" << '\n';
        return;
    }
    const auto text = getInnerString(originalCommand);   

    if (text.empty()) {
        std::cerr << AnsiCodes::RED << "ERROR: please close the message you'll add using \" at the end" << '\n';
        return ;
    }

    blockDevice->write(writtingPos, text);
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
    std::cout << "Commonly use commands" << '\n';
    std::cout << '\t' << HELP_ME << ": display a list of all commands" << '\n';
    std::cout << '\t' << LIST_ELEMENTS << ": display's a list of all partitions" << '\n';
    std::cout << '\t' << KILL << ": exit's this program" << '\n';
    std::cout << '\t' << CLEAR << ": remove's every element on the screen, for a cleaner terminal." << '\n';
    std::cout << '\n' << "Block device tools:" << '\n';
    std::cout << '\t' << CREATE_BLOCK_DEVICE << " <file name> <blocks size> <block quantity>: creates a block with the specified name, size of each block and how many blocks to create" << '\n';
    std::cout << "\t\t\t\t   (IMPORTANT: the size of the block device itself could be greater than expected due to additional information needed to be stored)" << '\n';;
    std::cout << '\t' << SELECT_BLOCK_DEVICE << " <file name> | <..>: selects the block with the specified name." << '\n';
    std::cout << "\t\t\t\t  " << "| go back to the root file \'~\' closing the current block." << '\n';
    std::cout << '\t' << BLOCK_DEVICE_INFO << ": displays some basic information about the selected file" << '\n'; 
    std::cout << '\t' << WRITE_INFO << " <block position> <data>: writed the data to the specified block, wrap the data between some \"\"" << '\n';
    std::cout << '\t' << READ_INFO << " <block position> <offset> <total characters>: reads the block specified starting from the offset until reaching the quantity of chars specified."<< '\n';
    std::cout << "\t\t\t\t" << " if nothing but the block is provided we'll read everything on that block!";
//    std::cout << "\t" << '\n';
    std::cout << AnsiCodes::DEFAULT << '\n';
}
