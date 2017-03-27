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

  //  terminal.mem_video = (int*)_128MB+_4KB;
    clear();

    set_cursor(0,0);

    //int val = 2 * NUM_COLS * NUM_ROWS;
    //memcpy((int *) VIDEO, (int *)terminal.mem_video,val);
    //printf("%d \n",val );
    buffer_key = terminal.buffer_key;
    buffer_idx = &(terminal.curr_idx);
    enter_flag = &(terminal.read_flag);
    cursor_x = &(terminal.pos_x);
    cursor_y = &(terminal.pos_y);
    //execute("shell");
}


int
terminal_read(unsigned char* buf, int num_bytes){
    int i=0;

    while (!terminal.read_flag);
    terminal.read_flag = 0;
    for (i = 0; i<num_bytes && i<BUFFER_SIZE;i++ ) {
        buf[i] = buffer_key[i];
        buffer_key[i] = KEY_EMPTY;
    }
    return i;
}

int
terminal_write(unsigned char* buf){
    int i ;
    cli();
    i = printf((int*)buf);
    sti();
    return i;
}
