#include "RelCacheTable.h"

#include <cstring>



RelCacheEntry* RelCacheTable::relCache[MAX_OPEN];

int RelCacheTable::getRelCatEntry(int relId, RelCatEntry *relCatBuf){
    
    if(relId < 0 || relId >= MAX_OPEN){
        return E_OUTOFBOUND;
    }

    if(relCache[relId] == NULL){
        return E_RELNOTOPEN;
    }
    *relCatBuf = relCache[relId]->relCatEntry;

    return SUCCESS;
    
}

void RelCacheTable::recordToRelCatEntry(union Attribute record[RELCAT_NO_ATTRS], RelCatEntry* relCatEntry){

    strcpy(relCatEntry->relName, record[RELCAT_REL_NAME_INDEX].sVal);
    relCatEntry->numAttrs = (int)record[RELCAT_NO_ATTRIBUTES_INDEX].nVal;
    relCatEntry->numRecs = (int)record[RELCAT_NO_RECORDS_INDEX].nVal;
    relCatEntry->firstBlk = (int)record[RELCAT_FIRST_BLOCK_INDEX].nVal;
    relCatEntry->lastBlk = (int)record[RELCAT_LAST_BLOCK_INDEX].nVal;
    relCatEntry->numSlotsPerBlk = (int)record[RELCAT_NO_SLOTS_PER_BLOCK_INDEX].nVal;

}


// STAGE - 4


int RelCacheTable::getSearchIndex(int relId, RecId * searchIndex){

    if(relId < 0 || relId >= MAX_OPEN){
        return E_OUTOFBOUND;
    }

    if(relCache[relId] == NULL){
        return E_RELNOTOPEN;
    }

    *searchIndex = relCache[relId]->searchIndex;

    return SUCCESS;

}

int RelCacheTable::setSearchIndex(int relId, RecId* searchIndex){

    if(relId < 0 || relId >= MAX_OPEN){
        return E_OUTOFBOUND;
    }

    if(relCache[relId] == NULL){
        return E_RELNOTOPEN;
    }
    relCache[relId]->searchIndex = *searchIndex;

    return SUCCESS;
}

int RelCacheTable::resetSearchIndex(int relId){

    if(relId < 0 || relId >= MAX_OPEN){
        return E_OUTOFBOUND;
    }

    if(relCache[relId] == NULL){
        return E_RELNOTOPEN;
    }
    RelCacheTable::relCache[relId]->searchIndex = {-1, -1};

    return SUCCESS;
}


// STAGE - 7

int RelCacheTable::setRelCatEntry(int relId, RelCatEntry * relCatBuf){

    if(relId < 0 || relId >= MAX_OPEN){
        return E_OUTOFBOUND;
    }
    if(relCache[relId] == NULL){
        return E_RELNOTOPEN;
    }

   // relCache[relId]->relCatEntry = *relcatBuf;
    memcpy(&(RelCacheTable::relCache[relId]->relCatEntry), relCatBuf, sizeof(RelCatEntry));

    relCache[relId]->dirty = true;

    return SUCCESS;
}

void RelCacheTable::relCatEntryToRecord(RelCatEntry * relCatBuf,  union Attribute record[RELCAT_NO_ATTRS]){

    strcpy(record[RELCAT_REL_NAME_INDEX].sVal, relCatBuf->relName);
    record[RELCAT_NO_ATTRIBUTES_INDEX].nVal = relCatBuf->numAttrs ;
    record[RELCAT_NO_RECORDS_INDEX].nVal = relCatBuf->numRecs ;
    record[RELCAT_FIRST_BLOCK_INDEX].nVal = relCatBuf->firstBlk ;
    record[RELCAT_LAST_BLOCK_INDEX].nVal = relCatBuf->lastBlk;
    record[RELCAT_NO_SLOTS_PER_BLOCK_INDEX].nVal =  relCatBuf->numSlotsPerBlk;
}
