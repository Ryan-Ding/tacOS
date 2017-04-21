#ifndef TERMINAL_H
#define TERMINAL_H

#include "lib.h"
#include "types.h"
#include "keyboard.h"

/* the following struct is created for future
*   cp when multiple terminals need to be supported */

#define _128MB 0x8000000
#define _4KB 0x1000
#define BUFFER_SIZE 128
#define PID_SIZE 6
#define SHELL_LENGTH 7
#define OUR_SHELL_LENGTH 8
typedef struct {
    int pos_x;
    int pos_y;
    volatile int read_flag;
    int curr_idx;
    unsigned char buffer_key[BUFFER_SIZE];
    int * mem_video;
    int num_process;
    int pid_array[PID_SIZE];
}terminal_t;

extern void init_terminal();
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd,const void* buf, int32_t nbytes);
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);
/*int get_curr_pid();
void close_process();
*/
#endif
