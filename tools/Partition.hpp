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
    void setPartitionConfiguration(unsigned int blocks_cant, unsigned int blocks_size);
    
    void createPartition(const char* fileName);
//Private functions used within the code
private:
    int calculate_partition_size(int blocks, int blocks_size) 
    { return blocks * blocks_size; }
};

#endif //__PARTIONER__