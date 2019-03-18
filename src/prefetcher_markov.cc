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

const unsigned int MAX_TABLE_ROWS = 8*128;
const unsigned int MAX_TABLE_COLUMNS = 2*4;

//Addr miss_table[MAX_LIST_SIZE][LIST_INPUTS];
//Addr miss_table_index[MAX_LIST_SIZE];

//--------------------------------------------
//--------------------------------------------
class Table {
    private:
    table_input  miss_table[MAX_TABLE_ROWS][MAX_TABLE_COLUMNS];
    table_input  index_list[MAX_TABLE_ROWS];
    int last_miss_index=-1;

    public:

    void insert_miss (Addr miss)
    {
	int rows    =    MAX_TABLE_ROWS;
	int columns = MAX_TABLE_COLUMNS;
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
	    if (index_list[i].addr == miss) {
		index_list[i].count++;
                last_miss_index = i;
		found = true;
		break;
	    }
	}
	if (found == false) {
            int lowest = 0;
            for (int i=1; i < rows; i++) {
                if (index_list[i].count <= index_list[lowest].count){
                    lowest = i;
                }
            }
	    last_miss_index = lowest;
            index_list[lowest].addr = miss;
            index_list[lowest].count = 0;
	    for (int i=0; i<columns;i++) {
	        miss_table[lowest][i].addr = 0;
                miss_table[lowest][i].count = 0;
	    }
	}
    }

    Addr get_next_miss (Addr miss) {
	int rows = MAX_TABLE_ROWS;
	int columns = MAX_TABLE_COLUMNS;
        for (int i=0; i<rows; i++) {
            if (index_list[i].addr == miss) {
                int highest = 0;
		for (int j=1; j<columns; j++) {
                    if (miss_table[i][j].count >= miss_table[i][highest].count){
                        highest = j;
		    }
		}
	    return miss_table[i][highest].addr;
	    }
        }
	return 0;
    }
};

static Table * markov_table;
//--------------------------------------------
//--------------------------------------------

void prefetch_init(void)
{
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */

	markov_table = new Table;
    DPRINTF(HWPrefetch, "Initialized Markov prefetcher\n");
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
        markov_table->insert_miss(stat.mem_addr);
        Addr pf_addr = markov_table->get_next_miss(stat.mem_addr);
	/*if (pf_addr==0) {
            pf_addr = stat.mem_addr + BLOCK_SIZE;
        }*/
	if (!in_cache(pf_addr) && pf_addr!=0){// && pf_addr<MAX_PHYS_MEM_ADDR){
	    issue_prefetch(pf_addr);
	}
    }
}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
