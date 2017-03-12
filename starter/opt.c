#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"

//max size for a single addr
#define MAXBUF 256

extern int memsize;

extern char *tracefile;

extern int debug;

extern struct frame *coremap;

int *trAddrs;
int *countList;
int *memFrames;
int addrCount;
int indexTrace;

/* Page to evict is chosen using the optimal (aka MIN) algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

 //size --> addrcount
 //vaddrcount -->indexTrace
int opt_evict() {
	int added = 0;
	int i=0;

	while(i < memsize){
		//loop through the memsize
		int curr = indexTrace;
		//loop through the rest of the addrs (starting from the indexTrace)
		while(curr < memsize){
			//if current address is within the frames in the memory
			if(trAddrs[curr] == memFrames[i]){
				//store the location of addr in countList
				countList[i] = curr;
				added = 1;
				break;
			}
			curr++;
		}
		i++;
	}

	//if address is not found in memory then we can evict any frame
	//return the first frame in the memory
	if(added == 1){
		return 0;
	}
	i = 0;
	int maxCount = 0;
	int victim = 0;
	while(i < memsize){
		if(countList[i] > maxCount){
			//if a bigger count is found then reset maxCount
			maxCount = countList[i];
			//reset your victim
			victim = i;
		}
		i++;
	}

	return victim;
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
	//get frame number of page table entry
	int frame;
	frame = p->frame >> PAGE_SHIFT;
	//add the frame to the memory
	memFrames[frame] = trAddrs[indexTrace];
	indexTrace++;
	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {

	//need to loop through the trace file and store each addr in a list
	//tracefile is from running sim.c
	FILE *tFile = fopen(tracefile, "r");
	char buffer[MAXBUF];
	char t;
	addr_t virtualAddr = 0;

	//open the file, return error if the file can't be opened
	if(tFile == NULL){
		perror("Error : Could not open tracefile");
		exit(1);
	}

	//need to figure out size for storing the tracefile addrs
	//before creating the list
	addrCount = 0;
	while(fgets(buffer, MAXBUF, tFile)!= NULL){
		addrCount++;
	}

	//create tr_addrs list
	trAddrs = malloc(sizeof(int) * addrCount);
	countList = malloc(sizeof(int) * memsize);
	memFrames = malloc(sizeof(int) * memsize);



	//set the bit reading back to the beggining of the file
	fseek(tFile, 0, SEEK_SET);
	int i = 0;
	//now add each addr in the tracefile to out trAddrs
	while(fgets(buffer, MAXBUF, tFile)!= NULL){
		if(buffer[0] != '='){
			sscanf(buffer, "%c %lX", &t, &virtualAddr);
			trAddrs[i] = virtualAddr;
			i++;
		}
	}
	indexTrace = 0;
}