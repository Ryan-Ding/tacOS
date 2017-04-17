// this file support terminal driver
#include "terminal.h"

volatile terminal_t terminal;

/*
 * init_terminal
 * input: NONE
 * description: This function inittialize terminal
 * side effect : none
 */

void init_terminal(){
    int i;
    terminal.pos_x = 0;
    terminal.pos_y = 0;
    for (i=0; i< BUFFER_SIZE; i++)
        terminal.buffer_key[i] = '\0';
    terminal.read_flag = 0;
    terminal.curr_idx = 0;
    // may need a function to map the virtual address with physical addresses
    terminal.num_process = 0;
   for (i =0; i<PID_SIZE; i++) {
       terminal.pid_array[i] = -1;
   }

    clear();

    set_cursor(0,0);

    buffer_key = terminal.buffer_key;
    buffer_idx = &(terminal.curr_idx);
    enter_flag = &(terminal.read_flag);
    cursor_x = &(terminal.pos_x);
    cursor_y = &(terminal.pos_y);

}
/*
 * terminal_read
 * input: pointer to the buffer to be copied, num of bytes to be copied
 * description: This function reads content from the buffer
 * side effect : return numofbytes read, clear key buffer
 */


int
terminal_read(unsigned char* buf, int num_bytes){
    int i=0;
    sti();
    while (!terminal.read_flag);
    terminal.read_flag = 0;
    for (i = 0; i<num_bytes && i<BUFFER_SIZE;i++ ) {
        buf[i] = buffer_key[i];
        buffer_key[i] = KEY_EMPTY;
    }
    cli();
    return i;
}
/*
 * terminal_write
 * input: pointer to the buffer to be displayed, num of bytes to be written
 * description: This function output chars in the buffer to screen
 * side effect : return numofbytes written
 */
int
terminal_write(unsigned char* buf,int num_bytes){
    int i ;
    int count = 0;

    cli();
    for (i = 0; i < num_bytes;i++)
    {
        if (buf == NULL)
            break;

        putc(*buf);
        buf++;
        count++;
    }
    sti();
    return count;
}

/*
 * terminal_open
 * input: none
 * description: open the terminal
 * return value: 0
 * side effect : none
 */

int terminal_open(){
    return 0;
}

/*
 * terminal_close
 * input: none
 * description: close the terminal
 * return value: 0
 * side effect : none
 */

int terminal_close(){
    return 0;
}
/*
int get_curr_pid() {
    int i =terminal.num_process;
    if( i == 0) return -1;
    return terminal.pid_array[i-1];
}
void close_process(){
    int i =terminal.num_process;
    terminal.pid_array[i-1] = -1;
    terminal.num_process--;
}
*/
