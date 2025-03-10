#include "OpenRelTable.h"
#include<stdlib.h>
#include <cstring>


AttrCacheEntry *createAttrCacheEntryList(int size) {
  AttrCacheEntry *head = nullptr, *curr = nullptr;
  head = curr = (AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));
  size--;
  while (size--) {
    curr->next = (AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));
    curr = curr->next;
  }
  curr->next = nullptr;
  return head;
}

OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

OpenRelTable::OpenRelTable() {

  // initialize relCache and attrCache with nullptr
  for (int i = 0; i < MAX_OPEN; ++i) {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
    OpenRelTable::tableMetaInfo[i].free = true;
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


    OpenRelTable::tableMetaInfo[0].free = false;
    OpenRelTable::tableMetaInfo[1].free = false;

    strcpy(tableMetaInfo[0].relName, "RELATIONCAT");
    strcpy(tableMetaInfo[1].relName, "ATTRIBUTECAT");

}


  OpenRelTable::~OpenRelTable(){

    for(int i = 2; i < MAX_OPEN; ++i){
        
      if (!tableMetaInfo[i].free) {
        OpenRelTable::closeRel(i); // we will implement this function later
      }
        
    }
    
    if(RelCacheTable::relCache[ATTRCAT_RELID]->dirty == true){

      RelCatEntry relCatBuf;
      RelCacheTable::getRelCatEntry(RELCAT_RELID, &relCatBuf);

      Attribute RelCatRecord[RELCAT_NO_ATTRS];
      RelCacheTable::relCatEntryToRecord(&relCatBuf, RelCatRecord);
      RecId rec_id = RelCacheTable::relCache[ATTRCAT_RELID]->recId;
      RecBuffer relCatBlock(rec_id.block);
      relCatBlock.setRecord(RelCatRecord, rec_id.slot);
    }

    free(RelCacheTable::relCache[ATTRCAT_RELID]);
    
    if(RelCacheTable::relCache[RELCAT_RELID]->dirty == true){

      RelCatEntry relCatBuf;
      RelCacheTable::getRelCatEntry(RELCAT_RELID, &relCatBuf);

      Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
      RelCacheTable::relCatEntryToRecord(&relCatBuf, attrCatRecord);
      RecId rec_id = RelCacheTable::relCache[RELCAT_RELID]->recId;
      RecBuffer relCatBlock(rec_id.block);
      relCatBlock.setRecord(attrCatRecord, rec_id.slot);
    }
    
    free(RelCacheTable::relCache[RELCAT_RELID]);

    for(int i = 0 ; i < ATTRCAT_NO_ATTRS; i++){

      AttrCacheEntry *attrCacheBuf = AttrCacheTable::attrCache[RELCAT_RELID] , *next1= NULL;
      next1 = attrCacheBuf->next;
      if(attrCacheBuf->dirty == true){
        RecId recid = attrCacheBuf->recId;
        RecBuffer attrBlock(recid.block);
        Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
        AttrCacheTable::attrCatEntryToRecord(&(attrCacheBuf->attrCatEntry), attrCatRecord);
        attrBlock.setRecord(attrCatRecord, recid.slot);
      }
      
      free(attrCacheBuf);
      attrCacheBuf = next1;

      AttrCacheEntry *attrCacheBuffer = AttrCacheTable::attrCache[ATTRCAT_RELID], *next2 = NULL;
      next2 = attrCacheBuffer->next;
      if(attrCacheBuf->dirty == true){
        RecId recid = attrCacheBuffer->recId;
        RecBuffer attrBlock(recid.block);
        Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
        AttrCacheTable::attrCatEntryToRecord(&(attrCacheBuffer->attrCatEntry), attrCatRecord);
        attrBlock.setRecord(attrCatRecord, recid.slot);
      }
      free(attrCacheBuffer);
      attrCacheBuffer = next2;

    }

  }

  // STAGE - 5
  int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

    for(int i = 0; i < MAX_OPEN ; i++){
      
      if(strcmp(tableMetaInfo[i].relName, relName) == 0 && tableMetaInfo[i].free == false){
          return i;
      }
    }

  return E_RELNOTOPEN;
 }


 int OpenRelTable::getFreeOpenRelTableEntry() {

  for(int i = 0; i < MAX_OPEN; i++){

    if(tableMetaInfo[i].free == true){
      return i;
    }
  }
    return E_CACHEFULL;
}

int OpenRelTable::openRel(char relName[ATTR_SIZE]) {

  int relid = getRelId(relName);
  if(relid !=E_RELNOTOPEN){
    return relid;
  }

  int relId = getFreeOpenRelTableEntry();

  if(relId == E_CACHEFULL)
      return E_CACHEFULL;
  

  RelCacheTable::resetSearchIndex(RELCAT_RELID);

    Attribute record;
    strcpy(record.sVal, relName);

  RecId relcatRecId = BlockAccess::linearSearch(RELCAT_RELID, RELCAT_ATTR_RELNAME, record, EQ);
  if (relcatRecId.block == -1 && relcatRecId.slot == -1) {
    return E_RELNOTEXIST;
  }

  RecBuffer relBuf(relcatRecId.block);
  Attribute relRec[RELCAT_NO_ATTRS];
  relBuf.getRecord(relRec, relcatRecId.slot);

  RelCacheEntry *relCacheBuffer = NULL;

	relCacheBuffer = (RelCacheEntry*) malloc (sizeof(RelCacheEntry));
  RelCacheTable::recordToRelCatEntry(relRec, &(relCacheBuffer->relCatEntry));

  relCacheBuffer->recId = relcatRecId;
  RelCacheTable::relCache[relId] = relCacheBuffer;



  AttrCacheEntry *listHead = NULL;

  AttrCacheEntry *attrCacheBuffer = NULL;

  //attrCacheBuffer = (AttrCacheEntry *) malloc(sizeof(AttrCacheEntry));
  RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

  int numberOfAttributes = RelCacheTable::relCache[relId]->relCatEntry.numAttrs;
  listHead = createAttrCacheEntryList(numberOfAttributes);
  attrCacheBuffer = listHead;
  
  for(int i = 0; i < numberOfAttributes; i++){

    RecId attrCatRecId = BlockAccess::linearSearch(ATTRCAT_RELID, RELCAT_ATTR_RELNAME, record, EQ);

    RecBuffer attrbuf(attrCatRecId.block);
    Attribute attrRec[ATTRCAT_NO_ATTRS];
    attrbuf.getRecord(attrRec, attrCatRecId.slot);

    AttrCacheTable::recordToAttrCatEntry(attrRec, &attrCacheBuffer->attrCatEntry);
    
    attrCacheBuffer->recId = attrCatRecId;

    if(listHead == NULL)
        listHead = attrCacheBuffer;
    else
      attrCacheBuffer = attrCacheBuffer->next;

  }

  AttrCacheTable::attrCache[relId]=listHead;

  tableMetaInfo[relId].free = false;
  strcpy(tableMetaInfo[relId].relName,relName);


  return relId;


}


 // STAGE - 7 (Modified)

int OpenRelTable::closeRel(int relId){

  if(relId == RELCAT_RELID || relId == ATTRCAT_RELID)
    return E_NOTPERMITTED;

  if(relId < 0 && relId >= MAX_OPEN)
      return E_OUTOFBOUND;
  
  if(tableMetaInfo[relId].free == true)
    return E_RELNOTOPEN;

  if(RelCacheTable::relCache[relId]->dirty == true){

    RelCatEntry relCatBuf = RelCacheTable::relCache[relId]->relCatEntry;
    RecId recId = RelCacheTable::relCache[relId]->recId;
    Attribute rec[RELCAT_NO_ATTRS];

    RelCacheTable::relCatEntryToRecord(&relCatBuf, rec);

    RecBuffer relCatBlock(recId.block);

    relCatBlock.setRecord(rec, recId.slot);
  }

    free(RelCacheTable::relCache[relId]);
    AttrCacheEntry *head = AttrCacheTable::attrCache[relId];
	  AttrCacheEntry *next = head->next;

	  while (next) {
      free (head);
      head = next;
      next = next->next;
	  } 
	free(head);	
    tableMetaInfo[relId].free = true;
    AttrCacheTable::attrCache[relId] = nullptr;
  RelCacheTable::relCache[relId] = nullptr;
  return SUCCESS;
}

