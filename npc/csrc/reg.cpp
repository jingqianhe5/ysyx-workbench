#include <stdint.h>
#include <stdlib.h>
#include <cstring>
#include "verilated.h"
#include "verilated_fst_c.h"
#include "Vysyx_26010032_npc.h"//引入对应的v文件
#include "Vysyx_26010032_npc___024root.h"
#include "sim.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
uint32_t isa_reg_display(char *args,bool *success){
    const char *reg_name = args;

    if (reg_name[0] == '$') {
      reg_name++;
    }
  for (int i = 0; i < 32;i++){
    if (strcmp(regs[i],reg_name)==0){
      uint32_t value = top->rootp->ysyx_26010032_npc__DOT__u_ysyx_26010032_GPR_rs1__DOT__rf[i];
      *success = true;
      return value;
    }

  }
    *success = false;
    return 0;
}
