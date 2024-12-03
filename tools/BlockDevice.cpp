#include "BlockDevice.hpp"
#include <fstream>

void BlockDevice::deviceInfo()
{
    if (!isDeviceOpen()) {
        throw Warning("Please open a block device first.");
    }
    size_t deviceSize = sizeof(superblock) + ((superblock.blockSize + sizeof(bool)) * superblock.blockCount);

    std::cout << AnsiCodes::BLUE;
    std::cout << "|--------------------------" << '\n';
    std::cout << "| Device name: " + deviceName << '\n';
    std::cout << "| Device size: " << deviceSize << '\n';
    std::cout << "| Block quantity: " << superblock.blockCount << '\n';
    std::cout << "| Individual block size: " << (superblock.blockSize + sizeof(bool)) << '\n';
    std::cout << "| Superblock size: " << sizeof(Superblock) << '\n';
    std::cout << "|--------------------------" << '\n';
    std::cout << AnsiCodes::DEFAULT << '\n';
}

bool BlockDevice::create(const std::string &deviceName, size_t blockSize, const size_t blockCount)
{
    std::ofstream file(ROOT / deviceName, std::ios::binary);
    if (!file) {
        throw FileCrash(deviceName);
    }

    Superblock superblock(blockSize, blockCount);
    file.write(reinterpret_cast<const char *>(&superblock), sizeof(Superblock));
    Block block(blockSize);
    for (int i = 0; i < blockCount; i++) {
        file.write(reinterpret_cast<const char *>(&block.free), sizeof(bool));
        file.write(block.bits.data(), blockSize);
    }

    file.close();
    return true;
}

bool BlockDevice::open(const std::string &deviceName)
{
    if (isDeviceOpen()) {
        throw Warning("Please close current block device before opening another one!");
    }

    this->deviceName = deviceName;
    std::ifstream file((getDevicePath()), std::ios::binary);
    if (!file) {
        this->deviceName = "";
        throw FileCrash(deviceName);
    }

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char *>(&superblock), sizeof(Superblock));

    file.close();
    return true;
}

bool BlockDevice::close()
{
    deviceName = "";
    superblock.clear();

    return true;
}

bool BlockDevice::writeBlock(size_t blockPos, const std::string &data)
{
    if (!isDeviceOpen()) {
        throw Warning("Please open any of the devices first.", true);
    }

    if (blockPos == superblock.blockCount) {
        throw Warning("The device starts counting from 0 instead of 1");
    }

    if (blockPos > superblock.blockCount) {
        throw Crash("The device only counts with " + std::to_string(superblock.blockCount) + " blocks!");
    }

    size_t offset = superblock.calculateOffsetOf(blockPos);
    std::fstream file(getDevicePath(), std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        throw FileCrash(deviceName);
    }

    file.seekp(offset, std::ios::beg);
    Block block(data, superblock.blockSize);
    file.write(reinterpret_cast<const char *>(&block.free), sizeof(bool));
    if (!block.free)  {
        file.close();
        return false;
    }
    file.write(block.bits.data(), superblock.blockSize);
    file.close();
    return true;
}

std::string BlockDevice::readBlock(size_t blockPos)
{
    if (!isDeviceOpen()) {
        throw Warning("Please open any of the devices first.", true);
    }

    if (blockPos == superblock.blockCount) {
        throw Warning("The device starts counting from 0 instead of 1");
    }

    if (blockPos > superblock.blockCount) {
        throw Crash("The device only counts with " + std::to_string(superblock.blockCount) + " blocks!");
    }
    size_t offset = superblock.calculateOffsetOf(blockPos);
    std::ifstream file(getDevicePath(), std::ios::binary);
    if (!file) {
        throw FileCrash(deviceName);
    }

    file.seekg(offset, std::ios::beg);
    Block block;
    file.read(reinterpret_cast<char *>(&block.free), sizeof(bool));
    if (block.free) {
        throw Warning("The block #" + std::to_string(blockPos) + " is empty!");
    }
    block.bits.resize(superblock.blockSize);
    file.read(block.bits.data(), superblock.blockSize);

    return block.bits;
}
