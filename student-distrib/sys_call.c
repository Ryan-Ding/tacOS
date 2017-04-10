#include "sys_call.h"

boot_block_t* boot_block_ptr;

void init_sys_call(){
	boot_block_ptr = get_boot_block_info();
	process_bitmap = 0;
	kernel_stack_top = KERNEL_END_ADDR;
}

void program_loader(const uint8_t* filename){
	dentry_t search_for_dir_entry;
	if(read_dentry_by_name(filename, &search_for_dir_entry) == -1) {return;}
	uint32_t inode = search_for_dir_entry.inode_num;
	inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));
    uint32_t length = inode_ptr->length;
    read_data(inode,0,(uint8_t*)PROGRAM_IMAGE_ADDR,length);
}

int32_t system_halt (uint8_t status)
{
	//TODO
	return 0;
}

int32_t system_execute (const uint8_t* command)
{
	int32_t new_pid = 999;
	pcb_t new_pcb;
	uint32_t old_ebp = 0, old_esp = 0;
	/* Parse Command Arguments */
	{
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
	}
	/* Check for executable */
	dentry_t search_for_dir_entry;
    //printf("The size of inode is: %d\n",sizeof(dentry_t));
	//check file validity
    // if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1){
    //     return -1;
    // }
	// if(check_if_executable(search_for_dir_entry) == -1) {
	// 	return -1;
	// }
 	/* Set up new paging and load program image */
	// fetch pid from PCB
	if ( (new_pid = find_available_pid()) < 0 ) {
		return -1;
	}

	asm volatile (
		"movl %%ebp, %0;"
		"movl %%esp, %1;"		
		: "=g"(old_ebp), "=g"(old_esp)
	);

	printf("old_ebp: %u \n old_esp: %u \n",old_ebp,old_esp);
	printf("new_pid is %u \n",new_pid);
	/* Create PCB and open file descriptors */
	new_pcb.pid = new_pid;
	new_pcb.old_ebp = old_ebp;
	new_pcb.old_esp = old_esp;
	if (new_pid != PID_PD_OFFSET) {
		new_pcb.parent = (pcb_t*) kernel_stack_top;
	} else {
		new_pcb.parent = NULL;		
	}
	memcpy((void*) (kernel_stack_top - KERNEL_STACK_ENTRY_SIZE), (void*) &new_pcb, sizeof(typeof(pcb_t)));
	kernel_stack_top -= KERNEL_STACK_ENTRY_SIZE;
	/* Prepare for Context Switch; tss.esp0/ebp */

	/* Prepare for fake iret */

	

    //set up paging
    //TODO
    //file loader

    //set up PCB

    //http://www.jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html
    //uint32_t ret_val;
    // cli();
	// asm volatile(
	//  "mov $0x2B, %ax;"
    //  "mov %ax, %ds;"
    //  "mov %ax, %es;"
    //  "mov %ax, %fs;"
    //  "mov %ax, %gs;"
                  
    //  "mov %esp, %eax;"
    //  "pushl $0x2B;"
    //  "pushl %eax;"

    //  //modify eflags
    //  "pushf;"

    //  "pushl $0x23;"
    //  "pushl $0;"
    //  "iret;");
	// sti();

	return 0;
	}










