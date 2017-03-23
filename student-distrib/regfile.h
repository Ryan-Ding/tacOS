#ifndef REGFILE_H
#define REGFILE_H
#include "filesys.h"

int reg_open(const uint8_t* filename);
int reg_write(int32_t fd, const void* buf, int32_t nbytes);
int reg_read(int32_t fd, void* buf, int32_t nbytes);
int reg_close(int32_t fd);

#endif



