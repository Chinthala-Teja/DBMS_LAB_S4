#include "BlockBuffer.h"
#include<cstdio>
//#include <cstdlib>
#include <cstring>

BlockBuffer::BlockBuffer(int blockNum){
    this->blockNum = blockNum;
}
RecBuffer::RecBuffer(int blockNum) : BlockBuffer::BlockBuffer(blockNum) {}



int BlockBuffer::getHeader(struct HeadInfo *head){

  unsigned char *bufferPtr;
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;   // return any errors that might have occured in the process
  }

    unsigned char buff[BLOCK_SIZE];
    
    Disk::readBlock(buff, this->blockNum);

    //memcpy(&head->blockType, buff , 4);
    // memcpy(&head->pblock, buff + 4, 4);
    memcpy(&head->lblock, buff + 8, 4);
    memcpy(&head->rblock, buff + 12, 4);
    memcpy(&head->numEntries, buff + 16, 4);
    memcpy(&head->numAttrs, buff + 20, 4);
    memcpy(&head->numSlots, buff + 24, 4);
    //memcpy(&head->reserved, buff + 28, 4);
    return SUCCESS;

}

int RecBuffer::getRecord(union Attribute *rec, int slotNum) {


  unsigned char *bufferPtr;
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;   // return any errors that might have occured in the process
  } 
    unsigned char buff[BLOCK_SIZE];
    Disk::readBlock(buff, this->blockNum);

    struct HeadInfo head;
    this->getHeader(&head);

    int attrCount = head.numAttrs;
    int slotCount= head.numSlots;
    int recordSize = attrCount * ATTR_SIZE;

    int offset  = HEADER_SIZE + slotCount + slotNum * recordSize;
    
    unsigned char *slotPointer = buff + offset;
    //printf("offset : %d\n",offset);
    memcpy(rec, slotPointer, recordSize);
    return SUCCESS;
}

int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char **bufferPtr){

    int bufferNum = StaticBuffer::getBufferNum(this->blockNum);

    if(bufferNum == E_BLOCKNOTINBUFFER){
      bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);
  
      if(bufferNum == E_OUTOFBOUND){
        return E_OUTOFBOUND;
      }

      Disk::readBlock(StaticBuffer::blocks[bufferNum],this->blockNum);

    }
    *bufferPtr = StaticBuffer::blocks[bufferNum];
    return SUCCESS;
}
