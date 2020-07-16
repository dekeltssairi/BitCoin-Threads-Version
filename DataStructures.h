typedef struct {
    int         height;        // Incrementeal ID of the block in the chain
    int         timestamp;    // Time of the mine in seconds since epoch
    unsigned int     prev_hash;    // Hash value of the previous block
    int         nonce;        // Incremental integer to change the hash value
    int         relayed_by;    // Miner ID
    unsigned int     hash;        // Current block hash value
    int        difficulty;    // Amount of preceding zeros in the hash
} BLOCK_T;

typedef struct blockNode {
  BLOCK_T block;
  struct blockNode* next;
} BlockNode;

typedef struct blockList {
  BlockNode* head;
  BlockNode* tail;
} BlockList;

BlockList list;
void InitializeList();
void AddBlockToList(BLOCK_T i_Block);
void AddBlockNodeToList(BlockNode* i_BlockNode);
