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
        terminal.buffer_key[i] = KEY_EMPTY;
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


int32_t
terminal_read(int32_t fd, void* buf, int32_t nbytes){
    int i = 0;
    uint8_t* buff = (uint8_t*)buf;    
    if(fd == FD_STDOUT)	//can't read from stdout
		return -1;
    sti();
    while (!terminal.read_flag);
    terminal.read_flag = 0;
    for (i = 0; i<nbytes && i<BUFFER_SIZE && buffer_key[i] != KEY_EMPTY;i++ ) {
        buff[i] = buffer_key[i];
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
int32_t
terminal_write(int32_t fd,const void* buf, int32_t nbytes){
    int i ;
    int count = 0;
    uint8_t* buff = (uint8_t*)buf;
    if (fd == STDIN  || buf == NULL || buff[0] == '\0') { return -1; }
    cli();
    for (i = 0; i < nbytes; i++)
    {
        if (buff == NULL)
            break;
        putc(*buff);
        buff++;
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

int32_t terminal_open(const uint8_t* filename){
    return 0;
}

/*
 * terminal_close
 * input: none
 * description: close the terminal
 * return value: 0
 * side effect : none
 */

int32_t terminal_close(int32_t fd){
    return -1;
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
