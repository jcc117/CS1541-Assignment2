/* This file contains a rough implementation of an L1 cache in the absence of an L2 cache*/
#include <stdlib.h>
#include <stdio.h>

#define SW 1;
#define LW 0;
#define D 0;
#define I 1;

typedef struct cache_blk_t { // note that no actual data will be stored in the cache 
	char IorD; // Always 1 for I instructions; Always 0 for D instructions
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
	
	// block size
	int blocksize;
	// associativity	
  	int I_assoc;
	int D_assoc;
	int L2_assoc;
	// number of sets	
  	int I_nsets;
	int D_nsets;
	int L2_nsets;
	// the miss penalties			
  	int L1_lat;	
  	int L2_lat;
  	int mem_lat;
	// a pointer to the array of Instruction cache blocks
  	struct cache_blk_t **I_blocks;	
	// a pointer to the array of Data cache blocks
  	struct cache_blk_t **D_blocks;
	// a pointer to the array of L2 cache blocks
  	struct cache_blk_t **L2_blocks;	
} cache_t;

/*Setting up the Cache*/
// cache_t *theCache;
// theCache = malloc(sizeof(cache_t));
// theCache = cache_create(4, 8, 4, 1, 0, 200);
// We may need to be able to free this stuff
struct cache_t * cache_create(	int I_size, int I_assoc, 
								int D_size, int D_assoc,
								int L2_size, int L2_assoc,
								int L1_lat, int L2_lat, int mem_lat, int blocksize) {
  int i, I_nblocks, B_nblocks, L2_nblocks, I_nsets, D_nsets, L2_nsets ;
  struct cache_t *C = (struct cache_t *)calloc(1, sizeof(struct cache_t));

  I_nblocks = I_size *1024 / blocksize ;// number of blocks in the cache
  I_nsets = I_nblocks / I_assoc ;			// number of sets (entries) in the cache
  
  D_nblocks = D_size *1024 / blocksize ;// number of blocks in the cache
  D_nsets = D_nblocks / D_assoc ;			// number of sets (entries) in the cache
  
  C->blocksize = blocksize ;
  
  C->I_nsets = I_nsets;
  C->I_assoc = I_assoc;
  
  C->D_nsets = D_nsets;
  C->D_assoc = D_assoc;
  
  C->L1_lat = L1_lat;
  C->L2_lat = L2_lat;
  C->mem_lat = mem_lat;

  C->I_blocks= (struct cache_blk_t **)calloc(L1_nsets, sizeof(struct cache_blk_t *));
  C->D_blocks= (struct cache_blk_t **)calloc(L1_nsets, sizeof(struct cache_blk_t *));
  C->L2_blocks= (struct cache_blk_t **)calloc(L2_nsets, sizeof(struct cache_blk_t *));

  	for(i = 0; i < I_nsets; i++) {
		C->I_blocks[i] = (struct cache_blk_t *)calloc(assoc, sizeof(struct cache_blk_t));
		C->I_blocks[i].IorD = I;
	}
	
  	for(i = 0; i < D_nsets; i++) {
  		C->D_blocks[i] = (struct cache_blk_t *)calloc(assoc, sizeof(struct cache_blk_t));
  		C->D_blocks[i].IorD = D;
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
	cache_blk_t **blkptr;
	int assoc;
	if(whichone == I)
	{
		blkptr = cp->I_blocks;
		assoc = cp->I_assoc;
	}
	else if(whichone == D)
	{
		blkptr = cp->D_blocks;
		assoc = cp->D_assoc;
	}
	else // if(whichone == 2)
	{
		blkptr = cp->L2_blocks;
		assoc = cp->L2_assoc;
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
/*
	How this works:
	It tries to find an object in a given L1 cache.  Add L1 latency.
	-> If hit, great, we're done.
	-> if not, we check L2 and add its latency to latency
		-> If we find it in L2, we need to check if we need to evict in L1
			-> if yes, evict, enter, and mark the entry as dirty in L2
			-> if no, just enter
		-> If we do not find it in L2, we have to find it in memory, which uses the mem latency.
			-> once we find it, if we have to evict in L2, we also have to evict in the APPROPRIATE L1 cache (I or D)
			-> Regardless we enter it into L2 and the appropriate L1.
*/
int cache_access(struct cache_t *cp, char *access_other, unsigned long address, int access_type, int which_L1)
{
	int i,latency ;
	int block_address ;
	int L1_index ;
	int L2_index ;
	int L1_tag ;
	int L2_tag ;
	int L1_assoc;
	int L2_assoc;
	int way ;
	int way2;
	int max ;
	
	char l1_evict = 1;
	char l2_evict = 1;
	
	cache_blk_t **l1ptr;
	
	// Get block address
	block_address = (address / cp->blocksize);
	
	// Calculate the appropriate parameters for the L1 cache, given which_L1
	if(which_L1 == D)
	{
		l1ptr = cp->D_blocks;
		L1_tag = block_address / cp->D_nsets;
		L1_index = block_address - (L1_tag * cp->D_nsets) ;
		L1_assoc = cp->D_assoc;
	}
	else if(which_L1 == I)
	{
		l1ptr = cp->I_blocks;
		L1_tag = block_address / cp->I_nsets;
		L1_index = block_address - (L1_tag * cp->I_nsets) ;
		L1_assoc = cp->I_assoc;
	}
	else
	{
		fprintf(stderr, "Error! Invalid Cache access!\n");
		return 0;
	}

	L2_tag = block_address / cp->L2_nsets;
	L2_index = block_address - (L2_tag * cp->L2_nsets) ;
	L2_assoc = cp->L2_assoc;

	latency = cp->L1_lat; // Probably 0
	
	/* look for the requested block in L1*/
	for (i = 0; i < L1_assoc; i++) {	
		if (l1ptr[L1_index][i].tag == L1_tag && l1ptr[L1_index][i].valid == 1) {
	  		updateLRU(cp, L1_index, way, which_L1) ;
	  		if (access_type == 1) l1ptr[L1_index][i].dirty = 1 ;
	  		{
				// Cache Hit
	  			return(latency);
			}
		}
	}
	
	/************* L1 cache miss. **************/
  	latency += cp->L2_lat;	/* account for reading the block from L2*/
	
	/*READING FROM L2*/
	for (i = 0; i < L2_assoc; i++) {	/* look for the requested block in L2 */
		if (cp->L2_blocks[L2_index][i].tag == L2_tag && cp->L2_blocks[L2_index][i].valid == 1)
		{
	  		updateLRU(cp, L2_index, way, 2) ;
	  	 	if (access_type == 1)
			{
				cp->L2_blocks[L2_index][i].dirty = 1 ;
			}
			// Now we must determine if we need to evict from L1
			for (way=0 ; way< L1_assoc ; way++) {		/* look for an invalid entry to fill */
				if (l1ptr[L1_index][way].valid == 0) // Found empty spot, enter value
				{
			    	l1ptr[L1_index][way].valid = 1 ;
			    	l1ptr[L1_index][way].tag = L1_tag ;
					updateLRU(cp, L1_index, way, which_L1); 
					
					// Set dirty bit appropriately
					if(access_type == 1)
					{
						l1ptr[L1_index][way].dirty = 1 ;
					}
					else
					{
				  		l1ptr[L1_index][way].dirty = 0;
					}
					l1_evict = 0;
					return latency;
			  	}
		  	}
			if(l1_evict == 1) // need to evict
			{
				max = l1ptr[L1_index][0].LRU ;	/* find the LRU block */
				way = 0 ;
				for (i=1 ; i< L1_assoc ; i++) {  
					if (l1ptr[L1_index][i].LRU > max)
				  	{
				   		max = l1ptr[L1_index][i].LRU ;
				    	way = i ;
				  	}
					if (l1ptr[L1_index][way].dirty == 1)  
					{
						latency += cp->L2_lat;	/* for writing back the evicted block */
					}
					latency += cp->L2_lat;		/* for reading the block from memory*/
				}
			}
	  	  	return latency ;
	 	}
	 }
	
	
	/************* L2 cache miss. **************/
	latency += cp->mem_lat; // Apply memory latency
	
	for (way=0 ; way< L2_assoc ; way++) {		/* look for an invalid entry to fill in L2*/
		if (cp->L2_blocks[L2_index][way].valid == 0) // Found empty spot, enter value
		{
	    	cp->L2_blocks[L2_index][way].valid = 1 ;
	    	cp->L2_blocks[L2_index][way].tag = L2_tag ;
			cp->L2_blocks[L2_index][way].dirty = 0 ;
			updateLRU(cp, L2_index, way, 2); 
			
			l2_evict = 0;
			// Now find empty spot in L1
			for (way2 = 0; way2 < L1_assoc; way2++) [
				if (l1ptr[L1_index][way].valid == 0) // Found empty spot, enter value
				{
			    	l1ptr[L1_index][way].valid = 1 ;
			    	l1ptr[L1_index][way].tag = L1_tag ;
					l1ptr[L1_index][way].dirty = 0 ;
					updateLRU(cp, L1_index, way, which_L1); 
					return latency;
				}
			}
	  	}
  	}
	if(l2_evict == 1) // need to evict
	{
		max = cp->L2_blocks[L2_index][0].LRU ;	/* find the L2 LRU block */
		way = 0 ;
		for (i=1 ; i< L2_assoc ; i++) {  
			if (cp->L2_blocks[L2_index][i].LRU > max)
		  	{
		   		max = cp->L2_blocks[L2_index][i].LRU ;
		    	way = i ;
		  	}
		}
		
		if (cp->L2_blocks[L2_index][way].dirty == 1)  
		{
			latency += cp->mem_lat;	/* for writing back the evicted block */
		}
		latency += cp->mem_lat;		/* for reading the block from memory*/
		
		cp->L2_blocks[L2_index][way].valid = 1;
		cp->L2_blocks[L2_index][way].tag = L1_tag;
		cp->L2_blocks[L2_index][way].dirty = 0;
		cp->L2_blocks[L2_index][way].irod = which_L1;
		
		if(cp->L2_blocks[L2_index][way].iord == D)
		{
			l1ptr = cp->D_blocks;
			L1_tag = block_address / cp->D_nsets;
			L1_index = block_address - (L1_tag * cp->D_nsets) ;
			L1_assoc = cp->D_assoc;
		}
		if(cp->L2_blocks[L2_index][way].iord == I)
		{
			l1ptr = cp->I_blocks;
			L1_tag = block_address / cp->I_nsets;
			L1_index = block_address - (L1_tag * cp->I_nsets) ;
			L1_assoc = cp->I_assoc;
		}
		else
		{
			fprintf(stderr, "Eviction Type Error!");
			return latency;
		}
		
		max = l1ptr[L1_index][0].LRU ;	/* find the L1 LRU block */
		way = 0 ;
		
		for (i=1 ; i< L1_assoc ; i++) {  
			if (l1ptr[L1_index][i].LRU > max)
		  	{
		   		max = l1ptr[L1_index][i].LRU ;
		    	way = i ;
		  	}
		}
		
		if (l1ptr[L1_index][way].dirty == 1)  
		{
			latency += cp->L2_lat;	/* for writing back the evicted block */
		}
		latency += cp->L2_lat;		/* for reading the block from memory*/
		
		l1ptr[L1_index][way].valid = 1;
		l1ptr[L1_index][way].tag = L1_tag;
		l1ptr[L1_index][way].dirty = 0;
		l1ptr[L1_index][way].irod = cp->L2_blocks[L2_index][way].iord;
	}
	
	return latency;
	
	// max = l1ptr[L1_index][0].LRU ;	/* find the LRU block */
	// way = 0 ;
	// for (i=1 ; i< cp->assoc ; i++) {
	// 	if (l1ptr[L1_index][i].LRU > max)
	//   	{
	//    		max = l1ptr[L1_index][i].LRU ;
	//     	way = i ;
	//   	}
	// 	if (l1ptr[L1_index][way].dirty == 1)
	// 	{
	// 		latency = latency + cp->L1_mem_latency;	/* for writing back the evicted block */
	// 	}
	// 	latency = latency + cp->L1_mem_latency;		/* for reading the block from memory*/
	// }
	
	
	//////////////////////////////
	//  max = cp->L2_blocks[L2_index][0].LRU ;	/* find the LRU block */
	//  way = 0 ;
	//  for (i=1 ; i< cp->assoc ; i++)
	//   if (cp->L2_blocks[L2_index][i].LRU > max) {
	//     max = cp->L2_blocks[L2_index][i].LRU ;
	//     way = i ;
	//   }
	// if (cp->L2_blocks[L2_index][way].dirty == 1)
	// {
	// 	latency = latency + cp->L2_mem_latency;	/* for writing back the evicted block */
	// }
	// latency = latency + cp->L2_mem_latency;		/* for reading the block from memory*/
	//
	// 				/* should instead write to and/or read from L2, in case you have an L2 */
	// cp->L2_blocks[L2_index][way].tag = L2_tag ;
	// updateLRU(cp, L2_index, way, 2) ;
	// cp->L2_blocks[L2_index][i].dirty = 0 ;
	// if(access_type == 1) cp->L2_blocks[L2_index][i].dirty = 1 ;
	// return(latency) ;
}
