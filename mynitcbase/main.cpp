#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include<iostream>
#include <string.h>

int main(int argc, char *argv[]) {
  Disk disk_run;

  unsigned char buf1[BLOCK_SIZE];  
  Disk::readBlock( buf1, 4738);
  char msg1[] = "DBMS_LAB";
  memcpy(buf1 + 90, msg1, 9);
  
  Disk::writeBlock(buf1, 4738);

  unsigned char buf2[BLOCK_SIZE];
  Disk::readBlock( buf2, 4738);
  char msg2[9];
  memcpy(msg2, buf2 + 90, 9);
  std::cout << msg2;

  //Exercise
    unsigned char buff[BLOCK_SIZE];

 Disk::readBlock( buff , 0);
  printf("\n");
  for(int i = 0; i<10; i++)
      printf("%d ",buff[i]);
  printf("\n");
  return 0;
}
