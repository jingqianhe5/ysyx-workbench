#ifndef __SIM_H__
#define __SIM_H__
#include "verilated.h"
#include "verilated_fst_c.h"
#include "Vysyx_26010032_npc.h"//引入对应的v文件
#include "Vysyx_26010032_npc___024root.h"


void step_and_dump_wave();
void sim_init();
void sim_exit();
bool run_step(int times);
extern bool keep ;//保持仿真
extern VerilatedContext* contextp ;
extern VerilatedFstC* tfp ;
extern Vysyx_26010032_npc* top;//对应的V文件

#endif