#ifndef __BLOCK_DEVICE_HPP__
#define __BLOCK_DEVICE_HPP__
#include "Exceptions.h"
#include <filesystem>
#include <cstdint>

class BlockDevice {
private:
    struct Block {
        std::string bits;

        Block()
        : bits("")
        {}

        Block(const std::string& data, size_t size)
        : bits(data)
        { bits.resize(size); }

        Block(const size_t size) 
        : bits("")
        { bits.resize(size); }

        inline const size_t size() { return bits.size(); }
        inline char* data() { return bits.data(); }

        char& operator[](int pos) {
            return bits[pos];
        }

        std::string operator<<(const Block& block) {
            return bits + " " + block.bits;
        }

        std::string operator+(const Block& block) {
            return *this << block;
        }
    };
private:
    struct Header {
        size_t blockSize;
        size_t blockCount;

        Header()
        : blockSize(-1), blockCount(-1)
        {}

        Header(size_t _blockSize, size_t _blockCount)
        : blockSize(_blockSize), blockCount(_blockCount)
        {}

        void clear() {
            blockSize = -1;
            blockCount = -1;
        }

        int calculateOffsetOf(size_t blockPos) {
            return sizeof(Header) + (blockSize * blockPos);
        }        
    }__attribute((packed));
private:
    struct Inode {
        char filename[64];
        uint64_t offset[8];
        uint64_t size;
        bool free;

        void clearOffset() {
            for (int i = 0; i < 8; i++) {
                offset[i] = -1;
            }
        }

        void remove() {
            clearOffset();
        }

        void setInode(const std::string& _filename) {
            _filename.copy(filename, 64);
            clearOffset();
            free = false;
        }

        Inode()
        : size(0), free(true) 
        {
            for (int i = 0; i < 64; i++) {
                filename[i] = ' ';
            }            
            clearOffset();
        }

    }__attribute((packed));
private:
    struct Superblock {
        uint32_t initialBlock; //Where the first writing block starts
        uint32_t byteMapPos; //Where the map of free blocks starts
        uint32_t inodesInitialBlockPos;
        uint32_t inodesFinalBlockPos;
        uint32_t blocksRequiered;
        uint32_t inodesPerBlock;

        Superblock()
        : initialBlock(0), byteMapPos(0), inodesInitialBlockPos(0), inodesFinalBlockPos(0), inodesPerBlock(0)
        {}

        Superblock(uint32_t _inodesPerBlock, uint32_t inodesInitialBlock, uint32_t inodesFinalBlock, uint32_t _blocksRequiered)
        : byteMapPos(1), inodesPerBlock(_inodesPerBlock), inodesInitialBlockPos(inodesInitialBlock), inodesFinalBlockPos(inodesFinalBlock), initialBlock(inodesFinalBlock + 1), blocksRequiered(_blocksRequiered)
        {}
    }__attribute((packed));
private:
    using Path = std::filesystem::path;
    std::string deviceName;
    const Path ROOT;

    Header header;
public:
    BlockDevice()
    : ROOT("devices"), deviceName("")
    {
        if (!std::filesystem::exists(ROOT)) {
            std::filesystem::create_directory(ROOT);
        }
    }
    void showSuperblockInfo();
    void deviceInfo();

    bool create(const std::string& deviceName, size_t blockSize, size_t blockCount);
    bool close();
    bool open(const std::string& deviceName);
    bool writeBlock(size_t blockPos, const std::string& data);
    Block readBlock(size_t blockPos);
    std::pair<int, int> getInodePos(const Superblock& superblock, std::string& filename);
    void removeFile(std::string& filename);
    void writeFile(std::string& filename, std::string& text);
    void listFiles();
    void format();

    inline Path getRootDirectory() { return ROOT; }
    inline Path getDevicePath() { return (isDeviceOpen()) ? ROOT / deviceName : ROOT; }
    inline bool isDeviceOpen() { return !deviceName.empty(); }
};

#endif// __BLOCK_DEVICE_HPP__