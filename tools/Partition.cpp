#include "Partition.hpp"

const void Disk_Partitioner::createPartition(const std::string &partitionName, size_t &block_Cant, size_t &block_Size)
{
    std::ofstream writer(partitionName, std::ios::binary);

    if (writer.is_open()) 
    {
        writer.write(reinterpret_cast<char *>(&block_Cant), sizeof(size_t));
        writer.write(reinterpret_cast<char *>(&block_Size), sizeof(size_t));

        bool free;
        char* bytes;
        for (int i = 0; i < block_Cant; i++) {
            bytes = new char[block_Size - 1];
            writer.write(reinterpret_cast<char *>(&free), sizeof(bool));
            writer.write(reinterpret_cast<char *>(&bytes), sizeof(char) * (block_Size - 1));
            if (bytes) {
                delete [] bytes;
                bytes = nullptr;
            }
        }

        writer.close();
    }

}

const void Disk_Partitioner::readPartition(const std::string &partitionName)
{

}
