#include "fsop.h"

/*
reg_open
Description:open a regular file
Input:	filename - the filename to be opened
Output: none
Return value: A file descriptor
Side Effect: corresponding file is open

*/

int32_t reg_open(const uint8_t* filename)
{

	return 0;
}

/*
reg_write
Description: write a regular file
Input:	fd - file descriptor 
		buf - the buffer to be used
		nbytes - number of bytes to be written
Output: none
Return value: 0
Side Effect: none

*/

int32_t reg_write(int32_t fd, const void* buf, int32_t nbytes)
{
	return 0;
}


/*
reg_read
Description: write a regular file
Input:	fd - file descriptor 
		buf - the buffer to be used
		nbytes - number of bytes to be written
Output: A buffer contains the file content
Return value: Number of bytes that have been successfully read
Side Effect: None

*/

int32_t reg_read(int32_t fd, const void* buf, int32_t nbytes)
{
	file_desc_entry_t* fdd = (file_desc_entry_t*)fd;
    uint32_t inode = fdd->inode;
    uint32_t offset = fdd->file_position; 

	return read_data(inode,offset,(uint8_t*)buf,nbytes);
}

/*
reg_close
Description: close a regular file
Input:	fd - file descriptor
Output: none
Return value: 0
Side Effect: None
*/

int32_t reg_close(int32_t fd)
{
	file_desc_entry_t* fdd = (file_desc_entry_t*) fd;
	fdd->flag = 0;
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
Input:	fd - file descriptor 
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
Input:	fd - file descriptor
Output: none
Return value: 0
Side Effect: None
*/
int32_t dir_close(int32_t fd)
{
	return 0;
}




