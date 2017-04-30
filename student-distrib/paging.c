#include "paging.h"

/* reference: http://wiki.osdev.org/Setting_Up_Paging */

/*
 * create_empty_page_directory
 * input: NONE
 * description: create a basic empty page directory
 * side effect: set pages as not present
 */

void create_empty_page_directory(uint32_t pid) {
	int i;
	page_directory_t * page_directory = &(page_directory_list[pid + PID_PD_OFFSET]);
	for (i = 0; i < PAGE_DIRECTORY_NUM; ++i) {
		(*page_directory)[i] = NOT_PRESENT_MASK; // set present bit to be 10 (meaning not present)
	}
}

/*
 * create_empty_page_table
 * input: NONE
 * description: create a basic empty page table
 * side effect: none
 */

void create_empty_page_table(uint32_t pid) {
	int i;
	page_table_t * page_table = &(page_table_list[pid + PID_PD_OFFSET]);
	for (i = 0; i < PAGE_TABLE_NUM; ++i) {
		(*page_table)[i] = (i * 0x1000) | NOT_PRESENT_MASK; // leaving trailing 12 bits for starting addresses to be 0
	}
}

/*
 * init_first_page_directory
 * input: NONE
 * description: initialize the first page directory entry
 * side effect: put the newly created page table into blank page directory
 */
void init_first_page_directory_entry(uint32_t pid) {
	page_directory_t * page_directory = &(page_directory_list[pid + PID_PD_OFFSET]);
	page_table_t * page_table = &(page_table_list[pid + PID_PD_OFFSET]);
	(*page_directory)[0] =  ((unsigned int) page_table)  |  SMALL_PAGE_DIRECTORY_ENTRY_MASK;
}


/*
 * init_kernel_page
 * input: NONE
 * description: initialize the kernel page
 * side effect: none
 */

void init_kernel_page(uint32_t pid) {
	page_directory_t * page_directory = &(page_directory_list[pid + PID_PD_OFFSET]);
	(*page_directory)[KERNEL_PAGE_DIRECTORY_OFFSET] = 	((unsigned int)  KERNEL_START_ADDR) | LARGE_PAGE_DIRECTORY_ENTRY_MASK;
}

/*
 * add_video_memory
 * input: idx of the page table to set
 * description: set a page in the page table to be present
 * side effect: none
 */
//TODO change to |=
void add_video_memory(uint32_t pid, uint32_t page_table_idx) {
	int i;	
	page_table_t * page_table = &(page_table_list[pid + PID_PD_OFFSET]);
	// (*page_table)[page_table_idx] = ((unsigned int) ((*page_table)[page_table_idx]) ) | PAGE_TABLE_ENTRY_MASK; // assign page table address and mark as present
	(*page_table)[page_table_idx] = ((unsigned int) VIDEO_MEM_PHYS_ADDR ) | PAGE_TABLE_ENTRY_MASK; // assign page table address and mark as present
	// set up video memory back up 
	for (i = 0; i < TERM_NUM; ++i) {
		(*page_table)[page_table_idx + (i + 1)] = (VIDEO_MEM_PHYS_ADDR + (i + 1) * PAGE_SIZE) | PAGE_TABLE_ENTRY_MASK;
	}

	// set up permanently valid VIDEO_MEM_PHYS_ADDR
	(*page_table)[PERMANENT_PHYS_ADDR >> 12] = VIDEO_MEM_PHYS_ADDR | PAGE_TABLE_ENTRY_MASK;
	
}

/*
 * load_page_directory
 * input: pointer to the page directory to be loaded
 * description: load the desired page directory
 * side effect: none
 */

void load_page_directory(uint32_t pid) {
	unsigned int* page_dir = (unsigned int*) &(page_directory_list[pid + PID_PD_OFFSET]);
	asm volatile (
		"pushl %%eax;"
		"movl %0, %%eax;"
		"movl %%eax, %%cr3;"
		"popl %%eax;"
		:
		: "r"(page_dir)
		: "%eax", "cc"
	);
}

uint32_t get_cr3(uint32_t pid){
	return (uint32_t) &(page_directory_list[pid + PID_PD_OFFSET]);
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
 * description: enable  mixed paging for 4mb and 4kb at the same time
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


//TODO INTERFACE


/*
 * create_program_page()
 * input: pid number
 * description:create a page for the given process
 * side effect: none
 */

void create_program_page(uint32_t pid){
	page_directory_t * page_directory = &(page_directory_list[pid + PID_PD_OFFSET]);
	(*page_directory)[PROGRAM_IMAGE_PAGE_OFFSET] = 	((unsigned int)  PROGRAM_IMAGE_START_ADDRESS(pid)) | PROGRAM_IMAGE_ENTRY_MASK;
}

/*
 * paging_init()
 * input: NONE
 * description: initialize page directory and page table,
 *  put page table in the page directory and enable paging
 * common tasks for initalize new process
 * side effect: none
 */

void paging_init(uint32_t pid) {
	create_empty_page_directory(pid); //set up PDE/PTE
	create_empty_page_table(pid);
	init_first_page_directory_entry(pid);	//initialize first page directory entry
	init_kernel_page(pid);	//initialize second PDE
	add_video_memory(pid, VIDEO_PAGE_TABLE_IDX);	//set up video memory
	if (pid !=  PID_PD_OFFSET) {
		// create progam image page
		create_program_page(pid);
	}
	load_page_directory(pid);	//load page directory
	enable_mix_paging_size();	//enable page directory to be varied sized
	enable_paging();	//enable paging finally
	// uint32_t new_cr3=get_cr3(pid);
	// asm volatile("movl %0, %%cr3\n\t": : "r"(new_cr3):"cc");
}
