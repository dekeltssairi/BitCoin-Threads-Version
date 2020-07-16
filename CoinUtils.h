
//----------------------include---------------
#include <stdio.h>
#include <string.h>
//#include <stdlib.h>
#include <zlib.h>
#include <unistd.h>
#include "DataStructures.h"
#include <pthread.h>

//-------------------defines------------------
#define NUM_OF_MINERS_THREADS 4
#define DIFFICULTY 16
//-------------------function declarations


void Start();
void ListenToServer();
void waitForServerToProccessBlock();
void waitForServerToAddGenesisBlock();
void WaitForAMinerToGiveBlock();
void notifyServerBlockGiven();
void MinigingBlock(BLOCK_T* i_Block);
void MiningHash(BLOCK_T* i_Block);
unsigned int  MakeHashFromStruct(BLOCK_T* i_Block);
void SetCurrentTimeStamp(BLOCK_T* i_Block);
BLOCK_T* GenerateInitialGensisBlock();
void NotifyMinersGenesisBlockAdded();
int IsGivenBlockValid();
void CreateFourMiners();
void AddBlockGivenByMinerToList();
void UploadServer();
void SendBlockToServer(int i_MinersNumber,BLOCK_T* i_BlockToSend, int i_CurrentBlockUnderWork);
void* FakeMiner(void* i_MinersNumber);
