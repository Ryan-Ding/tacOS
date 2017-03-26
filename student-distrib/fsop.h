#ifndef FSOP_H
#define FSOP_H
#include "filesys.h"
#include "sys_call.h"

int32_t reg_open(const uint8_t* filename);
int32_t reg_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t reg_read(int32_t fd, const void* buf, int32_t nbytes);
int32_t reg_close(int32_t fd);

int32_t dir_open(const uint8_t* filename);
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_close(int32_t fd);


#endif



