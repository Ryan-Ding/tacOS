#include "filesys.h"
static boot_block_t* boot_block_ptr = NULL;

file_desc_entry_t file_desc_table[FDT_SIZE];  


file_ops_table_t rtc_ops_table;
file_ops_table_t dir_ops_table;
file_ops_table_t reg_ops_table;


/*
fetch_boot_block_info
    Description: read the filesys information and stores a copy of ptr
    Input: frequency - the frequency to be set
    Output: NONE
    Return value: NONE
    Side Effect: global variable boot_block_ptr is intialized 
*/
void fetch_boot_block_info (module_t* module_ptr) {
    uint32_t filesys_start_addr = module_ptr->mod_start;
    uint32_t filesys_end_addr = module_ptr->mod_end;
    printf("filesys size is: %d \n", (filesys_end_addr - filesys_start_addr) / FILE_SYS_BLOCK_SIZE );
    boot_block_ptr = (boot_block_t*)filesys_start_addr;
    printf("inode number is: %d \n", boot_block_ptr->num_inodes );
    printf("data block number is: %d \n", boot_block_ptr->num_data_blocks );
    printf("matched? %d, \n", (boot_block_ptr->num_inodes + boot_block_ptr->num_data_blocks + 1 ) == (filesys_end_addr - filesys_start_addr) / FILE_SYS_BLOCK_SIZE );    
    

}

/*
init_file_system()
    Description: Initial the filesys
    Input:None
    Output:None
    Return value:None
    Side Effect: ops_tables are filled
*/
void init_file_system()
{
    int i;       

    rtc_ops_table.open = rtc_open_syscall;
    rtc_ops_table.read = rtc_read;
    rtc_ops_table.write = rtc_write_syscall;
    rtc_ops_table.close = rtc_close;

    dir_ops_table.open = dir_open;
    dir_ops_table.read = dir_read;
    dir_ops_table.write = dir_write;
    dir_ops_table.close = dir_close;

    reg_ops_table.open = reg_open;
    reg_ops_table.read = reg_read;
    reg_ops_table.write = reg_write;
    reg_ops_table.close = reg_close;



    for (i=0;i<FDT_SIZE;i++)
    {
        file_desc_table[i].flag = 0;
    }
    file_desc_table[STDIN].flag = 1;
    file_desc_table[STDO].flag = 1;


}




/*
read_dentry_by_name
    Description: read the directory entry given its name
    Input: fname - the name char array to be found
           dentry - directory entry to be initialized
    Output: NONE
    Return value: 0 on success
                  1 on failure
    Side Effect: dentry struct is intialized with appropriate entry values
*/
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
    int i; // loop var
    // three cases for failure: 
    // (1) fname is null, 
    // (2) filename not found 
    // (3) dentry is null
    if (fname == NULL || dentry == NULL) { return -1; }
    for (i = 0; i < MAX_DIR_ENTRY_SIZE; ++i) {
        if ( strncmp( (int8_t*) fname, (int8_t*)boot_block_ptr->dir_entries[i].filename, FILENAME_SIZE) == 0 ) {
            // that's when we find that entry with the appropriate name
            // do memberwise copy
            strncpy((int8_t*)dentry->filename, (int8_t*)fname, FILENAME_SIZE);
            dentry->filetype = boot_block_ptr->dir_entries[i].filetype;
            dentry->inode_num = boot_block_ptr->dir_entries[i].inode_num;
            return 0;
        }
    }
    return -1;
}


/*
read_dentry_by_index
Description: read the dentry value 
Input: index - index of the dir_entries
       dentry - the output dentry
Output: A dentry of the index th dir_entries
Return value: 0 - success
             -1 - failure
Side Effect: None
*/

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {

     if (index >= MAX_DIR_ENTRY_SIZE)
         return -1;
     if (dentry == NULL)
        return -1;
    dentry_t current = boot_block_ptr->dir_entries[index];

    memcpy(dentry,&current,sizeof(dentry_t));
    return 0;
}

/*
read_data()
Description: Read the data from the file
Input:  inode - The inode number
        offset - The position of starting point
        buf - the output buffer
        length - the number of bytes to be read
Output: A buffer contains the data
Return value: -1 failure
              otherwise, number of successful read
Side Effect: None
*/

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    uint32_t bytes_to_be_copied = 0;
    uint32_t count = 0; // number of bytes successfully stored in the buffer
    uint32_t index = offset / FILE_SYS_BLOCK_SIZE; // the data block index to be started with
    uint32_t start_block_offset =  offset % FILE_SYS_BLOCK_SIZE; // the starting position within the first data block
    uint32_t offset_temp = start_block_offset;
    uint32_t* data_ptr = NULL ; // pointer to the current data block to be copied
    inode_block_t* inode_ptr = NULL; // inode pointer
    uint32_t num_data = boot_block_ptr->num_data_blocks;
    uint32_t num_inodes = boot_block_ptr->num_inodes; // total number of inode blocks, defined for convenience

    if (inode >= num_inodes) { return -1; }

    inode_ptr = (inode_block_t*) LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode); 
    

    // if the length is larger than what is actually remained, we clip it 
    if ((length + offset) > (inode_ptr->length))
    {
        length = inode_ptr->length - offset;
    }

    if (offset >= inode_ptr->length) { return 0; } // if offset is not valid, we copy nothing

    while (length > 0) {
        // if the data block value is not valid, return what we have copied so far
        if (index > num_data) { return count; } 

        data_ptr = (uint32_t*) LOCATE_DATA_BLOCK((uint32_t)boot_block_ptr, num_inodes, (inode_ptr->data_blocks)[index] );
        bytes_to_be_copied =  min(length, FILE_SYS_BLOCK_SIZE - offset_temp);
        memcpy(buf, data_ptr + offset_temp, bytes_to_be_copied);
        // prepare for further copies
        length -= bytes_to_be_copied;
        count += bytes_to_be_copied;
        buf += bytes_to_be_copied;  // update buf ptr position to always point to the first empty space 
        if (bytes_to_be_copied == (FILE_SYS_BLOCK_SIZE - offset_temp)) {
            ++index; // the current data block has all been copied, start a new block next time
            offset_temp = 0;
        }  else {
            offset_temp += bytes_to_be_copied; // there are still leftovers in the same data block
        }
    }

    return count;
}

/*
open
Description:find the diretory entry corresponding to the named le, allo
ate an unused le desriptor, and set up any data neessary to handle the given type of file;
Input: pointer to file name
Output: N/A
Return value: unused file descriptor; -1 for failure
Side Effect: corresponding file is open

*/


int32_t open(uint8_t* file_name){

    int32_t fd;
    int i;

    if(!file_name) 
        return -1;

    dentry_t search_for_dir_entry;
    //printf("The size of inode is: %d\n",sizeof(dentry_t));
    if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1){
        return -1;
    }

    for (i=0;i<FDT_SIZE+1;i++)
    {
        if (i == FDT_SIZE)
        {
            return -1;
        }

        if (file_desc_table[i].flag == 0)
        {
            break;
        }

    }
        

    fd = i;
    // printf("File type: %d ",search_for_dir_entry.filetype );
    uint32_t inode = search_for_dir_entry.inode_num;

    file_desc_table[fd].inode = inode;
    file_desc_table[fd].flag = 1;
    file_desc_table[fd].file_position = 0;

    // printf("File size: %d      ",length );
    // printf("File name: %s\n", search_for_dir_entry.filename);

    switch (search_for_dir_entry.filetype) {
        case FILE_TYPE_RTC:
            file_desc_table[fd].file_ops_table_ptr = &rtc_ops_table;
            break;
        case FILE_TYPE_DIRECTORY:
            file_desc_table[fd].file_ops_table_ptr = &dir_ops_table;
            break;
        case FILE_TYPE_REGULAR:
            file_desc_table[fd].file_ops_table_ptr = &reg_ops_table;
            break;
    }

    return fd;


}



/*
reg_open
Description:open a regular file
Input:  filename - the filename to be opened
Output: none
Return value: A file descriptor
Side Effect: corresponding file is open

*/

int32_t reg_open(const uint8_t* filename)
{

    return open((uint8_t*)filename);
}

/*
reg_write
Description: write a regular file
Input:  fd - file descriptor 
        buf - the buffer to be used
        nbytes - number of bytes to be written
Output: none
Return value: -1
Side Effect: none

*/

int32_t reg_write(int32_t fd, const void* buf, int32_t nbytes)
{
    return -1;
}


/*
reg_read
Description: write a regular file
Input:  fd - file descriptor 
        buf - the buffer to be used
        nbytes - number of bytes to be written
Output: A buffer contains the file content
Return value: Number of bytes that have been successfully read
Side Effect: None

*/

int32_t reg_read(int32_t fd, void* buf, int32_t nbytes)
{
    if (fd == -1)
        return -1;

    if (file_desc_table[fd].flag == 0)
        return 0;

    file_desc_entry_t fdd = file_desc_table[fd];
    uint32_t inode = fdd.inode;
    uint32_t offset = fdd.file_position; 

    int n = read_data(inode,offset,(uint8_t*)buf,nbytes);
    if (n < 0)
        return n;

    file_desc_table[fd].file_position += n;
    return n;
}

/*
reg_close
Description: close a regular file
Input:  fd - file descriptor
Output: none
Return value: 0
Side Effect: None
*/

int32_t reg_close(int32_t fd)
{
    file_desc_table[fd].flag = 0;

    return 0;
}

/*
dir_open
Description:open a directory
Input:filename - the filename to be opened
Output: none
Return value: A file descriptor
Side Effect: corresponding file is open

*/

int32_t dir_open(const uint8_t* filename)
{
    return 0;
}

/*
dir_write
Description: write a directory
Input:  fd - file descriptor 
        buf - the buffer to be used
        nbytes - number of bytes to be written
Output: none
Return value: 0
Side Effect: none
*/
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes)
{
    return 0;
}

/*
dir_close
Description: close a dir file
Input:  fd - file descriptor
Output: none
Return value: 0
Side Effect: None
*/
int32_t dir_close(int32_t fd)
{
    return 0;
}





/*
open
Description:Read the directory
Input: None
Output: None
Return value: number of bytes copied
Side Effect: The info of file inside the directory is printed 

*/


int32_t dir_read(int32_t fd, void* buf, int32_t nbytes)
{

    int i,j,k;
    int32_t n = 0;
    for (i = 0; i < MAX_DIR_ENTRY_SIZE; ++i) {
        if(boot_block_ptr->dir_entries[i].filename[0] == 0)
            continue;
        //read all the file's name
    dentry_t search_for_dir_entry;
    //printf("The size of inode is: %d\n",sizeof(dentry_t));
        if(read_dentry_by_index(i, &search_for_dir_entry) == -1){
            return -1;
        }


        printf("file name:");
        for (j=0 ; j<FILENAME_SIZE ; j++)
        {
            if (search_for_dir_entry.filename[j] == '\0')
                break;

        }
        printf(" ");
        for (k=0;k<FILENAME_SIZE-j;k++)
        {
            printf(" ");
        }


        for (j=0 ; j<FILENAME_SIZE ; j++)
        {
            if (search_for_dir_entry.filename[j] == '\0')
                break;

            printf("%c",search_for_dir_entry.filename[j]);

        }
        printf("  ");


        printf("File type: %d ",search_for_dir_entry.filetype );
        //print the file type
        uint32_t inode = search_for_dir_entry.inode_num;
        inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));
        uint32_t length = inode_ptr->length;
        //print the file size
        printf("     File size: %d\n",length );
        n += length;
        //print the file name
        // printf("File name: %s\n", search_for_dir_entry.filename);



        
    }
    

    return n;
}





/*
test_read_file_by_index
Description:Test read_dentry_by_index and read_data
Input: index - the index to be read
Output: None
Return value: None
Side Effect: corresponding file is read
*/
void test_read_file_by_index(uint32_t index){

    dentry_t search_for_dir_entry;
    //printf("The size of inode is: %d\n",sizeof(dentry_t));

    if(read_dentry_by_index(index, &search_for_dir_entry) == -1){
        return;
    }

uint32_t inode = search_for_dir_entry.inode_num;
inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));
uint32_t length = inode_ptr->length;

//printf("Length %d \n", length);
uint8_t buf[length];

int n = read_data(inode,0,buf,length);
int i ;
for (i = 0; i< n; i++)
    {
        printf("%c",buf[i]);
    }
printf("filename: %s", search_for_dir_entry.filename);

}

/*
test_read_file_by_name
Description:Test read_dentry_by_name and read_data
Input: None
Output: None
Return value: None
Side Effect: corresponding file is read
*/

void test_read_file_by_name()
{
    dentry_t search_for_dir_entry;
    uint8_t file_name[32] = "frame0.txt"; 
    if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1) {return;}
    uint32_t inode = search_for_dir_entry.inode_num;
    inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));
    uint32_t length = inode_ptr->length;
    uint8_t buf[length];
    int n = read_data(inode,0,buf,length);
    int i ;
    for (i = 0; i< n; i++)
    {
        printf("%c",buf[i]);
    }
    printf("filename: %s", search_for_dir_entry.filename);

}


/*
test_dir_read
Description:Test dir_read
Input: None
Output: None
Return value: None
Side Effect: The dentry value is printed
*/

void test_dir_read(){

    dir_read(0,0,0);
}

/*
test_reg_read
Description:Test reg_read
Input: None
Output: None
Return value: None
Side Effect: The file is read
*/

void test_reg_read()
{
    int i;
    // dentry_t search_for_dir_entry;
    uint8_t file_name[32] = "frame1.txt"; 
    // if(read_dentry_by_name(file_name, &search_for_dir_entry) == -1) {return;}
    int32_t fd = reg_open(file_name);

    uint32_t inode = file_desc_table[fd].inode;
    inode_block_t* inode_ptr= (inode_block_t*)(LOCATE_INODE_BLOCK((uint32_t)boot_block_ptr, inode));
    uint32_t length = inode_ptr->length;
    uint8_t buf[length];
    
    int n = reg_read(fd,buf,length);
    for (i = 0; i< n; i++)
    {
        printf("%c",buf[i]);
    }
    printf("filename: %s", file_name);

}





