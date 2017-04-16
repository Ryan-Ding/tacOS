#include "sys_call.h"

boot_block_t* boot_block_ptr;
pcb_t* curr_process;
uint32_t kernel_stack_top;

//extern pcb_t* curr_process;
void init_sys_call(){
	boot_block_ptr = get_boot_block_info();
	process_bitmap = 0;
	kernel_stack_top = KERNEL_END_ADDR;
	curr_process = NULL;
}

void program_loader(const uint8_t* filename){
	dentry_t search_for_dir_entry;
	if(read_dentry_by_name(filename, &search_for_dir_entry) == -1) {return;}
	uint32_t inode = search_for_dir_entry.inode_num;
	inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));
    uint32_t length = inode_ptr->length;
    read_data(inode,0,(uint8_t*)PROGRAM_IMAGE_ADDR,length);
}

int32_t check_if_executable(dentry_t* search_for_dir_entry){
	uint8_t buf[4];
	if(read_data(search_for_dir_entry->inode_num, 0, buf, 4 )!=4)
		return -1;
	// read_data(search_for_dir_entry->inode_num, 0, buf, 4 );

	if (buf[0]==MAGIC_NUM_1 && buf[1]==MAGIC_NUM_2 && buf[2]== MAGIC_NUM_3 && buf[3] == MAGIC_NUM_4)
		return 0;

	return -1;
}

uint32_t get_first_instruction(dentry_t* dir_entry){
	uint8_t buf[4];
	read_data(dir_entry->inode_num, EXECUTABLE_STARTING_ADDR, buf, 4);
	return ((uint32_t)((buf[3]<<THREE_BYTES_SHIFT) | (buf[2]<<TWO_BYTES_SHIFT) | (buf[1]<<ONE_BYTE_SHIFT) | buf[0]));
}
                                          
int32_t system_halt (uint8_t status)
{
	printf("system halt \n");
	uint32_t i;
	pcb_t * parent_pcb = curr_process->parent;
    if (parent_pcb==NULL) { // no task running any more, terminating shell
        return 0;//restart the shell
    }
	i = curr_process->process_number;
  	//  mark the current process as not running
	process_bitmap &= (0 << i);
	curr_process->parent = NULL;

	// restore cr3
	asm volatile (
		"pushl %%eax;"
		"movl %0, %%eax;"
		"movl %%eax, %%cr3;"
		"popl %%eax;"
		:
		: "r"(curr_process->old_cr3)
		: "%eax", "cc"
	);

    uint32_t ret_status = status;
	// should be parent's
	i = curr_process->old_esp;
    //restore esp and ebp
    asm volatile("movl %0, %%esp	;"
                 "pushl %1	        ;"
                 ::"g"(i),"g"(ret_status));

	i = curr_process->old_ebp;
    asm volatile("movl %0, %%ebp"::"g"(i));

	// update curr_process as parent process
	/*if (parent_pcb!= NULL) {
		j = parent_pcb->process_number;
		curr_process = curr_process->parent;
	}else {
		j = 0;
	}*/

    asm volatile("popl %eax");
    asm volatile("jmp return_from_halt");
    // asm volatile("leave");
    // asm volatile("ret");
	return 0;
}

int32_t system_execute (const uint8_t* command)
{
	printf("system execute \n");
	int32_t new_pid;
	pcb_t new_pcb;
	uint32_t old_ebp = 0, old_esp = 0, old_cr3=0;
	/* Parse Command Arguments */

		int i=0;
		int j=0;
		if(!command)
		{
			printf("command must be valid\n");
		}
		//printf("%s\n",strchr(command,' '));

		uint8_t file_name[128] = {0};
		uint8_t arguments[128] = {0};

		while(command[i]!=' ' && command[i]!='\0')
		{
			file_name[i] = command[i];
			i++;
		}

		file_name[i]='\0';

		i++;
		while(command[i]!='\0')
		{
			arguments[j] = command[i];
			i++;
			j++;
		}
		arguments[i]='\0';

	/* Check for executable */
	dentry_t search_for_dir_entry;
    //printf("The size of inode is: %d\n",sizeof(dentry_t));

	//check file validity
    if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1){
        return -1;
    }

	   if (check_if_executable(&search_for_dir_entry)!=0){
		printf("Not executable!\n");
		return -1;
	}

 	/* Set up new paging and load program image */
	// fetch pid from PCB
	if ( (new_pid = find_available_pid()) < 0 ) {
		return -1;
	}

	//get first instruction
	uint32_t new_instruction=get_first_instruction(&search_for_dir_entry);

	/*creat new pcb*/
	asm volatile (
		"movl %%ebp, %0;"
		"movl %%esp, %1;"
		: "=g"(old_ebp), "=g"(old_esp)
	);

	asm volatile (
		"movl %%cr3, %0;"
		: "=r"(old_cr3)
	);


	// printf("old_ebp: %u \n old_esp: %u \n",old_ebp,old_esp);
	// printf("new_pid is %u \n",new_pid);
	/* Create PCB and open file descriptors */
	new_pcb.pid = new_pid;
	new_pcb.old_ebp = old_ebp;
	new_pcb.old_esp = old_esp;
	new_pcb.old_cr3 = old_cr3;
	if (new_pid != PID_PD_OFFSET) {
		new_pcb.parent = (pcb_t*) curr_process;
	} else {
		new_pcb.parent = NULL;
	}
	paging_init(new_pid + 1);
	program_loader(file_name);

	memcpy((void*) (kernel_stack_top - KERNEL_STACK_ENTRY_SIZE), (void*) &new_pcb, sizeof(typeof(pcb_t)));
	curr_process = (pcb_t*) (kernel_stack_top - KERNEL_STACK_ENTRY_SIZE);
	kernel_stack_top -= KERNEL_STACK_ENTRY_SIZE;


	/* Prepare for Context Switch; tss.esp0/ebp */
	tss.ss0 = KERNEL_DS;
	tss.esp0 = kernel_stack_top-4;

	/* Prepare for fake iret */

	// uint32_t new_cr3=get_cr3(new_pid);
	// asm volatile("movl %0, %%cr3\n\t": : "r"(new_cr3):"cc");
    //http://www.jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html
    //uint32_t ret_val;

    cli();
	asm volatile(
	"pushl %%eax;"
	 "mov %0, %%ax;"
     "mov %%ax, %%ds;"
     "popl %%eax;"

     //push ss
     "pushl %0;"
     //push esp
     "pushl %1;"
     //push eflags
     "pushfl;"
     // "pushl %%eax;"
     //push cs
     "pushl %2;"
     //push eip
     "pushl %3;"
     // "wtf: "
     // "jmp wtf;"
     "iret;"
     :
     : "g"(USER_DS), "g"(0x08400000-4), "g"(USER_CS), "g"(new_instruction)
     : "cc","memory"
     );

  //   i = 0;
  //   asm volatile(
	 // // "mov %0, %%ax;"
  // //    "mov %%ax, %%ds;"

  //    //push ss
  //    "pushl %0;"
  //    :
  //    :"r"(USER_DS)
  //    : "cc","memory"
  //   );
  //   //printf("Checkpoint %d \n",++i);
  //    //push esp
  //   asm volatile (

  //    "pushl %0;"
  //    "pushl $0x202;"
  //    :
  //    :"r"(0x08400000-4)
  //    : "cc","memory"
  //    );
  //   //printf("Checkpoint %d \n",++i);
  //    //push eflags

  //    // "pushl %%eax;"
  //    //push cs
  //   asm volatile (
  //    "pushl %0;"
  //    :
  //    :"r"(USER_CS)
  //    : "cc","memory"
  //    );
  //   //printf("Checkpoint %d \n",++i);
  //    //push eip
  //   asm volatile (
  //    "pushl %0;"
  //    :
  //    :"r"(new_instruction)
  //    : "cc","memory"
  //    );
  //   //printf("Checkpoint %d \n",++i);
  //    // "wtf: "
  //    // "jmp wtf;"
  //   printf("buffer is  %x \n",new_instruction);

  //    asm volatile(
  //    "iret;"
  //    :
  //    :
  //    : "cc","memory"

  //    );

  sti();
  asm volatile (
	"return_from_halt: "
	"leave;"
	"ret"
  );

	return 0;
}



int32_t system_read (int32_t fd, void* buf, int32_t nbytes)
{
	// printf("system read \n");
	if(fd<FD_MIN || fd>FD_MAX)	//check range
		return -1;
	
	if (fd == FD_STDIN) {
		terminal_read(fd, buf);
	}

	if(fd == FD_STDOUT)	//can't read from stdout
		return -1;
	
	return (curr_process->file_desc_table[fd].file_ops_table_ptr->read)(fd, buf, nbytes);		//to be replaced by pcb
}

int32_t system_write (int32_t fd, const void* buf, int32_t nbytes)
{
	printf("system write \n");
	if(fd<FD_MIN || fd>FD_MAX)	//check range
		return -1;

	if(fd == FD_STDIN)	//can't read from stdin
		return -1;

	if (fd == FD_STDOUT){
		terminal_write(buf, nbytes);
		return 0;
	}
	
	if(curr_process->file_desc_table[fd].flag == 0)	//can't write if it's not open yet
		return -1;

	return (curr_process->file_desc_table[fd].file_ops_table_ptr->write)(fd, buf, nbytes); //to be replaced by pcb
}

int32_t system_open (const uint8_t* filename)
{
	printf("system open\n");
	if(!filename)
		return -1;

	if(filename == (uint8_t*)"stdin")
		return 0;
	if(filename == (uint8_t*)"stdout")
		return 1;

	return fs_open(filename);

}
