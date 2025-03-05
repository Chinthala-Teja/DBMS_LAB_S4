#include "Algebra.h"
#include<iostream>
#include <cstring>


bool isNumber(char *str) {
    int len;
    float ignore;
    /*
      sscanf returns the number of elements read, so if there is no float matching
      the first %f, ret will be 0, else it'll be 1
  
      %n gets the number of characters read. this scanf sequence will read the
      first float ignoring all the whitespace before and after. and the number of
      characters read that far will be stored in len. if len == strlen(str), then
      the string only contains a float with/without whitespace. else, there's other
      characters.
    */
    int ret = sscanf(str, "%f %n", &ignore, &len);
    return ret == 1 && len == strlen(str);
  }
  

int Algebra::select(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strVal[ATTR_SIZE]) {
    
    int srcRelId = OpenRelTable::getRelId(srcRel);      // we'll implement this later
    if (srcRelId == E_RELNOTOPEN) {
      return E_RELNOTOPEN;
    }
    AttrCatEntry attrCatEntry;
    int res = AttrCacheTable::getAttrCatEntry(srcRelId, attr, &attrCatEntry);

    if(res == E_ATTRNOTEXIST)
        return E_ATTRNOTEXIST;

    int type = attrCatEntry.attrType;
    
    Attribute attrVal;

    if (type == NUMBER) {
        if (isNumber(strVal)) {       // the isNumber() function is implemented below
            attrVal.nVal = atof(strVal);
            } 
        else {
            return E_ATTRTYPEMISMATCH;
        }
    }
    else if (type == STRING) {
    
        strcpy(attrVal.sVal, strVal);    
    }

    RelCacheTable::resetSearchIndex(srcRelId);

    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);


    printf("|");
    for (int i = 0; i < relCatEntry.numAttrs; ++i) {
        
      AttrCatEntry attrCatEntry;

        AttrCacheTable::getAttrCatEntry(srcRelId, i, &attrCatEntry);

        printf(" %s |", attrCatEntry.attrName);
    }
    printf("\n");


    while (true) {
        RecId searchRes = BlockAccess::linearSearch(srcRelId, attr, attrVal, op);
    
        if (searchRes.block != -1 && searchRes.slot != -1) {
    
          // get the record at searchRes using BlockBuffer.getRecord
            RecBuffer BlockBuffer(searchRes.block);
            AttrCatEntry attrCatEntry;

      
            Attribute rec[relCatEntry.numAttrs];

            BlockBuffer.getRecord(rec, searchRes.slot);
            
            for(int i = 0; i < relCatEntry.numAttrs; i++){
                
                AttrCacheTable::getAttrCatEntry(srcRelId, i, &attrCatEntry);

                if(attrCatEntry.attrType == NUMBER)
                    printf(" %d |",(int)rec[i].nVal);
                else
                printf(" %s |",rec[i].sVal);
            }
            printf("\n");
    
        } else {
    
          // (all records over)
          break;
        }
       
      }
    
      return SUCCESS;
}

// STAGE - 7
int Algebra::insert(char relName[ATTR_SIZE], int nAttrs, char record[][ATTR_SIZE]){

    if(strcmp(relName,"RELATIONCAT") == 0 || strcmp(relName, "ATTRIBUTECAT") == 0)
        return E_NOTPERMITTED;
    
    int relId = OpenRelTable::getRelId(relName);

    if(relId == E_RELNOTOPEN)
      return E_RELNOTOPEN;
    
    RelCatEntry relCatBuf;
    RelCacheTable::getRelCatEntry(relId, &relCatBuf);

    if(relCatBuf.numAttrs != nAttrs)
      return E_NATTRMISMATCH;
    
    union Attribute recordValues[nAttrs];

    for(int i = 0; i < nAttrs; i++){

      AttrCatEntry attrCatEntry;
      
      AttrCacheTable::getAttrCatEntry(relId, i, &attrCatEntry);
      
      int type = attrCatEntry.attrType;

      if(type == NUMBER){

        if(isNumber(record[i])){
          recordValues[i].nVal = atof(record[i]);
        }
        else  
          return E_ATTRTYPEMISMATCH;
      }
      else if( type == STRING)
          strcpy(recordValues[i].sVal,record[i]);
    }

   int retVal =  BlockAccess::insert(relId, recordValues);

   return retVal;
}


