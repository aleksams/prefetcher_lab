#include "interface.hh"
#include <cmath>
#include <math.h>

struct table_input {
  Addr addr;
  int count;
};

const unsigned int MAX_LIST_SIZE = pow(2,12);
class Table {
  private:
    table_input history_list[MAX_LIST_SIZE];
    table_input miss_list[MAX_LIST_SIZE/2];

  public:
    void insert_miss_ghb(Addr miss){

        int slots = MAX_LIST_SIZE;

        for(int i=0; i < (slots - 1); i++){
          history_list[i].addr = history_list[i+1].addr;
        }
        history_list[slots-1].addr = miss;
        //history_list[0].count++;
    }

    Addr get_next_miss_ghb(Addr miss)
    {
        bool found = false;
        int highest = 0;
        int slots = MAX_LIST_SIZE;
        int miss_size = MAX_LIST_SIZE/2;

        //if(history_list[0].count >= MAX_LIST_SIZE){
        for(int i=0; i < miss_size; i++){
          miss_list[i].addr = -1;
          miss_list[i].count = 0;
        }
        for(int i=0; i < (slots-1); i++){
          if(history_list[i].addr == miss){
            for(int j=0; j < miss_size; j++){
              if(miss_list[j].addr == history_list[i+1].addr){
                miss_list[j].count++;
                found = true;
                break;
              }
            }
            if(found == false){
                for(int k=0; k < miss_size; k++){
                  if (miss_list[k].count == 0){
                    miss_list[k].addr = history_list[i+1].addr;
                    miss_list[k].count++;
                    break;
                  }
                }
              }
            }
            found = false;
          }
        for(int i=1; i < miss_size; i++){
          if(miss_list[i].count >= miss_list[highest].count){
           	highest = i;
	         }
        }
        return miss_list[highest].addr;

      //else{
        //return -1;
      }
};


static Table * ghb;
//--------------------------------



void prefetch_init(void)

{
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */
    ghb = new Table;
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
      ghb->insert_miss_ghb(stat.mem_addr);
      Addr pf_addr = ghb->get_next_miss_ghb(stat.mem_addr);
	/*if (pf_addr==-1) {
            pf_addr = stat.mem_addr + BLOCK_SIZE;
        }*/
	if (!in_cache(pf_addr) && pf_addr != -1){// && pf_addr!=0){// && pf_addr<MAX_PHYS_MEM_ADDR){
	    issue_prefetch(pf_addr);
	}
  }
}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
