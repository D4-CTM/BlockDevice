#ifndef __PARTIONER__
#define __PARTIONER__
#include <fstream>

class Disk_partioner {
//Strucs and subclasses used to create the 'disk partition' simulation file
private:
    struct Superblock {
        unsigned int blocks_size;
        unsigned int blocks_cant;
    };

    struct Block {
        void setBlockData(int bytes_cant) {
            bytes = new unsigned char[bytes_cant];
            free = true;
        }

        unsigned char* bytes;
        bool free;
    };

//private variables
private:
    Superblock* sb;
    Block* blocks;
//main methods
public:

    Disk_partioner(unsigned int blocks_cant, unsigned int blocks_size) 
    { setPartitionConfiguration(blocks_cant, blocks_size); }

    //set the data of the superblock to indicate how many blocks and how many they are gonna weight
    void setPartitionConfiguration(unsigned int blocks_cant, unsigned int blocks_size) {
        sb = new Superblock();
        sb->blocks_cant = blocks_cant;
        sb->blocks_size = blocks_size;

        blocks = new Block[blocks_cant];
        
    }

    void createPartition(const char* fileName) {
        if (sb == nullptr) {
            throw std::runtime_error("The superblock data is not set!");
        }

        std::ofstream fileWriter(fileName, std::ios::binary);
        if (fileWriter.is_open()) {
            fileWriter.write(reinterpret_cast<char *>(&sb), sizeof(Superblock));
            
            fileWriter.close();
        }
    }

//Private functions used within the code
private:
    inline int calculate_partition_size(int blocks, int blocks_size) 
    { return blocks * blocks_size; }
};

#endif //__PARTIONER__