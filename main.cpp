#include <iostream>
#include <sstream>
#include "./tools/BlockDevice.hpp"

void help() {
    std::cout << AnsiCodes::BLUE;
    std::cout << "Commonly use commands" << '\n';
    std::cout << "\t --help: display a list of all commands" << '\n';
    std::cout << "\t ld: display's a list of all devices" << '\n';
    std::cout << "\t kill: exits the program" << '\n';
    std::cout << "\t clear: remove's every element on the screen, for a cleaner terminal!" << '\n';
    std::cout << "\nBlock device basic tools:" << '\n';
    std::cout << "\t create <file name> <blocks size> <block quantity>: creates a block device with the specified name, size of each block and how many blocks to create." << '\n';
    std::cout << "\t\t\t\t   (IMPORTANT: the size of the block device itself could be greater than expected due to a little bit of additional information needed to be stored)" << '\n';;
    std::cout << "\t select <file name>: selects the block device with the specified name." << '\n';
    std::cout << "\t close: closes the current block device." << '\n';
    std::cout << "\t info: displays some basic information about the selected block device." << '\n'; 
    std::cout << "\t write <block position> <data>: writes the data to the specified block, wrap the data between \"\"" << '\n';
    std::cout << "\t read <block position> <offset> <total characters>: reads the block specified starting from the offset until reaching the quantity of chars specified."<< '\n';
    std::cout << "\t\t\t\t if nothing but the block is provided it'll read everything on the block!" << '\n';
    std::cout << "\nFile manager tools: " << '\n';
    std::cout << "\tformat: removes all the files on the device." << '\n';
    std::cout << "\tls: list all the files on the device." << '\n';
    std::cout << "\tsuperblock: see the information stored on the superblock." << '\n';
    std::cout << "\twritef <filename> <text>: creates a file with the specified name with the specified text, wrap the data between \"\"." << '\n';
    std::cout << "\trm <filename>: deletes the specified file from the device." << '\n';    
    std::cout << "\tcat <filename>: display the content on a specific file." << '\n';
    std::cout << "\thexdump <filename>: display the content on a specific file on hex values." << '\n';
//    std::cout << "\t" << '\n';
    std::cout << AnsiCodes::DEFAULT << '\n';
}

void createBlockDevice(std::istringstream& iss, BlockDevice& blockDevice) {
    std::string filename = "";
    int blockSize = -1;
    int blockCount = -1;
    
    iss >> filename >> blockSize >> blockCount;
    
    if (filename.empty()) {
        throw Crash("Please input the name of the device!");
    }

    if (blockSize < 512) {
        throw Warning("The block size should be, at least, 512 bytes");
    }

    if (blockCount < 100) {
        throw Warning("The quantity of blocks on every device should be, at least, of a 100");
    }

    blockDevice.create(filename, blockSize, blockCount);
}

void openBlockDevice(std::istringstream& iss, BlockDevice& blockDevice) {
    std::string filename;
    iss >> filename;

    if (filename.empty()) {
        throw Crash("Please input the name of the device!");
    }

    blockDevice.open(filename);
}

bool writeOnDevice(std::istringstream& iss, BlockDevice& blockDevice) {
    int blockPos = -1;
    iss >> blockPos;
    if (blockPos < 0) {
        throw Warning("Please input a valid position in which you'll write the data!", true);
    }

    std::string info;
    char quote;
    iss >> quote;
    if (quote != '"') {
        throw Warning("Please use \" to mark the beginning of your text.");
    }
    std::getline(iss, info, quote);

    return blockDevice.writeBlock(blockPos, info);    
}

void readOnDevice(std::istringstream& iss, BlockDevice& blockDevice) {
    int blockPos = -1;
    iss >> blockPos;
    if (blockPos < 0) {
        throw Warning("Please input a valid block position", true);
    }

    auto block = blockDevice.readBlock(blockPos);
    int offset = 0;
    int readLength = block.size();

    iss >> offset >> readLength;
    if (offset < 0 || readLength < 0) {
        throw Warning("The offset and size of reading should be greater than 0");
    }

    std::cout << AnsiCodes::DEFAULT;
    for (int i = 0; i < readLength; i++) {
        if (block[i] == '\0') break;
        std::cout << std::hex << (int) block[offset + i] << ' ';
    }
    std::cout << std::endl;
}

void listDevices(BlockDevice& blockDevice) {
    for (const auto& file : std::filesystem::directory_iterator(blockDevice.getRootDirectory())) {
        std::cout << AnsiCodes::CYAN << file.path().filename() << ' ';
    }
    std::cout << AnsiCodes::DEFAULT << std::endl;
}

void writeFile(std::istringstream& iss, BlockDevice& blockDevice) {
    std::string filename = "";
    std::string text = "";

    iss >> filename;
    if (filename.empty()) {
        throw Warning("Please input the name of the file.");
    }

    if (filename.length() > 64) {
        throw Warning("Please limit the name of the file to just 64 bytes.");
    }

    char quote;
    iss >> quote;
    if (quote != '"') {
        throw Warning("Please use \" to mark the beginning of your text.");
    }
    
    std::getline(iss, text, quote);
    if (text.empty()) {
        throw Warning("Please input the text you'll send into the file.");
    }

    blockDevice.writeFile(filename, text);
}

void removeFile(std::istringstream& iss, BlockDevice& BlockDevice) {
    std::string filename = "";
    iss >> filename;
    if (filename.empty()) {
        throw Warning("Please input the filename");
    }

    BlockDevice.removeFile(filename);
}

void getFileContent(std::istringstream& iss, BlockDevice& BlockDevice) {
    std::string filename = "";
    iss >> filename;
    if (filename.empty()) {
        throw Warning("Please input the filename");
    }

    std::cout << BlockDevice.getContent(filename).bits << '\n';
}

void getHexFileContent(std::istringstream& iss, BlockDevice& BlockDevice) {
    std::string filename = "";
    iss >> filename;
    if (filename.empty()) {
        throw Warning("Please input the filename");
    }

    auto blocks = BlockDevice.getContent(filename);
    for (int i = 0; i < blocks.size(); i++) {
        if (blocks[i] == '\0') break;
        std::cout << std::hex << (int) blocks[i] << ' ';
    }
    std::cout << '\n';
}

int main() {
    BlockDevice blockDevice;
    std::string text;
    bool loop = true;
    help();
    while (loop) {
        try {
            AnsiCodes::showRootDirectory(blockDevice.getDevicePath());
            getline(std::cin, text);
            std::istringstream iss(text);
            std::string command;
            iss >> command;

            if (command == "clear") {
                system("clear");
            } else if (command == "--help") {
                help();
            } else if (command == "kill") {
                loop = false;
                break ;
            }else if (command == "info") {
                blockDevice.deviceInfo();
            } else if (command == "create") {
                createBlockDevice(iss, blockDevice);
            } else if (command == "select") {
                openBlockDevice(iss, blockDevice);
            } else if (command == "close") {
                blockDevice.close();
            } else if (command == "write") {
                writeOnDevice(iss, blockDevice);
            } else if (command == "read") {
                readOnDevice(iss, blockDevice);
            } else if (command == "ld") {
                listDevices(blockDevice);
            } else if (command == "superblock") {
                blockDevice.showSuperblockInfo();
            } else if (command == "ls") {
                blockDevice.listFiles();
            } else if (command == "writef") {
                writeFile(iss, blockDevice);
            } else if (command == "format") {
                blockDevice.format();
            } else if (command == "rm") {
                removeFile(iss, blockDevice);
            } else if (command == "cat") {
                getFileContent(iss, blockDevice);
            } else if (command == "hexdump") {
                getHexFileContent(iss, blockDevice);
            }

            std::cin.clear();
        } catch (Crash& e) {
            e.what();
        } catch (Warning& e) {
            e.what();
        } catch (std::exception& e) {
            std::cerr << AnsiCodes::RED << "ERROR: Something interrupted the operation!" << AnsiCodes::DEFAULT << std::endl;
        }
    }

    return 0;
}