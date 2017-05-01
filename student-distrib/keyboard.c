/* keyboard.c
this file handles interrupt for keyboard driver
*/


#include "keyboard.h"

volatile unsigned char* buffer_key;
volatile int* enter_flag;
volatile int* buffer_idx;
volatile int* cursor_x;
volatile int* cursor_y;
//flags
int curr_case = 0;
int ctrl_on = 0;
int alt_on = 0;

// test function vars
int file_idx = FS_IDX;
int rtc_frq =  RTC_INI_FRQ;

// scancode lookup table used for screen ecoing
static unsigned char scancode_set[CASE_NUM][KEY_NUM] = {
  // regular
  {
    '\0','\0', '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=',
    '\0', /* backspace */
    '\0', /* tab */
    'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0',/* enter */
    '\0',	/* left Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';','\'', '`',
    '\0',/* left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/',
    '\0',/* right shift */
    '*',
    '\0',	/* alt */
    ' ',	/* space */
    '\0',	/* caps lock */
  },
  // Caps only
  {
    '\0','\0', '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=',
    '\0', /* backspace */
    '\0', /* tab */
    'Q', 'W', 'E', 'R','T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\0',/* enter */
    '\0',	/* left Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    ';','\'', '`',
    '\0',/* left shift */
    '\\', 'Z', 'X', 'C', 'V', 'B', 'N',
    'M', ',', '.', '/',
    '\0',/* right shift */
    '*',
    '\0',	/* alt */
    ' ',	/* space */
    '\0',	/* caps lock */
  },
  // SHIFT ONLY
  {
    '\0','\0', '!', '@', '#', '$', '%', '^', '&', '*',
    '(', ')', '_', '+',
    '\0', /* backspace */
    '\0', /* tab */
    'Q', 'W', 'E', 'R','T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\0',/* enter */
    '\0',	/* left Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    ':','\"', '~',
    '\0',/* left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N',
    'M', '<', '>', '?',
    '\0',/* right shift */
    '*',
    '\0',	/* alt */
    ' ',	/* space */
    '\0',	/* caps lock */
  },
  // SHIFT AND CAPS, the effect is design choice, differs on different OS
  {
    '\0','\0', '!', '@', '#', '$', '%', '^', '&', '*',
    '(', ')', '_', '+', '\0', '\0',
    'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0',
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':','\"', '~','\0',
    '|', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', '\0','*','\0',
    ' ', '\0',
  }
};

/*
* handle_press
* input: scancode received from the keyboard
* description: This function echo characters to the terminal and perform function tests
*/

void
handle_press(unsigned char scancode){
  // only handle keys that we've defined
  if (scancode >=KEY_NUM)
  return;
  unsigned char key_pressed = scancode_set[curr_case][scancode];
  int i ;
  if (ctrl_on && (key_pressed == 'l'|| key_pressed == 'L') ) {
    clear(); // clear video memory
    for (i = 0; i < BUFFER_SIZE; i++) {
      buffer_key[i]=KEY_EMPTY;
    }
    (*buffer_idx)=0;
    *cursor_x = 0;
    *cursor_y = 0;
    set_cursor(*cursor_x,*cursor_y);
  }
  else if (ctrl_on && key_pressed == 'c') {
    send_eoi(KEYBOARD_IRQ);
    sti();
    // wait until it is within the display terminal's process execution
    while (curr_process->terminal_id != curr_display_term) ;

    for ( i = 0; i < *buffer_idx; i++) {
      buffer_key[i]=KEY_EMPTY;

    }
    *buffer_idx = 0;
    system_halt(-1);
  }
//   else if (ctrl_on && key_pressed == '1'){ // test read file
//     clear();
//     set_cursor(0,0);
//     test_dir_read();
//   }
//   else if (ctrl_on && key_pressed == '2'){ // test read file
//     clear();
//     set_cursor(0,0);
//     test_reg_read();
//   }
//   else if (ctrl_on && key_pressed == '3'){ // test read file by idx
//     clear();
//     set_cursor(0,0);
//     test_read_file_by_index(file_idx++);
//   } else if (ctrl_on && key_pressed == '4'){ // rtc test
//     clear();
//     set_cursor(0,0);
//     rtc_write(rtc_frq);
//     rtc_frq = rtc_frq<<1;
//   } else if (ctrl_on && key_pressed == '5'){ // stop rtc
//     clear();
//     set_cursor(0,0);
//     rtc_frq = RTC_INI_FRQ;
//     rtc_stop();
//   }
  else if (*buffer_idx<(BUFFER_SIZE-1)) {


    buffer_key[*buffer_idx]=key_pressed;
    keyboard_putc(key_pressed);
    (*buffer_idx)++;
    (*cursor_x)++;
    correct_cursor();

    // if((*cursor_x)==NUM_COLS-1) { // edge cases when changing lines and scrolling is needed
    //   *cursor_x = 0;
    //   if ((*cursor_y)<NUM_ROWS-1 ) {  (*cursor_y)++; }
    //   else{ scroll_line();}
    //   putc(key_pressed);
    //
    // }
    // else{
    //   //  (*cursor_x)++;
    //   putc(key_pressed);
    // }
  }




}



/*
* keyboard_init
* input: NONE
* description: This function initialize interrupt handler for keyboard
*/

void
keyboard_init(void){
  // call wrapper function
  set_intr_gate(KEYBOARD_IRQ + MASTER_IDT_OFFSET, keyboard_interrupt_handler);

  // enable keyboard irq line
  enable_irq(KEYBOARD_IRQ);
}

/*
* keyboard_interrupt
* input: NONE
* description: This function handles input from keyboard
* side effect : mark flags for different cases
*/

void
keyboard_interrupt(void){
  //disable interrupt from this device
  cli();

  unsigned char scancode;
  int i;

  // get input from keyboard
  while (1) {
    scancode = inb(KEYBOARD_SCANCODE_PORT);
    if (scancode > 0) { break; }
  }

  // case 0 : regurlar 1: caps only 2: shift only 3: caps and shift
  switch (scancode) {

    case CAPS:
    if (curr_case == CASE_CAPS)
    curr_case = CASE_REG;
    else if (curr_case == CASE_BOTH)
    curr_case = CASE_SHIFT;
    else if (curr_case == CASE_SHIFT)
    curr_case = CASE_BOTH;
    else
    curr_case = CASE_CAPS;
    break;
    case BACKSPACE:
    // if(!(*cursor_y==0 && *cursor_x ==0)) {
    //   if (*buffer_idx > 0) {
    //     delete_content();
    //   }
    // }
    if (*buffer_idx > 0) {
      (*buffer_idx)--;
      buffer_key[*buffer_idx]= KEY_EMPTY;
      delete_content();
      (*cursor_x)--;
      correct_cursor();

    }
    break;

    case ENTER:
    *enter_flag = 1;
    buffer_key[*buffer_idx]=LINE_END;
    for (i = *buffer_idx + 1; i < BUFFER_SIZE; i++) {
      buffer_key[i] = KEY_EMPTY;
    }

    (*buffer_idx)=0;
    //change_line();
    (*cursor_x) = 0;
    (*cursor_y)++;

    correct_cursor();
    break;

    case LEFT_SHIFT_PRESSED:
    curr_case = (curr_case==CASE_CAPS) ? CASE_BOTH : CASE_SHIFT;
    break;
    case RIGHT_SHIFT_PRESSED:
    curr_case = (curr_case==CASE_CAPS) ? CASE_BOTH : CASE_SHIFT;
    break;
    case CTRL_PRESSED:
    ctrl_on = 1;
    break;
    case LEFT_SHIFT_RELEASED:
    curr_case = (curr_case==CASE_BOTH) ? CASE_CAPS : CASE_REG;
    break;
    case RIGHT_SHIFT_RELEASED:
    curr_case = (curr_case==CASE_BOTH) ? CASE_CAPS : CASE_REG;
    break;
    case CTRL_RELEASED:
    ctrl_on = 0;
    break;
    case ALT_PRESSED:
    alt_on = 1;
    break;
    case ALT_RELEASED:
    alt_on = 0;
    break;
    case F1:
    if(alt_on){
      return switch_terminal(FIRST_TERM);
    }
    break;
    case F2:
    if(alt_on){
      return switch_terminal(SECOND_TERM);
    }
    break;

    case F3:
    if(alt_on){
      return switch_terminal(THIRD_TERM);
    }
    break;


    default:
    handle_press(scancode);
    break;
  }
  // only print when it is pressed instead of released
  //if( scancode < 0x80) { printf("%x pressed \n",scancode); }

  send_eoi(KEYBOARD_IRQ);

  //enable interrupt from this device
  sti();

}

/*
* switch_terminal
* input: the new terinal to be switched to
* description: This function switch to the new terminal requested by the user
* side effect : store and restore current terminal info 
*/
void switch_terminal(uint32_t new_terminal_id) {
    //uint8_t shell_program[] = "shell";
    if (terminal[new_terminal_id].curr_process == NULL) {
        if (check_available_pid() < 0) {
            send_eoi(KEYBOARD_IRQ);
            sti();
            return;
        }
        curr_process->is_blocked_by_new_terminal = 1; // remind pit to open new terminal on its behalf
        switch_term(new_terminal_id);
        restore_term(new_terminal_id);
        send_eoi(KEYBOARD_IRQ);
        // asm volatile("movl %%esp, %0;"
        //              "movl %%ebp, %1;"
        //              : "=g"(curr_process->fake_esp), "=g"(curr_process->fake_ebp)
        //              :
        //              : "memory", "cc"
        // );
        sti();
        // printf("Reached here");
    } else {
        switch_term(new_terminal_id);
        restore_term(new_terminal_id);
        send_eoi(KEYBOARD_IRQ);
        sti();
    }
    return;
//   uint8_t shell_program[] = "shell";
//   if (terminal[new_terminal_id].curr_process == NULL) {
//     if (check_available_pid() < 0) {
//       send_eoi(KEYBOARD_IRQ);
//       sti();
//       return;
//     }
//     switch_term(new_terminal_id);
//     restore_term(new_terminal_id);
//     curr_term = new_terminal_id;
//     curr_process = NULL;
//     send_eoi(KEYBOARD_IRQ);
//     system_execute(shell_program);
//   } else {
//     switch_term(new_terminal_id);
//     restore_term(new_terminal_id);
//     send_eoi(KEYBOARD_IRQ);
//     sti();
//   }
//   return;
}
