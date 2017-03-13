#include "paging.h"

/* reference: http://wiki.osdev.org/Setting_Up_Paging */

/*
 * create_empty_page_directory
 * input: NONE
 * description: create a basic empty page directory
 * side effect: set pages as not present
 */

void create_empty_page_directory() {
	int i;
	for (i = 0; i < PAGE_DIRECTORY_NUM; ++i) {
		page_directory[i] = NOT_PRESENT_MASK; // set present bit to be 10 (meaning not present)
	}
}

/*
 * create_empty_page_table
 * input: NONE
 * description: create a basic empty page table
 * side effect: none
 */

void create_empty_page_table() {
	int i;
	for (i = 0; i < PAGE_TABLE_NUM; ++i) {
		page_table[i] = (i * 0x1000) | NOT_PRESENT_MASK; // leaving trailing 12 bits for starting addresses to be 0
	}
}

/*
 * init_first_page_directory
 * input: NONE
 * description: initialize the first page directory entry
 * side effect: put the newly created page table into blank page directory
 */
void init_first_page_directory_entry() {
	page_directory[0] =  ((unsigned int) page_table)  |  SMALL_PAGE_DIRECTORY_ENTRY_MASK; 
}


/*
 * init_kernel_page
 * input: NONE
 * description: initialize the kernel page
 * side effect: none
 */

void init_kernel_page() {
	page_directory[KERNEL_PAGE_DIRECTORY_OFFSET] = 	((unsigned int)  KERNEL_START_ADDR) | LARGE_PAGE_DIRECTORY_ENTRY_MASK;
}

/*
 * add_video_memory
 * input: idx of the page table to set
 * description: set a page in the page table to be present
 * side effect: none
 */

void add_video_memory(uint32_t page_table_idx) {
	page_table[page_table_idx] = ((unsigned int) (page_table[page_table_idx]) ) | PAGE_TABLE_ENTRY_MASK; // assign page table address and mark as present 	
}

/*
 * load_page_directory
 * input: pointer to the page directory to be loaded
 * description: load the desired page directory
 * side effect: none
 */

void load_page_directory(unsigned int* page_dir) {
	asm volatile (
		"movl %0, %%eax;"
		"movl %%eax, %%cr3;"
		:
		: "r"(page_dir)
		: "%eax", "cc"
	);
}

/*
 * enable_paging
 * input: NONE
 * description: enable paging
 * side effect: none
 */

void enable_paging() {
	asm volatile (
		"movl %%cr0, %%eax;"
		"orl %0, %%eax;"
		"movl %%eax, %%cr0;"
		:
		: "r" (PAGING_BIT_MASK)
		: "%eax", "cc"
	);
}

/*
 * enable_mix_paging
 * input: NONE
 * description: ???
 * side effect: none
 */

void enable_mix_paging_size() {
	asm volatile (
		"movl %%cr4, %%eax;"
		"orl %0, %%eax;"
		"movl %%eax, %%cr4;"
		:
		: "r" (PSE_BIT_MASK)
		: "%eax", "cc"
	);
}

/*
 * paging_init()
 * input: NONE
 * description: initialize page directory and page table, 
 *  put page table in the page directory and enable paging
 * side effect: none
 */

void paging_init() {
	create_empty_page_directory();
	create_empty_page_table();
	init_first_page_directory_entry();
	init_kernel_page();
	add_video_memory(VIDEO_PAGE_TABLE_IDX);	
	
	load_page_directory(page_directory);
	enable_mix_paging_size();
	enable_paging();
	
}



