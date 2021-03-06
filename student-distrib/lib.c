/* lib.c - Some basic library functions (printf, strlen, etc.)
* vim:ts=4 noexpandtab
*/

#include "lib.h"
#include "terminal.h"
#include "paging.h"
//static int screen_x;
//static int screen_y;
static char* video_mem = (char *)VIDEO;

uint8_t last_one;
uint8_t last_two;
/*
* void clear(void);
*   Inputs: void
*   Return Value: none
*	Function: Clears video memory
*/

void
clear(void)
{
  int32_t i;
  //clear the screen
  for(i=0; i<NUM_ROWS*NUM_COLS; i++) {
    *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1)) = ' ';
    if (curr_display_term ==0) {
      *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_0;
    } else if (curr_display_term ==1){
      *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_1;
    }else {
      *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_2;
    }
  }
  //load_page_directory(terminal[curr_term].curr_process->pid + 1);
}

/*
* void init_color(void);
*   Inputs: void
*   Return Value: none
*	Function: init color for terminal
*/

void
init_color(void)
{
  int32_t i;
  //clear the screen
  for(i=0; i<NUM_ROWS*NUM_COLS; i++) {
    //*(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1)) = ' ';
    if (curr_display_term ==0) {
      *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_0;
    } else if (curr_display_term ==1){
      *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_1;
    }else {
      *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_2;
    }
  }
  //load_page_directory(terminal[curr_term].curr_process->pid + 1);
}

/* void delete_content
* input: void
* return value: none
* function: clear one character in the video memory
*/
void delete_content(void){
  int32_t i;
  if(!((*cursor_y)==0 && (*cursor_x) ==0)) { i = NUM_COLS * (*cursor_y) + (*cursor_x) - 1; }

  // mark last char printed as empty
  *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1)) = ' ';
  if (curr_display_term == 0) {
    *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_0;
  } else if (curr_display_term ==1){
    *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_1;
  }else {
    *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_2;
  }
  //load_page_directory(terminal[curr_term].curr_process->pid + 1);


}



/* void scroll_line
* input: void
* return value: none
* function: scroll up so the content fit in the screen
* this will effectively remove the first line and clear the bottom line
*/

void scroll_line(void){
  int i,j;
  int old_position, new_position;

  for (j = 1; j<NUM_ROWS; j++) {
    for (i = 0; i<NUM_COLS; i++) {
      new_position = NUM_COLS*(j-1) + i;
      old_position = NUM_COLS*j + i;
      *(uint8_t *)(video_mem +(new_position<<1)) = *(uint8_t *)(video_mem +(old_position<<1)); // copy the buffer from lower line to upper
      *(uint8_t *)(video_mem +(new_position<<1)+1) = *(uint8_t *)(video_mem +(old_position<<1)+1);
    }
  }
  // empty the last line
  for (i =0; i<NUM_COLS; i++) {
    j = NUM_COLS*(NUM_ROWS-1)+i;
    *(uint8_t *)(video_mem + (j << 1)) = ' ';
  }

}

/* void keyboard_scroll_line
* input: void
* return value: none
* function: scroll up so the content fit in the screen
* this will effectively remove the first line in phsycial video memoryand clear the bottom line
*/
void keyboard_scroll_line(void){
  int i,j;
  int old_position, new_position;
  //load_page_directory(terminal[curr_display_term].curr_process->pid + 1);
  for (j = 1; j<NUM_ROWS; j++) {
    for (i = 0; i<NUM_COLS; i++) {
      new_position = NUM_COLS*(j-1) + i;
      old_position = NUM_COLS*j + i;
      // copy the buffer from lower line to upper
      *(uint8_t *)(PERMANENT_PHYS_ADDR +(new_position<<1)) = *(uint8_t *)(PERMANENT_PHYS_ADDR +(old_position<<1));
      *(uint8_t *)(PERMANENT_PHYS_ADDR +(new_position<<1)+1) = *(uint8_t *)(PERMANENT_PHYS_ADDR +(old_position<<1)+1);
    }
  }
  // empty the last line
  for (i =0; i<NUM_COLS; i++) {
    j = NUM_COLS*(NUM_ROWS-1)+i;
    *(uint8_t *)(PERMANENT_PHYS_ADDR + (j << 1)) = ' ';
  }
  //load_page_directory(terminal[curr_term].curr_process->pid + 1);
}

/*
* terminal_putc
* input:  character to be printed
* description: the print function for terminal_write
* return value: none
* side effect : print c to terminal, correct position if needed
*/
void
terminal_putc(uint8_t c)
{
  //terminal_t back_struct = terminal[curr_term];
 //init_terminal();
  int i = (NUM_COLS*terminal[curr_term].pos_y + terminal[curr_term].pos_x);
  if(c == '\n' || c == '\r') {
    terminal[curr_term].pos_y++;
    terminal[curr_term].pos_x=0;
  } else {
    // use different mapping address for scheduling_term and display term
    if(curr_term==curr_display_term)
    {
      // display the input  with specific color
      *(uint8_t *)(video_mem + (i << 1)) = c;
      if (curr_term == 0) {
        *(uint8_t *)(video_mem /*+ (1 + curr_term) * FOUR_KB*/ + (i << 1) + 1) = ATTRIB_0;
      } else if (curr_term ==1){
        *(uint8_t *)(video_mem/* +  (1 + curr_term) * FOUR_KB*/ + (i << 1) + 1) = ATTRIB_1;
      }else {
        *(uint8_t *)(video_mem /* + (1 + curr_term) * FOUR_KB */+ (i << 1) + 1) = ATTRIB_2;
      }
    }
    else{
      *(uint8_t *)(video_mem + (1 + curr_term) * PAGE_SIZE + (i << 1)) = c;

      if (curr_term == 0) {
        *(uint8_t *)(video_mem + (1 + curr_term) * PAGE_SIZE + (i << 1) + 1) = ATTRIB_0;
      } else if (curr_term ==1){
        *(uint8_t *)(video_mem + (1 + curr_term) * PAGE_SIZE + (i << 1) + 1) = ATTRIB_1;
      }else {
        *(uint8_t *)(video_mem + (1 + curr_term) * PAGE_SIZE + (i << 1) + 1) = ATTRIB_2;
      }
    }


    //increase the cursor
    terminal[curr_term].pos_x++;
    terminal[curr_term].pos_y = (terminal[curr_term].pos_y+ (terminal[curr_term].pos_x / NUM_COLS));
    terminal[curr_term].pos_x %= NUM_COLS;
  }
  // fix position
  while (terminal[curr_term].pos_y >= NUM_ROWS) {
    scroll_line();
    terminal[curr_term].pos_y--;
  }
  if (curr_display_term== curr_term) {
    set_cursor(terminal[curr_term].pos_x,terminal[curr_term].pos_y);
  }

}

/*
* keyboard_putc
* input:  character to be printed
* description: the print function for keyboard input
* return value: none
* side effect : print c to display terminal, correct position if needed
*/
void
keyboard_putc(uint8_t c)
{
  //init_color();
  int i = (NUM_COLS*(*cursor_y) + (*cursor_x));

  // keyboard always print to physical location
  *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1)) = c;
  if (curr_display_term == 0) {
    *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_0;
  } else if (curr_display_term ==1){
    *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_1;
  }else {
    *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_2;
  }


}

void init_mouse_start_position(int x, int y){
  int i = NUM_COLS * y + x;
  uint8_t old_color = *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1);
  if(old_color <<4 != 0)
  *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = old_color <<4;
  //printf("NEW COLOR %x,%x\n",old_color <<4 );

}


void draw_mouse_cursor(int x,int y,int last_x, int last_y){
  //printf("x & y changed values: %d, %d\n",x,y );
  int i;
  uint8_t old_color;
//  if()
  if (x!=last_x || y!= last_y) {
    i = NUM_COLS * last_y + last_x;
    old_color = *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1);
    *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = old_color >>4;
    i = NUM_COLS*y + x;
    old_color = *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1);
    *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = old_color <<4;
  }




  //*(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1)) = (uint8_t)219;

  // *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_3;
  // if (curr_display_term == 0) {
  //   *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_0;
  // } else if (curr_display_term ==1){
  //   *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_1;
  // }else {
  //   *(uint8_t *)(PERMANENT_PHYS_ADDR + (i << 1) + 1) = ATTRIB_2;
  // }

}


/* void set_cursor
* input: position x and y to set the cursor
* return value: none
* function: set the cursor at the desired position
* the function handles edge cases where x.y positions are illegal
*/

void set_cursor(int32_t x, int32_t y){
  // set the cursor position

  unsigned short position = NUM_COLS* y + x; //offset
  // cursor LOW port to vga INDEX register
  outb(FOUR_BIT_MASK,LOW_PORT);
  outb((unsigned char)(position & EIGHT_BIT_MASK),HIGH_PORT);
  // cursor HIGH port to vga INDEX register
  outb(FOUR_BIT_MASK - 1,LOW_PORT);
  outb((unsigned char )((position >> ONE_BYTE) & EIGHT_BIT_MASK),HIGH_PORT);
}

/* void correct_cursor
* input: none
* return value: none
* function: correct cursor position if they're out of bound
* the function handles edge cases where x.y positions are illegal
*/

void correct_cursor(){

  // fix cursor position from x--,x++, and y++
  if (*cursor_x < 0) {
    if(*cursor_y ==0) { *cursor_x = 0; }
    else {
      (*cursor_y)--;
      *cursor_x = NUM_COLS-1;
    }
  }
  else if( *cursor_x == NUM_COLS) {
    if((*cursor_y) ==NUM_ROWS-1) {
      keyboard_scroll_line();
    }
    else {(*cursor_y)++;}
    (*cursor_x) = 0;
  }
  while (*cursor_y >= NUM_ROWS) {
    keyboard_scroll_line();
    (*cursor_y)--;
  }
  set_cursor((*cursor_x),(*cursor_y));
}
/* Standard printf().
* Only supports the following format strings:
* %%  - print a literal '%' character
* %x  - print a number in hexadecimal
* %u  - print a number as an unsigned integer
* %d  - print a number as a signed integer
* %c  - print a character
* %s  - print a string
* %#x - print a number in 32-bit aligned hexadecimal, i.e.
*       print 8 hexadecimal digits, zero-padded on the left.
*       For example, the hex number "E" would be printed as
*       "0000000E".
*       Note: This is slightly different than the libc specification
*       for the "#" modifier (this implementation doesn't add a "0x" at
*       the beginning), but I think it's more flexible this way.
*       Also note: %x is the only conversion specifier that can use
*       the "#" modifier to alter output.
* */
int32_t
printf(int8_t *format, ...)
{
  /* Pointer to the format string */
  int8_t* buf = format;

  /* Stack pointer for the other parameters */
  int32_t* esp = (void *)&format;
  esp++;

  while(*buf != '\0') {
    switch(*buf) {
      case '%':
      {
        int32_t alternate = 0;
        buf++;

        format_char_switch:
        /* Conversion specifiers */
        switch(*buf) {
          /* Print a literal '%' character */
          case '%':
          putc('%');
          break;

          /* Use alternate formatting */
          case '#':
          alternate = 1;
          buf++;
          /* Yes, I know gotos are bad.  This is the
          * most elegant and general way to do this,
          * IMHO. */
          goto format_char_switch;

          /* Print a number in hexadecimal form */
          case 'x':
          {
            int8_t conv_buf[64];
            if(alternate == 0) {
              itoa(*((uint32_t *)esp), conv_buf, 16);
              puts(conv_buf);
            } else {
              int32_t starting_index;
              int32_t i;
              itoa(*((uint32_t *)esp), &conv_buf[8], 16);
              i = starting_index = strlen(&conv_buf[8]);
              while(i < 8) {
                conv_buf[i] = '0';
                i++;
              }
              puts(&conv_buf[starting_index]);
            }
            esp++;
          }
          break;

          /* Print a number in unsigned int form */
          case 'u':
          {
            int8_t conv_buf[36];
            itoa(*((uint32_t *)esp), conv_buf, 10);
            puts(conv_buf);
            esp++;
          }
          break;

          /* Print a number in signed int form */
          case 'd':
          {
            int8_t conv_buf[36];
            int32_t value = *((int32_t *)esp);
            if(value < 0) {
              conv_buf[0] = '-';
              itoa(-value, &conv_buf[1], 10);
            } else {
              itoa(value, conv_buf, 10);
            }
            puts(conv_buf);
            esp++;
          }
          break;

          /* Print a single character */
          case 'c':
          putc( (uint8_t) *((int32_t *)esp) );
          esp++;
          break;

          /* Print a NULL-terminated string */
          case 's':
          puts( *((int8_t **)esp) );
          esp++;
          break;

          default:
          break;
        }

      }
      break;

      default:
      putc(*buf);
      break;
    }
    buf++;
  }

  return (buf - format);
}

/*
* int32_t puts(int8_t* s);
*   Inputs: int_8* s = pointer to a string of characters
*   Return Value: Number of bytes written
*	Function: Output a string to the console
*/

int32_t
puts(int8_t* s)
{
  register int32_t index = 0;
  while(s[index] != '\0') {
    putc(s[index]);
    index++;
  }

  return index;
}

/*
* void putc(uint8_t c);
*   Inputs: uint_8* c = character to print
*   Return Value: void
*	Function: Output a character to the console, CALL terminal_write
*/

void
putc(uint8_t c)
{
  // int i = (NUM_COLS*(*cursor_y) + (*cursor_x));
  // if(c == '\n' || c == '\r') {
  //   (*cursor_y)++;
  //   (*cursor_x)=0;
  // } else {
  //   //load_page_directory(terminal[curr_display_term].curr_process->pid + 1);
  //   *(uint8_t *)(video_mem + (i << 1)) = c;
  //   if (curr_display_term == 0) {
  //     *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB_0;
  //   } else if (curr_display_term ==1){
  //     *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB_1;
  //   }else {
  //     *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB_2;
  //   }
  //   //load_page_directory(terminal[curr_term].curr_process->pid + 1);
  //   (*cursor_x)++;
  //   (*cursor_y) = ((*cursor_y) + ((*cursor_x) / NUM_COLS));
  //   (*cursor_x) %= NUM_COLS;
  // }
  // // if ((*cursor_y) == NUM_ROWS) {
  // //   scroll_line();
  // //   (*cursor_y)--;
  // // }
  // set_cursor((*cursor_x),(*cursor_y));
  terminal_putc(c);
}

/*
* int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix);
*   Inputs: uint32_t value = number to convert
*			int8_t* buf = allocated buffer to place string in
*			int32_t radix = base system. hex, oct, dec, etc.
*   Return Value: number of bytes written
*	Function: Convert a number to its ASCII representation, with base "radix"
*/

int8_t*
itoa(uint32_t value, int8_t* buf, int32_t radix)
{
  static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  int8_t *newbuf = buf;
  int32_t i;
  uint32_t newval = value;

  /* Special case for zero */
  if(value == 0) {
    buf[0]='0';
    buf[1]='\0';
    return buf;
  }

  /* Go through the number one place value at a time, and add the
  * correct digit to "newbuf".  We actually add characters to the
  * ASCII string from lowest place value to highest, which is the
  * opposite of how the number should be printed.  We'll reverse the
  * characters later. */
  while(newval > 0) {
    i = newval % radix;
    *newbuf = lookup[i];
    newbuf++;
    newval /= radix;
  }

  /* Add a terminating NULL */
  *newbuf = '\0';

  /* Reverse the string and return */
  return strrev(buf);
}

/*
* int8_t* strrev(int8_t* s);
*   Inputs: int8_t* s = string to reverse
*   Return Value: reversed string
*	Function: reverses a string s
*/

int8_t*
strrev(int8_t* s)
{
  register int8_t tmp;
  register int32_t beg=0;
  register int32_t end=strlen(s) - 1;

  while(beg < end) {
    tmp = s[end];
    s[end] = s[beg];
    s[beg] = tmp;
    beg++;
    end--;
  }

  return s;
}

/*
* uint32_t strlen(const int8_t* s);
*   Inputs: const int8_t* s = string to take length of
*   Return Value: length of string s
*	Function: return length of string s
*/

uint32_t
strlen(const int8_t* s)
{
  register uint32_t len = 0;
  while(s[len] != '\0')
  len++;

  return len;
}

/*
* void* memset(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set n consecutive bytes of pointer s to value c
*/

void*
memset(void* s, int32_t c, uint32_t n)
{
  c &= 0xFF;
  asm volatile("                  \n\
  .memset_top:            \n\
  testl   %%ecx, %%ecx    \n\
  jz      .memset_done    \n\
  testl   $0x3, %%edi     \n\
  jz      .memset_aligned \n\
  movb    %%al, (%%edi)   \n\
  addl    $1, %%edi       \n\
  subl    $1, %%ecx       \n\
  jmp     .memset_top     \n\
  .memset_aligned:        \n\
  movw    %%ds, %%dx      \n\
  movw    %%dx, %%es      \n\
  movl    %%ecx, %%edx    \n\
  shrl    $2, %%ecx       \n\
  andl    $0x3, %%edx     \n\
  cld                     \n\
  rep     stosl           \n\
  .memset_bottom:         \n\
  testl   %%edx, %%edx    \n\
  jz      .memset_done    \n\
  movb    %%al, (%%edi)   \n\
  addl    $1, %%edi       \n\
  subl    $1, %%edx       \n\
  jmp     .memset_bottom  \n\
  .memset_done:           \n\
  "
  :
  : "a"(c << 24 | c << 16 | c << 8 | c), "D"(s), "c"(n)
  : "edx", "memory", "cc"
);

return s;
}

/*
* void* memset_word(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set lower 16 bits of n consecutive memory locations of pointer s to value c
*/

/* Optimized memset_word */
void*
memset_word(void* s, int32_t c, uint32_t n)
{
  asm volatile("                  \n\
  movw    %%ds, %%dx      \n\
  movw    %%dx, %%es      \n\
  cld                     \n\
  rep     stosw           \n\
  "
  :
  : "a"(c), "D"(s), "c"(n)
  : "edx", "memory", "cc"
);

return s;
}

/*
* void* memset_dword(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set n consecutive memory locations of pointer s to value c
*/

void*
memset_dword(void* s, int32_t c, uint32_t n)
{
  asm volatile("                  \n\
  movw    %%ds, %%dx      \n\
  movw    %%dx, %%es      \n\
  cld                     \n\
  rep     stosl           \n\
  "
  :
  : "a"(c), "D"(s), "c"(n)
  : "edx", "memory", "cc"
);

return s;
}

/*
* void* memcpy(void* dest, const void* src, uint32_t n);
*   Inputs: void* dest = destination of copy
*			const void* src = source of copy
*			uint32_t n = number of byets to copy
*   Return Value: pointer to dest
*	Function: copy n bytes of src to dest
*/

void*
memcpy(void* dest, const void* src, uint32_t n)
{
  asm volatile("                  \n\
  .memcpy_top:            \n\
  testl   %%ecx, %%ecx    \n\
  jz      .memcpy_done    \n\
  testl   $0x3, %%edi     \n\
  jz      .memcpy_aligned \n\
  movb    (%%esi), %%al   \n\
  movb    %%al, (%%edi)   \n\
  addl    $1, %%edi       \n\
  addl    $1, %%esi       \n\
  subl    $1, %%ecx       \n\
  jmp     .memcpy_top     \n\
  .memcpy_aligned:        \n\
  movw    %%ds, %%dx      \n\
  movw    %%dx, %%es      \n\
  movl    %%ecx, %%edx    \n\
  shrl    $2, %%ecx       \n\
  andl    $0x3, %%edx     \n\
  cld                     \n\
  rep     movsl           \n\
  .memcpy_bottom:         \n\
  testl   %%edx, %%edx    \n\
  jz      .memcpy_done    \n\
  movb    (%%esi), %%al   \n\
  movb    %%al, (%%edi)   \n\
  addl    $1, %%edi       \n\
  addl    $1, %%esi       \n\
  subl    $1, %%edx       \n\
  jmp     .memcpy_bottom  \n\
  .memcpy_done:           \n\
  "
  :
  : "S"(src), "D"(dest), "c"(n)
  : "eax", "edx", "memory", "cc"
);

return dest;
}

/*
* void* memmove(void* dest, const void* src, uint32_t n);
*   Inputs: void* dest = destination of move
*			const void* src = source of move
*			uint32_t n = number of byets to move
*   Return Value: pointer to dest
*	Function: move n bytes of src to dest
*/

/* Optimized memmove (used for overlapping memory areas) */
void*
memmove(void* dest, const void* src, uint32_t n)
{
  asm volatile("                  \n\
  movw    %%ds, %%dx      \n\
  movw    %%dx, %%es      \n\
  cld                     \n\
  cmp     %%edi, %%esi    \n\
  jae     .memmove_go     \n\
  leal    -1(%%esi, %%ecx), %%esi    \n\
  leal    -1(%%edi, %%ecx), %%edi    \n\
  std                     \n\
  .memmove_go:            \n\
  rep     movsb           \n\
  "
  :
  : "D"(dest), "S"(src), "c"(n)
  : "edx", "memory", "cc"
);

return dest;
}

/*
* int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
*   Inputs: const int8_t* s1 = first string to compare
*			const int8_t* s2 = second string to compare
*			uint32_t n = number of bytes to compare
*	Return Value: A zero value indicates that the characters compared
*					in both strings form the same string.
*				A value greater than zero indicates that the first
*					character that does not match has a greater value
*					in str1 than in str2; And a value less than zero
*					indicates the opposite.
*	Function: compares string 1 and string 2 for equality
*/

int32_t
strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
{
  int32_t i;
  for(i=0; i<n; i++) {
    if( (s1[i] != s2[i]) ||
    (s1[i] == '\0') /* || s2[i] == '\0' */ ) {

      /* The s2[i] == '\0' is unnecessary because of the short-circuit
      * semantics of 'if' expressions in C.  If the first expression
      * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
      * s2[i], then we only need to test either s1[i] or s2[i] for
      * '\0', since we know they are equal. */

      return s1[i] - s2[i];
    }
  }
  return 0;
}

/*
* int8_t* strcpy(int8_t* dest, const int8_t* src)
*   Inputs: int8_t* dest = destination string of copy
*			const int8_t* src = source string of copy
*   Return Value: pointer to dest
*	Function: copy the source string into the destination string
*/

int8_t*
strcpy(int8_t* dest, const int8_t* src)
{
  int32_t i=0;
  while(src[i] != '\0') {
    dest[i] = src[i];
    i++;
  }

  dest[i] = '\0';
  return dest;
}

/*
* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
*   Inputs: int8_t* dest = destination string of copy
*			const int8_t* src = source string of copy
*			uint32_t n = number of bytes to copy
*   Return Value: pointer to dest
*	Function: copy n bytes of the source string into the destination string
*/

int8_t*
strncpy(int8_t* dest, const int8_t* src, uint32_t n)
{
  int32_t i=0;
  while(src[i] != '\0' && i < n) {
    dest[i] = src[i];
    i++;
  }

  while(i < n) {
    dest[i] = '\0';
    i++;
  }

  return dest;
}

/*
* int8_t* strcpy_uint(int8_t* dest, const int8_t* src, uint32_t n)
*   Inputs: int8_t* dest = destination string of copy
*			const int8_t* src = source string of copy
*			uint32_t n = number of bytes to copy
*   Return Value: pointer to dest
*	Function: copy n bytes of the source string into the destination string
*/

uint8_t*
strncpy_uint(uint8_t* dest, const uint8_t* src, uint32_t n)
{
  int32_t i=0;
  while(src[i] != '\0' && i < n) {
    dest[i] = src[i];
    i++;
  }

  while(i < n) {
    dest[i] = '\0';
    i++;
  }

  return dest;
}


/*
* void test_interrupts(void)
*   Inputs: void
*   Return Value: void
*	Function: increments video memory. To be used to test rtc
*/

void
test_interrupts(void)
{
  int32_t i;
  for (i=0; i < NUM_ROWS*NUM_COLS; i++) {
    video_mem[i<<1]++;
  }
}

uint32_t max(uint32_t a, uint32_t b) { return a >= b ? a : b;}

uint32_t min(uint32_t a, uint32_t b) { return a < b ? a : b; }

// int get_screenx() { return (*cursor_x); }
// int get_screeny() { return (*cursor_y); }
