#include "BlockBuffer.h"
#include<cstdio>
//#include <cstdlib>
#include <cstring>

BlockBuffer::BlockBuffer(int blockNum){
    this->blockNum = blockNum;
}
RecBuffer::RecBuffer(int blockNum) : BlockBuffer::BlockBuffer(blockNum) {}

RecBuffer::RecBuffer() : BlockBuffer('R'){}


int BlockBuffer::getHeader(struct HeadInfo *head){

  unsigned char *bufferPtr;
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;   // return any errors that might have occured in the process
  }

    // unsigned char buff[BLOCK_SIZE];
    
    // Disk::readBlock(buff, this->blockNum);

    memcpy(&head->blockType, bufferPtr , 4);
    memcpy(&head->pblock, bufferPtr + 4, 4);
    memcpy(&head->lblock, bufferPtr + 8, 4);
    memcpy(&head->rblock, bufferPtr + 12, 4);
    memcpy(&head->numEntries, bufferPtr + 16, 4);
    memcpy(&head->numAttrs, bufferPtr + 20, 4);
    memcpy(&head->numSlots, bufferPtr + 24, 4);
    //memcpy(&head->reserved, buff + 28, 4);
    return SUCCESS;

}

int RecBuffer::getRecord(union Attribute *rec, int slotNum) {


  unsigned char *bufferPtr;
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;   // return any errors that might have occured in the process
  } 
    // unsigned char buff[BLOCK_SIZE];
    // Disk::readBlock(buff, this->blockNum);

    struct HeadInfo head;
    this->getHeader(&head);

    int attrCount = head.numAttrs;
    int slotCount= head.numSlots;


    if(slotNum >= slotCount || slotNum < 0)
      return E_OUTOFBOUND;


    int recordSize = attrCount * ATTR_SIZE;

    int offset  = HEADER_SIZE + slotCount + slotNum * recordSize;
    
    unsigned char *slotPointer = bufferPtr + offset;
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
// STAGE - 7

int BlockBuffer::setHeader(struct HeadInfo *head){

    unsigned char *bufferPtr;

    int ret = loadBlockAndGetBufferPtr(&bufferPtr);

    if(ret != SUCCESS)
      return SUCCESS;
    
    struct HeadInfo *bufferHeader = (struct HeadInfo *)bufferPtr;

    bufferHeader->blockType = head->blockType;
    bufferHeader->pblock = head->pblock;
    bufferHeader->lblock = head->lblock;
    bufferHeader->rblock = head->rblock;
    bufferHeader->numEntries = head->numEntries;
    bufferHeader->numAttrs = head->numAttrs;
    bufferHeader->numSlots = head->numSlots;

    int re = StaticBuffer::setDirtyBit(this->blockNum);

    if(re != SUCCESS)
      printf("There is an error in the Code");
    
    return re;
}

int BlockBuffer::setBlockType(int blockType){

  unsigned char *bufferPtr;
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);

  if(ret != SUCCESS)
    return ret;
  
    *((int32_t *)bufferPtr) = blockType;

    StaticBuffer::blockAllocMap[this->blockNum] = blockType;

    int re = StaticBuffer::setDirtyBit(this->blockNum);

    if(re != SUCCESS)
      printf("There is an error in the Code");
    
    return re;
}

int BlockBuffer::getFreeBlock(int blockType){

  int blockNum = 0;
  for(; blockNum < DISK_BLOCKS; blockNum++){

    if(StaticBuffer::blockAllocMap[blockNum] == UNUSED_BLK)
          break;
  }

  if(blockNum == DISK_BLOCKS)
      return E_DISKFULL;
  
  this->blockNum = blockNum;

  int bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);

  HeadInfo head;
  head.pblock = -1;
  head.lblock = -1;
  head.rblock = -1;
  head.numAttrs = 0;
  head.numEntries = 0;
  head.numSlots = 0;
  
  setHeader(&head);

  setBlockType(blockType);

  return blockNum;
}

BlockBuffer::BlockBuffer(char blockType){

    int bType;
  if(blockType == 'R')
        bType = REC;
  else if(blockType == 'I')
    bType = IND_INTERNAL;
  else 
    bType = IND_LEAF;
  
  int blockNum = getFreeBlock(bType);

  if(blockNum == E_DISKFULL || blockNum < 0){
      printf("The Disk is Full : blockNum = %d", blockNum);
      return;
  }
  this->blockNum = blockNum;
    

}

// STAGE - 7

int RecBuffer::setSlotMap(unsigned char *slotMap){

  unsigned char *bufferPtr;
  int ret  = loadBlockAndGetBufferPtr(&bufferPtr);

  if(ret != SUCCESS)
    return SUCCESS;

    HeadInfo head;
    this->getHeader(&head);

    int numSlots = head.numSlots;

    //unsigned char buff[BLOCK_SIZE];

    memcpy(bufferPtr + HEADER_SIZE ,slotMap,numSlots);

    ret = StaticBuffer::setDirtyBit(this->blockNum);
  
    return ret ;
}

int BlockBuffer::getBlockNum(){
  return this->blockNum;
}
