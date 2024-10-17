#ifndef __PARTIONER__
#define __PARTIONER__
#include <fstream>

class Disk_partioner {
public:
    void createPartition(int blocks, int blocks_size) {
        
    }

private:
    int calculate_partition_size(int blocks, int blocks_size) 
    { return blocks * blocks_size; }
};

#endif //__PARTIONER__