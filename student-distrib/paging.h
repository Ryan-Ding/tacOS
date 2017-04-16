#ifndef PAGING_H
#define PAGING_H

#include "types.h"
#include "pcb.h"

#define VIDEO_PAGE_TABLE_IDX 0x000000B8

#define PAGE_DIRECTORY_NUM 1024
#define PAGE_DIRECTORY_SIZE (PAGE_DIRECTORY_NUM * 4)

#define PAGE_TABLE_NUM 1024
#define PAGE_TABLE_SIZE (PAGE_TABLE_NUM * 4)

#define NOT_PRESENT_MASK 0x00000002
#define PAGING_BIT_MASK 0x80000000
#define SMALL_PAGE_DIRECTORY_ENTRY_MASK 0x00000003
#define LARGE_PAGE_DIRECTORY_ENTRY_MASK (0x00000080 | SMALL_PAGE_DIRECTORY_ENTRY_MASK)
#define PAGE_TABLE_ENTRY_MASK 0x00000003

#define PSE_BIT_MASK 0x00000010
#define SMALL_PAGE_TABLE_ADDR_OFFSET 12
#define KERNEL_PAGE_DIRECTORY_OFFSET 1

#define KERNEL_START_ADDR 0x00400000

/* stuff added in cp3 */

#define SUPERVISOR_MASK 0x4
#define PROGRAM_IMAGE_ENTRY_MASK  (LARGE_PAGE_DIRECTORY_ENTRY_MASK | SUPERVISOR_MASK)

typedef uint32_t page_directory_t[PAGE_DIRECTORY_NUM] __attribute__((aligned(PAGE_DIRECTORY_SIZE)));
typedef uint32_t page_table_t[PAGE_TABLE_NUM] __attribute__((aligned(PAGE_TABLE_SIZE)));

page_directory_t page_directory_list[MAX_PROCESS_NUM]; 
page_table_t page_table_list[MAX_PROCESS_NUM];

/* ends here */

// uint32_t page_directory[PAGE_DIRECTORY_NUM] __attribute__((aligned(PAGE_DIRECTORY_SIZE)));
//uint32_t page_table[PAGE_TABLE_NUM] __attribute__((aligned(PAGE_TABLE_SIZE)));
//uint32_t first_page_table[1024] __attribute__((aligned(4096)));

// functions to initialize page directory and page table
extern void create_empty_page_directory(uint32_t pid);
extern void create_empty_page_table(uint32_t pid);
extern void init_first_page_directory_entry(uint32_t pid);

extern void add_video_memory(uint32_t pid, uint32_t page_table_idx);
extern void init_kernel_page(uint32_t pid);


// functions written in assembly to enable paging
extern void load_page_directory(uint32_t pid);
uint32_t get_cr3(uint32_t pid);
extern void enable_paging();
extern void enable_mix_paging_size();


extern void paging_init();

extern void create_program_page(uint32_t pid);


#endif
