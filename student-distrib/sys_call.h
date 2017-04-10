#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "paging.h"
#include "pcb.h"
#include "filesys.h"
#define PROGRAM_IMAGE_ADDR 0x08048000

void init_sys_call();
void program_loader(const uint8_t* filename);


#endif
