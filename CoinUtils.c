
//----------------includes-----------------------//
#include "CoinUtils.h"
#include <stdlib.h>
#include <stdbool.h>

//----------------includes----------------------//

//----------------condition variables----------------------//
pthread_cond_t c_ServerAddedGenesisBlock = PTHREAD_COND_INITIALIZER;
int g_ServerDoneAddingGenesisBlock = 0;

pthread_cond_t c_MinerGaveBlock = PTHREAD_COND_INITIALIZER;
int g_MinerGaveBlock = 0;

pthread_cond_t c_ServerProcessigBlock = PTHREAD_COND_INITIALIZER;
int g_ServerDoneProcessingBlock = 0;

//----------------condition variables----------------------//

//----------------mutex----------------------//

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t m_SendBlockToServer = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t m_ServerDoneAddingGenesisBlock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t m_ServerDoneProccessing = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t m_CuurentBlockUnderWork = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t m_LastBlockAddedByServer = PTHREAD_MUTEX_INITIALIZER;
//----------------mutex----------------------//


//----------------globes----------------------//
BLOCK_T g_BlockGivenByMiner;
BLOCK_T g_LastBlockAddedByServer;
int g_CuurentBlockUnderWork = 0;
int g_MinersNumberThatGaveBlock = 0;
//----------------globes----------------------//


void Start()
{
  CreateFourMiners();
  UploadServer();
}

void ListenToServer(void* i_MinersNumber)
{
  int minersNumber = *((int*)i_MinersNumber);
  int currentBlockUnderWork;

  waitForServerToAddGenesisBlock();
  while (1)
  {
    currentBlockUnderWork = g_CuurentBlockUnderWork;

    while (currentBlockUnderWork == g_CuurentBlockUnderWork)
    {
        BLOCK_T* newBlockT = (BLOCK_T*)calloc(1,sizeof(BLOCK_T));
        pthread_mutex_lock(&m_LastBlockAddedByServer);
        newBlockT->height = g_LastBlockAddedByServer.height  + 1;
        newBlockT->prev_hash = g_LastBlockAddedByServer.hash;
        pthread_mutex_unlock(&m_LastBlockAddedByServer);

        newBlockT->relayed_by = (int)pthread_self();
        MinigingBlock(newBlockT);
        SendBlockToServer(minersNumber, newBlockT, currentBlockUnderWork);
    }
  }
}



void SendBlockToServer(int i_MinersNumber,BLOCK_T* i_BlockToSend, int i_CurrentBlockUnderWork)
{
  pthread_mutex_lock(&m_SendBlockToServer);
  if ( i_CurrentBlockUnderWork == g_CuurentBlockUnderWork)
  {
    g_BlockGivenByMiner = *i_BlockToSend;
    printf("Miner #%d: Mined a new block #%d, with the hash 0x%x\n",i_MinersNumber, i_CurrentBlockUnderWork, g_BlockGivenByMiner.hash);
    g_MinersNumberThatGaveBlock = i_MinersNumber;
    notifyServerBlockGiven();
    waitForServerToProccessBlock();
  }
  pthread_mutex_unlock(&m_SendBlockToServer);
}



void* FakeMiner(void* i_MinersNumber)
{
  int minersNumber = *((int*)i_MinersNumber);
  int currentBlockUnderWork;

  waitForServerToAddGenesisBlock();
  while (1)
  {
    currentBlockUnderWork = g_CuurentBlockUnderWork;

    while (currentBlockUnderWork == g_CuurentBlockUnderWork)
    {
      usleep(50000);
        BLOCK_T* newBlockT = (BLOCK_T*)calloc(1,sizeof(BLOCK_T));
        pthread_mutex_lock(&m_LastBlockAddedByServer);
        newBlockT->height = g_LastBlockAddedByServer.height  + 1;
        newBlockT->prev_hash = g_LastBlockAddedByServer.hash;
        pthread_mutex_unlock(&m_LastBlockAddedByServer);
        newBlockT->hash = 0xffffffff;
        SendBlockToServer(minersNumber, newBlockT, currentBlockUnderWork);
    }
  }
 }

void waitForServerToProccessBlock()
{
  pthread_mutex_lock(&m_ServerDoneProccessing);
  while (g_ServerDoneProcessingBlock == 0)
  {
    pthread_cond_wait(&c_ServerProcessigBlock, &m_ServerDoneProccessing);
  }

  g_ServerDoneProcessingBlock = 0;
  pthread_mutex_unlock(&m_ServerDoneProccessing);
}



void waitForServerToAddGenesisBlock()
{
  pthread_mutex_lock(& m_ServerDoneAddingGenesisBlock);
  while (g_ServerDoneAddingGenesisBlock == 0)
  {
    pthread_cond_wait(&c_ServerAddedGenesisBlock, & m_ServerDoneAddingGenesisBlock);
  }

  pthread_mutex_unlock(& m_ServerDoneAddingGenesisBlock);
}

void notifyServerBlockGiven()
{
  g_MinerGaveBlock = 1;
  pthread_cond_signal(&c_MinerGaveBlock);
}

void MinigingBlock(BLOCK_T* i_Block)
{
  SetCurrentTimeStamp(i_Block);
  MiningHash(i_Block);
}

void MiningHash(BLOCK_T* i_Block)
{
  unsigned int mask = 0xffff0000;
  unsigned int crcHash;

  do {
        i_Block-> nonce =  i_Block-> nonce + 1;
        crcHash = MakeHashFromStruct(i_Block);
   } while((mask & crcHash) != 0);

   i_Block-> hash = crcHash;
}

void WaitForAMinerToGiveBlock()
{
  pthread_mutex_lock(&m);
  while (g_MinerGaveBlock == 0)
  {
    pthread_cond_wait(&c_MinerGaveBlock, &m);
  }
  pthread_mutex_unlock(&m);

  g_MinerGaveBlock = 0;
}

unsigned int MakeHashFromStruct(BLOCK_T* i_Block)
{
    unsigned long crcHash = 0;
    int valueInStruct;

    for (size_t i = 0; i < 5; i++) {

      valueInStruct = (int)*((int*)i_Block + i);
      crcHash = crc32(crcHash, (const void*)&valueInStruct,sizeof(int));
    }

    return crcHash;
}

void SetCurrentTimeStamp(BLOCK_T* i_Block)
{
  time_t seconds = time(NULL);
  i_Block-> timestamp = seconds;
}

BLOCK_T* GenerateInitialGensisBlock()
{
  BLOCK_T* newBlockT = (BLOCK_T*)calloc(1,sizeof(BLOCK_T));

  newBlockT->difficulty = DIFFICULTY;
  newBlockT-> nonce = -1;
  newBlockT-> hash = 0;
  newBlockT-> prev_hash = 0;
  MinigingBlock(newBlockT);

  return newBlockT;
}

void NotifyMinersGenesisBlockAdded()
{
  pthread_mutex_lock(&m_ServerDoneAddingGenesisBlock);
  g_ServerDoneAddingGenesisBlock = 1;
  pthread_mutex_unlock(&m_ServerDoneAddingGenesisBlock);

  pthread_cond_broadcast(&c_ServerAddedGenesisBlock);
}

int IsGivenBlockValid()
{
  bool validBlock = false;
  unsigned int crcHashMiner, crcHash, mask = 0xffff0000;
  bool isMinerHashHaveLeadingZeros = (g_BlockGivenByMiner.hash & mask) == 0;

  if (isMinerHashHaveLeadingZeros)
  {
    crcHash = MakeHashFromStruct(&g_BlockGivenByMiner);
    bool isHashIdentical = g_BlockGivenByMiner.hash == crcHash;
    validBlock = isHashIdentical;
    pthread_mutex_lock(&m_LastBlockAddedByServer);
    validBlock = g_BlockGivenByMiner.height == g_LastBlockAddedByServer.height -1;
    validBlock = g_BlockGivenByMiner.prev_hash == g_LastBlockAddedByServer.hash;
    pthread_mutex_unlock(&m_LastBlockAddedByServer);               // becuase the miner hash has leading zeros..
  }

  return validBlock;
}

void CreateFourMiners()
{
    pthread_t minersThreadsId[NUM_OF_MINERS_THREADS  + 1];
    int* minersNumber;

    for (int i = 0; i < NUM_OF_MINERS_THREADS; i++)
    {
        minersNumber = (int*)malloc(sizeof(int) * 1);
        *minersNumber = i + 1;
        pthread_create(&minersThreadsId[i], NULL, (void*)ListenToServer, (void*)minersNumber);
    }

    minersNumber = (int*)malloc(sizeof(int) * 1);
    *minersNumber = NUM_OF_MINERS_THREADS + 1;
    pthread_create(&minersThreadsId[NUM_OF_MINERS_THREADS], NULL, (void*)FakeMiner, (void*)minersNumber);
}


void AddBlockGivenByMinerToList()
{
  pthread_mutex_lock(&m_LastBlockAddedByServer);
  AddBlockToList(g_BlockGivenByMiner);
  pthread_mutex_unlock(&m_LastBlockAddedByServer);

}

void UploadServer()
{
  bool validBlockHasGiven = false;
  InitializeList();
  BLOCK_T* genesisBlock = GenerateInitialGensisBlock();
  g_LastBlockAddedByServer = *genesisBlock;
  AddBlockToList(*genesisBlock);
  NotifyMinersGenesisBlockAdded();
  while (1)
  {
      while(!validBlockHasGiven)
      {
        WaitForAMinerToGiveBlock();
        if (IsGivenBlockValid())
        {
          validBlockHasGiven = true;
          printf("Server: New block added by %d, attributes: height(%d), timestamp(%d), hash(0x%x), prev_hash(0x%x), difficulty(%d), nonce(%d)\n",
                  g_MinersNumberThatGaveBlock, g_BlockGivenByMiner.height, g_BlockGivenByMiner.timestamp, g_BlockGivenByMiner.hash, g_BlockGivenByMiner.prev_hash,
                  DIFFICULTY, g_BlockGivenByMiner.nonce);
          AddBlockGivenByMinerToList();
          g_LastBlockAddedByServer = g_BlockGivenByMiner;
          g_CuurentBlockUnderWork = g_CuurentBlockUnderWork + 1;

        }
        else
        {
          validBlockHasGiven = false;
          printf("Server, Error: Miner #%d sent an invalid block\n",g_MinersNumberThatGaveBlock);
        }

        g_ServerDoneProcessingBlock = 1;
        pthread_cond_signal(&c_ServerProcessigBlock);
      }

      validBlockHasGiven = false;
    }
}
