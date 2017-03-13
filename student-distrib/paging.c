#include "paging.h"

/* reference: http://wiki.osdev.org/Setting_Up_Paging */

void create_empty_page_directory() {
	int i;
	for (i = 0; i < PAGE_DIRECTORY_NUM; ++i) {
		page_directory[i] = NOT_PRESENT_MASK; // set present bit to be 10 (meaning not present)
	}
}

void create_empty_page_table() {
	int i;
	for (i = 0; i < PAGE_TABLE_NUM; ++i) {
		page_table[i] = (i * 0x1000) | NOT_PRESENT_MASK; // leaving trailing 12 bits for starting addresses to be 0
	}
}

void init_first_page_directory_entry() {
	page_directory[0] =  ((unsigned int) page_table)  |  SMALL_PAGE_DIRECTORY_ENTRY_MASK; 
}

void init_kernel_page() {
	page_directory[KERNEL_PAGE_DIRECTORY_OFFSET] = 	((unsigned int)  KERNEL_START_ADDR) | LARGE_PAGE_DIRECTORY_ENTRY_MASK;
}

void add_video_memory(uint32_t page_table_idx) {
	page_table[page_table_idx] = ((unsigned int) (page_table[page_table_idx]) ) | PAGE_TABLE_ENTRY_MASK; // assign page table address and mark as present 	
}

void load_page_directory(unsigned int* page_dir) {
	asm volatile (
		"movl %0, %%eax;"
		"movl %%eax, %%cr3;"
		:
		: "r"(page_dir)
		: "%eax", "cc"
	);
}
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



// #define ASM 1 
// #include "paging.h"


// .text

// .globl enable_paging
// .globl load_page_directory
// .globl enable_mix_paging_size
// load_page_directory:
//     pushl %ebp
//     movl %esp, %ebp
//     movl 8(%esp), %eax
//     movl %eax, %cr3
//     movl %ebp, %esp
//     popl %ebp
//     ret

// enable_paging:
//     pushl %ebp
//     movl %esp, %ebp
//     movl %cr0, %eax
//     orl $PAGING_BIT_MASK, %eax
//     movl %eax, %cr0
//     movl %ebp, %esp
//     popl %ebp
//     ret

// enable_mix_paging_size:
//     pushl %ebp
//     movl %esp, %ebp
//     movl %cr4, %eax
//     orl $PSE_BIT_MASK, %eax
//     movl %eax, %cr4
//     movl %ebp, %esp
//     popl %ebp
//     ret


// # set up blank 
