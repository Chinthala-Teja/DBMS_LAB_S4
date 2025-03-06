#include "BlockAccess.h"
#include <iostream>
#include <cstring>


RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op) {

        RecId prevRecId; 
        RelCacheTable::getSearchIndex(relId, &prevRecId);

        int block = -1, slot = -1;

        if(prevRecId.block == -1 && prevRecId.slot == -1){

                RelCatEntry relCatBuf;
            RelCacheTable::getRelCatEntry( relId, &relCatBuf);

            block = relCatBuf.firstBlk;
            slot = 0;
        }
        else{
                block = prevRecId.block;
                slot  = prevRecId.slot + 1;
        }
        RelCatEntry relCatBuffer;
	RelCacheTable::getRelCatEntry(relId, &relCatBuffer);
        while(block != -1){

                RecBuffer RecBuf(block);
                Attribute CatRecord[RELCAT_NO_ATTRS];
                RecBuf.getRecord(CatRecord, slot);

                HeadInfo head;
                RecBuf.getHeader(&head);
                
                unsigned char *slotMap =  (unsigned char *)malloc(sizeof(unsigned char) * head.numSlots);

                RecBuf.getSlotMap(slotMap);


                if(slot >= relCatBuffer.numSlotsPerBlk){

                        block = head.rblock;
                        slot = 0;
                        continue;
                }
                if(slotMap[slot] == SLOT_UNOCCUPIED){
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

       while(blockNum != -1){

                RecBuffer recBuf(blockNum);
                HeadInfo head;
                recBuf.getHeader(&head);
                unsigned char slotMap[head.numSlots];
                recBuf.getSlotMap(slotMap);
                int slot=-1;

                for(int i = 0; i < numOfSlots; i++){

                        if(slotMap[i] == SLOT_UNOCCUPIED){
                                //rec_id.slot = i;
                                //rec_id.block = blockNum;
                                slot=i;
                                break;
                        }
                }

                if(slot!=-1){
                        rec_id.slot=slot;
                        rec_id.block=blockNum;

                }
                // if(rec_id.block == -1 && rec_id.slot == -1)
                //         break;
                prevBlockNum = blockNum;
                blockNum = head.rblock;
                

       }

       if(rec_id.block == -1 && rec_id.slot == -1){


                int ret;
                
                if(relId == RELCAT_RELID)
                        return E_MAXRELATIONS;
                
                        
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

                if(prevBlockNum != -1){

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
