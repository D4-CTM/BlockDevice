#include "Partition.hpp"

const void Disk_Partitioner::create(const std::string &partitionName, size_t &block_Cant, size_t &block_Size)
{
    std::ofstream writer(ROOT + "/" + partitionName, std::ios::binary);

    if (writer.is_open()) 
    {
        writer.write(reinterpret_cast<char *>(&block_Cant), sizeof(size_t));
        writer.write(reinterpret_cast<char *>(&block_Size), sizeof(size_t));

        bool free;
        unsigned char* bytes;
        for (int i = 0; i < block_Cant; i++) {
            bytes = new unsigned char[block_Size - 1];
            writer.write(reinterpret_cast<char *>(&free), sizeof(bool));
            writer.write(reinterpret_cast<char *>(&bytes), sizeof(unsigned char) * (block_Size - 1));
            if (bytes) {
                delete [] bytes;
                bytes = nullptr;
            }
        }

        writer.close();
    }

}

const void Disk_Partitioner::select(const std::string& _partitionName)
{
    std::ifstream reader(ROOT + "/" + _partitionName, std::ios::binary);

    if (reader.is_open()) {
        reader.read(reinterpret_cast<char *>(&block_Cant), sizeof(size_t));
        reader.read(reinterpret_cast<char *>(&block_Size), sizeof(size_t));
        partitionName = _partitionName;
        reader.close();
    }
}

const void Disk_Partitioner::info()
{
    std::cout << "|----------------------------------|" << '\n';
    std::cout << "| Partition name: " << partitionName << '\n';
    std::cout << "| Partition size: " << (2 * sizeof(size_t)) + (block_Cant * (block_Size)) << '\n';
    std::cout << "| Block quantity: " << block_Cant << '\n';
    std::cout << "| Blocks size: " << block_Size << '\n';
    std::cout << "|----------------------------------|" << '\n';
}
