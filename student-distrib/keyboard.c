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

    if (ctrl_on && (key_pressed == 'l'|| key_pressed == 'L') ) {
        clear(); // clear video memory
        set_cursor(0,0);
    }
    else if (ctrl_on && key_pressed == 'c') {
        send_eoi(KEYBOARD_IRQ);
        sti();
        system_halt(-1);
    }
    else if (ctrl_on && key_pressed == '1'){ // test read file
      clear();
      set_cursor(0,0);
      test_dir_read();
    }
    else if (ctrl_on && key_pressed == '2'){ // test read file
      clear();
      set_cursor(0,0);
      test_reg_read();
    }
    else if (ctrl_on && key_pressed == '3'){ // test read file by idx
      clear();
      set_cursor(0,0);
      test_read_file_by_index(file_idx++);
    } else if (ctrl_on && key_pressed == '4'){ // rtc test
      clear();
      set_cursor(0,0);
      rtc_write(rtc_frq);
      rtc_frq = rtc_frq<<1;
    } else if (ctrl_on && key_pressed == '5'){ // stop rtc
      clear();
      set_cursor(0,0);
      rtc_frq = RTC_INI_FRQ;
      rtc_stop();
    }
    else if (*buffer_idx<BUFFER_SIZE) {


        buffer_key[*buffer_idx]=key_pressed;
        (*buffer_idx)++;
        if((*cursor_x)==NUM_COLS-1) { // edge cases when changing lines and scrolling is needed
          putc(key_pressed);
          *cursor_x = 0;
          if ((*cursor_y)<NUM_ROWS-1 ) {  (*cursor_y)++; }
          //else{ scroll_line();}
        }
        else{
          (*cursor_x)++;
          putc(key_pressed);
        }
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
            if(!(*cursor_y==0 && *cursor_x ==0)) { 
                if (*buffer_idx > 0) {
                    delete_content(); 
                }
            }
            if((*cursor_x) == 0) {
                if (*cursor_y > 0) {
                  *cursor_x = NUM_COLS - 1;
                  (*cursor_y)--;
                }
            }
            else {
              (*cursor_x)--;
            }

            if (*buffer_idx > 0) {
                (*buffer_idx)--;
                buffer_key[*buffer_idx]= KEY_EMPTY;

            }
            break;

        case ENTER:
            *enter_flag = 1;
            buffer_key[*buffer_idx]=LINE_END;
            (*buffer_idx)=0;
            change_line();
            (*cursor_x) = 0;
            if((*cursor_y)<NUM_ROWS-1)
              (*cursor_y)++;
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
                if (terminal[0].curr_process == NULL) {
                    if (check_available_pid() < 0) {
                        break;
                    }    
                    curr_term = 0;
                    //curr_display_term = 0;
                    switch_term(0);
                    curr_process = NULL;
                    send_eoi(KEYBOARD_IRQ);
                    system_execute("shell");
                    return;
                }   else {
                    switch_term(0);
                    send_eoi(KEYBOARD_IRQ);
                    sti();
                    return;
                }  
            }
            break;
        case F2:
            if(alt_on){
                if (terminal[1].curr_process == NULL) {
                    if (check_available_pid() < 0) {
                        break;
                    }    
                    curr_term = 1;
                    //curr_display_term = 1;
                    switch_term(1);
                    curr_process = NULL;
                    send_eoi(KEYBOARD_IRQ);
                    system_execute("shell");
                    return;
                }    
                switch_term(1);
                    send_eoi(KEYBOARD_IRQ);
                    sti();
                    return;
            }
            break;
        case F3:
           if(alt_on){
                if (terminal[2].curr_process == NULL) {
                    if (check_available_pid() < 0) {
                        break;
                    }
                    curr_term = 2;
                    // curr_display_term = 2;
                    switch_term(2);
                    curr_process = NULL;
                    send_eoi(KEYBOARD_IRQ);
                    system_execute("shell");
                    return;
                }    
                switch_term(2);
                send_eoi(KEYBOARD_IRQ);
                sti();
                return;
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
