#ifndef __SDB_H__
#define __SDB_H__


#include <stdint.h>
int cmd_x(char *args);
uint32_t pmem_read(uint32_t addr,int len);
int cmd_si(char *args);
int cmd_info(char *args);
int cmd_c(char *args);
int cmd_p(char *args);
int cmd_w(char *args);
int cmd_d(char *args);

#endif