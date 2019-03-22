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

const unsigned int MAX_TABLE_ROWS = 4096;
const unsigned int MAX_TABLE_COLUMNS = 8;

//Addr miss_table[MAX_LIST_SIZE][LIST_INPUTS];
//Addr miss_table_index[MAX_LIST_SIZE];

//--------------------------------------------
//--------------------------------------------
class Table {
    private:
    table_input  miss_table[MAX_TABLE_ROWS][MAX_TABLE_COLUMNS];
    table_input  index_list[MAX_TABLE_ROWS];
    int last_miss_index=-1;
    int entries = 0;

    public:

    void insert_miss (Addr miss)
    {
      int rows    =    MAX_TABLE_ROWS;
      int columns = MAX_TABLE_COLUMNS;
      bool found = false;
        if (last_miss_index!=-1) {
          for (int i=0; i < columns; i++) {
            if (miss_table[last_miss_index][i].addr == miss){
               found = true; //use this flag to determine that the addr was already in the matrix
               break;
            }
          }
          if (found == false){
            for(i = 0; i < (rows-1); i++){
              miss_table[last_miss_index][i].addr = miss_table[i+1].addr;
            }
            miss_table[rows-1].addr = miss;
          }

          found = false;
          if(entries < MAX_TABLE_ROWS){
            for (int i=0; i<rows; i++) {
              if (index_list[i].addr == miss) {
                last_miss_index = i;
                found = true;
                entries++;
                break;
              }
            }
            if (found == false) {
              index_list[entries].addr = miss;
            }
          }
        }
      }

    Addr get_next_miss (Addr miss) {
      int rows = MAX_TABLE_ROWS;
      int columns = MAX_TABLE_COLUMNS;
      for (int i=0; i<rows; i++) {
        if (index_list[i].addr == miss) {
          for (int j=0; j<columns; j++) {
            if(!in_cache(miss_table[j].addr)){
              issue_prefetch(miss_table[j].addr);
            }
          }
        }
        break;
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
      get_next_miss(stat.mem_addr);

    }

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
