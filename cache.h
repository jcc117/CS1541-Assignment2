/* This file contains a rough implementation of an L1 cache in the absence of an L2 cache*/
#include <stdlib.h>
#include <stdio.h>

typedef struct cache_blk_t { // note that no actual data will be stored in the cache 
  unsigned long tag;
  char valid;
  char dirty;
  unsigned LRU;	//to be used to build the LRU stack for the blocks in a cache set
} cache_blk_t;


// This is modified to include 2 identical L1 caches and 1 L2 cache
typedef struct cache_t {
	// The cache is represented by a 2-D array of blocks. 
	// The first dimension of the 2D array is "nsets" which is the number of sets (entries)
	// The second dimension is "assoc", which is the number of blocks in each set.
	// Associativiety and the block size is common amongst all caches
  int blocksize;				// block size
  int assoc;					// associativity
  int L1_nsets;					// number of sets
  int L1_mem_latency;				// the miss penalty
  struct cache_blk_t **L1_a_blocks;	// a pointer to the array of cache blocks
  struct cache_blk_t **L1_b_blocks;	// a pointer to the array of cache blocks
  int L2_nsets;					// number of sets
  int L2_mem_latency;				// the miss penalty
  struct cache_blk_t **L2_blocks;	// a pointer to the array of cache blocks
} cache_t;

/*Setting up the Cache*/
// cache_t *theCache;
// theCache = malloc(sizeof(cache_t));
// theCache = cache_create(4, 8, 4, 1, 0, 200);
// We may need to be able to free this stuff
struct cache_t * cache_create(int L1_size, int L2_size, int blocksize, int assoc, int L1_mem_latency, L2_mem_latency) {
  int i, L1_nblocks , L1_nsets, L2_nblocks , L2_nsets ;
  struct cache_t *C = (struct cache_t *)calloc(1, sizeof(struct cache_t));

  L1_nblocks = L1_size *1024 / blocksize ;// number of blocks in the cache
  L1_nsets = L1_nblocks / assoc ;			// number of sets (entries) in the cache
  
  L2_nblocks = L2_size *1024 / blocksize ;// number of blocks in the cache
  L2_nsets = L2_nblocks / assoc ;			// number of sets (entries) in the cache
  
  C->blocksize = blocksize ;
  C->L1_nsets = L1_nsets  ;
  C->L2_nsets = L2_nsets  ;
  C->assoc = assoc;
  C->L1_mem_latency = L1_mem_latency;
  C->L2_mem_latency = L2_mem_latency;

  C->L1_a_blocks= (struct cache_blk_t **)calloc(L1_nsets, sizeof(struct cache_blk_t *));
  C->L1_b_blocks= (struct cache_blk_t **)calloc(L1_nsets, sizeof(struct cache_blk_t *));
  C->L2_blocks= (struct cache_blk_t **)calloc(L2_nsets, sizeof(struct cache_blk_t *));

  for(i = 0; i < L1_nsets; i++) {
		C->L1_a_blocks[i] = (struct cache_blk_t *)calloc(assoc, sizeof(struct cache_blk_t));
		C->L1_b_blocks[i] = (struct cache_blk_t *)calloc(assoc, sizeof(struct cache_blk_t));
	}
	
  for(i = 0; i < L2_nsets; i++) {
  		C->L2_blocks[i] = (struct cache_blk_t *)calloc(assoc, sizeof(struct cache_blk_t));
  }
  return C;
}

//------------------------------//
// Whichone refers to the following:
// 0: L1_A
// 1: L1_B
// 2: L2
void updateLRU(struct cache_t *cp ,int index, int way, int whichone) {
	int k ;
	cache_t *blkptr;
	if(whichone == 0)
	{
		blkptr = cp->L1_a_blocks;
	}
	else if(whichone == 1)
	{
		blkptr = cp->L1_b_blocks;
	}
	else // if(whichone == 2)
	{
		blkptr = cp->L2_blocks;
	}
	for (k=0 ; k< cp->assoc ; k++) 
	{
		if(blkptr[index][k].LRU < blkptr[index][way].LRU)
		{
	    	blkptr[index][k].LRU = blkptr[index][k].LRU + 1 ;
	 	}
	}
	blkptr[index][way].LRU = 0 ;
}

// if which_L1 = 0, it's L1_A
// else L1_B
int cache_access(struct cache_t *cp, unsigned long address, int access_type, int which_L1)
{
	int i,latency ;
	int block_address ;
	int L1_index ;
	int L2_index ;
	int L1_tag ;
	int L2_tag ;
	int way ;
	int max ;

	block_address = (address / cp->blocksize);
	L1_tag = block_address / cp->L1_nsets;
	L1_index = block_address - (L1_tag * cp->L1_nsets) ;

	L2_tag = block_address / cp->L2_nsets;
	L2_index = block_address - (L2_tag * cp->L2_nsets) ;

	latency = 0;
	for (i = 0; i < cp->assoc; i++) {	/* look for the requested block */
	  if (cp->blocks[index][i].tag == tag && cp->blocks[index][i].valid == 1) {
	  updateLRU(cp, index, i) ;
	  if (access_type == 1) cp->blocks[index][i].dirty = 1 ;
	  return(latency);					/* a cache hit */
	  }
	}
	/* a cache miss */
	for (way=0 ; way< cp->assoc ; way++)		/* look for an invalid entry */
	    if (cp->blocks[index][way].valid == 0) {
		  latency = latency + cp->mem_latency;	/* account for reading the block from memory*/
										/* should instead read from L2, in case you have an L2 */
	      cp->blocks[index][way].valid = 1 ;
	      cp->blocks[index][way].tag = tag ;
		  updateLRU(cp, index, way); 
		  cp->blocks[index][way].dirty = 0;
	      if(access_type == 1) cp->blocks[index][way].dirty = 1 ;
		  return(latency);				/* an invalid entry is available*/
	  }

	 max = cp->blocks[index][0].LRU ;	/* find the LRU block */
	 way = 0 ;
	 for (i=1 ; i< cp->assoc ; i++)  
	  if (cp->blocks[index][i].LRU > max) {
	    max = cp->blocks[index][i].LRU ;
	    way = i ;
	  }
	if (cp->blocks[index][way].dirty == 1)  
	{
		latency = latency + cp->mem_latency;	/* for writing back the evicted block */
	}
	latency = latency + cp->mem_latency;		/* for reading the block from memory*/
	
					/* should instead write to and/or read from L2, in case you have an L2 */
	cp->blocks[index][way].tag = tag ;
	updateLRU(cp, index, way) ;
	cp->blocks[index][i].dirty = 0 ;
	if(access_type == 1) cp->blocks[index][i].dirty = 1 ;
	return(latency) ;
}
