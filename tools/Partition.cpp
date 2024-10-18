#include "Partition.hpp"

void Disk_partioner::setPartitionConfiguration(unsigned int blocks_cant, unsigned int blocks_size)
{
    sb = new Superblock();
    sb->blocks_cant = blocks_cant;
    sb->blocks_size = blocks_size;

    blocks = new Block[blocks_cant];
    for (int i = 0; i < blocks_cant; i++) {
        blocks[i].setBlockData(blocks_size);
    }

}

void Disk_partioner::createPartition(const char *fileName)
{
    if (sb == nullptr) {
        throw std::runtime_error("The superblock data is not set!");
    }

    std::ofstream fileWriter(fileName, std::ios::binary);
    if (fileWriter.is_open()) {
        fileWriter.write(reinterpret_cast<char *>(&sb), sizeof(Superblock));
        fileWriter.write(reinterpret_cast<char *>(&blocks), sizeof(Superblock));            
        fileWriter.close();
    }
}
