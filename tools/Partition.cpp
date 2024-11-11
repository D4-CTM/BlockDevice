#include "Partition.hpp"

const void Disk_Partitioner::create(const std::string &partitionName, size_t &block_Cant, size_t &block_Size)
{
    std::ofstream writer(ROOT + "/" + partitionName, std::ios::binary);

    if (writer.is_open()) 
    {
        writer.write(reinterpret_cast<char *>(&block_Cant), sizeof(size_t));
        writer.write(reinterpret_cast<char *>(&block_Size), sizeof(size_t));

        bool free = true;
        unsigned char* bytes = new unsigned char[block_Size];;

        for (int i = 0; i < block_Size; i++) {
            bytes[i] = '`';
        }

        for (int i = 0; i < block_Cant; i++) {
            writer.write(reinterpret_cast<char *>(&free), sizeof(bool));
            writer.write(reinterpret_cast<char *>(&bytes), sizeof(unsigned char) * (block_Size));
        }

        if (bytes) {
            delete [] bytes;
            bytes = nullptr;
        }

        writer.close();
    }

}
/*
*   we can use \0 to indicate the end of the char
*/
const bool Disk_Partitioner::write(const int &blockPos, const std::vector<unsigned char>& text)
{
    if (partitionName.empty()) {
        std::cerr << AnsiCodes::RED << "ERROR: please selcet a partition first" << '\n';
        return false;
    }

    if (blockPos > block_Cant) {
        std::cerr << AnsiCodes::RED << "ERROR: " << partitionName << " only has " << block_Cant << " blocks!" << '\n';
        return false;
    } else if (blockPos == block_Cant) {
        std::cerr << AnsiCodes::RED << "ERROR: we start counting the blocks from 0 to " << (block_Cant - 1) << '\n';
        return false;
    }

    if (block_Size < text.size()) {
        std::cerr << AnsiCodes::BRIGHT_MAGENTA << "WARNING: you'll only save " << (block_Size) << " chars!" << '\n' << '\n';
    }

    std::fstream writer(ROOT + "/" + partitionName, std::ios::in | std::ios::binary | std::ios::out);
    if (!writer.is_open()) {
        std::cerr << AnsiCodes::RED << "ERROR: the file could't been open!" << '\n';
        return false;
    }

    const int writtingPos = initialBlockPosition + ((block_Size + 1) * blockPos);
    writer.seekp(writtingPos, std::ios::beg);

    bool free = false;
    writer.write(reinterpret_cast<char *>(&free), sizeof(bool));
    unsigned char data[block_Size];
    
    for (int i = 0; i < block_Size; i++) {

        if (i < text.size()) {
            data[i] = text[i];
        } else data[i] = '`';

    }

    writer.write(reinterpret_cast<char *>(&data), sizeof(unsigned char) * (block_Size));

    writer.close();
    return true;
}

const void Disk_Partitioner::select(const std::string &_partitionName)
{
    std::ifstream reader(ROOT + "/" + _partitionName, std::ios::binary);

    if (reader.is_open()) {
        reader.read(reinterpret_cast<char *>(&block_Cant), sizeof(size_t));
        reader.read(reinterpret_cast<char *>(&block_Size), sizeof(size_t));
        partitionName = _partitionName;
        reader.close();
    }
}

const std::vector<unsigned char> Disk_Partitioner::readBlock(int &blockPos)
{
    std::vector<unsigned char> cArray;
    if (partitionName.empty()) {
        std::cerr << AnsiCodes::RED << "ERROR: please select a partition first" << '\n';
        return cArray;
    }

    if (blockPos > block_Cant) {
        std::cerr << AnsiCodes::RED << "ERROR: " << partitionName << " only has " << block_Cant << " blocks!" << '\n';
        return cArray;
    } else if (blockPos == block_Cant) {
        std::cerr << AnsiCodes::RED << "ERROR: we start counting the blocks from 0 to " << (block_Cant - 1) << '\n';
        return cArray;
    }

    std::ifstream reader(ROOT + "/" + partitionName, std::ios::binary);

    if (!reader.is_open()) {
        std::cerr << AnsiCodes::RED << "ERROR: " << partitionName << " couldn't open" << '\n';
        return cArray;
    }
    const int readingPos = initialBlockPosition + ((block_Size + 1) * blockPos);
    reader.seekg(readingPos, std::ios::beg);

    bool free;
    reader.read(reinterpret_cast<char *>(&free), sizeof(bool));

    if (free) {
        std::cerr << AnsiCodes::RED << "ERROR: you haven't write anything on the block #" << blockPos << '\n';
        return cArray;
    }

    unsigned char data[block_Size];
    reader.read(reinterpret_cast<char *>(&data), sizeof(unsigned char) * (block_Size));
    reader.close();

    for (int i = 0; i < block_Size; i++) {
        if (data[i] != '`') {
            cArray.push_back(data[i]);
        }
    }

    return cArray;
}

const void Disk_Partitioner::info()
{
    std::cout << "|----------------------------------|" << '\n';
    std::cout << "| Partition name: " << partitionName << '\n';
    std::cout << "| Partition size: " << (2 * sizeof(size_t)) + (block_Cant * (block_Size + 1)) << '\n';
    std::cout << "| Block quantity: " << block_Cant << '\n';
    std::cout << "| Blocks size: " << (block_Size + 1) << '\n';
    std::cout << "|----------------------------------|" << '\n';
}
