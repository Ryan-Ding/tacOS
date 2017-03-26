#ifndef TERMINAL_H
#define TERMINAL_H

#include "lib.h"
#include "types.h"
#include "keyboard.h"

/* the following struct is created for future
*   cp when multiple terminals need to be supported */

#define _128MB 0x8000000
#define _4KB 0x1000

typedef struct {
    int pos_x;
    int pos_y;
    int read_flag;
    int curr_idx;
    unsigned char buffer_key[BUFFER_SIZE];
    unsigned char * mem_video;

}terminal_t;

extern void init_terminal();
int terminal_read(int fd,unsigned char* buf, int num_bytes);
int terminal_write(int fd,unsigned char* buf, int num_bytes);
//int terminal_open();
//int terminal_close();


#endif
