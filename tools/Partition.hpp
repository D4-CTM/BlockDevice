#ifndef __PARTITION__
#define __PARTITION__

class Partition {
private:
    struct SuperBlock {
        unsigned char partition_name[20];
        unsigned int blocks_size;
        unsigned int blocks_cant;
    };
    SuperBlock* SB;
public:

    Partition(const unsigned char name[20], unsigned int size, unsigned int cant) {
        SB = new SuperBlock();
        SB->blocks_size = size;
        SB->blocks_cant = cant;
        for (int i = 0; i < 20; i++) {
            SB->partition_name[i] = name[i];
        }

    }



    const SuperBlock getSuperBlock()
    { return *SB; }

};

#endif //__PARTITION__
