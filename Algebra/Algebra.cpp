#include "Algebra.h"
#include <iostream>
#include <cstring>

bool isNumber(char *str)
{
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

// STAGE - 9 ( modified)

int Algebra::select(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strVal[ATTR_SIZE])
{

  int srcRelId = OpenRelTable::getRelId(srcRel);

  if (srcRelId == E_RELNOTOPEN)
  {
    return E_RELNOTOPEN;
  }
  AttrCatEntry attrCatEntry;
  int res = AttrCacheTable::getAttrCatEntry(srcRelId, attr, &attrCatEntry);

  if (res == E_ATTRNOTEXIST)
    return E_ATTRNOTEXIST;

  int type = attrCatEntry.attrType;
  Attribute attrVal;

  if (type == NUMBER)
  {
    if (isNumber(strVal))
    { // the isNumber() function is implemented below
      attrVal.nVal = atof(strVal);
    }
    else
    {
      return E_ATTRTYPEMISMATCH;
    }
  }
  else if (type == STRING)
  {

    strcpy(attrVal.sVal, strVal);
  }

  // RelCacheTable::resetSearchIndex(srcRelId);

  RelCatEntry relCatEntry;
  RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);
  int src_nAttrs = relCatEntry.numAttrs;
  char attr_names[src_nAttrs][ATTR_SIZE];
  int attr_types[src_nAttrs];

  // printf("|");
  for (int i = 0; i < src_nAttrs; ++i)
  {

    AttrCatEntry attrCatEntry;
    AttrCacheTable::getAttrCatEntry(srcRelId, i, &attrCatEntry);

    strcpy(attr_names[i], attrCatEntry.attrName);
    attr_types[i] = attrCatEntry.attrType;
    // printf(" %s |", attrCatEntry.attrName);
  }

  int ret = Schema::createRel(targetRel, src_nAttrs, attr_names, attr_types);

  if (ret != SUCCESS)
    return ret;

  int targetRelId = OpenRelTable::openRel(targetRel);

  if (targetRelId == E_CACHEFULL)
  {

    Schema::deleteRel(targetRel);
    return targetRelId;
  }

  Attribute record[src_nAttrs];

  RelCacheTable::resetSearchIndex(srcRelId);
  // AttrCacheTable::resetSearchIndex(srcRelId, attr);

  while (BlockAccess::search(srcRelId, record, attr, attrVal, op) == SUCCESS)
  {

    int ret = BlockAccess::insert(targetRelId, record);

    if (ret != SUCCESS)
    {
      Schema::closeRel(targetRel);
      Schema::deleteRel(targetRel);
      return ret;
    }
  }
  Schema::closeRel(targetRel);
  return SUCCESS;
}

// STAGE - 7
int Algebra::insert(char relName[ATTR_SIZE], int nAttrs, char record[][ATTR_SIZE])
{

  if (strcmp(relName, "RELATIONCAT") == 0 || strcmp(relName, "ATTRIBUTECAT") == 0)
    return E_NOTPERMITTED;

  int relId = OpenRelTable::getRelId(relName);

  if (relId == E_RELNOTOPEN)
    return E_RELNOTOPEN;

  RelCatEntry relCatBuf;
  RelCacheTable::getRelCatEntry(relId, &relCatBuf);

  if (relCatBuf.numAttrs != nAttrs)
    return E_NATTRMISMATCH;

  union Attribute recordValues[nAttrs];

  for (int i = 0; i < nAttrs; i++)
  {

    AttrCatEntry attrCatEntry;

    AttrCacheTable::getAttrCatEntry(relId, i, &attrCatEntry);

    int type = attrCatEntry.attrType;

    if (type == NUMBER)
    {

      if (isNumber(record[i]))
      {
        recordValues[i].nVal = atof(record[i]);
      }
      else
        return E_ATTRTYPEMISMATCH;
    }
    else if (type == STRING)
      strcpy(recordValues[i].sVal, record[i]);
  }

  int retVal = BlockAccess::insert(relId, recordValues);

  return retVal;
}

// STAGE - 9

int Algebra::project(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE])
{

  int srcRelId = OpenRelTable::getRelId(srcRel);

  if (srcRelId == E_RELNOTOPEN)
    return E_RELNOTOPEN;

  RelCatEntry RelEntry;
  RelCacheTable::getRelCatEntry(srcRelId, &RelEntry);

  int noOfAtrrs = RelEntry.numAttrs;
  char attrNames[noOfAtrrs][ATTR_SIZE];
  int attrTypes[noOfAtrrs];

  for (int i = 0; i < noOfAtrrs; i++)
  {

    AttrCatEntry attrCatEntry;
    AttrCacheTable::getAttrCatEntry(srcRelId, i, &attrCatEntry);
    strcpy(attrNames[i], attrCatEntry.attrName);
    attrTypes[i] = attrCatEntry.attrType;
  }

  int ret = Schema::createRel(targetRel, noOfAtrrs, attrNames, attrTypes);

  if (ret != SUCCESS)
    return ret;

  int target_relId = OpenRelTable::openRel(targetRel);

  if (target_relId < 0 || target_relId >= MAX_OPEN)
  {
    Schema::deleteRel(targetRel);
    return target_relId;
  }

  RelCacheTable::resetSearchIndex(srcRelId);
  Attribute record[noOfAtrrs];

  while (BlockAccess::project(srcRelId, record) == SUCCESS)
  {

    int ret = BlockAccess::insert(target_relId, record);

    if (ret != SUCCESS)
    {

      Schema::closeRel(targetRel);
      Schema::deleteRel(targetRel);
      return ret;
    }
  }
  Schema::closeRel(targetRel);
  return SUCCESS;
}

int Algebra::project(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], int tar_nAttrs, char tar_Attrs[][ATTR_SIZE])
{

  int srcRelId = OpenRelTable::getRelId(srcRel);

  if (srcRelId == E_RELNOTOPEN)
  {
    return E_RELNOTOPEN;
  }

  RelCatEntry relCatEntry;
  RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);

  int numOfAttrs = relCatEntry.numAttrs;

  int attr_offset[tar_nAttrs];
  int attr_types[tar_nAttrs];

  for (int i = 0; i < tar_nAttrs; i++)
  {

    AttrCatEntry attrCatEntry;
    int ret = AttrCacheTable::getAttrCatEntry(srcRelId, tar_Attrs[i], &attrCatEntry);

    if (ret != SUCCESS)
      return ret;

    attr_offset[i] = attrCatEntry.offset;
    attr_types[i] = attrCatEntry.attrType;
  }

  int ret = Schema::createRel(targetRel, tar_nAttrs, tar_Attrs, attr_types);

  if (ret != SUCCESS)
    return ret;

  int target_relId = OpenRelTable::openRel(targetRel);

  if (target_relId < 0 || target_relId >= MAX_OPEN)
  {

    Schema::deleteRel(targetRel);
    return target_relId;
  }

  RelCacheTable::resetSearchIndex(srcRelId);
  Attribute record[numOfAttrs];

  while (BlockAccess::project(srcRelId, record) == SUCCESS)
  {

    Attribute proj_record[tar_nAttrs];
    for (int i = 0; i < tar_nAttrs; i++)
    {

      proj_record[i] = record[attr_offset[i]];
    }
    int ret = BlockAccess::insert(target_relId, proj_record);

    if (ret != SUCCESS)
    {
      Schema::closeRel(targetRel);
      Schema::deleteRel(targetRel);
      return ret;
    }
  }

  Schema::closeRel(targetRel);
  return SUCCESS;
}
