#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include<iostream>
#include <string.h>

int main(int argc, char *argv[]) {
  Disk disk_run;
// StaticBuffer buffer;
  // OpenRelTable cache;

  //return FrontendInterface::handleFrontend(argc, argv);

  RecBuffer Relbuf(RELCAT_BLOCK);
  struct HeadInfo head;
  Relbuf.getHeader(&head);

  RecBuffer Attrbuf(ATTRCAT_BLOCK);
  struct HeadInfo head1;
  Attrbuf.getHeader(&head1);

  for(int i = 0; i < head.numEntries; i++){

      Attribute rec[ATTR_SIZE];
      Relbuf.getRecord(rec, i);
      printf("Relation: %s\n",rec[RELCAT_REL_NAME_INDEX].sVal);
  
      
      for(int j = 0; j < head1.numEntries; j++){
  
          Attribute rec1[ATTR_SIZE];
          Attrbuf.getRecord(rec1 , j);
          //printf("%d\n",head1.numEntries);
          
          if(strcmp(rec[RELCAT_REL_NAME_INDEX].sVal,rec1[ATTRCAT_REL_NAME_INDEX].sVal) == 0){
            const char *attrType = rec1[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
            printf("%s : %s\n", rec1[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
          }
        }
        printf("\n");

  } 

}

//EXERXISE Q1

#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include<iostream>
#include <string.h>

int main(int argc, char *argv[]) {
  Disk disk_run;
// StaticBuffer buffer;
  // OpenRelTable cache;

  //return FrontendInterface::handleFrontend(argc, argv);

  RecBuffer Relbuf(RELCAT_BLOCK);
  struct HeadInfo head;
  Relbuf.getHeader(&head);


  for(int i = 0; i < head.numEntries; i++){

    Attribute rec[ATTR_SIZE];
    Relbuf.getRecord(rec, i);
    printf("Relation: %s\n",rec[RELCAT_REL_NAME_INDEX].sVal);
    int k = ATTRCAT_BLOCK;
    
    while(k!=-1){
      
      RecBuffer Attrbuf(k);
      struct HeadInfo head1;
      Attrbuf.getHeader(&head1);
        for(int j = 0; j < head1.numEntries; j++){

          Attribute rec1[ATTR_SIZE];
          Attrbuf.getRecord(rec1 , j);
          //printf("%d\n",head1.numEntries);
          
          if(strcmp(rec[RELCAT_REL_NAME_INDEX].sVal,rec1[ATTRCAT_REL_NAME_INDEX].sVal) == 0){
            const char *attrType = rec1[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
            printf("%s : %s\n", rec1[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
            }
        }
        k = head1.rblock;
       
      }
      printf("\n");
  }

}

