#include "OpenRelTable.h"
#include<stdlib.h>
#include <cstring>

OpenRelTable::OpenRelTable() {

  // initialize relCache and attrCache with nullptr
  for (int i = 0; i < MAX_OPEN; ++i) {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
  }


   RecBuffer relCatBlock(RELCAT_BLOCK);

   Attribute relCatRecord[RELCAT_NO_ATTRS];
   relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_RELCAT);

   struct RelCacheEntry relCacheEntry;
   RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
   relCacheEntry.recId.block = RELCAT_BLOCK;
   relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT;

   RelCacheTable::relCache[RELCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
   *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntry;






   RecBuffer attrRelCatBlock(RELCAT_BLOCK);
   
   Attribute attrRelCatRecord[RELCAT_NO_ATTRS];
   attrRelCatBlock.getRecord(attrRelCatRecord, RELCAT_SLOTNUM_FOR_ATTRCAT);

   struct RelCacheEntry attrRelCacheEntry;
   RelCacheTable::recordToRelCatEntry( attrRelCatRecord, &attrRelCacheEntry.relCatEntry);
    attrRelCacheEntry.recId.block = RELCAT_BLOCK;
    attrRelCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;

    RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[ATTRCAT_RELID]) = attrRelCacheEntry;




     RecBuffer studentRelCatBlock(RELCAT_BLOCK);

    Attribute studentRelCatRecord[RELCAT_NO_ATTRS];
    studentRelCatBlock.getRecord(studentRelCatRecord, 2);

    struct RelCacheEntry studentRelCacheEntry;
    RelCacheTable::recordToRelCatEntry(studentRelCatRecord, &studentRelCacheEntry.relCatEntry);
    studentRelCacheEntry.recId.block = RELCAT_BLOCK;
    studentRelCacheEntry.recId.slot = 2;

    // Allocate memory on the heap for the cache entry
    RelCacheTable::relCache[2] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[2]) = studentRelCacheEntry;


  RecBuffer attrCatBlock0(ATTRCAT_BLOCK);

  Attribute attrCatRecord0[ATTRCAT_NO_ATTRS];

  AttrCacheEntry *attrCacheHead0 = nullptr;
  AttrCacheEntry *attrCachePrev0 = nullptr;

  for(int i=0; i<=5; i++){

    struct AttrCacheEntry *attrCacheEntry0 = ( struct AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));
    attrCatBlock0.getRecord(attrCatRecord0, i);
    
    AttrCacheTable::recordToAttrCatEntry(attrCatRecord0, &attrCacheEntry0->attrCatEntry);
    attrCacheEntry0->recId.block = ATTRCAT_BLOCK;
    attrCacheEntry0->recId.slot = i;
    attrCacheEntry0->next = nullptr;

    if(attrCachePrev0 == nullptr){
      attrCacheHead0 = attrCacheEntry0;
    }
    else{
      attrCachePrev0->next = attrCacheEntry0;
    }
    attrCachePrev0 = attrCacheEntry0;

    //if(i==5) attrCacheEntry0->next = nullptr;
  }



AttrCacheTable::attrCache[RELCAT_RELID] = attrCacheHead0;


  RecBuffer attrCatBlock1(ATTRCAT_BLOCK);
  Attribute attrCatRecord1[ATTRCAT_NO_ATTRS];

  AttrCacheEntry *attrCacheHead1 = nullptr;
  AttrCacheEntry *attrCachePrev1 = nullptr;

  for(int i = 6; i <= 11; i++){

    struct AttrCacheEntry *attrCacheEntry1 = (struct AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));
    attrCatBlock1.getRecord(attrCatRecord1, i);
     AttrCacheTable::recordToAttrCatEntry(attrCatRecord1, &attrCacheEntry1->attrCatEntry);
    attrCacheEntry1->recId.block = ATTRCAT_BLOCK;
    attrCacheEntry1->recId.slot = i;
    attrCacheEntry1->next = nullptr;

    if(attrCachePrev1 == nullptr){
      attrCacheHead1 = attrCacheEntry1;
    }
    else{
      attrCachePrev1->next = attrCacheEntry1;
    }
    attrCachePrev1 = attrCacheEntry1;
  }

    AttrCacheTable::attrCache[ATTRCAT_RELID] = attrCacheHead1;



   RecBuffer studentAttrCatBlock(ATTRCAT_BLOCK);
  Attribute studentAttrCatRecord[ATTRCAT_NO_ATTRS];

  AttrCacheEntry *studentAttrCacheHead = nullptr;
  AttrCacheEntry *studentAttrCachePrev = nullptr;

  for(int i = 12; i <= 17; i++){

    AttrCacheEntry *attrCacheEntry2 = ( AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));
    studentAttrCatBlock.getRecord(studentAttrCatRecord, i);
     AttrCacheTable::recordToAttrCatEntry(studentAttrCatRecord, &attrCacheEntry2->attrCatEntry);
    attrCacheEntry2->recId.block = ATTRCAT_BLOCK;
    attrCacheEntry2->recId.slot = i;
    attrCacheEntry2->next = nullptr;

    if(studentAttrCachePrev == nullptr){
      studentAttrCacheHead = attrCacheEntry2;
    }
    else{
      studentAttrCachePrev->next = attrCacheEntry2;
    }
      studentAttrCachePrev = attrCacheEntry2;
  }

    AttrCacheTable:: attrCache[2] = studentAttrCacheHead;

}


  OpenRelTable::~OpenRelTable(){

    for(int i=0; i < MAX_OPEN; ++i){
        
        if(RelCacheTable::relCache[i]){
        free(RelCacheTable::relCache[i]);
        free(AttrCacheTable::attrCache[i]);
        }
        
    }
  }
