#ifndef __Disk_Partitioner_HPP__
#define __Disk_Partitioner_HPP__
#include <iostream>
#include <fstream>

class Disk_Partitioner {
private:
    const int initialBlockPosition = 2 * (sizeof(size_t));
    size_t block_Cant;
    size_t block_Size;
public:
    Disk_Partitioner() 
    : block_Cant(0), block_Size(0)
    {}

    const void createPartition(const std::string& partitionName, size_t& block_Cant, size_t& block_Size);
    const void selectPartition(const std::string& partitionName);
    const void readPartition(const std::string& partitionName);

    constexpr inline size_t getBlockCant() { return block_Cant; }
    constexpr inline size_t getBlockSize() { return block_Size; }

    ~Disk_Partitioner()
    {}

};

#endif  //__Disk_Partitioner_HPP__