#include "sys_call.h"

boot_block_t* boot_block_ptr;
pcb_t* curr_process;
uint32_t kernel_stack_top;

/*
 * init_sys_call
 * input: none
 * description: prepare for system call
 * return value: none
 * side effect : get boot block pointer and initialize process bitmap
 */

void init_sys_call(){
	//get boot block pointer and initialize process bitmap
	boot_block_ptr = get_boot_block_info();
	process_bitmap = 0;
	//initialize kernel stack pointer variable
	kernel_stack_top = KERNEL_END_ADDR;
	curr_process = NULL;

}

/*
 * program_loader
 * input: filename - pointer to the filename of file to be open
 * description: load the file pointed by the file pointer
 * return value: none
 * side effect : copy disk blocks to contiguous physical memory
 */

void program_loader(const uint8_t* filename){
	//get directory entry of the file
	dentry_t search_for_dir_entry;
	if(read_dentry_by_name(filename, &search_for_dir_entry) == -1) {return;}

	//initialize inode block pointer
	uint32_t inode = search_for_dir_entry.inode_num;
	inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));

	//load the file into virtual address
    uint32_t length = inode_ptr->length;
    read_data(inode,0,(uint8_t*)PROGRAM_IMAGE_ADDR,length);
}

/*
 * check_if_executable
 * input: dir_entry - pointer to the dentry to be checked
 * description:check if the input dentry is executable or not
 * return value: 0 if yes. -1 if not
 * side effect : none
 */
int32_t check_if_executable(dentry_t* search_for_dir_entry){
	uint8_t buf[4];
	if(read_data(search_for_dir_entry->inode_num, 0, buf, 4 )!=4)
		return -1;
	// read_data(search_for_dir_entry->inode_num, 0, buf, 4 );

	//check if the file executable bytes match elf requirement
	if (buf[0]==CHECK_ELF_1 && buf[1]==CHECK_ELF_2 && buf[2]== CHECK_ELF_3 && buf[3] == CHECK_ELF_4)
		return 0;

	return -1;
}
/*
 * get first instruction
 * input: dir_entry - pointer to the dentry to be inferenced
 * description:get the first instruction in the dentry
 * return value: the first executable in the given dentry
 * side effect : none
 */

uint32_t get_first_instruction(dentry_t* dir_entry){
	uint8_t buf[4];
	//get the starting point instruction from executable
	read_data(dir_entry->inode_num, EXECUTABLE_STARTING_ADDR, buf, 4);

	//comply to little endian rule
	return ((uint32_t)((buf[3]<<THREE_BYTES_SHIFT) | (buf[2]<<TWO_BYTES_SHIFT) | (buf[1]<<ONE_BYTE_SHIFT) | buf[0]));
}

/*
 * system_halt
 * input: status - status that is returned from previous progress
 * description: halt the current process
 * return value: return value from previous process
 * side effect : clear related value for the process to be halted, mark the process as not running, return to parent stack
 */

int32_t system_halt (uint8_t status)
{
	//printf("system halt");
	uint32_t i;
	pcb_t * parent_pcb = curr_process->parent;
    // if (parent_pcb==NULL) { // no task running any more, terminating shell
    //     return 0;//restart the shell
    // }
	i = curr_process->pid;
  	//  mark the current process as not running
	process_bitmap &= ~((1 << i));
	// curr_process->parent = NULL;
	if(parent_pcb==NULL){
		paging_init(0);
	}
	else
	{
	paging_init(parent_pcb->pid + 1);
	}


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

    asm volatile (
		"pushl %%eax;"
		"movl %0, %%eax;"
		"movl %%eax, %%ss;"
		"popl %%eax;"
		:
		: "r"(curr_process->old_ss)
		: "%eax", "cc"
	);

    //restore tss values
	tss.esp0 = curr_process->old_esp0;
	tss.ss0 = curr_process->old_ss;
	kernel_stack_top = curr_process->old_kernel_stack_top;
	// update curr_process as parent process
	/*if (parent_pcb!= NULL) {
		j = parent_pcb->process_number;
		curr_process = curr_process->parent;
	}else {
		j = 0;
	}*/
	curr_process = curr_process->parent;
	asm volatile("popl %eax");
    asm volatile("jmp return_from_halt");
    // asm volatile("leave");
    // asm volatile("ret");
	return 0;
}

/*
 * system_execute
 * input: command - pointer to the command to be executed
 * description: execute the given command
 * return value: return value from previous process
 * side effect :create new pcb, fdts for the current command, prepare for context switch
 */
int32_t system_execute (const uint8_t* command)
{

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

		uint8_t file_name[FILE_NAME_BUFFER_SIZE] = {0};
		uint8_t arguments[ARG_BUFFER_SIZE] = {0};

		//parsing the file name
		while(command[i]!=' ' && command[i]!='\0')
		{
			file_name[i] = command[i];
			i++;
		}

		file_name[i]='\0';

		//parsing arguments from command
		i++;
		while(command[i]!='\0')
		{
			arguments[j] = command[i];
			i++;
			j++;
		}
		arguments[j]='\0';

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
	new_pcb.old_esp0 = tss.esp0;
	new_pcb.old_kernel_stack_top = kernel_stack_top;
	strncpy_uint(new_pcb.args,arguments,j);

	if (new_pid != PID_PD_OFFSET) {
		new_pcb.parent = (pcb_t*) curr_process;
	} else {
		new_pcb.parent = NULL;
	}

	asm volatile(
		"movl  %%ss, %0;"
		: "=r" (new_pcb.old_ss)
		:
	);

	//initialize paging and load program image
	paging_init(new_pid + 1);
	program_loader(file_name);

	//put pcb in corresponding location in memory
	memcpy((void*) (kernel_stack_top - KERNEL_STACK_ENTRY_SIZE), (void*) &new_pcb, sizeof(typeof(pcb_t)));
	curr_process = (pcb_t*) (kernel_stack_top - KERNEL_STACK_ENTRY_SIZE);
	kernel_stack_top -= KERNEL_STACK_ENTRY_SIZE;

	// initialize file descriptor table
	init_new_fdt();


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
     "iret;"
     :
     : "g"(USER_DS), "g"(IRET_ESP), "g"(USER_CS), "g"(new_instruction)
     : "cc","memory"
     );

//leave and return

  sti();
  asm volatile (
	"return_from_halt: "
	"leave;"
	"ret"
  );

	return 0;
}


/*
 * system_read
 * input:fd - fd number
 * buf - pointer to the buffer to be read
 * nbytes - num of bytes to be read
 * description: read from the given buffer
 * return value: bytes read
 * side effect :none
 */
int32_t system_read (int32_t fd, void* buf, int32_t nbytes)
{
	// printf("fd is %d, nbytes is %d \n" , fd, nbytes);

	if(fd<FD_MIN || fd>FD_MAX)	//check range
		return -1;

	if(fd == FD_STDOUT)	//can't read from stdout
		return -1;

	return (curr_process->file_desc_table[fd].file_ops_table_ptr->read)(fd, buf, nbytes);		//to be replaced by pcb
}

/*
 * system_write
 * input:
 * fd- fd number
 * buf - pointer to the buffer to be written 
 * nbytes - num of bytes to be written
 * description: write to the given buffer
 * return value: bytes written
 * side effect :none
 */
int32_t system_write (int32_t fd, const void* buf, int32_t nbytes)
{

	if(fd<FD_MIN || fd>FD_MAX)	//check range
		return -1;


	if(curr_process->file_desc_table[fd].flag == 0)	//can't write if it's not open yet
		return -1;

	return (curr_process->file_desc_table[fd].file_ops_table_ptr->write)(fd, buf, nbytes); //to be replaced by pcb
}

/*
 * system_open
 * input: pointer to the file to be opened
 * description: open the file specified by filename
 * return value: 0 if success, -1 if failed
 * side effect :none
 */

int32_t system_open(const uint8_t* filename){
	int i;
	for (i = 0; i< FILENAME_SIZE && filename[i] != KEY_EMPTY;i++)
	{
		printf("%c",filename[i]);
	}
	printf("\n");

	if (strncmp((const int8_t*)filename,(int8_t *) "stdin",IN_LENGTH) == 0) {
		return 0;
	}
	if (strncmp((const int8_t*)filename,(int8_t *) "stdout", OUT_LENGTH) == 0) {
		return 0;
	}
	return fs_open((uint8_t*)filename);
}

/*
 * system_close
 * input:fd number to be closed
 * description: close the given file
 * return value: 0 if successfully closed, -1 if not
 * side effect :none
 */

int32_t system_close (int32_t fd){
	int retval;
	file_desc_entry_t* current_file = &(curr_process->file_desc_table[fd]);
	// error if file is not in use
	if (current_file->flag == 0 || fd <= FD_STDOUT || fd > FD_MAX) {
		return -1;
	}
	retval = current_file->file_ops_table_ptr->close(fd);
	// asm volatile("call  *%0;"
	// 			 :
	// 			 : "g" (current_file->file_ops_table_ptr->close));
	// save return value
	// asm volatile("movl %%eax, %0":"=g"(i));
	// clear file info
	current_file->file_ops_table_ptr = NULL;
	current_file->inode = 0;
	current_file->file_position = 0;
	current_file->flag = 0;
	return  retval;
}

int32_t system_getargs (uint8_t* buf, int32_t nbytes){
	if (buf == NULL || nbytes < 0 || curr_process->args == NULL)
		return -1;

	strncpy_uint(buf,curr_process->args,nbytes);
	return 0;
}

