#include "scheduler.h"


/*
 * switch_task
 * input: none
 * description: round robin scheduling
 * return value:none
 * side effect : return next process
 */
void switch_task() {
  int32_t esp,ebp;
  // the following store esp and ebp information so it can be used in the future
  asm volatile ("movl %%esp, %0":"=r"(esp));
  asm volatile ("movl %%ebp, %0":"=r"(ebp));
  //esp + = 8; // TODO should we consider the local vars?
  int32_t i;
  pcb_t* next_process = NULL;
  int tmp_term = curr_term;
  if (curr_process == NULL) { return; }

  // store curr ebp esp of the process in the pit handler


  if (curr_process->is_blocked_by_new_terminal > 0) {
        curr_process->ebp = ebp;
        curr_process->esp = esp;
        uint8_t shell_program[] = "shell";
        curr_process->is_blocked_by_new_terminal = 0;
        curr_term = curr_display_term;
        init_color();
        curr_process = NULL;
        system_execute(shell_program);
        return; // TODO wether we need this or not
  }



  // get the next process
  for (i = 0; i < TERM_NUM; i++) {
    tmp_term = (tmp_term + 1) % 3;
    if(terminal[tmp_term].curr_process != NULL ) {
      if (tmp_term != curr_term) {
         next_process = terminal[tmp_term].curr_process;
      } else {
        next_process = (pcb_t*)curr_process;
      }
      break;
    }
  }


  if (curr_process->is_blocked_by_new_terminal == 0){
  // if ((wait_to_read[tmp_term] == 1) && (curr_display_term == curr_term)) {return;}
  }


  curr_process->ebp = ebp;
  curr_process->esp = esp;


  if (next_process == NULL) { return; }
  esp = next_process->esp;
  ebp = next_process->ebp;

  if (ebp == 0 || esp == 0) { return; } // remove for debug purpose

  curr_term = tmp_term;

  // if (next_process->is_blocked_by_new_terminal == 1) {
  //   esp = next_process->fake_esp;
  //   ebp = next_process->fake_ebp;
  //   next_process->is_blocked_by_new_terminal = 0;
  // }


  // restore tss
  tss.esp0 = next_process->curr_esp0;
  tss.ebp = ebp;
  // switch to the page of next process
  // TO DO
  curr_process = next_process;
  load_page_directory(next_process->pid + 1);

  asm volatile("movl %0, %%esp;"
               "movl %1, %%ebp;"
               :
               :"r"(esp),"r"(ebp)
               : "memory","cc","ebp","esp"
  );

  asm("leave;");
  asm("ret;");

}
