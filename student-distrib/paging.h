#ifndef PAGING_H
#define PAGING_H

#include "types.h"

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

uint32_t page_directory[PAGE_DIRECTORY_NUM] __attribute__((aligned(PAGE_DIRECTORY_SIZE)));
uint32_t page_table[PAGE_TABLE_NUM] __attribute__((aligned(PAGE_TABLE_SIZE)));
//uint32_t first_page_table[1024] __attribute__((aligned(4096)));

// functions to initialize page directory and page table
extern void create_empty_page_directory();
extern void create_empty_page_table();
extern void init_first_page_directory_entry();

extern void add_video_memory(uint32_t page_table_idx);
extern void init_kernel_page();


// functions written in assembly to enable paging
extern void load_page_directory(unsigned int* page_dir);
extern void enable_paging();
extern void enable_mix_paging_size();


extern void paging_init();

#endif