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


    if(slotNum >= slotCount || slotNum < 0)
      return E_OUTOFBOUND;


    int recordSize = attrCount * ATTR_SIZE;

    int offset  = HEADER_SIZE + slotCount + slotNum * recordSize;
    
    unsigned char *slotPointer = buff + offset;
    //printf("offset : %d\n",offset);
    memcpy(rec, slotPointer, recordSize);
    return SUCCESS;
}

int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char **bufferPtr){

    int bufferNum = StaticBuffer::getBufferNum(this->blockNum);

    if(bufferNum != E_BLOCKNOTINBUFFER){

      StaticBuffer::metainfo[bufferNum].timeStamp = 0;

      for(int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++){

        if(StaticBuffer::metainfo[bufferIndex].free == false){

            if(bufferIndex == bufferNum)
              continue;
            StaticBuffer::metainfo[bufferIndex].timeStamp++;
     }
    }
  }
  else{
      bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);
  
      if(bufferNum == E_OUTOFBOUND){
        return E_OUTOFBOUND;
      }
      Disk::readBlock(StaticBuffer::blocks[bufferNum],this->blockNum);
    }


  *bufferPtr = StaticBuffer::blocks[bufferNum];
  return SUCCESS;
}

// STAGE 4

int RecBuffer::getSlotMap(unsigned char *slotMap){

  unsigned char *bufferPtr;

  int ret = loadBlockAndGetBufferPtr(&bufferPtr);

  if(ret != SUCCESS)
    return ret;

  struct HeadInfo head;
  this->getHeader(&head);

  int slotCount = head.numSlots;
  unsigned char *slotMapInBuffer = bufferPtr + HEADER_SIZE;

  memcpy(slotMap, slotMapInBuffer, slotCount);
  
  return SUCCESS;
  
}

 // STAGE - 5

int compareAttrs(union Attribute attr1, union Attribute attr2, int attrType){

  double diff;

  if(attrType == STRING)
    diff = strcmp(attr1.sVal, attr2.sVal);
  else
    diff = attr1.nVal - attr2.nVal;

  if(diff > 0)
    return 1;
  else if(diff == 0)
    return 0;
  else
    return -1;
}

// STAGE - 6

int RecBuffer::setRecord(union Attribute *rec, int slotNum){

    unsigned char *bufferPtr;

    int ret = loadBlockAndGetBufferPtr(&bufferPtr);

    if(ret != SUCCESS)
      return ret;
    
    HeadInfo head;
    this->getHeader(&head);

    int attributeCount = head.numAttrs;
    int slotCount = head.numSlots;

    if(slotNum >= slotCount || slotNum < 0)
      return E_OUTOFBOUND;

    int recordSize = attributeCount * ATTR_SIZE;

    int offset = HEADER_SIZE + slotCount + (recordSize * slotNum);

    unsigned char *slotPointer = bufferPtr + offset;

    memcpy( slotPointer, rec, recordSize);

    int re = StaticBuffer::setDirtyBit(this->blockNum);

    if(re !=SUCCESS)
        printf("There is an issue in the code");
    
  return SUCCESS;


}
