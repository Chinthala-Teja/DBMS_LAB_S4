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
                int attrOffset=attrCatBuf.offset;
            
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
