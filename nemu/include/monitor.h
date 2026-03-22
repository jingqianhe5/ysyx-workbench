#ifndef __MONITOR_H__
#define __MONITOR_H__

#include <stdint.h>
#include <stdbool.h>

void ftrace_init(char *elf_file);


void find_name(int rd, int rs1, uint32_t current_pc, uint32_t target_pc);

#endif 