#include "StaticBuffer.h"

unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];

StaticBuffer::StaticBuffer(){

    for(int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++){

        metainfo[bufferIndex].free = true;
        metainfo[bufferIndex].dirty = false;
        metainfo[bufferIndex].blockNum = -1;
        metainfo[bufferIndex].timeStamp = -1;
    }
}

StaticBuffer::~StaticBuffer(){

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
    if(allocatedBuffer == -1){
        int largeTimestamp = -2, largeTSIndex = 0;
        for(int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++){

            if(metainfo[bufferIndex].timeStamp > largeTimestamp){
                
                largeTSIndex = bufferIndex;
                largeTimestamp = metainfo[bufferIndex].timeStamp;
            }
        }
        if(metainfo[largeTSIndex].dirty == true){
            Disk::writeBlock(blocks[largeTSIndex], metainfo[largeTSIndex].blockNum);
            allocatedBuffer = largeTSIndex;
        }
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

        if(metainfo[bufferIndex].blockNum == blockNum){
            
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
