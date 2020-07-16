#include "DataStructures.h"
#include <stdlib.h>

void AddBlockNodeToList(BlockNode* i_BlockNode)
{
  if (list.head != NULL)
  {
    list.tail->next = i_BlockNode;
    list.tail = list.tail->next;
  }
  else
  {
    list.head = list.tail = i_BlockNode;
  }
}

void AddBlockToList(BLOCK_T i_Block)
{
  BlockNode* blockNode = (BlockNode*)calloc(1, sizeof(BlockNode));
  blockNode->block = i_Block;
  AddBlockNodeToList(blockNode);

}
void InitializeList()
{
  list.head = NULL;
  list.tail = NULL;
}
