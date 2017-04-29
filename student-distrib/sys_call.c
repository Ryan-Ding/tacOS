#include "sys_call.h"

boot_block_t* boot_block_ptr;

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
 * parse_argument
 * input: command -- the original command
 		  file_name -- file_name buffer
 		  arguments -- arguments buffer
 * description: parsing command
 * return value: the length of arguments
 * side effect : none
 */

uint32_t parse_argument(const uint8_t* command, uint8_t* file_name, uint8_t* arguments)
{

		int i=0;
		int j=0;
		int k=0;

	while(command[i] == ' '&& command !='\0')
		{
			i++;
		}



		//parsing the file name
		while(command[i]!=' ' && command[i]!='\0')
		{
			file_name[k] = command[i];
			i++;
			k++;
		}

		file_name[k]='\0';
		//parsing arguments from command
		i++;
		if (command[i] != '\0')
		{
			while(command[i] == ' ') {i++;}


			while(command[i]!='\0')
		 	{
				arguments[j] = command[i];
				i++;
				j++;
			}
		}
		j++;
		arguments[j]='\0';

		return j;

}



void remap_video(uint32_t new_terminal_id) {
	uint32_t curr_displayed_pid = terminal[curr_display_term].curr_process->pid; // current displayed terminal active process pid
	uint32_t is_new_term_initialized = terminal[new_terminal_id].curr_process != NULL;
	uint32_t new_pid;
	
	// the first thing we do is to set up the paging frame at the standpoint of the curr_displayed process
	load_page_directory(1 + curr_displayed_pid);

	page_directory_t * page_directory = &(page_directory_list[curr_displayed_pid + 1 + PID_PD_OFFSET]);
	page_table_t * page_table = &(page_table_list[curr_displayed_pid + 1 + PID_PD_OFFSET]);
	// save video memory to back up
	uint32_t backup_vid_offset = (1 + curr_display_term) * PAGE_SIZE;
	memcpy( (void*) VIDEO_MEM_PHYS_ADDR + backup_vid_offset, (void*) VIDEO_MEM_PHYS_ADDR, PAGE_SIZE); //store to back upo
	backup_vid_offset = (1 + new_terminal_id) * PAGE_SIZE;
	memcpy((void*) VIDEO_MEM_PHYS_ADDR, (void*) VIDEO_MEM_PHYS_ADDR + backup_vid_offset, PAGE_SIZE); // restore backup of new term to screen

	// then map the vidmap of the new terminal to the actual phys video mem
	if (is_new_term_initialized) {
		new_pid = terminal[new_terminal_id].curr_process->pid; // curr process of newly open term might be null
	  	page_directory = &(page_directory_list[new_pid + 1 + PID_PD_OFFSET]);
	  	page_table = &(page_table_list[new_pid + 1 + PID_PD_OFFSET]);
		(*page_table)[VIDEO_PAGE_TABLE_IDX] = VIDEO_MEM_PHYS_ADDR | PAGE_TABLE_ENTRY_MASK;
		if (terminal[new_terminal_id].curr_process->is_user_vid_mapped == 1) {
			(*page_table)[USER_VIDEO_MEM_PAGE_TABLE_OFFSET] = (VIDEO_MEM_PHYS_ADDR) | PAGE_TABLE_ENTRY_MASK | SUPERVISOR_MASK;
		}
		load_page_directory(new_pid + 1); // assign page table address and mark as present
	}

	page_directory = &(page_directory_list[curr_displayed_pid + 1 + PID_PD_OFFSET]);
	page_table = &(page_table_list[curr_displayed_pid + 1 + PID_PD_OFFSET]);	
	// restore backup video to physical video memory
	
	// lastly, map the vidmap of current terminal to back up
	backup_vid_offset = (1 + curr_display_term) * PAGE_SIZE; // TODO
	if (terminal[curr_display_term].curr_process->is_user_vid_mapped == 1) {
		(*page_table)[USER_VIDEO_MEM_PAGE_TABLE_OFFSET] = (VIDEO_MEM_PHYS_ADDR + backup_vid_offset) | PAGE_TABLE_ENTRY_MASK | SUPERVISOR_MASK;
	}
	(*page_table)[VIDEO_PAGE_TABLE_IDX] = (VIDEO_MEM_PHYS_ADDR + backup_vid_offset ) | PAGE_TABLE_ENTRY_MASK; // assign page table address and mark as present
	load_page_directory(curr_displayed_pid + 1);

	// finally, restore the paging frame of the current terminal pid
	load_page_directory( terminal[curr_term].curr_process->pid + 1 );
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
	cli();
	//printf("system halt");
	uint32_t i;
	if(curr_process->parent == NULL)
	{
		printf("You exit the last shell\n");
		printf("Restart a new shell\n");
		uint8_t filename[] = "shell";
		system_execute(filename);
		system_halt(-1);
	}
	pcb_t * parent_pcb = curr_process->parent;


	i = curr_process->pid;
  	//  mark the current process as not running
	process_bitmap &= ~((1 << i));
	// curr_process->parent = NULL;

	paging_init(parent_pcb->pid + 1);

	// // restore cr3
	// asm volatile (
	// 	"pushl %%eax;"
	// 	"movl %0, %%eax;"
	// 	"movl %%eax, %%cr3;"
	// 	"popl %%eax;"
	// 	:
	// 	: "r"(curr_process->old_cr3)
	// 	: "%eax", "cc"
	// );

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
	terminal[curr_term].curr_process = curr_process;

	sti();

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

	int j;
	int32_t new_pid;
	pcb_t new_pcb;
	uint32_t old_ebp = 0, old_esp = 0, old_cr3=0;
	/* Parse Command Arguments */
	if(!command) {
		printf("command must be valid\n");
	}
	//printf("%s\n",strchr(command,' '));

	uint8_t file_name[FILE_NAME_BUFFER_SIZE] = {0};
	uint8_t arguments[ARG_BUFFER_SIZE] = {0};

	j = parse_argument(command, file_name, arguments);
	/* Check for executable */
	dentry_t search_for_dir_entry;
    //printf("The size of inode is: %d\n",sizeof(dentry_t));

	//check file validity
    if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1) {
        return -1;
    }

	if (check_if_executable(&search_for_dir_entry)!= 0) {
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
	new_pcb.terminal_id = curr_term;
	new_pcb.is_user_vid_mapped = 0;
    new_pcb.fake_ebp = 0;
    new_pcb.fake_esp = 0;
    new_pcb.is_blocked_by_new_terminal = 0;

	strncpy_uint(new_pcb.args,arguments,j);


	new_pcb.parent = terminal[curr_term].curr_process;


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
	

	if (curr_process == NULL) {
		curr_process = (pcb_t*) (kernel_stack_top - KERNEL_STACK_ENTRY_SIZE);
		// update terminal curr proces pointer
		terminal[curr_term].curr_process = curr_process;
	} else {
		curr_process = (pcb_t*) (kernel_stack_top - KERNEL_STACK_ENTRY_SIZE);
		// update terminal curr proces pointer
		terminal[curr_term].curr_process = curr_process;
	}

	kernel_stack_top -= KERNEL_STACK_ENTRY_SIZE;

	// initialize file descriptor table
	init_new_fdt();


	/* Prepare for Context Switch; tss.esp0/ebp */
	tss.ss0 = KERNEL_DS;
	tss.esp0 = kernel_stack_top - 4;
	curr_process->curr_esp0 = kernel_stack_top - 4;

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
	 "sti;"
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

	if(fd<FD_MIN || fd>FD_MAX || curr_process->file_desc_table[fd].flag == 0)	//check range
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
	// int i;
	// for (i = 0; i< FILENAME_SIZE && filename[i] != KEY_EMPTY;i++)
	// {
	// 	printf("%c",filename[i]);
	// }
	// printf("\n");
	
	// check if it is null string
	if (*filename == '\0') { return -1; }

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
	// printf("fd %d about to be closed\n", fd);
	file_desc_entry_t* current_file = &(curr_process->file_desc_table[fd]);
	// error if file is not in use
	if (current_file->flag == 0 || fd <= FD_STDOUT || fd > FD_MAX) {
		return -1;
	}
	return current_file->file_ops_table_ptr->close(fd);

}

/*
 * system_getargs
 * input: buf -- the user space buffer
 		  nbytes -- number of bytes to be copied 
 * description: copy the arguments to user space buffer
 * return value: 0 on success -1 failure
 * side effect :none
 */


int32_t system_getargs (uint8_t* buf, int32_t nbytes){
	if (buf == NULL || nbytes < 0 || curr_process->args == NULL)
		return -1;

	strncpy_uint(buf,curr_process->args,nbytes);
	return 0;
}

/*
 * system_vidmap
 * input: address where video memory will be stored at 
 * description: maps the text mode video memory into user space at a preset
 				virtual address
 * return value: 0 on success and otherwise
 * side effect :none
 */
int32_t system_vidmap (uint8_t** screen_start) {
	// check address validity
	uint32_t backup_vid_offset;
	uint32_t pid = curr_process->pid;	
	page_directory_t * page_directory = &(page_directory_list[pid + 1 + PID_PD_OFFSET]);
	page_table_t * page_table = &(page_table_list[pid + 1 + PID_PD_OFFSET]);
	void* base_addr = (void*) (PROGRAM_IMAGE_ADDR & PROGRAM_IMG_BASE_ADDR_MASK);
	if ( (void*) screen_start < base_addr || (void*) screen_start >= (base_addr + PORGRAM_IMAGE_SIZE) ) {
		// printf("address user passed in is not valid\n");
		return -1;
	}

	// set up paging entry
	(*page_directory)[USER_VIDEO_MEM_PAGE_DIRECTORY_OFFSET] = 	((uint32_t) (&((*page_table)[USER_VIDEO_MEM_PAGE_TABLE_OFFSET])) )  | ( SMALL_PAGE_DIRECTORY_ENTRY_MASK  | SUPERVISOR_MASK);


	// //depending whether the terminal is focused, map the video memory to back up or screen
	// 	if(is_terminal_focused())
	// 		p->pages->pgt[VIDMAP_PGT_IDX] = VID_PHY_MEM | PRESENT | READ_WRITE_ENABLE | USER_ACCESS;
	// 	else
	// 		p->pages->pgt[VIDMAP_PGT_IDX] = ( VID_PHY_START + (get_terminal_index() * PAGE_4KB) )   | PRESENT | READ_WRITE_ENABLE | USER_ACCESS;
		
	if (is_terminal_active()) {
		(*page_table)[USER_VIDEO_MEM_PAGE_TABLE_OFFSET] = VIDEO_MEM_PHYS_ADDR | PAGE_TABLE_ENTRY_MASK | SUPERVISOR_MASK;
	} else {
		backup_vid_offset = (1 + curr_process->terminal_id) * PAGE_SIZE;
		(*page_table)[USER_VIDEO_MEM_PAGE_TABLE_OFFSET] = (VIDEO_MEM_PHYS_ADDR + backup_vid_offset) | PAGE_TABLE_ENTRY_MASK | SUPERVISOR_MASK;
	}
	
	curr_process->is_user_vid_mapped = 1;
	// update screen_start
	*screen_start = (uint8_t*) VIDEO_MEM_USER_ADDR;
	// need a way to represent that the video memory is mapped for the user program? 
	// may be stored in pcb
	// TODO
	return 0;
}









