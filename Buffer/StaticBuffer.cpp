#include "StaticBuffer.h"

unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];
unsigned char StaticBuffer::blockAllocMap[DISK_BLOCKS];

StaticBuffer::StaticBuffer(){

    for(int i = 0, k = 0; i <= 3; i++){

        unsigned char buffer[BLOCK_SIZE];
        Disk::readBlock(buffer, i);

        for(int j = 0; j < BLOCK_SIZE; j++, k++){
            blockAllocMap[k] = buffer[j]; 
        }

    }

    for(int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++){

        metainfo[bufferIndex].free = true;
        metainfo[bufferIndex].dirty = false;
        metainfo[bufferIndex].blockNum = -1;
        metainfo[bufferIndex].timeStamp = -1;
    }
}

StaticBuffer::~StaticBuffer(){

    for(int i = 0, k = 0; i <= 3; i++){

        unsigned char buffer[BLOCK_SIZE];

        for(int j = 0; j < BLOCK_SIZE; j++, k++){
            buffer[j] = blockAllocMap[k]; 
        }

        Disk::writeBlock(buffer, i);

    }

    
    for(int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++){

        if(metainfo[bufferIndex].free == false && metainfo[bufferIndex].dirty == true){
            
            Disk::writeBlock(blocks[bufferIndex],metainfo[bufferIndex].blockNum);
        }
}
}
 
// STAGE - 6 ( modified)

int StaticBuffer::getFreeBuffer(int blockNum){

    if(blockNum < 0 || blockNum > DISK_BLOCKS){
        return E_OUTOFBOUND;
    }

    // for(int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++){

    //     if(StaticBuffer::metainfo[bufferIndex].free == false){
    //         StaticBuffer::metainfo[bufferIndex].timeStamp++;
    //  }
    // }
    int allocatedBuffer = -1;

    for(int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++){

        if(metainfo[bufferIndex].free == true){
            allocatedBuffer = bufferIndex;
            break;
        }
    }
        int largeTimestamp = 0, largeTSIndex = 0;
        for(int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++){

            if(metainfo[bufferIndex].timeStamp > largeTimestamp){
                
                largeTSIndex = bufferIndex;
                largeTimestamp = metainfo[bufferIndex].timeStamp;
            }
        }
        if(metainfo[largeTSIndex].dirty == true  && allocatedBuffer == -1) {
            Disk::writeBlock(blocks[largeTSIndex], metainfo[largeTSIndex].blockNum);
            allocatedBuffer = largeTSIndex;
        }
    

    metainfo[allocatedBuffer].free = false;
    metainfo[allocatedBuffer].dirty = false;
    metainfo[allocatedBuffer].blockNum = blockNum;
    metainfo[allocatedBuffer].timeStamp = 0;

    return allocatedBuffer;
}

int StaticBuffer::getBufferNum(int blockNum){

    if(blockNum < 0 || blockNum > DISK_BLOCKS){
        return E_OUTOFBOUND;
    }

    for(int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++){

        if(metainfo[bufferIndex].blockNum == blockNum && metainfo[bufferIndex].free == false){
            
            return bufferIndex;
            
        }
    }
    return E_BLOCKNOTINBUFFER;

}


// STAGE - 6

int StaticBuffer::setDirtyBit(int blockNum){

    int buffIndex = getBufferNum(blockNum);

    if(buffIndex == E_BLOCKNOTINBUFFER)
        return E_BLOCKNOTINBUFFER;
    
    if(blockNum == E_OUTOFBOUND)
        return E_OUTOFBOUND;
    else
        metainfo[buffIndex].dirty = true;
    
    return SUCCESS;
}
