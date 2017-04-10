#include "sys_call.h"

boot_block_t* boot_block_ptr;

void init_sys_call(){
	boot_block_ptr = get_boot_block_info();
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

	//check file validity
	dentry_t search_for_dir_entry;
    //printf("The size of inode is: %d\n",sizeof(dentry_t));
    if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1){
        return -1;
    }

    //set up paging
    paging_init();

    //TODO
    //file loader

    //set up PCB

    //http://www.jamesmolloy.co.uk/tutorial_html/10.-User%20Mode.html
    //uint32_t ret_val;
    cli();
	asm volatile(
	 "mov $0x2B, %ax;"
     "mov %ax, %ds;"
     "mov %ax, %es;"
     "mov %ax, %fs;"
     "mov %ax, %gs;"
                  
     "mov %esp, %eax;"
     "pushl $0x2B;"
     "pushl %eax;"

     //modify eflags
     "pushf;"

     "pushl $0x23;"
     "pushl $0;"
     "iret;");
	sti();

	return 0;
	}










