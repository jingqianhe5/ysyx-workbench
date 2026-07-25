#include "verilated.h"
#include "verilated_fst_c.h"
#include "Vysyx_26010032_npc.h"//引入对应的v文件
#include "Vysyx_26010032_npc___024root.h"
#include "memory.h"
#include "sim.h"
#include "watchpoint.h"

VerilatedContext* contextp = NULL;
VerilatedFstC* tfp = NULL;
Vysyx_26010032_npc* top;//对应的V文件
bool keep = true;//保持仿真

void step_and_dump_wave(){//推进仿真时间
  top->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void sim_init(){//仿真初始化
  contextp = new VerilatedContext;
  tfp = new VerilatedFstC;
  top = new Vysyx_26010032_npc;//对应的v文件
  contextp->traceEverOn(true);
  top->trace(tfp, 0);
  tfp->open("obj_dir/wave_file.fst");
}

void sim_exit(){//退出仿真
  step_and_dump_wave();
  tfp->close();
}

bool run_step(int times){//推进一个周期
  for(int i = 0; i < times&&keep; i++) {
    if(top->rst!=1){
      uint32_t current_pc = top->out_pc;
      if (current_pc < 0x80000000 || current_pc >= 0x80000000 + PMEM_SIZE - 4) {
        printf("[PC越界] PC越界或不对齐: 0x%08x\n", current_pc);
        keep = false;
        return false; // 强行跳出循环，保存波形
      }
      top->ins = ((uint32_t)pmem[(current_pc-0x80000000)]|(uint32_t)pmem[((current_pc-0x80000000)+1)]<<8|(uint32_t)pmem[((current_pc-0x80000000)+2)]<<16|(uint32_t)pmem[((current_pc-0x80000000)+3)]<<24);
      printf("给指令：%08x\n",top->ins);
    }

    top->clk = 1;
    top->eval();
    step_and_dump_wave();
    top->clk = 0;
    top->eval();
    step_and_dump_wave();
    if (top->rst != 1 && check_wp()) {
      return true;   // 命中监视点，停止 si N 或 c
    }
  }
  return false;
}