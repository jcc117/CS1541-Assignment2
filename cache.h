/* This file contains a rough implementation of an L1 cache in the absence of an L2 cache*/
#include <stdlib.h>
#include <stdio.h>

#define SW 1
#define LW 0
#define D 0
#define I 1

typedef struct cache_blk_t { // note that no actual data will be stored in the cache 
	char IorD; // Always 1 for I instructions; Always 0 for D instructions
  	unsigned long tag;
  	char valid;
  	char dirty;
  	unsigned long LRU;	//to be used to build the LRU stack for the blocks in a cache set
    char in_L1; //Used for L2 caches to check if a block is in the L1 cache
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
  int i, I_nblocks, D_nblocks, L2_nblocks, I_nsets, D_nsets, L2_nsets ;
  struct cache_t *C = (struct cache_t *)calloc(1, sizeof(struct cache_t));

  I_nblocks = I_size *1024 / blocksize ;// number of blocks in the cache
  if(I_assoc != 0)
    I_nsets = I_nblocks / I_assoc ;			// number of sets (entries) in the cache
  else
    I_nsets = 0;
  
  D_nblocks = D_size *1024 / blocksize ;// number of blocks in the cache
  if(D_assoc != 0)
    D_nsets = D_nblocks / D_assoc ;			// number of sets (entries) in the cache
  else
    D_nsets = 0;
  
  C->blocksize = blocksize ;
  
  C->I_nsets = I_nsets;
  C->I_assoc = I_assoc;
  
  C->D_nsets = D_nsets;
  C->D_assoc = D_assoc;
  
  C->L1_lat = L1_lat;
  C->L2_lat = L2_lat;
  C->mem_lat = mem_lat;

  L2_nblocks = L2_size * 1024 / blocksize;
  if(L2_assoc == 0)
  	L2_nsets = 0;
  else
 	 L2_nsets = L2_nblocks / L2_assoc;
  C->L2_nsets = L2_nsets;
  C->L2_assoc = L2_assoc;

  C->I_blocks= (struct cache_blk_t **)calloc(I_nsets, sizeof(struct cache_blk_t *));
  C->D_blocks= (struct cache_blk_t **)calloc(D_nsets, sizeof(struct cache_blk_t *));
  C->L2_blocks= (struct cache_blk_t **)calloc(L2_nsets, sizeof(struct cache_blk_t *));

  	for(i = 0; i < I_nsets; i++) {
		C->I_blocks[i] = (struct cache_blk_t *)calloc(I_assoc, sizeof(struct cache_blk_t));
		C->I_blocks[i]->IorD = I;
	}
	
  	for(i = 0; i < D_nsets; i++) {
  		C->D_blocks[i] = (struct cache_blk_t *)calloc(D_assoc, sizeof(struct cache_blk_t));
  		C->D_blocks[i]->IorD = D;
  	}
	
  	for(i = 0; i < L2_nsets; i++) {
  		C->L2_blocks[i] = (struct cache_blk_t *)calloc(L2_assoc, sizeof(struct cache_blk_t));
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
	//printf("0\n");
	if(whichone == I)
	{
		blkptr = cp->I_blocks;
		assoc = cp->I_assoc;
		//printf("0\n");
	}
	else if(whichone == D)
	{
		blkptr = cp->D_blocks;
		assoc = cp->D_assoc;
		//printf("1\n");
	}
	else // if(whichone == 2)
	{
		blkptr = cp->L2_blocks;
		assoc = cp->L2_assoc;
		//printf("2\n");
	}
	for (k=0 ; k < assoc ; k++) 
	{
		//printf("I crash here\n");
		//printf("%d, %d\n", index, way);	//Way calculation is WRONG!!!!!
		//printf("%d, %d, %d, %d\n", blkptr[index][k].LRU, blkptr[index][way].LRU, index, way);
		if(blkptr[index][k].LRU <= blkptr[index][way].LRU)
		{
	    	blkptr[index][k].LRU++;// = blkptr[index][k].LRU + 1 ;
	 	}
	 	//printf("%d, %d, %d, %d\n", blkptr[index][k].LRU, blkptr[index][way].LRU, index, way);
	 	//printf("not here\n");
	 	//exit(0);
	}
	//printf("done\n");
	blkptr[index][way].LRU = 0 ;
}

// Finds least recently used block
int findLRU(cache_blk_t **bptr, int index, int assoc) {
	int max = bptr[index][0].LRU ;	/* find the L1 LRU block */
	int way = 0 ;
	int i;
	
	for (i=1 ; i< assoc ; i++) {  
		if (bptr[index][i].LRU > max)
	  	{
	   		max = bptr[index][i].LRU ;
	    	way = i ;
	  	}
	}
	return way;
}

//Finds least recently used block that is not in an L1 cache
//Returns -1 if none are not in an L1 cache
int findLRU2(cache_blk_t **bptr, int index, int assoc)
{
  int max = -1;
  int way = -1;
  int i;

  for(i = 0; i < assoc; i++)
  {
    if(bptr[index][i].LRU > max && bptr[index][i].in_L1 == 0)
    {
      max = bptr[index][i].LRU;
      way = i;
    }
  }
  return way;
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
	int evict_tag;
	int evict_addr;
	int evict_index;
	char l2_evict_type;
	
	char l1_evict = 1;
	char l2_evict = 1;
	
	cache_blk_t **l1ptr;
	
	// Get block address
	block_address = (address / cp->blocksize);
	
	*access_other = 0;

	//printf("here?\n");
	
	// Calculate the appropriate parameters for the L1 cache, given which_L1
	if(which_L1 == D)
	{
		l1ptr = cp->D_blocks;
    if(cp->D_nsets == 0)  //Check if we are using perfect caches
      return 0;
		L1_tag = block_address / cp->D_nsets;
		L1_index = block_address - (L1_tag * cp->D_nsets) ;
		L1_assoc = cp->D_assoc;
	}
	else if(which_L1 == I)
	{
		l1ptr = cp->I_blocks;
    if(cp->I_nsets == 0)  //Check if we are using perfect caches
      return 0;
		L1_tag = block_address / cp->I_nsets;
		L1_index = block_address - (L1_tag * cp->I_nsets) ;
		L1_assoc = cp->I_assoc;
	}
	else
	{
		fprintf(stderr, "Error! Invalid Cache access!\n");
		return 0;
	}

	//printf("I'm here\n");
	//printf("%d\n", cp->L2_nsets);


	latency = cp->L1_lat; // Probably 0
	
	/* look for the requested block in L1*/
	for (i = 0; i < L1_assoc; i++) {	
		if (l1ptr[L1_index][i].tag == L1_tag && l1ptr[L1_index][i].valid == 1) {
	  		//printf("down here\n");
	  		updateLRU(cp, L1_index, i, which_L1) ;
        /****************************************************************************************
        update L2 lru if applicable
        *************************************/
	  		if (access_type == 1) l1ptr[L1_index][i].dirty = 1 ;
	  		{
				// Cache Hit
	  			return(latency);
			}
		}
	}

	//printf("Made it here\n");
	
	/************* L1 cache miss. **************/
	// Check to see if we have L2 cache
	if(cp->L2_nsets == 0) // l2 Cache dne
	{
		//fprintf(stderr, "No L2 Cache\n");
		// Now we must determine if we need to evict from L1
		//fprintf(stderr, "Looking for empty spot in L1\n");
		for (way=0 ; way< L1_assoc ; way++) {		/* look for an invalid entry to fill */
			if (l1ptr[L1_index][way].valid == 0) // Found empty spot, enter value
			{
				//fprintf(stderr, "Empty L1 Cache spot found, inserting\n");
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
				return latency + cp->mem_lat;
		  	}
	  	}
		if(l1_evict == 1) // need to evict
		{
			//fprintf(stderr, "Evicting L1 Cache spot\n");
			way = findLRU(l1ptr, L1_index, L1_assoc); /*Find the LRU L1 Block*/ 
			if (l1ptr[L1_index][way].dirty == 1)  
			{
				latency += cp->mem_lat;	/* for writing back the evicted block? */
			}
			latency += cp->mem_lat;		/* for reading the block from memory*/
			
			l1ptr[L1_index][way].tag = L1_tag; // Update new tag
			updateLRU(cp, L1_index, way, which_L1); 
		}
  	  	return latency ;
 	}

 	L2_tag = block_address / cp->L2_nsets;
	L2_index = block_address - (L2_tag * cp->L2_nsets) ;
	L2_assoc = cp->L2_assoc;

	//fprintf(stderr, "L1 Cache miss, checking L2\n");
  	latency += cp->L2_lat;	/* account for reading the block from L2*/
	
	/*READING FROM L2*/
	for (i = 0; i < L2_assoc; i++) {	/* look for the requested block in L2 */
		if (cp->L2_blocks[L2_index][i].tag == L2_tag && cp->L2_blocks[L2_index][i].valid == 1)
		{
       //printf("Way is %d\n", i);
	  		updateLRU(cp, L2_index, i, 2) ;
        cp->L2_blocks[L2_index][i].in_L1 = 1;
        //printf("its in the update\n");
	  	 	if (access_type == 1)
			{
				cp->L2_blocks[L2_index][i].dirty = 1 ;
			}
        //printf("come down here\n");
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
        //printf("here\n");
        //Set dirty bit in the L2 cache
        int L1_nsets;
        if(which_L1 == I) //Set L1 nsets correctly
          L1_nsets = cp->I_nsets;
        else
          L1_nsets = cp->D_nsets;

        int b_addr = L1_index + (l1ptr[L1_index][way].tag * L1_nsets);  //Calculate block address
        int other_L2_tag = b_addr / cp->L2_nsets; //Get L2 tag
        int other_L2_index = b_addr - (other_L2_tag * cp->L2_nsets) ; //Get l2 index

				way = findLRU(l1ptr, L1_index, L1_assoc); /*Find the LRU L1 Block*/ 
				if (l1ptr[L1_index][way].dirty == 1)  //Write back the block to L1 if necessary
				{
					latency += cp->L2_lat;	/* for writing back the evicted block? */

					int k;
					for(k = 0; k < L2_assoc; k++)	//Set the dirty bit to 1, it must be in the L2 cache
					{
						if(cp->L2_blocks[other_L2_index][k].tag == other_L2_tag)
							cp->L2_blocks[other_L2_index][k].dirty = 1;
					} 
				}

        int k;
        for(k = 0; k < L2_assoc; k++) //Set the in_L1 bit to 0, it must be in the L2 cache
        {
          if(cp->L2_blocks[other_L2_index][k].tag == other_L2_tag)
            cp->L2_blocks[other_L2_index][k].in_L1 = 0;
        } 
				//latency += cp->L2_lat;		/* for reading the block from memory*/
				
				l1ptr[L1_index][way].tag = L1_tag; // Update new tag
				// Set dirty bit appropriately
				if(access_type == 1)
				{
					l1ptr[L1_index][way].dirty = 1 ;
				}
				else
				{
			  		l1ptr[L1_index][way].dirty = 0;
				}
				updateLRU(cp, L1_index, way, which_L1); 
			}
	  	  	return latency ;
	 	}
	 }
	
	 //printf("L2 miss\n");
	/************* L2 cache miss. **************/
	latency += cp->mem_lat; // Apply memory latency
	
	for (way=0 ; way< L2_assoc ; way++) {		/* look for an invalid entry to fill in L2*/
		if (cp->L2_blocks[L2_index][way].valid == 0) // Found empty spot, enter value
		{
	    	cp->L2_blocks[L2_index][way].valid = 1 ;
	    	cp->L2_blocks[L2_index][way].tag = L2_tag ;
			cp->L2_blocks[L2_index][way].dirty = 0 ;
			cp->L2_blocks[L2_index][way].IorD = which_L1;
        cp->L2_blocks[L2_index][way].in_L1 = 1;
			updateLRU(cp, L2_index, way, 2); 
			
			l2_evict = 0;
			// Now find empty spot in L1
			for (way2 = 0; way2 < L1_assoc; way2++) {
				if (l1ptr[L1_index][way].valid == 0) // Found empty spot, enter value
				{
			    	l1ptr[L1_index][way].valid = 1 ;
			    	l1ptr[L1_index][way].tag = L1_tag ;
					// Set dirty bit appropriately
					if(access_type == 1)
					{
						l1ptr[L1_index][way].dirty = 1 ;
					}
					else
					{
				  		l1ptr[L1_index][way].dirty = 0;
					}
					updateLRU(cp, L1_index, way, which_L1); 
					return latency;
				}
			}

			/*That L1 is full. Evict something from it*/
      int L1_nsets;
      if(which_L1 == I) //Set L1 nsets correctly
        L1_nsets = cp->I_nsets;
      else
        L1_nsets = cp->D_nsets;

      int b_addr = L1_index + (l1ptr[L1_index][way].tag * L1_nsets);  //Calculate block address
      int other_L2_tag = b_addr / cp->L2_nsets; //Get L2 tag
      int other_L2_index = b_addr - (other_L2_tag * cp->L2_nsets) ; //Get l2 index

			way = findLRU(l1ptr, L1_index, L1_assoc); /*Find the LRU L1 Block*/ 
			if (l1ptr[L1_index][way].dirty == 1)  
			{
				latency += cp->L2_lat;	/* for writing back the evicted block */

        //Set dirty bit in the L2 cache
          int k;
          for(k = 0; k < L2_assoc; k++) //Set the dirty bit to 1, it must be in the L2 cache
          {
            if(cp->L2_blocks[other_L2_index][k].tag == other_L2_tag)
              cp->L2_blocks[other_L2_index][k].dirty = 1;
          } 
			}

      int k;
      for(k = 0; k < L2_assoc; k++) //Set the in_L1 bit to 0
      {
        if(cp->L2_blocks[other_L2_index][k].tag == other_L2_tag)
          cp->L2_blocks[other_L2_index][k].in_L1 = 0;
      } 
			//latency += cp->L2_lat;		/* for reading the block from memory*/
			
			l1ptr[L1_index][way].tag = L1_tag; // Update new tag
			// Set dirty bit appropriately
			if(access_type == 1)
			{
				l1ptr[L1_index][way].dirty = 1 ;
			}
			else
			{
		  		l1ptr[L1_index][way].dirty = 0;
			}
			updateLRU(cp, L1_index, way, which_L1); 
			return latency;
	  	}
  	}
	if(l2_evict == 1) // need to evict
	{
		// Evict L2 first
		int dirty_flag = 0;
    way = findLRU2(cp->L2_blocks, L2_index, L2_assoc);
    if(way == -1) //Everything in the set is in an L1 cache(special case)
    {
		  way = findLRU(cp->L2_blocks, L2_index, L2_assoc); /*Find the LRU L2 Block*/ 
    }

		if (cp->L2_blocks[L2_index][way].dirty == 1)  
		{
			latency += cp->mem_lat;	/* for writing back the evicted block */
			dirty_flag = 1;
		}
		//latency += cp->mem_lat;		/* for reading the block from memory*/
		
		evict_tag = cp->L2_blocks[L2_index][way].tag; // Store evicted tag
		cp->L2_blocks[L2_index][way].tag = L2_tag; // Update new tag
		l2_evict_type = cp->L2_blocks[L2_index][way].IorD; // Store old irod
		
		cp->L2_blocks[L2_index][way].IorD = which_L1; // enter new IorD
		cp->L2_blocks[L2_index][way].dirty = 0; // Clean as snow 
		cp->L2_blocks[L2_index][way].valid = 1;
		updateLRU(cp, L2_index, way, 2); 
		
		// reverse engineer the entry we need to evict
		// L2_tag = block_address / cp->L2_nsets;
		// L2_index = block_address - (L2_tag * cp->L2_nsets) ;
		evict_addr = L2_index + (evict_tag * cp->L2_nsets);
		 
		// Now we find the same entry in the L1 cache with the same irod as the one we evicted from L2.
		if(l2_evict_type == I) // evict from I
		{
			evict_tag = evict_addr / cp->I_nsets; // overwrite evict_tag
			evict_index = evict_addr - (evict_tag * cp->I_nsets) ;
			for (i = 0; i < cp->I_assoc; i++) {	
				if (cp->I_blocks[evict_index][i].tag == evict_tag && cp->I_blocks[evict_index][i].valid == 1) {
			  		cp->I_blocks[evict_index][i].valid = 0; // evicted!

			  		if(cp->I_blocks[evict_index][i].dirty == 1 && !dirty_flag)	//Check if the block is dirty and wasn't already written back
			  			latency += cp->mem_lat;
					break;
				}
			}
		}
		else if(l2_evict_type == D) // evict from D
		{
			evict_tag = evict_addr / cp->D_nsets; // overwrite evict_tag
			evict_index = evict_addr - (evict_tag * cp->D_nsets) ;
			for (i = 0; i < cp->D_assoc; i++) {	
				if (cp->D_blocks[evict_index][i].tag == evict_tag && cp->D_blocks[evict_index][i].valid == 1) {
			  		cp->D_blocks[evict_index][i].valid = 0; // evicted!

			  		if(cp->D_blocks[evict_index][i].dirty == 1 && !dirty_flag)	//Check if the block is dirty and wasn't already written back
			  			latency += cp->mem_lat;
					break;
				}
			}
		}
		/******************************************************************************
			
		************************************************/

		//Search for an L1 Block(mostly same code as above)

		// Now we must determine if we need to evict from proper L1
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
			way = findLRU(l1ptr, L1_index, L1_assoc); /*Find the LRU L1 Block*/ 

      int L1_nsets;
      if(which_L1 == I) //Set L1 nsets correctly
        L1_nsets = cp->I_nsets;
      else
        L1_nsets = cp->D_nsets;

      int b_addr = L1_index + (l1ptr[L1_index][way].tag * L1_nsets);  //Calculate block address
      int other_L2_tag = b_addr / cp->L2_nsets; //Get L2 tag
      int other_L2_index = b_addr - (other_L2_tag * cp->L2_nsets) ; //Get l2 index

			if (l1ptr[L1_index][way].dirty == 1)  //Write back the block to L1 if necessary
			{
				latency += cp->L2_lat;	/* for writing back the evicted block */
				//Set dirty bit in the L2 cache

				int k;
				for(k = 0; k < L2_assoc; k++)	//Set the dirty bit to 1, it must be in the L2 cache
				{
					if(cp->L2_blocks[other_L2_index][k].tag == other_L2_tag)
						cp->L2_blocks[other_L2_index][k].dirty = 1;
				} 
			}

      int k;
      for(k = 0; k < L2_assoc; k++) //Set the dirty bit to 1, it must be in the L2 cache
      {
        if(cp->L2_blocks[other_L2_index][k].tag == other_L2_tag)
          cp->L2_blocks[other_L2_index][k].in_L1 = 0;
      } 
			//latency += cp->L2_lat;		/* for reading the block from memory*/
			
			l1ptr[L1_index][way].tag = L1_tag; // Update new tag
			// Set dirty bit appropriately
			if(access_type == 1)
			{
				l1ptr[L1_index][way].dirty = 1 ;
			}
			else
			{
		  		l1ptr[L1_index][way].dirty = 0;
			}
			updateLRU(cp, L1_index, way, which_L1);
			return latency; 
		}

		/*
		if(l2_evict_type == which_L1) // we only have to evict from one L1 cache, so now we can just enter the new value.
		{
			l1ptr[evict_index][i].tag = L1_tag; // Update new tag
			l1ptr[evict_index][i].valid = 1;
			cp->L2_blocks[L2_index][i].dirty = 0; // Clean as snow 
			updateLRU(cp, L2_index, way, which_L1); 
		}
		else // Now we have to find an empty spot/the LRU for the OTHER l1 cache, and enter the new data there.
		{
			*access_other = 1;
			// We can change the pointer and others.
			if(l2_evict_type == D)
			{
				l1ptr = cp->D_blocks;
				L1_tag = block_address / cp->D_nsets;
				L1_index = block_address - (L1_tag * cp->D_nsets) ;
				L1_assoc = cp->D_assoc;
			}
			else if(l2_evict_type == I)
			{
				l1ptr = cp->I_blocks;
				L1_tag = block_address / cp->I_nsets;
				L1_index = block_address - (L1_tag * cp->I_nsets) ;
				L1_assoc = cp->I_assoc;
			}
			
			l1_evict = 1;
			for (way=0 ; way< L1_assoc ; way++) {		// look for an invalid entry to fill
				if (l1ptr[L1_index][way].valid == 0) // Found empty spot, enter value
				{
			    	l1ptr[L1_index][way].valid = 1 ;
			    	l1ptr[L1_index][way].tag = L1_tag ;
					updateLRU(cp, L1_index, way, which_L1); 
					
					// Set dirty bit appropriately
					l1ptr[L1_index][way].dirty = 0;
					l1_evict = 0;
					return latency;
			  	}
		  	}
			if(l1_evict == 1) // need to evict
			{
				way = findLRU(l1ptr, L1_index, L1_assoc); //Find the LRU L1 Block 
				if (l1ptr[L1_index][way].dirty == 1)  
				{
					latency += cp->L2_lat;	// for writing back the evicted block
				}
				latency += cp->L2_lat;		// for reading the block from memory
				
				l1ptr[L1_index][way].tag = L1_tag; // Update new tag
				updateLRU(cp, L1_index, way, which_L1); 
			}
		}
		*/
	}
	return latency;
}
