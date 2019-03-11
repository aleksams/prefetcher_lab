/*
 * A sample prefetcher which does sequential one-block lookahead.
 * This means that the prefetcher fetches the next block _after_ the one that
 * was just accessed. It also ignores requests to blocks already in the cache.
 */

#include "interface.hh"

struct table_input {
	Addr addr;
	int count;
}

const unsigned int MAX_TABLE_SIZE = 64;
const unsigned int MAX_TABLE_INPUTS = 4;

//Addr miss_table[MAX_LIST_SIZE][LIST_INPUTS];
//Addr miss_table_index[MAX_LIST_SIZE];

//--------------------------------------------
//--------------------------------------------
class Table {
    private:
    table_input  miss_table[][];
    table_input  miss_table_index[];
    int  last_miss_index;
    bool found;
    public:
    Table (int table_rows, int table_columns)
    {
        miss_table[table_rows][table_columns];
        miss_table_index[table_rows];
	last_miss_index = -1;
    }
    void insert_miss (Addr miss)
    {
	found = 0;
        if (last_miss_index!=-1) {
	    for (int i=0; i < MAX_TABLE_INPUTS; i++) {
		if (miss_table[last_miss_index][i].addr == miss){
			miss_table[last_miss_index][i].count++;
			found = 1; //use this flag to determine that the addr was already in the matrix
		}
	    }
	    if (found == 0){
			//Replace miss_table content with new miss.
  	    }
	}
        for (int i=0; i<MAX_TABLE_SIZE; i++) {
	    if (miss_table_index[i].addr == miss) {
		last_miss_index = i;
		//Need a flag to set that it was found.
	    }
	}
   	//CHECK FOR FOUND AND REPLACE IF NOT. 
   }
};
//--------------------------------------------
//--------------------------------------------

void prefetch_init(void)
{
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */

    //DPRINTF(HWPrefetch, "Initialized sequential-on-access prefetcher\n");
}

void prefetch_access(AccessStat stat)
{
    /* pf_addr is now an address within the _next_ cache block */
    //Addr pf_addr = stat.mem_addr + BLOCK_SIZE;

    /*
     * Issue a prefetch request if a demand miss occured,
     * and the block is not already in cache.
     */
    /*if (stat.miss && !in_cache(pf_addr)) {
        issue_prefetch(pf_addr);
    }*/
    if (stat.miss) {
        miss_list.create_node(stat.mem_addr);
        Addr pf_addr = miss_list.next_miss(stat.mem_addr);
	if (pf_addr==0) {
            pf_addr = stat.mem_addr + BLOCK_SIZE;
        }
	if (!in_cache(pf_addr)){
	    issue_prefetch(pf_addr);
	}
    }
}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
