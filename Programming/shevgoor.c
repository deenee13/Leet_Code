/*
 * =====================================================================================
 *
 * Registration number: 5
 * Title: Efficiently Prefetching Complex Address Patterns
 * Authors: Manjunath Shevgoor (University of Utah) - shevgoor@cs.utah.edu
 *	 Sahil Koladiya (University of Utah) - koladiya@cs.utah.edu
 *	 Rajeev Balasubramonian (University of Utah) - rajeev@cs.utah.edu
 *	 Zeshan Chishti (Intel) - zeshan.a.chishti@intel.com
 * =====================================================================================
*/


#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../inc/prefetcher.h"

/* VLDP Prefetcher uses utilities called uthash and utlist */
/* They can be downloaded from https://github.com/troydhanson/uthash */

#include "./uthash/src/utlist.h"
#include "./uthash/src/uthash.h"



void l2_cache_fill_stat(int cpu_num, unsigned long long int addr, int set, int way, int prefetch, unsigned long long int evicted_addr)
{

}


// *************************************************************************************
// ***************************** BEGINNING OF SEQ **************************************
// **********************************************************************************************

#define MAXDISTANCE 6  // max distance pointer
#define INTERVAL 512
#define TQSIZE 128
#define PFQSIZE 128
#define IP_MASK_8b (0x0ff)
#define IP_MASK_16b (0x0ffff)
#define IP_MASK_32b (0x0ffffffff)

// prefetch usefulness
/* every 2048 accesses, 4 evaluation periods, if prefetching is on, turn-off prefetches on blocks with addr, addr%4 == 2. 
then at the end of the current period, check the L2 demand access miss rate on blocks that prefetching was turned off, and miss rate for all other blocks. 
If hit rate for other blocks is lower, turn off prefetching for the next region.*/


#define TESTPERIOD 4*INTERVAL
// testing related
int testblks = 0;
int testblks_hits = 0;
int otherblks = 0;
int otherblks_hits = 0;
int testing = 0;
int testperiod = TESTPERIOD;
int nexttest = TESTPERIOD;
int cntoff = 0;
int cnton = 0;
int DIFFLIMIT = INTERVAL*0.6;
// test queue for testing timeliness and relative success
typedef struct q {
  int tail;
  struct {
    unsigned long long int addr;
  }ent[TQSIZE];
}q_t;

// prefetch queue for prefetch filtering
typedef struct pq {
  int tail;
  struct {
    unsigned long long int addr;
  }ent[PFQSIZE];
}pq_t;

pq_t *pfq;

q_t *tq;
int tqhits = 0;
int avgtqhits = 0;
int tqmhits = 0;
unsigned long long int sumdiff = 0;
int l2acc = 0;
int l2miss = 0;
int high = 0;
int low = 0;
int pfcut = 0;
int pfcut2 = 0;
int low2 = 0;
int l2_mshr_limit = 8;
int distance = 1;
int pf_turn_on = 0;
int pf_turn_off = 0;


int dist[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
int dist_ptr = 1;  // start with offset 1
// ******************************

// ****************************************************
// ***************************** ENDING OF SEQ **************************************
// **********************************************************************************


// *********************************************************************************************
// ******************************* BEGINNING OF IP STRIDE **************************************
//**********************************************************************************************
#define IP_TRACKER_COUNT 1024
#define IP_PREFETCH_DEGREE 1
#define IP_DISTANCE 2

typedef struct ip_tracker
{
  // the IP we're tracking
  unsigned int ip;//32 bits

  // the last address accessed by this IP
  unsigned short last_addr;//16 bits
  // the stride between the last two addresses accessed by this IP
  short last_stride;//8 bits

  // use LRU to evict old IP trackers
  unsigned long long int lru_cycle;
} ip_tracker_t;

ip_tracker_t trackers[IP_TRACKER_COUNT];

// *********************************************************************************************
// ************************************* END OF IP STRIDE **************************************
//**********************************************************************************************


/*----------------------------------------------------------------------------------   
 *
 * VLDP PRFETCHER CODE
 *
 *---------------------------------------------------------------------------------- */

#define DRAM_QUEUE 100
#define PREFETCH_QUEUE 100
#define ROW_HIT_PER_PAGE 3

#define CACHE_FILL_PREFETCH_THRESHOLD 1

#define MAX_DEGREE 3
int MAX_DEGREE_PREFETCHED = 3;

#define DEGREE1_THRESHOLD 25
#define DEGREE2_THRESHOLD 50
#define DEGREE3_THRESHOLD 75


#define PRINT_CACHE_MISS_DETAILS 0
#define PRINT_ACCESSED_SERVICED_PREDICTED_TIME 0
typedef enum  {PREFETCH, CACHE_ACCESS, DEMAND_SERVICED, PREFETCH_SERVICED}access_type_t;

#if VLDP_DEBUG
#define VLDP_DEBUG_MSG(...) printf(__VA_ARGS__)
#else
#define VLDP_DEBUG_MSG(...)
#endif

#define PRINT_TABLE_INTERVAL 10000000

#define MIN_TAG_LENGTH 0
#define MIN_DEGREE 1
#define ACCURACY_FLAG 0
#define ACCURACY_THRESHOLD 2

//Shared Prediction Tables
#define PER_CORE_PREDICTION_TABLES 1

#define NUM_DELTAS_TRACKED_PER_PAGE 8
#define NUM_PAGES_TRACKED_PER_CORE 128
#define NUM_ENTRIES_PER_DELTA_PRED_TABLE 64

#define NUM_ENTRIES_IN_OFFSET_TABLE 64

#define ZERO_ORDER_REPL_POLICY FIFO
#define HIGHER_ORDER_REPL_POLICY FIFO

#define MAX_SAT_VALUE 4

#define NUM_PROCS 1
#define NUM_PAGE_BITS 12
#define VLDP_DEBUG 0

#define NUM_DELTA_PREDICTION_TABLES 4

#if PER_CORE_PREDICTION_TABLES==1
#define VLDP_TABLE delta_prediction_table[cpu_id]
#else
#define VLDP_TABLE delta_prediction_table
#endif

//virtual dram queue structure 
typedef struct dram_queue {
    unsigned long long int cache_line_addr;
	unsigned long long int cycle_added;
	int flag;
	struct dram_queue *next; 
} dram_queue;

dram_queue *head_dram = NULL;

typedef struct prefetch_queue {
	unsigned long long int base_cache_line_addr;
	unsigned long long int prefetch_cache_line_addr;
	unsigned long long int cycle_added;
	int degree;
	struct prefetch_queue *next;
} prefetch_queue ;

prefetch_queue *head_prefetch = NULL;

// Function Call
void insert_virtual_dram_queue(unsigned long long int , unsigned long long int , int );
int find_line_virtual_dram_queue(unsigned long long int);
int cnt_dram_pending_requests(unsigned long long int);

void insert_prefetch_queue(unsigned long long int, unsigned long long int, int);
int find_line_prefetch_queue(unsigned long long int );
void delete_line_prefetch_queue(unsigned long long int );



// Defination of the Functions form Karsli paper
void IP_STRIDE_ini();
void SEQT_ini(int low_bw);
void IP_STRIDE(unsigned long long int addr, unsigned long long int ip, int cache_hit);
void SEQT(unsigned long long int addr, unsigned long long int ip, int cache_hit);
void tq_ini();
int intq(unsigned long long int addr);
void pfq_ini();
int inpfq(unsigned long long int addr);






// Variable Defination
static unsigned long long int degree_accesses = 0; 
static unsigned long long int degree_total = 0; 

typedef unsigned long long int physical_address_t;
typedef unsigned long long int cycles_t;
typedef unsigned long long int uint64;
typedef unsigned int uint32;


//Tables can have different replacement policies
typedef enum {LRU, FIFO} replacement_policy_t;

// Initializing the paramters
void l2_prefetcher_initialize(int cpu_num)
{
	// Combining both the pefetcher
	IP_STRIDE_ini();
    SEQT_ini(knob_low_bandwidth);

	printf("#------------- vldp_prefetcher --------------------\n");
	// you can inspect these knob values from your code to see which configuration you're runnig in
	printf("Knobs visible from prefetcher: %d %d %d\n", knob_scramble_loads, knob_small_llc, knob_low_bandwidth);

	printf ("#------------- Printing Defines --------------------\n");
	printf ("%50s = %-6d\n","NUM_DELTAS_TRACKED_PER_PAGE", NUM_DELTAS_TRACKED_PER_PAGE);
	printf ("%50s = %-6d\n","NUM_DELTA_PREDICTION_TABLES", NUM_DELTA_PREDICTION_TABLES);
	printf ("%50s = %-6d\n","NUM_PAGES_TRACKED_PER_CORE", NUM_PAGES_TRACKED_PER_CORE);
	printf ("%50s = %-6d\n","NUM_ENTRIES_PER_DELTA_PRED_TABLE", NUM_ENTRIES_PER_DELTA_PRED_TABLE);
	printf ("%50s = %-6d\n","NUM_ENTRIES_IN_OFFSET_TABLE", NUM_ENTRIES_IN_OFFSET_TABLE);
	printf ("%50s = %-6d\n","NUM_PROCS", NUM_PROCS);
	printf ("%50s = %-6d\n","NUM_PAGE_BITS", NUM_PAGE_BITS);
	printf ("%50s = %-6d\n","MAX_DEGREE_PREFETCHED", MAX_DEGREE_PREFETCHED);
	printf ("%50s = %-6d\n","PRINT_CACHE_MISS_DETAILS", PRINT_CACHE_MISS_DETAILS);
	printf ("%50s = %-6d\n","PRINT_ACCESSED_SERVICED_PREDICTED_TIME", PRINT_ACCESSED_SERVICED_PREDICTED_TIME);
	printf ("%50s = %-6d\n","PRINT_TABLE_INTERVAL", PRINT_TABLE_INTERVAL);
	printf ("%50s = %-6d\n","MIN_TAG_LENGTH", MIN_TAG_LENGTH);
	printf ("%50s = %-6d\n","MIN_DEGREE", MIN_DEGREE);
	printf ("%50s = %-6d\n","ACCURACY_FLAG", ACCURACY_FLAG);
	printf ("%50s = %-6d\n","ACCURACY_THRESHOLD", ACCURACY_THRESHOLD);
	printf ("%50s = %-6d\n","PER_CORE_PREDICTION_TABLES", PER_CORE_PREDICTION_TABLES);
	printf ("%50s = %-6d\n","ZERO_ORDER_REPL_POLICY", ZERO_ORDER_REPL_POLICY);
	printf ("%50s = %-6d\n","HIGHER_ORDER_REPL_POLICY", HIGHER_ORDER_REPL_POLICY);
	printf ("%50s = %-6d\n","MAX_SAT_VALUE", MAX_SAT_VALUE);

	printf ("#------------- End of   Defines --------------------\n");
}



// *********************************************************************************************
// ******************************* BEGINNING OF KARSLI **************************************
//**********************************************************************************************


void tq_ini() {
int i;
  for (i = 0; i < TQSIZE; i++) {
    tq->ent[i].addr = 0;
  }
  tq->tail = -1;
}

int intq(unsigned long long int addr){

  int i;
  int found = 0;
  for(i=0;i<TQSIZE;i++){
    if(addr == tq->ent[i].addr){
      found = 1;
      break;
    }
  }
  return found;
}

void pfq_ini(){

  int i;
  for(i=0;i<PFQSIZE;i++){
    pfq->ent[i].addr = 0;
  }
  pfq->tail = -1;

}

int inpfq(unsigned long long int addr){
  int i;
  int found = 0;
  for(i=0;i<PFQSIZE;i++){
    if(addr == pfq->ent[i].addr){
      found = 1;
      break;
    }
  }
  return found;
}


void SEQT_ini(int low_bw){


  tq = (q_t *)malloc(sizeof(q_t));
  pfq = (pq_t *)malloc(sizeof(pq_t));
	if(low_bw) DIFFLIMIT = INTERVAL*0.5;
  tq_ini();
  pfq_ini();

}

void IP_STRIDE_ini() {
  int i;
  for(i=0; i<IP_TRACKER_COUNT; i++)
    {
      trackers[i].ip = 0;
      trackers[i].last_addr = 0;
      trackers[i].last_stride = 0;
      trackers[i].lru_cycle = 0;
    }
}


void IP_STRIDE(unsigned long long int addr, unsigned long long int ip, int cache_hit) {
  // check for a tracker hit
  int tracker_index = -1;
  unsigned long long int addr_blk = addr >> 6;

  int i;
  for(i=0; i<IP_TRACKER_COUNT; i++)
    {
      if(trackers[i].ip == (ip & IP_MASK_32b))
	{
	  trackers[i].lru_cycle = get_current_cycle(0);
	  tracker_index = i;
	  break;
	}
    }

  if(tracker_index == -1)
    {
      // this is a new IP that doesn't have a tracker yet, so allocate one
      int lru_index=0;
      unsigned long long int lru_cycle = trackers[lru_index].lru_cycle;
      int i;
      for(i=0; i<IP_TRACKER_COUNT; i++)
	{
	  if(trackers[i].lru_cycle < lru_cycle)
	    {
	      lru_index = i;
	      lru_cycle = trackers[lru_index].lru_cycle;
	    }
	}

      tracker_index = lru_index;

      // reset the old tracker
      trackers[tracker_index].ip = ip & IP_MASK_32b;
      trackers[tracker_index].last_addr = addr_blk & IP_MASK_16b;
      trackers[tracker_index].last_stride = 0;
      trackers[tracker_index].lru_cycle = get_current_cycle(0);

      return;
    }

  // calculate the stride between the current address and the last address
  // this bit appears overly complicated because we're calculating
  // differences between unsigned address variables
  short stride = 0;
  if((addr_blk & IP_MASK_16b) > trackers[tracker_index].last_addr)
    {
      stride = ((addr_blk & IP_MASK_16b) - trackers[tracker_index].last_addr) & IP_MASK_8b;
    }
  else
    {
      stride = (trackers[tracker_index].last_addr - (addr_blk & IP_MASK_16b)) & IP_MASK_8b;
      stride *= -1;
    }

  // don't do anything if we somehow saw the same address twice in a row
  if(stride == 0)
    {
      return;
    }

  // only do any prefetching if there's a pattern of seeing the same
  // stride more than once
  if(stride == trackers[tracker_index].last_stride)
    {
      // do some prefetching
      int i;
      int tempdist = distance;
      tempdist = tempdist / 2;
      if (tempdist == 0) tempdist = 1;
      for(i=tempdist; i<IP_PREFETCH_DEGREE+tempdist; i++)
      //for(i=IP_DISTANCE; i<IP_PREFETCH_DEGREE+IP_DISTANCE; i++)
	{
	  unsigned long long int pf_address = (addr_blk + (stride*(i+1))) << 6;

	  // only issue a prefetch if the prefetch address is in the same 4 KB page 
	  // as the current demand access address
	  if((pf_address>>12) != (addr>>12))
	    {
	      break;
	    }

	  // check the MSHR occupancy to decide if we're going to prefetch to the L2 or LLC
	  if (!inpfq((pf_address >> 6))) {
	  if(get_l2_mshr_occupancy(0) < l2_mshr_limit)
	    {
	      l2_prefetch_line(0, addr, pf_address, FILL_L2);
	    }
	  else
	    {
	      l2_prefetch_line(0, addr, pf_address, FILL_LLC);
	    }
	 	pfq->tail = (pfq->tail+1) % PFQSIZE;
		pfq->ent[pfq->tail].addr = (pf_address >> 6);
	  }
	  
	}
    }

  trackers[tracker_index].last_addr = addr_blk & IP_MASK_16b;
  trackers[tracker_index].last_stride = stride;
}

void SEQT(unsigned long long int addr, unsigned long long int ip, int cache_hit) { // Starting of SEQT

  static int suml2occ = 0;
  static int numl2occhigh = 0;
  
  suml2occ +=  get_l2_mshr_occupancy(0);
  
  if(get_l2_mshr_occupancy(0)>=15)
    numl2occhigh++;
  
  unsigned long long int cycle = get_current_cycle(0); 
  l2acc++;
  if(!cache_hit)
    l2miss++;
  
  //search for l2acc in tq
  int m;
  for(m=0; m<TQSIZE; m++){
    if(((addr&~0x3f) >> 6) == tq->ent[m].addr){
      tqhits++;

      if(!cache_hit)
	tqmhits++;
      
      break;
      
    }
  }
  
  
  if((l2acc%INTERVAL)==0){
    //decide on distance

    
    if(tqhits < 16){
      if(pfcut > 1){
	dist_ptr = 0;
	pfcut2 = 0;
      }
      else{
	if(dist_ptr > 1)
	  dist_ptr--;
      }

      if((l2miss-tqmhits)>DIFFLIMIT)
	pfcut2++;
      else
	pfcut2 = 0;

      high = 0;
      low = 0;
      low2 = 0;
      pfcut++;
    }
    else{
      pfcut = 0;

      if(l2miss < 10){
	pfcut2 = 0;
      }
      else if((l2miss-tqmhits)>DIFFLIMIT){

	low2 = 0;

	if(pfcut2 > 1){
	  dist_ptr = 0;
	  pfcut2 = 0;
	}
	else{
	  pfcut2++;

	  if(dist_ptr == 0){
	    if(pf_turn_on > 1){
	      pf_turn_on = 0;
	      dist_ptr = 1;
	    }
	    else
	      pf_turn_on++;
	  }

	}
	high = 0;
	low = 0;
      }
      else if((l2miss!=0) && (tqmhits !=0)){
	if((l2miss/tqmhits < 2)){
	  
	  if(low2>=2){
	    if(dist_ptr<MAXDISTANCE){
	      dist_ptr++;
	      low2 = 0;
	    }
	  }
	  else{
	    low2++;
	  }
	}
	else
	  low2 = 0;

	high = 0;
	low = 0;
	pfcut2 = 0;
	
	
	if(dist_ptr == 0){
	  if(pf_turn_on > 1){
	    pf_turn_on = 0;
	    dist_ptr = 1;
	  }
	  else
	    pf_turn_on++;
	}
      }
      else{
	pfcut2 = 0;
	high = 0;
	low = 0;
	low2 = 0;

      }
    }

    distance = dist[dist_ptr];

	//reset table
	tq_ini();

    avgtqhits = 0;
    tqmhits = 0;
    sumdiff = 0;
    tqhits = 0;
    l2miss = 0;


    suml2occ = 0;
    numl2occhigh = 0;

    //testing related
    if(testing == 1){

      double tblks_hitrate = (double)testblks_hits/(double)testblks;
      double oblks_hitrate = (double)otherblks_hits/(double)otherblks;

      int pf_off = 0;

      if(knob_low_bandwidth){
	//turn off prefetching if really not worth it
	pf_off = ((double)oblks_hitrate < (1.2*(double)tblks_hitrate));

	if(pf_off == 0)
	  pf_off = (otherblks_hits < 16);
      }
      else{
	// give advantage to prefetching
	pf_off = (((double)tblks_hitrate/(double)oblks_hitrate) > 1.2);

	if(pf_off == 0)
	  pf_off = (otherblks_hits < 8);

      }

      int failedtest = ((testblks < 32) || (otherblks < 32));	


      if(!failedtest){
	if(pf_off){

	  distance = 0;
	  if(testperiod > INTERVAL)
	    testperiod = testperiod>>1;
	  
	  //update turning off counter
	  if(cntoff < 3)
	    cntoff++;
	  
	  cnton = 0;
	  
	}
	else{
	  if(testperiod < (32*INTERVAL))
	    testperiod = testperiod*2;
	  
	  //update turning off counter
	  cntoff = 0;
	  
	  if(cnton < 3)
	    cnton++;

	}
      }
      else{
	//failed test, try again next interval
	testperiod = INTERVAL;
      }
      nexttest += testperiod;
      
    }
    
    testing = 0;
    testblks_hits = 0;
    testblks = 0;
    otherblks_hits = 0;
    otherblks = 0;

  }

  //for now, do this only for low bandwidth
  if(l2acc==nexttest && knob_low_bandwidth){
    if(distance!=0)
      testing = 1;
    else
      nexttest += INTERVAL;
  }
  
  int Istblk = 0;

  if(testing == 1){

    
    //if keeps turning off prefetcher, increase the number of tblks
    /**/
    if(cntoff>=2)
      Istblk = (addr>>6)%3 != 1;
    else
      Istblk = (addr>>6)%4 == 2;



    if(Istblk){
      testblks++;

      if(cache_hit)
	testblks_hits++;
    }
    else{
      otherblks++;

      if(cache_hit)
	otherblks_hits++;
    }
    
  }


  unsigned long long int pf_address;

  pf_address = ((addr>>6)+distance)<<6;
  int samepage = (pf_address>>12) == (addr>>12);  

  //if distance is 0 (nopref), put in the queue as if distance = 1
  if(distance == 0)
    pf_address = ((addr>>6)+1)<<6;

  if(testing == 1){
    //if keeps turning off prefetcher, increase the number of tblks
    /**/
    if(cntoff>=2)
      Istblk = (pf_address>>6)%3 != 1;
    else
      Istblk = (pf_address>>6)%4 == 2;

   }


  int nopf = ((testing ==1) && (Istblk));
  
  
  if(!nopf){

    if(samepage && !inpfq(pf_address >> 6)){
      
      if(distance != 0){
	//todo: make occupancy limit check dynamic
	
	if(get_l2_mshr_occupancy(0) < l2_mshr_limit)
	  {
	    l2_prefetch_line(0, addr, pf_address, FILL_L2);
	  }
	else
	  {
	    l2_prefetch_line(0, addr, pf_address, FILL_LLC);	      
	  }
        
	//add prefetched addr to the prefetch queue
	pfq->tail = (pfq->tail+1) % PFQSIZE;
	pfq->ent[pfq->tail].addr = (pf_address >> 6);
	
      }
      
    }
    
    
  }
  
  
    //add potential prefetch addr to the test queue
  if(samepage && !intq(pf_address>> 6)){
    tq->tail = (tq->tail+1) % TQSIZE;
    tq->ent[tq->tail].addr = (pf_address >> 6);
  }
  
  
  
} // end SEQT()

// *********************************************************************************************
// ************************************* ENDING OF KARSLI **************************************
//**********************************************************************************************



int wrap_around (int a , int b)
{
	int ret;
	if (a>=b)
	{
		ret= a-b;
		assert (ret < b);
	}
	else if (a < 0)
	{
		ret = a+b;
		assert (ret > 0);
	}
	else if (a < b)
	{
		ret =a;
	}

	return ret;
}

int inc_sat(int a) 
{
	a++; 

	if (a> MAX_SAT_VALUE)
	{
		a=MAX_SAT_VALUE;
	}

	return a;
}
int dec_sat(int a) 
{
	a--; 

	if (a<0)
	{
		a=0;
	}

	return a;
}

int predicted_deltas[4];

typedef enum {PRED, UPDATE} pred_or_update_t;

//delta history table
typedef struct delta_history_buffer
{
	physical_address_t page_number;

	physical_address_t first_access_offset;

	long long int last_accessed_cline;

	int last_index_tracked;

	int deltas[NUM_DELTAS_TRACKED_PER_PAGE];

	int num_times_page_touched;	

	//store the tag length of the last predictor
	int provider;

	//cycle touched
	cycles_t last_touched_cycle;

} delta_buffer_t;

delta_buffer_t delta_buffer[NUM_PROCS][NUM_PAGES_TRACKED_PER_CORE];
int last_page_replaced_for_core[NUM_PROCS];


//Offset Pred Table
typedef struct offset_prediction_table
{
	physical_address_t offset;
	physical_address_t predicted_delta;

	int cpu_id;

	int accuracy;
	int replacement_accuracy;
	int num_used;

	int valid;
} offset_prediction_table_t;
offset_prediction_table_t offset_prediction_table [NUM_PROCS][NUM_ENTRIES_IN_OFFSET_TABLE];

//delta prediction Table
typedef struct delta_preditction_table
{
	//This will depend on the number of history elements that are considered
	//We can accomodate upto 8 stide histories
	int  tag[NUM_DELTA_PREDICTION_TABLES];

	int predicted_delta;

	int accuracy;

	int num_used;
	cycles_t last_used;

} delta_prediction_table_t;


#if PER_CORE_PREDICTION_TABLES==1
	delta_prediction_table_t delta_prediction_table[NUM_PROCS][NUM_DELTA_PREDICTION_TABLES][NUM_ENTRIES_PER_DELTA_PRED_TABLE];
#else
	delta_prediction_table_t delta_prediction_table[NUM_DELTA_PREDICTION_TABLES][NUM_ENTRIES_PER_DELTA_PRED_TABLE];
#endif


void initialize_vldp_vars()
{
#if PER_CORE_PREDICTION_TABLES==1
	memset(delta_prediction_table, 0, sizeof(delta_prediction_table_t)*NUM_PROCS*NUM_DELTA_PREDICTION_TABLES*NUM_ENTRIES_PER_DELTA_PRED_TABLE);
#else
	memset(delta_prediction_table, 0, sizeof(delta_prediction_table_t)*NUM_DELTA_PREDICTION_TABLES*NUM_ENTRIES_PER_DELTA_PRED_TABLE);
#endif

	memset(delta_buffer, 0, sizeof(delta_buffer_t)*NUM_PROCS*NUM_PAGES_TRACKED_PER_CORE);
	memset(last_page_replaced_for_core, 0, sizeof(int)*NUM_PROCS);

}

//Offset prediction functions
void print_snapshot_offset_prediction_table ()
{
	printf ("Offset Prediction Table\n");
	printf ("%s %s %s %s %s\n","Core", "Offset", "Stride", "Accuracy", "NumUsed");
	printf ("-------------------------------------------------------\n");
	for (int p=0; p < NUM_PROCS; p++)
	{
		for (int i=0; i < NUM_ENTRIES_IN_OFFSET_TABLE; i++)
		{
			printf ("%4d"  , offset_prediction_table[p][i].cpu_id);
			printf (" %6lld", offset_prediction_table[p][i].offset);
			printf (" %6lld", offset_prediction_table[p][i].predicted_delta);
			printf (" %8d" , offset_prediction_table[p][i].accuracy);
			printf (" %7d", offset_prediction_table[p][i].num_used);
			printf ("\n"  );
		}
	}
	printf ("\n"  );
}

physical_address_t get_offset_prediction (int cpu_id,physical_address_t first_access_offset )
{
	for (int i=0; i<NUM_ENTRIES_IN_OFFSET_TABLE; i++)
	{
		if ((offset_prediction_table[cpu_id][i].offset == first_access_offset)  )
		{
			assert ((!offset_prediction_table[cpu_id][i].valid) || (offset_prediction_table[cpu_id][i].cpu_id == cpu_id));

			offset_prediction_table[cpu_id][i].num_used++;
			return offset_prediction_table[cpu_id][i].predicted_delta;
		}
	}

	//If it's a miss, then delta 0 means nothing
	return 0;
}

// This function updates the offset prediction table
void	update_offset_prediction_table(int cpu_id,int delta_buffer_entry)
{
	physical_address_t first_access_offset ;
	int found=0;
	physical_address_t second_add_delta;


	first_access_offset = delta_buffer[cpu_id][delta_buffer_entry].first_access_offset ;
	second_add_delta   = delta_buffer[cpu_id][delta_buffer_entry].deltas[1];

	assert (second_add_delta != 0);

	for (int i=0; (i<NUM_ENTRIES_IN_OFFSET_TABLE) && (!found); i++)
	{
		if ((offset_prediction_table[cpu_id][i].offset == first_access_offset) )
		{
			assert ((!offset_prediction_table[cpu_id][i].valid) || (offset_prediction_table[cpu_id][i].cpu_id == cpu_id));

			found =1;

			if (offset_prediction_table[cpu_id][i].predicted_delta == second_add_delta)
			{
				offset_prediction_table[cpu_id][i].accuracy=inc_sat(offset_prediction_table[cpu_id][i].accuracy);
				offset_prediction_table[cpu_id][i].replacement_accuracy=inc_sat(offset_prediction_table[cpu_id][i].replacement_accuracy);
			}
			else
			{
				offset_prediction_table[cpu_id][i].accuracy=dec_sat(offset_prediction_table[cpu_id][i].accuracy);
				offset_prediction_table[cpu_id][i].replacement_accuracy=dec_sat(offset_prediction_table[cpu_id][i].replacement_accuracy);
				if (offset_prediction_table[cpu_id][i].accuracy ==0)
				{
					offset_prediction_table[cpu_id][i].predicted_delta = second_add_delta;
					offset_prediction_table[cpu_id][i].replacement_accuracy =0;
				}
			}
		}
	}

	if (!found)
	{
		int candidate_found=0;
		for (int i=0; (i<NUM_ENTRIES_IN_OFFSET_TABLE) && (!candidate_found); i++)
		{
			if (offset_prediction_table[cpu_id][i].replacement_accuracy ==0)
			{
				candidate_found=1;

				offset_prediction_table[cpu_id][i].offset = first_access_offset  ;
				offset_prediction_table[cpu_id][i].predicted_delta = second_add_delta ;
				offset_prediction_table[cpu_id][i].cpu_id = cpu_id;
				offset_prediction_table[cpu_id][i].replacement_accuracy = 0;
				offset_prediction_table[cpu_id][i].accuracy = 0;
				offset_prediction_table[cpu_id][i].num_used = 0;

				offset_prediction_table[cpu_id][i].valid = 1;

				break;
			}
		}

		if (!candidate_found)
		{
			for (int i=0; (i<NUM_ENTRIES_IN_OFFSET_TABLE) && (!found); i++)
			{
				offset_prediction_table[cpu_id][i].replacement_accuracy=dec_sat(offset_prediction_table[cpu_id][i].replacement_accuracy);
			}			
		}
	}
}

//end- Offset prediction functions

// Function to print the values
void print_tag(int * tag)
{
	int delta;
	printf ("Tag:[");
	for (int i= (NUM_DELTA_PREDICTION_TABLES -1); i >= 0; i--)
	{
		delta = *(tag+ i);
		if (delta == 999)
		{
			printf ("    *");
		}else
		{
			printf (" %4d", delta);
		}
	}
	printf ("] ");
}




//The tag is an array. This function loops through the array to compare them
int is_tag_equal (int *a, int *b, int tag_len)
{
	for (int i=0; i < tag_len; i++)
	{
		if (a[i] != b[i])
		{
			return 0;
		}
	}

	return 1;
}

void assign_tag (int *a, int *b)
{
	for (int i=0; i < NUM_DELTA_PREDICTION_TABLES; i++)
	{
		a[i] = b[i];
	}
}

// Function to print the values obtained from the
void print_snapshot_of_VLDP_prediction_tables(int clear_tables, int clear_stats)
{
	//Print the delta history table
	printf("Stride_History_Per_core_Per_Page:\n");
	printf("%4s|%14s|%19s|%9s|%5s|%s\n", "Proc", "Page",      "Last_Accessed_CLine", "Num_Touch", "Index", "Strides");
	for (int p=0; p < NUM_PROCS; p++)
	{
		for (int page=0; page< NUM_PAGES_TRACKED_PER_CORE; page++)
		{
			printf ("%4d|", p);
			printf ("%10lld|", delta_buffer[p][page].page_number);
			printf ("%19lld|", delta_buffer[p][page].last_accessed_cline);
			printf ("%9d|",  delta_buffer[p][page].num_times_page_touched);
			printf ("%5d|",  delta_buffer[p][page].last_index_tracked);

			for (int s=0; s< NUM_DELTAS_TRACKED_PER_PAGE; s++)
				printf ("%4d ", delta_buffer[p][page].deltas[s]);

			printf("\n");
		}
	}

	//print the prefetch prediction tables

#if PER_CORE_PREDICTION_TABLES==1
	int p=NUM_PROCS;
#else
	int p=1;
#endif

	printf ("\n\nPrefetch_Prediction_Tables:\n");
	for (int cpu_id=0; cpu_id< p; cpu_id++)
	{
		for (int table=0; table< NUM_DELTA_PREDICTION_TABLES; table++)
		{
			printf ("Table %d\n", table);
			for (int entry=0; entry < NUM_ENTRIES_PER_DELTA_PRED_TABLE; entry++)
			{
				print_tag(&VLDP_TABLE[table][entry].tag[0]);
				printf ("N:%5d A:%d P:%-3d\n",VLDP_TABLE[table][entry].num_used,   VLDP_TABLE[table][entry].accuracy, VLDP_TABLE[table][entry].predicted_delta);
			}
		}
	}

	printf ("\n");
}


void reset_delta_history_buffer (int cpu_id, int index)
{
	memset(&delta_buffer[cpu_id][index],0, sizeof(delta_buffer_t));
}

int get_delta_buffer_entry(int cpu_id, physical_address_t page_number)
{
	int i;
	for (i=0; i < NUM_PAGES_TRACKED_PER_CORE; i++)
	{
		if (delta_buffer[cpu_id][i].page_number == page_number)
		{
			return i;
		}
	}

	assert (0);
	return -1;
}

//Get the last entry in the current page,cpu_id delta buffer. This will be the last prefetch prediction
int get_last_correct_delta(uint32 cpu_id, uint64 page_number)
{
	int delta_buffer_entry;

	delta_buffer_entry = get_delta_buffer_entry (cpu_id, page_number);

	//Update will have run and inserted an entry into the history_buffer
	assert (delta_buffer_entry>=0);

	//The last index tracked has been incremented after adding the last delta
	//For prediction, we need the latest deltas, for the update, we need the deltas before the latest
	int index = delta_buffer[cpu_id][delta_buffer_entry].last_index_tracked -1 ;

	int last_correct_delta;
	last_correct_delta  =  delta_buffer[cpu_id][delta_buffer_entry].deltas[index];

	VLDP_DEBUG_MSG ("last_correct_delta:%d\n", last_correct_delta);

	return last_correct_delta;
}

//Go into the delta history buffer and create the tag from the 
int * make_tag (uint32 cpu_id, uint64 page_number, uint32 tag_len, pred_or_update_t pred_or_update, int *tag, char *called_from)
{
	assert (tag_len<=NUM_DELTA_PREDICTION_TABLES);

	VLDP_DEBUG_MSG ("#------\nCalled From %s\n",called_from );

	if (tag_len==0)
	{
		return 0;
	}

	int first_index;
	int index;
	int delta_buffer_entry;

	for (int i=0; i<NUM_DELTA_PREDICTION_TABLES; i++ )
	{
		tag[i]=999;
	}

	delta_buffer_entry = get_delta_buffer_entry (cpu_id, page_number);

	//Update will have run and inserted an entry into the history_buffer
	assert (delta_buffer_entry>=0);

	//The last index tracked has been incremented after adding the last delta
	//For prediction, we need the latest deltas, for the update, we need the deltas before the latest
	first_index = wrap_around((delta_buffer[cpu_id][delta_buffer_entry].last_index_tracked -1 -pred_or_update), NUM_DELTAS_TRACKED_PER_PAGE);

	int test = (delta_buffer[cpu_id][delta_buffer_entry].last_index_tracked -1 -pred_or_update);
	int val;
	if (test< 0)
	{
		val= test+NUM_DELTAS_TRACKED_PER_PAGE;
	}
	else if (test>= NUM_DELTAS_TRACKED_PER_PAGE)
	{
		val= test-NUM_DELTAS_TRACKED_PER_PAGE;
	}
	else if (test <NUM_DELTAS_TRACKED_PER_PAGE)
	{
		val=test;
	}

	VLDP_DEBUG_MSG ("last_index_tracked=%d pred_or_up=%d test=%d val=%d first_index=%d\n",delta_buffer[cpu_id][delta_buffer_entry].last_index_tracked, pred_or_update, test, val, first_index);
	assert (val== first_index);



	for (int i=(tag_len-1); i >= 0; i--)
	{
		index= (first_index - i); 

		if (index< 0)
		{
			index= index+NUM_DELTAS_TRACKED_PER_PAGE;
		}

		assert (index>=0);


		//This is the tag creation, till here, we were just getting the deltas.

		tag[i] = delta_buffer[cpu_id][delta_buffer_entry].deltas[index];

		assert((127 + delta_buffer[cpu_id][delta_buffer_entry].deltas[index]) <= 256);
	}


	return &tag[0];
}

int update_delta_history_buffer(int cpu_id, physical_address_t page_number, long long int cline_address, int is_hit, int cache_hit)
{
	int found=0;
	long long int delta;
	int i;

	cycles_t oldest_cycle=get_current_cycle(cpu_id);
	int oldest_page_index=999;

	for (i=0; (i < NUM_PAGES_TRACKED_PER_CORE) && (!found); i++)
	{
		if (delta_buffer[cpu_id][i].page_number == page_number)
		{
			delta_buffer[cpu_id][i].last_touched_cycle = get_current_cycle(cpu_id);

			found=1;

			delta_buffer[cpu_id][i].num_times_page_touched++;

			delta=cline_address - delta_buffer[cpu_id][i].last_accessed_cline;

			if (delta ==0)
			{
				delta_buffer[cpu_id][i].num_times_page_touched--;
				return 0;
			}

			//The first time a page is accessed, the delta will be the cache line number, likely greater than 128
			if (delta > 64)
			{
				delta_buffer[cpu_id][i].deltas[delta_buffer[cpu_id][i].last_index_tracked] =210; 
			}
			else
			{
				delta_buffer[cpu_id][i].deltas[delta_buffer[cpu_id][i].last_index_tracked] =delta; 
			}

			delta_buffer[cpu_id][i].last_accessed_cline = cline_address;

			delta_buffer[cpu_id][i].last_index_tracked++;
			delta_buffer[cpu_id][i].last_index_tracked%= NUM_DELTAS_TRACKED_PER_PAGE;

			if (delta_buffer[cpu_id][i].num_times_page_touched <NUM_DELTAS_TRACKED_PER_PAGE)
			{
				assert (delta_buffer[cpu_id][i].num_times_page_touched == delta_buffer[cpu_id][i].num_times_page_touched);
			}


			VLDP_DEBUG_MSG ("\n%lld| Found! raw_delta=%lld delta_buffer[%d][%d].deltas[%d]=%d last_index_tracked=%d num_touch=%d\n",get_current_cycle(cpu_id),  delta, cpu_id, i, delta_buffer[cpu_id][i].last_index_tracked -1, delta_buffer[cpu_id][i].deltas[delta_buffer[cpu_id][i].last_index_tracked -1],  delta_buffer[cpu_id][i].last_index_tracked, delta_buffer[cpu_id][i].num_times_page_touched );

		}
	}


	if (!found)
	{
		for (int j=0; (j < NUM_PAGES_TRACKED_PER_CORE) ; j++)
		{
			if (delta_buffer[cpu_id][j].last_touched_cycle <= oldest_cycle)
			{
				oldest_cycle = delta_buffer[cpu_id][j].last_touched_cycle;
				oldest_page_index = j;
			}
		}
		assert(oldest_page_index != 999);

		i=last_page_replaced_for_core[cpu_id];

		reset_delta_history_buffer(cpu_id, i);

		delta_buffer[cpu_id][i].page_number = page_number;

		delta_buffer[cpu_id][i].last_touched_cycle = get_current_cycle(cpu_id);


		physical_address_t cline_b= cline_address;

		cline_b >>= 6;
		cline_b <<= 6;

		delta_buffer[cpu_id][i].first_access_offset = cline_address^cline_b;

		delta_buffer[cpu_id][i].num_times_page_touched++;

		delta=cline_address - delta_buffer[cpu_id][i].last_accessed_cline;

		assert (delta !=0);

		//The first time a page is accessed, the delta will be the cache line number, likely greater than 128

		delta_buffer[cpu_id][i].last_index_tracked=0;
		//The first time a page is accessed, the delta will be the cache line number, likely greater than 128
		if (delta> 64)
		{
			delta_buffer[cpu_id][i].deltas[delta_buffer[cpu_id][i].last_index_tracked] =220; 
		}
		else
		{
			delta_buffer[cpu_id][i].deltas[delta_buffer[cpu_id][i].last_index_tracked] =delta; 
		}

		VLDP_DEBUG_MSG ("delta=%d\n",delta_buffer[cpu_id][i].deltas[delta_buffer[cpu_id][i].last_index_tracked]);

		delta_buffer[cpu_id][i].last_accessed_cline = cline_address;

		delta_buffer[cpu_id][i].last_index_tracked++;

		delta_buffer[cpu_id][i].last_index_tracked = wrap_around (delta_buffer[cpu_id][i].last_index_tracked,NUM_DELTAS_TRACKED_PER_PAGE );

		last_page_replaced_for_core[cpu_id]++;
		last_page_replaced_for_core[cpu_id] = wrap_around (last_page_replaced_for_core[cpu_id],NUM_PAGES_TRACKED_PER_CORE );

	}

	return delta;
}



/*
	 This is the improtant function that acutally does the prefetch.
	 The assumption is that the data structures have already been updated with the latest cache line access.
	 Detailed Description:
	 1) Starting with the table tracking the longest history compare tags 
	 2) If there is match and the accuracy metrics meet some goal, then use it
	 3) Else go to the smaller history
	 4) Insert latest delta into the first order table in FIFO fashion


 */
int get_prefetch_delta_prediction(int cpu_id, physical_address_t page_number, long long int cline_address, pred_or_update_t pred_or_update, int current_degree)
{
	int *tag =NULL;
	int tag_array[NUM_DELTA_PREDICTION_TABLES];
	int mod_tag_array[NUM_DELTA_PREDICTION_TABLES];
	int found=0;
	int predicted_delta=0;
	int delta_buffer_entry;
	delta_buffer_entry = get_delta_buffer_entry (cpu_id, page_number);
	int num_touched = delta_buffer[cpu_id][delta_buffer_entry].num_times_page_touched;

	int tag_length;

	if (num_touched >NUM_DELTA_PREDICTION_TABLES)
	{
		tag_length=NUM_DELTA_PREDICTION_TABLES;
	}
	else
	{
		tag_length=num_touched-1;
	}

	while ((tag_length>0) && !found)
	{

		VLDP_DEBUG_MSG("Looking for predictor match in table:%d ", tag_length );
		tag= make_tag(cpu_id, page_number, tag_length, pred_or_update, &tag_array[0], "get_prefetch_delta_prediction1");

		for (int j=0; j<tag_length; j++)
		{
			if(tag[j]>200) 
			{
				printf ("\n\nERROR: ");print_tag(tag); printf ("\n");
			}
			assert(tag[j] < 200);
		}


		//Step 1: Make Copy of tag
		for (int i=NUM_DELTA_PREDICTION_TABLES-1; i>=0 ; i--)
		{
			mod_tag_array[i] = tag_array[i];
		}


		//Step 2: Left Shift the actual tag by (current_degree -1)
		for (int i=(NUM_DELTA_PREDICTION_TABLES-1); i >=(current_degree -1); i-- )
		{
			assert ((i-(current_degree -1)) >= 0);
			mod_tag_array[i] = mod_tag_array [i-(current_degree -1)];
		}

		//Step 3: Replace the right most deltas with deltas from predicted_deltas
		for (int i= (current_degree -2); i >=0; i--)
		{
			assert (i >=0);
			assert (((current_degree -2) - i) >=0);
			mod_tag_array[i] = predicted_deltas[(current_degree -2) - i];
		}

		//Step 4: Mask out all deltas greater than tag_length
		for (int i= (NUM_DELTA_PREDICTION_TABLES-1); i >= tag_length; i--)
		{
			mod_tag_array[i] = 999;
		}

		//Step 5: Replace the original tag
		tag = mod_tag_array;

		// Tag is ready ---------------------------- 


		for(int i=0; i < NUM_ENTRIES_PER_DELTA_PRED_TABLE; i ++)
		{
			if(ACCURACY_FLAG == 1)
			{
				if (is_tag_equal (&VLDP_TABLE[tag_length -1 ][i].tag[0], tag, tag_length))
				{	
					// for degree more than MIN_DEGREE, we have to look at accuracy as well. 

					if  ((VLDP_TABLE[tag_length -1 ][i].accuracy >= ACCURACY_THRESHOLD) ||( tag_length>=1)) {

						predicted_delta = VLDP_TABLE[tag_length -1 ][i].predicted_delta;
						VLDP_TABLE[tag_length -1 ][i].last_used = get_current_cycle(cpu_id);
						found=1;

						if (current_degree == 1)
						{
							VLDP_TABLE[tag_length -1 ][i].num_used++;

							//Update would have run before prefetch predtion. It will insert this cache access into history buffer.
							assert (delta_buffer_entry >=0);

							delta_buffer[cpu_id][delta_buffer_entry].provider = tag_length -1 ;					
						}

						VLDP_DEBUG_MSG(" -Found. Prediction=%d Provider=%d\n", predicted_delta, tag_length -1);

						break;
					}
					else
					{
						//We found the tag, but it does not have enough accuracy. But this still might be correct so if it is, accuracy needs to be increased
						if (current_degree == 1)
						{
							found=0;
							//Update would have run before prefetch predtion. It will insert this cache access into history buffer.
							assert (delta_buffer_entry >=0);
							delta_buffer[cpu_id][delta_buffer_entry].provider = tag_length -1 ;					
						}
						return 1000;
					}
				}

			}else{
				if (is_tag_equal (&VLDP_TABLE[tag_length -1 ][i].tag[0], tag, tag_length))
				{	
					//for degree more than MIN_DEGREE, we have to look at accuracy as well. 
					predicted_delta = VLDP_TABLE[tag_length -1 ][i].predicted_delta;
					found=1;

					if (current_degree == 1)
					{
						VLDP_TABLE[tag_length -1 ][i].num_used++;

						delta_buffer_entry = get_delta_buffer_entry (cpu_id, page_number);

						//Update would have run before prefetch predtion. It will insert this cache access into history buffer.
						assert (delta_buffer_entry >=0);

						delta_buffer[cpu_id][delta_buffer_entry].provider = tag_length -1 ;					
					}

					VLDP_DEBUG_MSG(" -Found. Prediction=%d Provider=%d\n", predicted_delta, tag_length -1);

					break;
				}			
			}
		}

		VLDP_DEBUG_MSG ("\n");

		tag_length--;

	}

	return predicted_delta;

}

/*
	 The zero order table is special compared to the rest. It is a table which tries to be a hit for every tag.
	 There is not way to do that because there are 256 different deltas possible.
 *	If push comes to shove, maybe we will make the 0 order table of 256 entries
 * LRU is another possiblility
 */
void update_zero_order_prediction_table(int cpu_id, physical_address_t page_number , long long int cline_address, int provider_found)
{

	VLDP_DEBUG_MSG ("\nupdate_zero_order_prediction_table: provider_found:%d\n", provider_found);

	replacement_policy_t replacement_policy;

	replacement_policy = ZERO_ORDER_REPL_POLICY;


	//First see if it exiest, if it does, then check if the prediction was correct
	//For a correct prediction, increment correctness counter
	//For a wrong prediction, decrement counter. Replace prediciton if counter is 0
	//For a miss, use the replacement policy
	int *tag;
	int tag_array[NUM_DELTA_PREDICTION_TABLES];

	tag = make_tag (cpu_id, page_number, 1, UPDATE, &tag_array[0], "update_zero_order_prediction_table");

	for (int i=0; i < NUM_ENTRIES_PER_DELTA_PRED_TABLE; i++)
	{
		if (is_tag_equal (&VLDP_TABLE[0][i].tag[0] , tag, 1))
		{
			VLDP_DEBUG_MSG ("Tag exixts in the 0 order buf index=%d\n", i);

			//Counters are only modified when the table is provider or provider +1
			//If order 0 is the provider, then the update has already been made.
			//The  only thing to do here is to add the tag if the tag is missing

			if (!provider_found)
			{
				int last_correct_delta;
				last_correct_delta = get_last_correct_delta(cpu_id, page_number);
				if (last_correct_delta == VLDP_TABLE[0][i].predicted_delta)
				{
					VLDP_TABLE[0][i].accuracy = inc_sat(VLDP_TABLE[0][i].accuracy)	;

					VLDP_DEBUG_MSG ("last_correct_delta:%d is the predicted delta. accuracy:%d\n", last_correct_delta, VLDP_TABLE[0][i].accuracy);
				}
				else
				{
					VLDP_TABLE[0][i].accuracy = dec_sat(VLDP_TABLE[0][i].accuracy)	;

					VLDP_DEBUG_MSG ("last_correct_delta:%d. Prediction %d is incorrect. accuracy:%d\n", last_correct_delta, VLDP_TABLE[0][i].predicted_delta, VLDP_TABLE[0][i].accuracy);
					if (VLDP_TABLE[0][i].accuracy ==0)
					{
						VLDP_TABLE[0][i].predicted_delta = last_correct_delta; 

						VLDP_DEBUG_MSG ("**changing prediction to:%d\n", last_correct_delta);
					}
				}
			}
			return;
		}
	}

	VLDP_DEBUG_MSG ("***tag not found\n");

	if (replacement_policy == FIFO)
	{
		static int last_evicted_index=0;

		VLDP_TABLE[0][last_evicted_index].accuracy=0;	
		VLDP_TABLE[0][last_evicted_index].predicted_delta=get_last_correct_delta(cpu_id, page_number);

		assign_tag(&VLDP_TABLE[0][last_evicted_index].tag[0] , tag);

		VLDP_DEBUG_MSG("FIFO evicting 0 order table:\n");
		VLDP_DEBUG_MSG("last_evicted_index=%d predicted_delta=%d ", last_evicted_index, VLDP_TABLE[0][last_evicted_index].predicted_delta);

		last_evicted_index++;
		//last_evicted_index%=NUM_ENTRIES_PER_DELTA_PRED_TABLE;
		last_evicted_index= wrap_around (last_evicted_index,NUM_ENTRIES_PER_DELTA_PRED_TABLE );
	}
	else
	{
		assert (0);
	}

	VLDP_DEBUG_MSG ("***Returning from update_zero_order\n");
}

/*
	 This function is called on every cache access. This fuction looks at the latest cache access and determines 
	 if the prediction made during the previous cache access from the core and page are correct. 
	 Following checks are performed:
	 1)update 0 order table in FIFO(?) order
	 2)If last access delta pattern is hit at this time, it can be correct or wrong
	 3)If miss, then nothing is done. The tags were inserted and then probably not promoted and then pushed out
	 4)If hit and prediction is correct, increment  correctness
	 5)If wrong, then check what is useless in the next higher order table. If anything is useless, then evict it. 
	 Decrement the correctness of the provider. If correctness is 0, then change its prediction value


Details:
1) See who the provider of the core,page is 
2) Update-
i)   See if the prediction still exists and is correct
ii)  If correct, increment correctness counter. 

3) Allocate-
i)   If wrong, check longer tag for useless entries
ii)  If useless entries exist, then make a new entry in the longer table
 */
void update_delta_prediction_tables(int cpu_id, physical_address_t page_number, long long int cline_address)
{

	VLDP_DEBUG_MSG ("\nupdate_delta_prediction_tables:\n");

	int delta_buffer_entry;
	int provider;
	int *tag;
	int tag_array[NUM_DELTA_PREDICTION_TABLES];
	int found_provider=0;
	int provider_predicted_correctly=0;

	delta_buffer_entry = get_delta_buffer_entry (cpu_id, page_number);

	//In case there are no providers, providers are defaulted to 0
	provider = delta_buffer[cpu_id][delta_buffer_entry].provider ;

	assert (provider >=0);
	assert (provider < NUM_DELTA_PREDICTION_TABLES);

	tag= make_tag(cpu_id, page_number, provider+1, UPDATE, &tag_array[0], "update_delta_prediction_tables1");

	if (VLDP_DEBUG)
	{
		printf ("\ndelta_buffer_entry=%d provides=%d \n", delta_buffer_entry, provider);
		print_tag(tag);
		printf("\n");
	}

	//Update the accuracy
	for (int i=0; (i <NUM_ENTRIES_PER_DELTA_PRED_TABLE) && (tag!=0); i++ )
	{
		if (is_tag_equal (&VLDP_TABLE[provider][i].tag[0] , tag, provider+1))
		{
			found_provider =1;

			int last_correct_delta;
			last_correct_delta = get_last_correct_delta(cpu_id, page_number);

			if (last_correct_delta == VLDP_TABLE[provider][i].predicted_delta)
			{
				VLDP_TABLE[provider][i].accuracy = inc_sat(VLDP_TABLE[provider][i].accuracy)	;
				provider_predicted_correctly=1;
			}
			else
			{
				VLDP_TABLE[provider][i].accuracy = dec_sat(VLDP_TABLE[provider][i].accuracy)	;

				if (VLDP_TABLE[provider][i].accuracy ==0)
				{
					VLDP_TABLE[provider][i].predicted_delta = last_correct_delta; 

					VLDP_DEBUG_MSG ("table=%d i=%d updating delta to:%d\n", provider, i, VLDP_TABLE[provider][i].predicted_delta) ;
				}
			}
		}
	}

	//Allocate: If the prediction by the provider is wrong, then there needs to be an allocation in the next longer table

	//An entry can be promoted to the second table only when the page has been touched 3 times..hence the last condition
	if ((found_provider || (provider==0)) && (!provider_predicted_correctly) && (provider< (NUM_DELTA_PREDICTION_TABLES -1) )&& (provider<= (delta_buffer[cpu_id][delta_buffer_entry].num_times_page_touched -4 ) ))
	{
		int allocate_in;

		allocate_in = provider+1;

		VLDP_DEBUG_MSG ("cpu_id=%d page_number=%lld tag_len=%d provider=%d num_touch=%d\n ",cpu_id, page_number, allocate_in+1, provider,delta_buffer[cpu_id][delta_buffer_entry].num_times_page_touched  );
		tag= make_tag(cpu_id, page_number, allocate_in+1, UPDATE, &tag_array[0], "update_delta_prediction_tables2");


		//First check if the tag exists, if it does, then it needs to be updated
		for (int i=0; i <NUM_ENTRIES_PER_DELTA_PRED_TABLE; i++ )
		{
			if ( is_tag_equal(&VLDP_TABLE[allocate_in][i].tag[0], tag, allocate_in+1))
			{
				VLDP_TABLE[allocate_in][i].accuracy=dec_sat(VLDP_TABLE[allocate_in][i].accuracy);
				break;
			}
		}

		//If tag does not exist, then replace according to replacement policy
		if (HIGHER_ORDER_REPL_POLICY == FIFO)
		{
			static int last_evicted_index=0;

			VLDP_TABLE[allocate_in][last_evicted_index].accuracy=0;	
			VLDP_TABLE[allocate_in][last_evicted_index].predicted_delta=get_last_correct_delta(cpu_id, page_number);

			assign_tag(&VLDP_TABLE[allocate_in][last_evicted_index].tag[0] , tag);

			VLDP_DEBUG_MSG("FIFO evicting %d order table:\n",allocate_in);
			VLDP_DEBUG_MSG("last_evicted_index=%d predicted_delta=%d ", last_evicted_index, VLDP_TABLE[allocate_in][last_evicted_index].predicted_delta);
			if (VLDP_DEBUG)
			{		
				print_tag(tag);
				printf ("\n");
			}

			last_evicted_index++;
			//last_evicted_index%=NUM_ENTRIES_PER_DELTA_PRED_TABLE;
			last_evicted_index= wrap_around (last_evicted_index,NUM_ENTRIES_PER_DELTA_PRED_TABLE );

		}
		else if (HIGHER_ORDER_REPL_POLICY == LRU)
		{
			cycles_t lru_cycle = get_current_cycle(cpu_id);
			int        lru_pos = NUM_ENTRIES_PER_DELTA_PRED_TABLE +1;

			for (int i=0; (i <NUM_ENTRIES_PER_DELTA_PRED_TABLE); i++ )
			{
				if (VLDP_TABLE[allocate_in][i].last_used < lru_cycle)
				{
					lru_pos=i;
				}
			}
			assert (lru_pos < NUM_ENTRIES_PER_DELTA_PRED_TABLE);

			assign_tag(&VLDP_TABLE[allocate_in][lru_pos].tag[0], tag);
			VLDP_TABLE[allocate_in][lru_pos].accuracy=0;
			VLDP_TABLE[allocate_in][lru_pos].predicted_delta= get_last_correct_delta(cpu_id, page_number);
			VLDP_TABLE[allocate_in][lru_pos].num_used=0;
			VLDP_TABLE[allocate_in][lru_pos].last_used=0;
		}
		else
		{
			assert (0);
		}
	}

	//-end allocate




	//This is update last because mispredicts made by 0 order will need to to propagate upwards.
	//if the provider is not found, then if the tag exists in 0 order, then the prediction might need to be updated
	//if the provider is found, and if it was 0 order, all the updates would have already happened.
	update_zero_order_prediction_table (cpu_id, page_number, cline_address, found_provider);

}


void schedule_prefetch_to_dram(){


	int l2_read_queue_level;
	int l2_mshr_queue_level;

	unsigned long long int page_prefetch_addr;
	unsigned long long int prefetch_addr;
	unsigned long long int prefetch_line;
	unsigned long long int base_addr;
	prefetch_queue *prefetch_el, *prefetch_temp;


	//delete old requests ... 
	LL_FOREACH_SAFE(head_prefetch, prefetch_el, prefetch_temp){

		if ( (get_current_cycle(0) - (prefetch_el -> cycle_added) ) > 2000){
			LL_DELETE(head_prefetch, prefetch_el);
			free(prefetch_el);
		}
	}

	int cnt = 0;
	//find the oldest thing with least degree from prefetch queue, and try to issue it depending on the follwoing two conditions. . . 

	LL_FOREACH_SAFE(head_prefetch, prefetch_el, prefetch_temp){

		page_prefetch_addr = (prefetch_el -> prefetch_cache_line_addr) >> 6;

		if( (cnt_dram_pending_requests(page_prefetch_addr) <= ROW_HIT_PER_PAGE)  ){

			//if request is too old don't issue it, delete it from prefetch queue ...
			if(  ( (prefetch_el -> degree)  >  MAX_DEGREE_PREFETCHED  )   ){
				LL_DELETE(head_prefetch, prefetch_el);
				free(prefetch_el);
			}else{
				cnt ++;	

				prefetch_addr = (prefetch_el -> prefetch_cache_line_addr) << 6;
				prefetch_line = (prefetch_el -> prefetch_cache_line_addr);
				base_addr = (prefetch_el -> base_cache_line_addr) << 6; 

				l2_read_queue_level = get_l2_read_queue_occupancy(0);
				l2_mshr_queue_level = get_l2_mshr_occupancy(0);

				//prefetch to l2
				if( (l2_read_queue_level < (0.75 *  L2_READ_QUEUE_SIZE))   &&   ( l2_mshr_queue_level < (0.75* L2_MSHR_COUNT) ))  {
					if(l2_prefetch_line(0, base_addr, prefetch_addr, FILL_L2) == 1){
						//insert to dram queue
						if(find_line_virtual_dram_queue(prefetch_line)    == 0){
							//l2 level ...
							insert_virtual_dram_queue(prefetch_line ,  get_current_cycle(0), 2);
						}	
					}
				}
				//prefetch to l3
				else{
					if(l2_prefetch_line(0, base_addr, prefetch_addr, FILL_LLC) == 1){
						//insert to dram queue
						if(find_line_virtual_dram_queue(prefetch_line)    == 0){
							//l2 level ...
							insert_virtual_dram_queue(prefetch_line ,  get_current_cycle(0), 3);
						}	
					}
				}

				LL_DELETE(head_prefetch, prefetch_el);
				free(prefetch_el);

				if(cnt > CACHE_FILL_PREFETCH_THRESHOLD){
					break;
				}

			}
		}
	}
}


void l2_cache_fill(int cpu_num, unsigned long long int addr, int set, int way, int prefetch, unsigned long long int evicted_addr)
{
	//remove line from dram queue for l2 fill 	
	//delete while iterating ..

	unsigned long long int curr_cache_line = addr >> 6;	
	dram_queue *dram_el, *dram_temp; 
	LL_FOREACH_SAFE(head_dram, dram_el, dram_temp){
		if( (dram_el -> cache_line_addr) == curr_cache_line){
			LL_DELETE(head_dram, dram_el);
			free(dram_el);
		}
	}

	//as we don't have l3 fill, check for lines came from l3, and delete them if they are more than 200 cycles old. 
	LL_FOREACH_SAFE(head_dram, dram_el, dram_temp){
		if( (dram_el -> flag == 3) &&  ( (get_current_cycle(0) - (dram_el -> cycle_added)) > 500 )  ){
			LL_DELETE(head_dram, dram_el);
			free(dram_el);
		}
	}

	//try to prefetch here ... 
	schedule_prefetch_to_dram();

	// uncomment this line to see the information available to you when there is a cache fill event
	l2_cache_fill_stat( cpu_num, addr,  set,  way,  prefetch,  evicted_addr);
}

void l2_prefetcher_heartbeat_stats(int cpu_num)
{
	static int cnt = 0; 

	cnt ++;

	if(cnt % 10 == 0){

		printf("\n HeartBeat: ");

		print_snapshot_offset_prediction_table ();
		print_snapshot_of_VLDP_prediction_tables(0,0);
		printf("\n");
	}
}

void l2_prefetcher_warmup_stats(int cpu_num)
{
	printf("Prefetcher warmup complete stats\n\n");
}

void l2_prefetcher_final_stats(int cpu_num)
{
	printf("avg degree = %f\n",  (float)degree_total/degree_accesses );
	printf("\nComplete: ");


	printf("\n");
	printf("\n");	
}

//dram queue ...
void insert_virtual_dram_queue(unsigned long long int cache_line_addr, unsigned long long int cycle_added, int flag){

	//only 100 elements can be in queue ... 
	dram_queue *el;
	int count; 
	LL_COUNT(head_dram, el, count);

	if(count >= DRAM_QUEUE){
		return;
	}

	if(MAX_DEGREE == 2){

		if( (count >= 0) && (count < DEGREE1_THRESHOLD) ){
			MAX_DEGREE_PREFETCHED = 2;

		}else if( (count >=  DEGREE1_THRESHOLD) ){
			MAX_DEGREE_PREFETCHED = 1;
		}
	}

	if(MAX_DEGREE == 3){

		if( (count >= 0) && (count < DEGREE1_THRESHOLD) ){
			MAX_DEGREE_PREFETCHED = 3;

		}else if( (count >=  DEGREE1_THRESHOLD) && (count < DEGREE2_THRESHOLD) ){
			MAX_DEGREE_PREFETCHED = 2;

		}else if( (count >= DEGREE2_THRESHOLD) ){
			MAX_DEGREE_PREFETCHED = 1;

		}
	}

	if(MAX_DEGREE == 4){

		if( (count >= 0) && (count < DEGREE1_THRESHOLD) ){
			MAX_DEGREE_PREFETCHED = 4;

		}else if( (count >=  DEGREE1_THRESHOLD) && (count < DEGREE2_THRESHOLD) ){
			MAX_DEGREE_PREFETCHED = 3;

		}else if( (count >= DEGREE2_THRESHOLD) && (count < DEGREE3_THRESHOLD) ){
			MAX_DEGREE_PREFETCHED = 2;

		}else if( (count >= DEGREE3_THRESHOLD)  ){
			MAX_DEGREE_PREFETCHED = 1;
		}
	}


	assert(count < DRAM_QUEUE);

	dram_queue *element = (dram_queue *)malloc(sizeof(dram_queue));

	if(!element){
		printf("can't malloc at dram queue...");
		exit(-1);
	}

	element -> cache_line_addr = cache_line_addr;
	element -> cycle_added	 = cycle_added;
	element -> flag		 = flag;

	LL_APPEND(head_dram, element);
}

int find_line_virtual_dram_queue(unsigned long long int cache_line){

	dram_queue *el; 
	int flag = 0;

	LL_FOREACH(head_dram, el){
		//line found
		if(  (el -> cache_line_addr) == cache_line ){
			flag = 1;
			break;
		}
	}

	if(flag == 1){
		return 1;
	}else{
		return 0; 
	}
}

int cnt_dram_pending_requests(unsigned long long int page_addr){

	unsigned long long int page_address;

	dram_queue *el; 

	int cnt = 0;

	LL_FOREACH(head_dram, el){
		page_address = (el -> cache_line_addr) >> 6  ;
		if(page_address == page_addr){
			cnt++;
		}
	}

	return cnt; 
}

//prefetch queue ...
void insert_prefetch_queue(unsigned long long int base_cache_line_addr, unsigned long long int prefetch_cache_line_addr, int degree){

	//only 100 elements can be in queue ... 
	prefetch_queue *el;
	int count; 
	LL_COUNT(head_prefetch, el, count);

	if(count >= PREFETCH_QUEUE){
		return;
	}

	prefetch_queue *element = (prefetch_queue *)malloc(sizeof(prefetch_queue));

	if(!element){
		printf("can't malloc at prefetch queue...");
		exit(-1);
	}

	element -> prefetch_cache_line_addr = prefetch_cache_line_addr;
	element -> base_cache_line_addr = base_cache_line_addr;
	element -> cycle_added	 = get_current_cycle(0);
	element -> degree	 = degree;

	LL_APPEND(head_prefetch, element);
}

void update_degree_prefetch_queue(unsigned long long int cache_line_addr, int degree){

	prefetch_queue *el; 

	LL_FOREACH(head_prefetch, el){
		//line found
		if(  (el -> prefetch_cache_line_addr)  == cache_line_addr ){
			el -> degree = degree;
			break;
		}
	}	
}

int find_line_prefetch_queue(unsigned long long int cache_line_addr){

	prefetch_queue *el; 
	int flag = 0;

	LL_FOREACH(head_prefetch, el){
		//line found
		if(  (el -> prefetch_cache_line_addr) == cache_line_addr ){
			flag = 1;
			break;
		}
	}

	if(flag == 1){
		return 1;
	}else{
		return 0; 
	}
}

void delete_line_prefetch_queue(unsigned long long int cache_line_addr){

	prefetch_queue *el, *temp;
	LL_FOREACH_SAFE(head_prefetch, el, temp){
		if( (el -> prefetch_cache_line_addr) == cache_line_addr){
			LL_DELETE(head_prefetch, el);
			free(el);
		}
	}
}

void l2_prefetcher_operate(int cpu_num, unsigned long long int addr, unsigned long long int ip, int cache_hit)
{

	// Functions call of karsli 
	IP_STRIDE(addr, ip, cache_hit);
 	SEQT(addr, ip, cache_hit);  

	// Actual code starts from here
	int is_hit=cache_hit;
	int cpu_id = cpu_num;

	long long int cur_cline_address = addr>>6;
	physical_address_t page_number  = addr>>NUM_PAGE_BITS;

	//insert to dram queue ... demand miss

	if(!cache_hit){


		if(find_line_virtual_dram_queue(cur_cline_address) == 0){
			insert_virtual_dram_queue(cur_cline_address, get_current_cycle(0), 1);
		}

		if(find_line_prefetch_queue(cur_cline_address) == 1){
			delete_line_prefetch_queue(cur_cline_address);
		}
	}

	static int first_time=1;

	if (first_time)
	{
		first_time=0;
		initialize_vldp_vars();
	}

	int delta;
	delta = update_delta_history_buffer(cpu_id, page_number, cur_cline_address, is_hit, cache_hit);

	//There is a chance that a cache line access is a cache miss, but exists in one of or structures.
	//In this case, the delta will be 0
	//
	if (delta == 0)
	{
		return;
	}

	//If this is the first access then issue prediction from offset prediction table

	int delta_buffer_entry ;
	int num_access ;
	physical_address_t first_access_offset ;

	delta_buffer_entry = get_delta_buffer_entry (cpu_id, page_number);
	first_access_offset = delta_buffer[cpu_id][delta_buffer_entry].first_access_offset ;
	num_access = delta_buffer[cpu_id][delta_buffer_entry].num_times_page_touched ;

	assert (num_access>0);

	if (num_access == 1)
	{
		physical_address_t offest_delta_prediction;
		offest_delta_prediction = get_offset_prediction (cpu_id,first_access_offset );

		if (offest_delta_prediction !=0)
		{
			unsigned long long int page_addr = (((addr>>6)+offest_delta_prediction)<<6) >> 12;
			if( (cnt_dram_pending_requests(page_addr) <= ROW_HIT_PER_PAGE)   ){

				if(l2_prefetch_line(0, addr, ((addr>>6)+offest_delta_prediction)<<6, FILL_L2) == 1){
					if(find_line_virtual_dram_queue(((addr>>6)+offest_delta_prediction)) == 0){
						//l2 level ...
						insert_virtual_dram_queue(((addr>>6)+offest_delta_prediction), get_current_cycle(0), 2);
					}
				}else if(l2_prefetch_line(0, addr, ((addr>>6)+offest_delta_prediction)<<6, FILL_LLC) == 1){
					if(find_line_virtual_dram_queue(((addr>>6)+offest_delta_prediction)) == 0){
						//l3 level ...
						insert_virtual_dram_queue(((addr>>6)+offest_delta_prediction), get_current_cycle(0), 3);
					}
				}

			}else{
				//add to prefetch queue 

				//before inserting, check if it already exists ... 
				if(find_line_prefetch_queue(((addr>>6)+offest_delta_prediction)<<6) == 0){
					insert_prefetch_queue(addr >> 6, ((addr>>6)+offest_delta_prediction), 1);
				}else{
					//line is already in queue, update it's degree
					update_degree_prefetch_queue( ((addr>>6)+offest_delta_prediction), 1); 
				}
			}
		}

		return;
	}

	//If this is the second access then update the offset prediction table
	if (num_access == 2)
	{
		update_offset_prediction_table(cpu_id,delta_buffer_entry);
	}

	//Prediction tables need to be updated for both correct and incorrect prefetches.
	//However an update can only be done when there are 2 deltas. Here, we see if the second delta can be predicted by the first delta
	//So only when there are three accesses to a page must we update
	if (num_access >= 3)
	{	
		update_delta_prediction_tables(cpu_id, page_number, cur_cline_address);
	}

	int total_delta=0;
	memset (predicted_deltas, 0, sizeof(int)*(MAX_DEGREE_PREFETCHED));

	degree_accesses++;
	degree_total += MAX_DEGREE_PREFETCHED;

	for (int deg=1; deg<=MAX_DEGREE_PREFETCHED; deg++ )
	{
		predicted_deltas[deg-1] = get_prefetch_delta_prediction(cpu_id, page_number, cur_cline_address, PRED, deg);		

		//if returned values is 0, means we didn't get anything, so we need to break this loop. 
		if(predicted_deltas[deg - 1] == 1000){
			break;
		}

		assert(predicted_deltas[deg-1] >= -63 && predicted_deltas[deg-1] <= 64);	

		total_delta += predicted_deltas[deg-1];


		if ( total_delta !=0)
		{
			VLDP_DEBUG_MSG ("Prefetching delta %d\n", total_delta);

			//before we issue prefetch to l2, check the read queue and mshr queue status ...
			int l2_read_queue_level = get_l2_read_queue_occupancy(cpu_num);
			int l2_mshr_queue_level = get_l2_mshr_occupancy(cpu_num);	
			unsigned long long int prefetch_addr = ((addr>>6)+total_delta)<<6;
			unsigned long long int prefetch_line = ((addr>>6)+total_delta);

			//in this case, addr and prefetch_addr must be from different physical pages ...
			unsigned long long int page_prefetch_addr = prefetch_addr >> 12;

			//first check how many pending dram reqeusts are there belong to this page ... 
			if( (cnt_dram_pending_requests(page_prefetch_addr) <= ROW_HIT_PER_PAGE)   ){

				//we can issue prefetch ...
				// to l2
				if( (l2_read_queue_level < (0.75 *  L2_READ_QUEUE_SIZE))   &&   ( l2_mshr_queue_level < (0.75* L2_MSHR_COUNT) ))  {


					if(l2_prefetch_line(0, addr, ((addr>>6)+total_delta)<<6, FILL_L2) == 1){
						//insert to dram queue
						if(find_line_virtual_dram_queue(prefetch_line) == 0){
							//l2 level ...
							insert_virtual_dram_queue(prefetch_line ,  get_current_cycle(0), 2);
						}
							
					}
				}
				//to l3
				else{

					if(l2_prefetch_line(0, addr, ((addr>>6)+total_delta)<<6, FILL_LLC) == 1){
						//insert to dram queue
						if(find_line_virtual_dram_queue(prefetch_line)    == 0){
							//l2 level ...
							insert_virtual_dram_queue(prefetch_line ,  get_current_cycle(0), 3);
						}	
					}
				}					
			}else{
				//we can't issue prefetch, insert to prefetch queue ...	
				//before inserting, check if it already exists ... 
				if(find_line_prefetch_queue(prefetch_line) == 0){
					insert_prefetch_queue( addr >> 6, prefetch_line, deg);
				}else{				
					//line is already in queue, update it's degree
					update_degree_prefetch_queue(prefetch_line, deg); 
				}							
			}
		}
	}
}
//-- end VLDP