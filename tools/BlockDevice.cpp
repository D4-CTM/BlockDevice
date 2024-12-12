#include "BlockDevice.hpp"

bool compare(std::string& string, const char charArray[], size_t charArraySize) {
    if (string.size() != charArraySize) return false;

    for (int i = 0; i < string.size(); i++) {
        if (string[i] != charArray[i]) return false;
    }

    return true;
}

void BlockDevice::deviceInfo()
{
    if (!isDeviceOpen()) {
        throw OpenDevice();
    }
    size_t deviceSize = sizeof(header) + (header.blockSize * header.blockCount);

    std::cout << AnsiCodes::BLUE;
    std::cout << "|--------------------------" << '\n';
    std::cout << "| Device name: " + deviceName << '\n';
    std::cout << "| Device size: " << deviceSize << '\n';
    std::cout << "| Block quantity: " << header.blockCount << '\n';
    std::cout << "| Individual block size: " << header.blockSize << '\n';
    std::cout << "| Header size: " << sizeof(Header) << '\n';
    std::cout << "|--------------------------" << '\n';
    std::cout << AnsiCodes::DEFAULT << '\n';
}

void BlockDevice::showSuperblockInfo()
{
    if (!isDeviceOpen()) {
        throw OpenDevice();
    }

    std::ifstream file((getDevicePath()), std::ios::binary);
    if (!file) {
        this->deviceName = "";
        throw FileCrash(deviceName);
    }

    file.seekg(header.calculateOffsetOf(0));
    Superblock superblock;
    file.read(reinterpret_cast<char *>(&superblock), sizeof(Superblock));
    std::cout << "initialBlock: " << superblock.initialBlock << '\n';
    std::cout << "bytesMapPos: " << superblock.byteMapPos << '\n';
    std::cout << "inodeInitialBlockPos: " << superblock.inodesInitialBlockPos << '\n';
    std::cout << "initialFinalBlockPos: " << superblock.inodesFinalBlockPos << '\n';
    std::cout << "inodesPerBlock: " << superblock.inodesPerBlock << '\n';
    file.close();
}

/*
*   Block device block division
*   0: superblock
*   1 -> n: bytesmap
*   n is obtained by the next division: blockSize/blockCount
*   n+1 -> m: inodes
*   m -> (blockCount): blocks
*/
bool BlockDevice::create(const std::string &deviceName, size_t blockSize, const size_t blockCount)
{
    if (isDeviceOpen()) {
        throw CloseDevice();
    }

    std::ofstream file(ROOT / deviceName, std::ios::binary);
    if (!file) {
        throw FileCrash(deviceName);
    }
    
    header = Header(blockSize, blockCount);
    file.write(reinterpret_cast<const char *>(&header), sizeof(Header));
    Block block(blockSize);
    for (int i = 0; i < blockCount; i++) {
        file.write(block.data(), blockSize);
    }
    
    file.close();
    return true;
}

bool BlockDevice::open(const std::string &deviceName)
{
    if (isDeviceOpen()) {
        throw CloseDevice();
    }

    this->deviceName = deviceName;
    std::ifstream file((getDevicePath()), std::ios::binary);
    if (!file) {
        this->deviceName = "";
        throw FileCrash(deviceName);
    }

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char *>(&header), sizeof(Header));

    file.close();
    return true;
}

bool BlockDevice::close()
{
    deviceName = "";
    header.clear();

    return true;
}

bool BlockDevice::writeBlock(size_t blockPos, const std::string &data)
{
    if (!isDeviceOpen()) {
        throw OpenDevice();
    }

    if (blockPos == header.blockCount) {
        throw Warning("the device starts counting from 0 instead of 1");
    }

    if (blockPos > header.blockCount) {
        throw Crash("the device only counts with " + std::to_string(header.blockCount) + " blocks!");
    }

    size_t offset = header.calculateOffsetOf(blockPos);
    std::fstream file(getDevicePath(), std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        throw FileCrash(deviceName);
    }

    file.seekp(offset, std::ios::beg);
    Block block(data, header.blockSize);
    file.write(block.bits.data(), header.blockSize);
    file.close();
    return true;
}

BlockDevice::Block BlockDevice::readBlock(size_t blockPos)
{
    if (!isDeviceOpen()) {
        throw OpenDevice();
    }

    if (blockPos == header.blockCount) {
        throw Warning("the device starts counting from 0 instead of 1");
    }

    if (blockPos > header.blockCount) {
        throw Crash("the device only counts with " + std::to_string(header.blockCount) + " blocks!");
    }

    size_t offset = header.calculateOffsetOf(blockPos);
    std::ifstream file(getDevicePath(), std::ios::binary);
    if (!file) {
        throw FileCrash(deviceName);
    }

    file.seekg(offset, std::ios::beg);
    Block block;
    block.bits.resize(header.blockSize);
    file.read(block.data(), header.blockSize);

    return block;
}
/*
*   first = InodeBlockPos
*   second = InodePositionOnBlock
*/
std::pair<int, int> BlockDevice::getInodePos(const Superblock& superblock, std::string& filename)
{
    std::pair<int, int> BlockPos = {-1, -1};
    std::ifstream file(getDevicePath(), std::ios::binary);
    Inode inode[superblock.inodesPerBlock];
    for (int i = superblock.inodesInitialBlockPos; i < superblock.inodesFinalBlockPos; i++) {
        file.seekg(header.calculateOffsetOf(i), std::ios::beg);
        file.read(reinterpret_cast<char *>(&inode), sizeof(Inode) * superblock.inodesPerBlock);
        for (int j = 0; j < superblock.inodesPerBlock; j++) {
            if (inode[j].free) {
                BlockPos = {i, j};
            } else if (compare(filename, inode[j].filename, sizeof(inode[j].filename))) {
                file.close();
                return {i, j};
            }
        }
    }

    file.close();
    return BlockPos;
}

void BlockDevice::removeFile(std::string &filename)
{
    if (!isDeviceOpen()) {
        throw OpenDevice();
    }
    
    for (int i = filename.length(); i < 64; i++) {
        filename.push_back(' ');
    }

    std::fstream file(getDevicePath(), std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        throw FileCrash(deviceName);
    }
    file.seekg(header.calculateOffsetOf(0), std::ios::beg);
    Superblock superblock;
    file.read(reinterpret_cast<char *>(&superblock), sizeof(Superblock));
    std::pair<int, int> inodePos = getInodePos(superblock, filename);
    if (inodePos.first == -1 || inodePos.second == -1) {
        file.close();
        throw Crash("the file wasn't found!");
    }

    Inode inode[superblock.inodesPerBlock];
    file.seekg(header.calculateOffsetOf(inodePos.first), std::ios::beg);
    file.read(reinterpret_cast<char *>(&inode), sizeof(Inode) * superblock.inodesPerBlock);
    if (inode[inodePos.second].free) {
        throw Crash("the file doesn't exist.");
    }
    const bool free = true;
    inode[inodePos.second].free = free;
    for (int i = 0; i < 8; i++) {
        if (inode[inodePos.second].offset[i] == -1) continue;
        file.seekp(header.calculateOffsetOf(superblock.byteMapPos) + inode[inodePos.second].offset[i], std::ios::beg);
        file.write(reinterpret_cast<const char *>(&free), sizeof(bool));
        inode[inodePos.second].offset[i] = -1;
    }
    file.seekp(header.calculateOffsetOf(inodePos.first), std::ios::beg);
    file.write(reinterpret_cast<const char *>(&inode), sizeof(Inode) * superblock.inodesPerBlock);
    file.close();
}

void BlockDevice::writeFile(std::string &filename, std::string &text)
{
    if (!isDeviceOpen()) {
        throw OpenDevice();
    }

    for (int i = filename.length(); i < 64; i++) {
        filename.push_back(' ');
    }
    const int maxStringIterations = 1 + (text.length()/header.blockSize);

    if (maxStringIterations >= 8) {
        throw Warning("the text inputed is too big to be stored.");
    }
    const int fileSize = text.length();
    int stringIteration = 0;

    std::fstream file(getDevicePath(), std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        throw FileCrash(deviceName);
    }
    Superblock superblock;
    file.seekg(header.calculateOffsetOf(0), std::ios::beg);
    file.read(reinterpret_cast<char *>(&superblock), sizeof(Superblock));

    std::pair<int, int> inodePos = getInodePos(superblock, filename);
    if (inodePos.first == -1 || inodePos.second == -1) {
        file.close();
        throw Crash("the file couldn't been reach nor created.");
    }

    file.seekg(header.calculateOffsetOf(inodePos.first), std::ios::beg);
    Inode inode[superblock.inodesPerBlock];
    bool free = false;
    file.read(reinterpret_cast<char *>(&inode), sizeof(Inode) * superblock.inodesPerBlock);

    if (!inode[inodePos.second].free) {
        removeFile(filename);
        writeFile(filename, text);
        return ;
    }

    file.seekg(header.calculateOffsetOf(superblock.byteMapPos), std::ios::beg);

    for (int i = 0; i < header.blockCount; i++) {
        file.read(reinterpret_cast<char *>(&free), sizeof(bool));
        if (!free) continue;
        writeBlock(i, text.substr(header.blockSize * stringIteration, header.blockSize));
        inode[inodePos.second].offset[stringIteration] = i;
        stringIteration++;
        free = false;
        file.seekp(header.calculateOffsetOf(superblock.byteMapPos) + i, std::ios::beg);
        file.write(reinterpret_cast<const char *>(&free), sizeof(bool));
        if (stringIteration >= maxStringIterations) break;
    }
    filename.copy(inode[inodePos.second].filename, 64);
    inode[inodePos.second].size = fileSize;
    inode[inodePos.second].free = false;        

    file.seekp(header.calculateOffsetOf(inodePos.first), std::ios::beg);
    file.write(reinterpret_cast<const char *>(&inode), sizeof(Inode) * superblock.inodesPerBlock);
    file.close();
    if (stringIteration < maxStringIterations) AnsiCodes::showError("There wasn't enough space for the whole text, it was cropped.");
}

BlockDevice::Block BlockDevice::getContent(std::string &filename)
{
    if (!isDeviceOpen()) {
        throw OpenDevice();
    }
    
    for (int i = filename.length(); i < 64; i++) {
        filename.push_back(' ');
    }

    std::ifstream file(getDevicePath(), std::ios::binary);
    if (!file) {
        throw FileCrash(deviceName);
    }
    file.seekg(header.calculateOffsetOf(0), std::ios::beg);
    Superblock superblock;
    file.read(reinterpret_cast<char *>(&superblock), sizeof(Superblock));
    std::pair<int, int> inodePos = getInodePos(superblock, filename);
    if (inodePos.first == -1 || inodePos.second == -1) {
        file.close();
        throw Crash("the file couldn't been reach nor found.");
    }

    Inode inode[superblock.inodesPerBlock];
    file.seekg(header.calculateOffsetOf(inodePos.first), std::ios::beg);
    file.read(reinterpret_cast<char *>(&inode), sizeof(Inode) * superblock.inodesPerBlock);
    if (inode[inodePos.second].free) {
        throw Crash("the file doesn't exist.");
    }

    Block block;
    for (int i = 0; i < 8; i++) {
        if (inode[inodePos.second].offset[i] == -1) continue;
        block << readBlock(inode[inodePos.second].offset[i]);
    }
    return block;
}

void printFilename(const char* filename) {
    std::cout << "\'";
    for (int i = 0; i < 64 ; i++) {
        if (filename[i] == ' ') break;
        std::cout << filename[i];
    }
    std::cout << "\'\t";
}

void BlockDevice::listFiles()
{
    if (!isDeviceOpen()) {
        throw OpenDevice();
    }

    std::ifstream file(getDevicePath(), std::ios::binary);
    if (!file) {
        throw FileCrash(deviceName);
    }

    file.seekg(header.calculateOffsetOf(0), std::ios::beg);
    Superblock superblock;
    file.read(reinterpret_cast<char *>(&superblock), sizeof(Superblock));
    Inode inode[superblock.inodesPerBlock];
    std::cout << AnsiCodes::BLUE;
    for (int i = superblock.inodesInitialBlockPos; i < superblock.inodesFinalBlockPos; i++) {
        file.seekg(header.calculateOffsetOf(i), std::ios::beg);
        file.read(reinterpret_cast<char *>(&inode), sizeof(Inode) * superblock.inodesPerBlock);
        for (int j = 0; j < superblock.inodesPerBlock; j++) {
            if (inode[j].free) continue;
            printFilename(inode[j].filename);
        }
    }
    std::cout << AnsiCodes::DEFAULT << '\n';
    file.close();
}

void BlockDevice::format()
{
    if (!isDeviceOpen()) {
        throw OpenDevice();
    }
    const uint32_t bytesMapSpace = 1 + (header.blockCount/header.blockSize);
    const uint32_t inodesInitialBlock = 1 + bytesMapSpace;
    const uint32_t inodesPerBlock = header.blockSize/sizeof(Inode);
    if (inodesPerBlock == 0) {
        throw Crash("to little space for the inodes, please make, at least, a block device with the block size of: " + std::to_string(sizeof(Inode)));
    }

    const uint32_t blocksRequiered = 256/inodesPerBlock;
    if (blocksRequiered >= header.blockCount) {
        throw Crash("Not enough space for the files. With the block size of " + std::to_string(header.blockSize) + " you need, at least, " + std::to_string(blocksRequiered + 8) + " blocks.");
    }

    if (header.blockCount > (blocksRequiered * inodesPerBlock) * 8) {
        const uint32_t suggestedBlockCount = (inodesInitialBlock + blocksRequiered) + ((blocksRequiered * inodesPerBlock) * 8);
        throw Warning("Maximum space needed for this block device (" + std::to_string(suggestedBlockCount) + ") exceeded!\n\tPlease input less blocks!");
    }
    std::fstream file(getDevicePath(), std::ios::in | std::ios::out | std::ios::binary);
    Superblock superblock(inodesPerBlock, inodesInitialBlock, (inodesInitialBlock + blocksRequiered), blocksRequiered);
    file.seekp(header.calculateOffsetOf(0), std::ios::beg);
    file.write(reinterpret_cast<const char *>(&superblock), sizeof(Superblock));

    file.seekp(header.calculateOffsetOf(1));

    bool free;
    for (int i = 0; i <= header.blockCount; i++) {
        free = (i > superblock.inodesFinalBlockPos);
        file.write(reinterpret_cast<const char *>(&free), sizeof(bool));
    }

    Inode inodes[superblock.inodesPerBlock];
    for (int i = 0; i < superblock.blocksRequiered; i++) {
        file.seekp(header.calculateOffsetOf(superblock.inodesInitialBlockPos + i));
        file.write(reinterpret_cast<const char *>(&inodes), sizeof(Inode) * superblock.inodesPerBlock);
    }
    file.close();
}
