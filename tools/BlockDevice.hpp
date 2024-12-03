#ifndef __BLOCK_DEVICE_HPP__
#define __BLOCK_DEVICE_HPP__
#include "Exceptions.h"
#include <filesystem>
#include <cstdint>

class BlockDevice {
private:
    struct Inode {
        char filename[64];
        uint64_t offset[8];
        uint64_t size;

        Inode(const std::string& _filename)
        : size(-1)
        {
            _filename.copy(filename, 64);
            for (int i = 0; i < 8; i++) {
                offset[i] = -1;
            }
        }
    };
private:
    struct Block {
        std::string bits;
        bool free;

        Block()
        : free(false), bits("")
        {}

        Block(const std::string& data, size_t size)
        : free(false), bits(data)
        { bits.resize(size); }

        Block(const size_t size) 
        : free(true), bits("")
        { bits.resize(size); }
    };
private:
    struct Superblock {
        size_t blockSize;
        size_t blockCount;

        Superblock()
        : blockSize(-1), blockCount(-1)
        {}

        Superblock(size_t _blockSize, size_t _blockCount)
        : blockSize(_blockSize), blockCount(_blockCount)
        {}

        void clear() {
            blockSize = -1;
            blockCount = -1;
        }

        size_t calculateOffsetOf(size_t blockPos) {
            return sizeof(Superblock) + ((blockSize + sizeof(bool)) * blockPos);
        }
        
    };
private:
    using Path = std::filesystem::path;

    std::string deviceName;
    const Path ROOT;

    Superblock superblock;
public:
    BlockDevice()
    : ROOT("devices"), deviceName("")
    {
        if (!std::filesystem::exists(ROOT)) {
            std::filesystem::create_directory(ROOT);
        }
    }
    void deviceInfo();

    bool create(const std::string& deviceName, size_t blockSize, size_t blockCount);
    bool close();
    bool open(const std::string& deviceName);
    bool writeBlock(size_t blockPos, const std::string& data);
    std::string readBlock(size_t blockPos);

    inline Path getRootDirectory() { return ROOT; }
    inline Path getDevicePath() { return (isDeviceOpen()) ? ROOT / deviceName : ROOT; }
    inline bool isDeviceOpen() { return !deviceName.empty(); }
};

#endif// __BLOCK_DEVICE_HPP__