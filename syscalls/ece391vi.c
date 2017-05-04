#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

#define BUFSIZE 1024

int main ()
{
    uint32_t cnt = 0;
    uint32_t fd;
    uint8_t buf[BUFSIZE];

    ece391_fdputs(1, (uint8_t*)"Vi\n");
    if (0 != ece391_getargs (buf, 1024)) {
        ece391_fdputs (1, (uint8_t*)"could not read arguments\n");
  return 3;
    }

    if (-1 == (fd = ece391_open (buf))) {
        ece391_fdputs (1, (uint8_t*)"file not found\n");
  return 2;
    }

    while (1){
    cnt = ece391_read (0, buf, 1023);
    if (0 == ece391_strncmp (buf, (uint8_t*)"exit",4))
        break;

    ece391_write(fd,buf,cnt);
    }



    return 0;

}
