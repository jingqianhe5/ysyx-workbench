#ifndef __DIFFTEST_H__
#define __DIFFTEST_H__


#include <stdint.h>
typedef struct {
  uint32_t gpr[32];
  uint32_t pc;
} CPU_state;
bool difftest_single_step(const CPU_state *dut_state);
void init_difftest(void *mem_buf,long img_size,void *reg_buf);

#endif