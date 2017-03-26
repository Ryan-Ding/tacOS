// this file support terminal driver
#include "terminal.h"

terminal_t terminal;

// the following functions need more care
void init_terminal(){
    int i;
    terminal.pos_x = 0;
    terminal.pos_y = 0;
    for (i=0; i< BUFFER_SIZE; i++)
        terminal.buffer_key[i] = '\0';
    terminal.read_flag = 0;
    terminal.curr_idx = 0;
    // may need a function to map the virtual address with physical addresses

    terminal.mem_video = (unsigned char*)_128MB+_4KB;
    clear();

    set_cursor(0,0);
    memcpy((int *) VIDEO, (int *)terminal.mem_video,2*NUM_COLS*NUM_ROWS);

    buffer_key = terminal.buffer_key;
    buffer_idx = &(terminal.curr_idx);
    enter_flag = &(terminal.read_flag);
    //execute("shell");
}


int
terminal_read(int fd, unsigned char* buf, int num_bytes){
    int i;

    sti();
    while (!terminal.read_flag);
    terminal.read_flag = 0;
    for (i = 0; i<num_bytes && i<BUFFER_SIZE;i++ ) {
        buf[i] = buffer_key[i];
        buffer_key[i] = KEY_EMPTY;
    }
    return i;
}

int terminal_write(int fd, unsigned char* buf, int num_bytes){
    int i ;
    for (i = 0; i<num_bytes; i++)
        putc(buf[i]);

    return i;
}
