#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include<iostream>
#include <string.h>

int main(int argc, char *argv[]) {
  Disk disk_run;
 StaticBuffer buffer;
 OpenRelTable cache;

  //return FrontendInterface::handleFrontend(argc, argv);

  for(int i = 0; i <= 2; i++){

    RelCatEntry relCatBuf;
    RelCacheTable::getRelCatEntry(i, &relCatBuf);
    printf("Relation : %s\n", relCatBuf.relName);

    for(int j = 0; j < relCatBuf.numAttrs; j++){

    AttrCatEntry attrCatBuf;
    AttrCacheTable::getAttrCatEntry(i, j, &attrCatBuf);

    const char *attrType = attrCatBuf.attrType == NUMBER ? "NUM" : "STR";

    printf(" %s: %s\n", attrCatBuf.attrName, attrType);

    }
    printf("\n");

  }

  return 0;
}
