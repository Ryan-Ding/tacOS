#include "scheduler.h"


void switch_task(){

  int32_t i,esp,ebp;
    // the following store esp and ebp information so it can be used in the future
  asm volatile ("movl %%esp,%0":"=r"(esp));
  asm volatile ("movl %%ebp,%0":"=r"(ebp));

  pcb_t* next_process = NULL;
  int tmp_term = curr_term;
  if (curr_process == NULL) {
    return;
  }
  curr_process->ebp = ebp;
  curr_process->esp = esp;

// get the next process
for (i = 0; i < TERM_NUM; i++) {
  tmp_term = (tmp_term+1)%3;
  if(terminal[tmp_term].curr_process != NULL && terminal[tmp_term].curr_process->parent != NULL ){
    next_process = terminal[tmp_term].curr_process;
   break;
  }
}

  if (next_process == NULL) return;
  esp = next_process->esp;
  ebp = next_process->ebp;

  if (ebp==0 || esp ==0) {
    return;
  }

curr_term = tmp_term;

  // not sure for this tss.esp0 =
  tss.esp0 = next_process->curr_esp0;
 tss.ebp = next_process->ebp;
  // switch to the page of next process
  // TO DO
 load_page_directory(next_process->pid+1);
  asm volatile("movl %0, %%esp;"
               "movl %1, %%ebp;"
               "leave;"
               "ret"
               :
               :"r"(esp),"r"(ebp)
               : "memory","cc","ebp","esp"
  );
}
