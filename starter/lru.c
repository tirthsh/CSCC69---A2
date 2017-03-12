#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

/**
Idea: Using linked lists (LL)

You want to use linked lists to keep track of pages. Whenever a new page is added which is not
in the linked list at the moment, add the new page to the end of the LL (ie. make it the tail).

Example - Case 1: When a page is added and its not in the LL.
				  Suppose initially the pages in the LL are page1 and page2.
				  Add Page#:3 (added it to the tail)

        ------------------------------         -----------------------------         --------------------------
        |              |             |      \ |                  |         |       \ |               |        |
        |Head: Page#:1 |    NEXT     |--------|      Page#:2     |  NEXT   |-------- |    Page#:3    |  NULL  |
        |              |             |      / |(not tail anymore)|         |       / |	 (new tail)  |        |
        ------------------------------        ------------------------------         --------------------------

Now another scenario is when a page is accessed in memory but the page already exists.
In this case, you want that page to be the tail (since it's the most frequently used) and 
make the next page the head (this will be the least frequently used). 

Example - Case 2: When a page is accessed and its already in the LL.
				  Suppose initially the pages in the LL are page1 and page2.
				  Accessing Page#:1 (make this the new tail)

        ------------------------------         -----------------------------         ----------------------------
        |                   |        |      \ |                  |         |       \ |                  |       |
        |New head: Page#:2  |  NEXT  |--------|    Page#:3       |   NEXt  |---------|New Tail: Page#:2 |  NULL |
        |                   |        |      / |(not tail anymore)|         |       / |	 (new tail)     |       |
        ------------------------------        ------------------------------         ---------------------------

Either way, we evict the head since the head will always be the Least Frequently used page. 

This algorithm is mentioned in class. Prof used a stack; instead we used a LL since its eaiser to manage pointers 
and move things around. 

**/


/* Need to create a linked list to keep track of pagges.
 * Used the following source to work with linked lists in C: http://www.learn-c.org/en/Linked_lists 
 */

typedef struct node{
	int frame_num;
	struct node *next_page;
}node;

node *head;
node *tail;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {

	//memory cannot be empty 
	//otherwise you cannot evict
	assert(head != NULL);

	//get the frame number of the head since you're going to evicting the head
	int get_frame = head->frame_num;

	//make the next node the new head
	node *temp = (node *)(head->next_page);
	//evict the current head
	free(head);
	head = temp;

	//evict the head
	return get_frame;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {

	//keeps track if page to evict is already in the list or not
	int found_victim = 0;

	int get_frame = p->frame >> PAGE_SHIFT;

	//allocate memory size for the new page
	node *new_page = (node*)malloc(sizeof(node));
	//assign the new frame its frame number
	new_page->frame_num = get_frame;
	new_page->next_page = NULL;

	//helps traverse through the linked list
	node *iterator = head;
	node *prev_page = NULL;

	//traverse through list to find if page already exists in the list
	while(iterator != NULL){

		//means the page already exists
		//make it the tail
		if(iterator->frame_num == get_frame){
			found_victim = 1;

			//the new  node is now the new tail
			tail->next_page = new_page;
			tail = new_page;

			//you only move it to head if there are more than 1 pages in the list
			if(prev_page != NULL){
				prev_page->next_page = iterator->next_page;			
			}
			else{ //otherwise, page matched is the head
				head = iterator->next_page;
			}
			//remove the page you matched
			free(iterator);

			//you want to exit the loop once you find the page 
			break;
		}

		//keep going to the next page in the list and update the prev page
		prev_page = iterator;
		/*if(iterator->next_page != NULL){
			iterator = iterator->next_page;
		}
		else{
			break;
		}*/

		iterator = iterator->next_page;
	}

	//not in the list
	//only do this if the page wasn't found in the list
	if(found_victim == 0){

		//if a page is not in the list, add it to the tail
		//this will be the new most recently used page
		if(tail == NULL){
			tail = new_page;
			head = new_page;
		}
		else{
			tail->next_page = new_page;
			//the new tail is the new page
			tail = new_page;
			//new_page->next_page = NULL;
		}
	}

	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {

	head = NULL;
	tail = NULL;

}



