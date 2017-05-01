// this file support terminal driver
#include "terminal.h"

volatile terminal_t terminal[TERM_NUM];

volatile int curr_term = 0;
volatile uint32_t wait_to_read[TERM_NUM];

uint8_t curr_display_term = 0;

/*
 * init_terminal
 * input: NONE
 * description: This function inittialize terminal
 * side effect : none
 */

void init_terminal(){
    int i,j;

    for(j = 0;j<TERM_NUM;j++){
      wait_to_read[j] = 0;
      terminal[j].pos_x = 0;
      terminal[j].pos_y = 0;
      terminal[j].read_flag = 0;
      terminal[j].curr_idx = 0;
      terminal[j].num_process = 0;
      terminal[j].curr_process = NULL;
      for (i=0; i< BUFFER_SIZE; i++){
        terminal[j].buffer_key[i] = KEY_EMPTY;
      }
      for (i =0; i<PID_SIZE; i++) {
              terminal[j].pid_array[i] = -1;
      }
      clear();
    }
    restore_term(0);
    curr_term = 0;
    curr_display_term = 0;

}
/*
void save_term(int term){
  int i;
  for (i=0; i< BUFFER_SIZE; i++){
    terminal[term].buffer_key[i] = buffer_key[i];
  }

  terminal[term].curr_idx = *buffer_idx;
  terminal[term].read_flag = *enter_flag;
  terminal[term].pos_x = *cursor_x;
  terminal[term].pos_y = *cursor_y;

}
*/

/*
* restore_term
* input: the new terinal to be restored
* description: restore the information from term
* side effect : none 
*/

void restore_term(int term){
  buffer_key = terminal[term].buffer_key;

  buffer_idx = &(terminal[term].curr_idx);
  enter_flag = &(terminal[term].read_flag);
  cursor_x = &(terminal[term].pos_x);
  cursor_y = &(terminal[term].pos_y);
  set_cursor(*cursor_x,*cursor_y);

}
/*
* switch_term
* input: the new terinal to be switched to
* description: This function switch to the new terminal requested by the user
* side effect : change paging
*/
void switch_term(int term) {
    if (curr_display_term == term) {
        printf("same terminal! \n");
        return;
    }
    //save_term(curr_display_term);
    remap_video(term);
    //restore_term(term);

    // curr_term = term;
    // printf("curr process spawn in terminal %d\n", curr_process->terminal_id );
    // printf("current active terminal: %d\n", curr_term);
    // printf("current displayed terminal: %d\n", curr_display_term);
    // printf("want to be spawn in terminal %d\n", term );

    curr_display_term = term;
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
    int j;
    uint8_t* buff = (uint8_t*)buf;
    if(fd == FD_STDOUT)	//can't read from stdout
		return -1;
    sti();
    wait_to_read[curr_term] = 1;
    while (!terminal[curr_display_term].read_flag);
    terminal[curr_display_term].read_flag = 0;
    for (i = 0; i<nbytes && i<BUFFER_SIZE && terminal[curr_display_term].buffer_key[i] != KEY_EMPTY;i++ ) {
        buff[i] = terminal[curr_display_term].buffer_key[i];
        terminal[curr_display_term].buffer_key[i] = KEY_EMPTY;
    }
    for (j = i+1;j<nbytes;j++ ){
      buff[j] = KEY_EMPTY;
    }
    wait_to_read[curr_term] = 0;
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
    if (fd == STDIN  || buf == NULL) { return -1; }
    cli();
    for (i = 0; i < nbytes; i++)
    {
        if (buff == NULL)
            break;
        terminal_putc(*buff);
        buff++;
        count++;
    }
    // *cursor_x = get_screenx(); // update cursorx and cursor y based on the
    // *cursor_y = get_screeny(); // newest cursor pos updated by write
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

int32_t is_terminal_active() {
    return (curr_process->terminal_id == curr_display_term );
}
