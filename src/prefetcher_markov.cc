/*
 * A sample prefetcher which does sequential one-block lookahead.
 * This means that the prefetcher fetches the next block _after_ the one that
 * was just accessed. It also ignores requests to blocks already in the cache.
 */

#include "interface.hh"

struct table_input {
	Addr addr;
	int count;
};

const unsigned int MAX_TABLE_ROWS = 64;
const unsigned int MAX_TABLE_COLUMNS = 4;

//Addr miss_table[MAX_LIST_SIZE][LIST_INPUTS];
//Addr miss_table_index[MAX_LIST_SIZE];

//--------------------------------------------
//--------------------------------------------
class Table {
    private:
    int rows = 64;
    int columns = 4;
    table_input  miss_table[rows][columns];
    table_input  index_list[rows];
    int last_miss_index;

    public:
    Table ();

    void insert_miss (Addr miss)
    {
	bool found = false;
        if (last_miss_index!=-1) {
	    for (int i=0; i < columns; i++) {
		if (miss_table[last_miss_index][i].addr == miss){
			miss_table[last_miss_index][i].count++;
			found = true; //use this flag to determine that the addr was already in the matrix
			break;
		}
	    }
	    if (found == false){
		//Replace miss_table content with new miss.
		int lowest = 0;
                for (int i=1; i < columns; i++) {
                    if (miss_table[last_miss_index][i].count <= miss_table[last_miss_index][lowest].count){
                        lowest = i;
                    }
                }
		miss_table[last_miss_index][lowest].addr = miss;
		miss_table[last_miss_index][lowest].count = 0;
  	    }
	}
	found = false;
        for (int i=0; i<rows; i++) {
	    if (index_table[i].addr == miss) {
		index_table[i].count++;
                last_miss_index = i;
		found = true;
		break;
	    }
	}
	if (found == false) {
            int lowest = 0;
            for (int i=1; i < rows; i++) {
                if (index_table[i].count <= index_table[lowest].count){
                    lowest = i;
                }
            }
            index_table[lowest].addr = miss;
            index_table[lowest].count = 0;
	}
    }
    
    Addr get_next_miss (Addr miss) {
        for (int i=0; i<rows; i++) {
            if (index_table[i].addr == miss) {
                int highest = 0;
		for (int j=1; j<columns; j++) {
                    if (miss_table[i][j].count >= miss_table[i][highest].count){
                        highest = j;
		    }
		}
		return miss_table[i][highest].addr;
	    }
        }
    }
};

Table markov_table();
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
        markov_table.insert_miss(stat.mem_addr);
        Addr pf_addr = markov_table.get_next_miss(stat.mem_addr);
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
