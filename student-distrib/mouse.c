// mouse.c this file handles interrupt from mouse
#include "mouse.h"

uint8_t cycle = 0;
uint32_t mouse_packet[3] = {0,0,0};

int32_t mouse_x[3] = {0,0,0};
int32_t mouse_y[3] = {0,0,0};
int32_t delta_x = 0;
int32_t delta_y = 0;
int32_t last_x = 0;
int32_t last_y = 0;
volatile uint8_t first_start_flag[3]  = {1,1,1};

void mouse_wait(uint8_t type ){
  uint32_t time = 100000; // time out length
  if(type==0)
  {
    while( time--) //Data
    {
      if((inb(MOUSE_STATUS_PORT) & 1)==1){break;}
    }
  }
  else
  {
    while( time--) //Signal
    {
      if((inb(MOUSE_STATUS_PORT) & 2)==0){break;}
    }
  }
  return;
}

uint8_t mouse_read(void){
  mouse_wait(0);
  return inb(MOUSE_SCANCODE_PORT);

}

void mouse_write(uint8_t word){
  mouse_wait(1);
  outb(0xD4,MOUSE_STATUS_PORT); //Preced before sending data/command
  mouse_wait(1);
  outb(word,MOUSE_SCANCODE_PORT);
}

void mouse_init(void){
  uint8_t status;
  cli();

  set_intr_gate(MOUSE_IRQ + SLAVE_IDT_OFFSET - MASTER_SIZE, mouse_interrupt_handler);
  mouse_wait(1);
  outb(0xA8,MOUSE_STATUS_PORT);//enable aux mouse device
  mouse_wait(1);
  //enable interrupt
  outb(0x20,MOUSE_STATUS_PORT);
  mouse_wait(0);
  status = inb(MOUSE_SCANCODE_PORT) |2;
  mouse_wait(1);
  outb(0x60,MOUSE_STATUS_PORT);
  mouse_wait(1);
  outb(status,MOUSE_SCANCODE_PORT);

  mouse_write(DEFAULT);
  mouse_read();

  mouse_write(ENABLE_DATA);
  mouse_read();

  //cycle = 0;
  //first_start_flag = 1;
  enable_irq(MOUSE_IRQ);
  sti();


}

// real keyboard interrupt handler
void mouse_interrupt(void){
  cli();
  uint8_t status = inb(MOUSE_STATUS_PORT);
  if (!(status & 0x01)) {
    sti();
  send_eoi(MOUSE_IRQ);
  return;
}
  switch (cycle) {
    case 0:
    mouse_packet[0] = inb(MOUSE_SCANCODE_PORT);
    cycle = 1;
    break;
    case 1:
    mouse_packet[1] = inb(MOUSE_SCANCODE_PORT);


    cycle = 2;
    case 2:
    mouse_packet[2] = inb(MOUSE_SCANCODE_PORT);
    if (mouse_packet[0] & YO_BIT || mouse_packet[0] & XO_BIT) {
						break;
		}
    delta_x = mouse_packet[1];
    delta_y = mouse_packet[2];
    if((mouse_packet[0] >> 4) & 0x01) {delta_x |= SIGN_EXT;} // get the fifth bit
    if((mouse_packet[0] >> 5) & 0x01) {delta_y |= SIGN_EXT;} // get the sixth bit
    //cycle =(cycle+1) % 3;
    if(delta_x!=0 || delta_y!=0){
      last_x = mouse_x[curr_display_term];
      last_y = mouse_y[curr_display_term];
      mouse_x[curr_display_term] += (int32_t)(delta_x/X_SCALE * NUM_COLS /MOUSE_SCALE);
      mouse_y[curr_display_term] -= (int32_t)(delta_y/Y_SCALE * NUM_ROWS /MOUSE_SCALE);
    //
      if(mouse_x[curr_display_term] < 0) mouse_x[curr_display_term] = 0;
      else if (mouse_x[curr_display_term] > NUM_COLS-1) mouse_x[curr_display_term] = NUM_COLS-1;
      if(mouse_y[curr_display_term] <0 )mouse_y[curr_display_term] = 0;
      else if (mouse_y[curr_display_term] > NUM_ROWS-1) mouse_y[curr_display_term] = NUM_ROWS-1;
      if (first_start_flag[curr_display_term])
      {
        first_start_flag[curr_display_term] = 0;
        init_mouse_start_position(mouse_x[curr_display_term],mouse_y[curr_display_term]);
      }
      draw_mouse_cursor(mouse_x[curr_display_term], mouse_y[curr_display_term], last_x, last_y);
    }
    cycle = 0;
    break;
  }
  send_eoi(MOUSE_IRQ);
  sti();


}
