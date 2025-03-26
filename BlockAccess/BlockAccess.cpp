#include "BlockAccess.h"
#include <iostream>
#include <cstring>


RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op) {

        RecId prevRecId; 
        RelCacheTable::getSearchIndex(relId, &prevRecId);    // get the search index of the relId 

        int block = -1, slot = -1;

        if(prevRecId.block == -1 && prevRecId.slot == -1){  // if the search index is {-1,-1}, then start search from first Block

                RelCatEntry relCatBuf;
            RelCacheTable::getRelCatEntry( relId, &relCatBuf);

            block = relCatBuf.firstBlk;
            slot = 0;
        }
        else{                                                // else increment slotno
                block = prevRecId.block;
                slot  = prevRecId.slot + 1;
        }
        RelCatEntry relCatBuffer;
	RelCacheTable::getRelCatEntry(relId, &relCatBuffer);

        while(block != -1){                              // TRAVERSE THROUGH ALL THE BLOCKS OF THAT RELATION          

                RecBuffer RecBuf(block);
                Attribute CatRecord[RELCAT_NO_ATTRS];
                RecBuf.getRecord(CatRecord, slot);

                HeadInfo head;
                RecBuf.getHeader(&head);
                
                unsigned char *slotMap =  (unsigned char *)malloc(sizeof(unsigned char) * head.numSlots);

                RecBuf.getSlotMap(slotMap);


                if(slot >= relCatBuffer.numSlotsPerBlk){                // IF SLOTS ARE OVER , GO TO head.rblock

                        block = head.rblock;
                        slot = 0;
                        continue;
                }
                if(slotMap[slot] == SLOT_UNOCCUPIED){                   // skip if the slot is free
                        slot++;
                        continue;
                }
                AttrCatEntry attrCatBuf;
                AttrCacheTable::getAttrCatEntry(relId, attrName, &attrCatBuf);

                Attribute *record =(Attribute *)malloc(sizeof(Attribute) * head.numAttrs);
                RecBuf.getRecord(record, slot);
                int attrOffset = attrCatBuf.offset;
            
                int cmpVal = compareAttrs(record[attrOffset], attrVal, attrCatBuf.attrType); // will store the difference between the attributesatBuf.offset], attrVal, attrCatBuf.attrType);
                

                if (
                (op == NE && cmpVal != 0) ||    // if op is "not equal to"
                (op == LT && cmpVal < 0) ||     // if op is "less than"
                (op == LE && cmpVal <= 0) ||    // if op is "less than or equal to"
                (op == EQ && cmpVal == 0) ||    // if op is "equal to"
                (op == GT && cmpVal > 0) ||     // if op is "greater than"
                (op == GE && cmpVal >= 0)       // if op is "greater than or equal to"
                ) {
                        RecId newIndex;
                        newIndex.block = block;
                        newIndex.slot = slot;
                        RelCacheTable::setSearchIndex(relId, &newIndex);
                        return RecId{block, slot};
                }
        slot++;
        }

    return RecId{-1,-1};
}

// STAGE - 6

int BlockAccess::renameRelation(char oldName[ATTR_SIZE], char newName[ATTR_SIZE]){

        RelCacheTable::resetSearchIndex(RELCAT_RELID);

        Attribute newRelationName;
        strcpy(newRelationName.sVal,newName);

        RecId newrelRec = linearSearch(RELCAT_RELID, RELCAT_ATTR_RELNAME , newRelationName, EQ );

        if(newrelRec.block !=-1 && newrelRec.slot != -1)
                return E_RELEXIST;
        
       RelCacheTable::resetSearchIndex(RELCAT_RELID);

       Attribute oldRelationName;
       strcpy(oldRelationName.sVal, oldName);

       RecId oldrelRec = linearSearch(RELCAT_RELID, RELCAT_ATTR_RELNAME , oldRelationName, EQ );

        if(oldrelRec.block == -1 && oldrelRec.slot == -1)
                return E_RELNOTEXIST;

        RecBuffer relCatbuffer(oldrelRec.block);
        Attribute Relrec[RELCAT_NO_ATTRS];
        relCatbuffer.getRecord(Relrec, oldrelRec.slot);

        strcpy(Relrec[RELCAT_REL_NAME_INDEX].sVal, newName);
        relCatbuffer.setRecord(Relrec,oldrelRec.slot);

        RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

        for(int i = 0; i < Relrec[RELCAT_NO_ATTRIBUTES_INDEX].nVal; i++){

                RecId attrRec = linearSearch(ATTRCAT_RELID, ATTRCAT_ATTR_RELNAME, oldRelationName, EQ);

                RecBuffer attrCatBuffer(attrRec.block);
                Attribute rec[ATTRCAT_NO_ATTRS];
                attrCatBuffer.getRecord(rec, attrRec.slot);

                strcpy(rec[ATTRCAT_REL_NAME_INDEX].sVal, newName);
                attrCatBuffer.setRecord(rec,attrRec.slot);
        }


        return SUCCESS;

}

int BlockAccess::renameAttribute(char relName[ATTR_SIZE], char oldName[ATTR_SIZE], char newName[ATTR_SIZE]){

        RelCacheTable::resetSearchIndex(RELCAT_RELID);

        Attribute relNameAttr;
        strcpy(relNameAttr.sVal,relName);

        RecId relRec = linearSearch(RELCAT_RELID, RELCAT_ATTR_RELNAME, relNameAttr, EQ);

        if(relRec.block == -1 && relRec.slot == -1)
                return E_RELNOTEXIST;
        
        RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

        RecId attrToRenameRecId{-1,-1};
        Attribute attrCatEntryRecord[ATTRCAT_NO_ATTRS];


        while(true){

                RecId SearchIndex = linearSearch(ATTRCAT_RELID, ATTRCAT_ATTR_RELNAME, relNameAttr, EQ);

                if(SearchIndex.block == -1 && SearchIndex.slot == -1)
                        break;
                
                RecBuffer attrRecBuff(SearchIndex.block);
                //Attribute attrRec[ATTRCAT_NO_ATTRS];
                attrRecBuff.getRecord(attrCatEntryRecord,SearchIndex.slot);

                if(strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,oldName) == 0){
                        attrToRenameRecId = SearchIndex;
                        break;
                }

                if(strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,newName) == 0){
                        return E_ATTREXIST;
                }
        }

        if(attrToRenameRecId.block == -1 && attrToRenameRecId.slot == -1)
                return E_ATTRNOTEXIST;
        
        RecBuffer attrRecBuff(attrToRenameRecId.block);
        Attribute rec[ATTRCAT_NO_ATTRS];
        attrRecBuff.getRecord(rec,attrToRenameRecId.slot);
        strcpy(rec[ATTRCAT_ATTR_NAME_INDEX].sVal, newName);
        attrRecBuff.setRecord(rec,attrToRenameRecId.slot);

        return SUCCESS;
}

// STAGE - 7

int BlockAccess::insert(int relId, Attribute *record){

        RelCatEntry relCatEntry;
       RelCacheTable::getRelCatEntry(relId, &relCatEntry);

       int blockNum = relCatEntry.firstBlk;

       RecId rec_id = {-1, -1};
       
       int numOfSlots = relCatEntry.numSlotsPerBlk;
       int numOfAttributes = relCatEntry.numAttrs;
       int prevBlockNum = -1;

       while(blockNum != -1){                                   // check the slot is free ....

                RecBuffer recBuf(blockNum);
                HeadInfo head;
                recBuf.getHeader(&head);
                unsigned char slotMap[head.numSlots];
                recBuf.getSlotMap(slotMap);
                //int slot =-1;

                for(int i = 0; i < head.numSlots; i++){

                        if(slotMap[i] == SLOT_UNOCCUPIED){
                                 rec_id.slot = i;
                                rec_id.block = blockNum;
                                //slot=i;
                                break;
                        }
                }

                // if(slot!=-1){
                //         rec_id.slot=slot;
                //         rec_id.block=blockNum;

                // }
                prevBlockNum = blockNum;
                blockNum = head.rblock;
                

       }

       if(rec_id.block == -1 && rec_id.slot == -1){             // if no slots are free, create a new Block 


                int ret;
                
                if(relId == RELCAT_RELID){
                        return E_MAXRELATIONS;
                }
                
                        
                RecBuffer blockBuffer;
                ret = blockBuffer.getBlockNum();
                if (ret == E_DISKFULL) {
                        return E_DISKFULL;
                }
                
                
                rec_id.block = ret;
                rec_id.slot = 0;

                HeadInfo head;
                // RecBuffer newBlock(rec_id.block);
                //newBlock.getHeader(&head);
                head.blockType = REC;
                head.pblock = -1;
                head.rblock = -1;
                head.numEntries = 0;
                if(relCatEntry.firstBlk!=-1){
                        head.lblock=prevBlockNum;
                }else{
                        head.lblock = -1;
                }
                head.numAttrs = relCatEntry.numAttrs;
                head.numSlots = relCatEntry.numSlotsPerBlk;

                blockBuffer.setHeader(&head);
                unsigned char slotMap[relCatEntry.numSlotsPerBlk];

                for(int i = 0; i < relCatEntry.numSlotsPerBlk; i++){
                                
                        slotMap[i] = SLOT_UNOCCUPIED;        
                }
                blockBuffer.setSlotMap(slotMap);

                if(prevBlockNum != -1){                         // connect the new block to right block of previousBlock's head

                        RecBuffer prevBlock(prevBlockNum);
                        HeadInfo head;
                        prevBlock.getHeader(&head);
                        head.rblock = rec_id.block;
                        prevBlock.setHeader(&head);
                }
                else{
                        relCatEntry.firstBlk = rec_id.block;
                        RelCacheTable::setRelCatEntry(relId, &relCatEntry);
                }
         relCatEntry.lastBlk = rec_id.block;
         RelCacheTable::setRelCatEntry(relId, &relCatEntry);

        }

        RecBuffer recBuf(rec_id.block);
        recBuf.setRecord(record, rec_id.slot);

        unsigned char slotMap[relCatEntry.numSlotsPerBlk];
        recBuf.getSlotMap(slotMap);
        slotMap[rec_id.slot] = SLOT_OCCUPIED;
        recBuf.setSlotMap(slotMap);
        HeadInfo head;
        recBuf.getHeader(&head);
        head.numEntries++;
        recBuf.setHeader(&head);

        relCatEntry.numRecs++;
        RelCacheTable::setRelCatEntry(relId, &relCatEntry);

        return SUCCESS;
}

// STAGE - 8

int BlockAccess::search(int relId, Attribute *record, char attrName[ATTR_SIZE], Attribute attrVal, int op){

        RecId recid;

        recid = linearSearch(relId, attrName, attrVal, op);

        if(recid.block == -1 && recid.slot == -1)
                return E_NOTFOUND;
        
        RecBuffer recbuf(recid.block);
        int ret = recbuf.getRecord(record, recid.slot);

        if (ret != SUCCESS)
                return ret;

        return SUCCESS;
}

int BlockAccess::deleteRelation(char relName[ATTR_SIZE]){

        if(strcmp(relName, RELCAT_RELNAME) == 0 || strcmp(relName, ATTRCAT_RELNAME)==0)
                return E_NOTPERMITTED;
        
        RelCacheTable::resetSearchIndex(RELCAT_RELID);

        Attribute relNameAttr;
        strcpy(relNameAttr.sVal, relName);

        RecId relRecid = linearSearch(RELCAT_RELID, RELCAT_ATTR_RELNAME, relNameAttr, EQ);

        if(relRecid.block == -1 && relRecid.slot == -1)
                return E_RELNOTEXIST;
        
        Attribute relCatEntryRecord[RELCAT_NO_ATTRS];
        RecBuffer relRecBuf(relRecid.block);
        relRecBuf.getRecord(relCatEntryRecord, relRecid.slot);

        int firstBlock = relCatEntryRecord[RELCAT_FIRST_BLOCK_INDEX].nVal;
        int numAttrs = relCatEntryRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal;

        while(firstBlock != -1){                                        // DELETING THE BLOCKS USING RELTION Cache

                RecBuffer recBlock(firstBlock);
                HeadInfo head;
                recBlock.getHeader(&head);
                firstBlock = head.rblock;
                recBlock.releaseBlock();
        }

        RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

        int numberOfAttrsDelted = 0;

        while(true){                                    // DELETING ATTRIBUTE CATALOG ENTRIES

                RecId attrCatRecId = linearSearch(ATTRCAT_RELID, ATTRCAT_ATTR_RELNAME, relNameAttr, EQ);

                if(attrCatRecId.block == -1 && attrCatRecId.slot == -1)
                        break;
                
                numberOfAttrsDelted++;

                RecBuffer recBuff(attrCatRecId.block);
                HeadInfo attrhead;
                recBuff.getHeader(&attrhead);
                Attribute attrCatEntryRecord[ATTRCAT_NO_ATTRS];
                recBuff.getRecord(attrCatEntryRecord, attrCatRecId.slot);

                int rootBlock = attrCatEntryRecord[ATTRCAT_ROOT_BLOCK_INDEX].nVal;
                
                unsigned char slotMap[attrhead.numSlots];
                recBuff.getSlotMap(slotMap);
                slotMap[attrCatRecId.slot] = SLOT_UNOCCUPIED;    // MARK THE SLOT (RECORD) AS UNOCCUPIED
                recBuff.setSlotMap(slotMap);
                attrhead.numEntries--;                           // DECREASE THE NUMBER OF ENTRIES 
                recBuff.setHeader(&attrhead);

                if(attrhead.numEntries == 0){
                        
                        RecBuffer leftBlock(attrhead.lblock);
                        HeadInfo lhead;
                        leftBlock.getHeader(&lhead);
                        lhead.rblock = attrhead.rblock;
                        leftBlock.setHeader(&lhead);
                
                        if(attrhead.rblock !=-1){

                                RecBuffer rblock(attrhead.rblock);
                                HeadInfo rhead;
                                rblock.getHeader(&rhead);
                                rhead.lblock = attrhead.lblock;
                                rblock.setHeader(&rhead);
                        }
                        else{
                                RelCatEntry relCatEntry;
                                RelCacheTable::getRelCatEntry(RELCAT_RELID, &relCatEntry);
                                relCatEntry.lastBlk = attrhead.lblock;
                        }

                        recBuff.releaseBlock();
                }

        }
        HeadInfo relHead;
        relRecBuf.getHeader(&relHead);
        relHead.numEntries--;
        relRecBuf.setHeader(&relHead);

        unsigned char slotMap[relHead.numSlots];
        relRecBuf.getSlotMap(slotMap);
        slotMap[relRecid.slot] = SLOT_UNOCCUPIED;
        relRecBuf.setSlotMap(slotMap);

        RelCatEntry relCatBuf;
        RelCacheTable::getRelCatEntry(RELCAT_RELID, &relCatBuf);
        relCatBuf.numRecs--;
        RelCacheTable::setRelCatEntry(RELCAT_RELID, &relCatBuf);

        RelCatEntry attrCatBuf;
        RelCacheTable::getRelCatEntry(ATTRCAT_RELID, &attrCatBuf);
        attrCatBuf.numRecs = attrCatBuf.numRecs - numberOfAttrsDelted;
        RelCacheTable::setRelCatEntry(ATTRCAT_RELID, &attrCatBuf);

        return SUCCESS;

}

// STAGE - 9

int BlockAccess::project(int relId, Attribute *record){

        RecId prev_recid;
        RelCacheTable::getSearchIndex(relId, &prev_recid);

        int block, slot;

        if(prev_recid.block == -1 && prev_recid.slot == -1){

                RelCatEntry relCatBuf;
                RelCacheTable::getRelCatEntry(relId, &relCatBuf);
    
                block = relCatBuf.firstBlk;
                slot = 0;

        }
        else{
                block = prev_recid.block;
                slot = prev_recid.slot + 1;
        }

        while(block != -1){

                RecBuffer recBlock(block);
                HeadInfo head;
                recBlock.getHeader(&head);
                unsigned char slotMap[head.numSlots];
                recBlock.getSlotMap(slotMap);

                if(slot >= head.numSlots){
                        block = head.rblock;
                        slot = 0;
                        continue;
                }
                else if(slotMap[slot] == SLOT_UNOCCUPIED){
                        slot++;
                }
                else{
                        break;
                }
        }
        if(block == -1)
                return E_OUTOFBOUND;
        
        RecId nextRecid{block,slot};
        nextRecid.block = block; 
        nextRecid.slot = slot;

        RelCacheTable::setSearchIndex(relId, &nextRecid);

        RecBuffer recBlock(nextRecid.block);
        recBlock.getRecord(record,nextRecid.slot);
        
        return SUCCESS;
}


