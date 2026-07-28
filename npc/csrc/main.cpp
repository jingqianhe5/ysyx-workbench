
#include "verilated.h"
#include "verilated_fst_c.h"
#include "Vysyx_26010032_npc.h"//引入对应的v文件
#include "Vysyx_26010032_npc___024root.h"
/*
#include "expr.h"//表达式求值文件
*/
#include "sim.h"
#include "reg.h"
#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include "expr.h"
#include "watchpoint.h"
#include <capstone/capstone.h>
#include "difftest.h"

// ================================================================
// 物理内存与程序镜像
// ================================================================
/*
#define PMEM_SIZE (128*1024*1024)
uint8_t pmem[PMEM_SIZE];

static bool load_image(const char *image_path) {
  FILE *fp = fopen(image_path, "rb");//打开文件
  if(fp==NULL){
    printf("文件打开失败");
    return false;
  }

  fseek(fp,0,SEEK_END);
  long size = ftell(fp);
  fseek(fp,0,SEEK_SET);

  if (fread(pmem, size, 1, fp));
  fclose(fp);
  return true;
}
*/
static csh disasm_handle;

static void init_disasm() {
  cs_err err = cs_open(
      CS_ARCH_RISCV,
      CS_MODE_RISCV32,
      &disasm_handle);

  if (err != CS_ERR_OK) {
    printf("Failed to initialize Capstone: %s\n",
           cs_strerror(err));
    exit(1);
  }
}
// ================================================================
// 调试器输入
// ================================================================
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(my NPC) :");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

// ================================================================
// 仿真周期与波形
// ================================================================
/*
VerilatedContext* contextp = NULL;
VerilatedFstC* tfp = NULL;
static Vysyx_26010032_npc* top;//对应的V文件
*/
/*
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
*/
// ================================================================
// CPU 执行控制
// ================================================================
//bool keep = true;//保持仿真
extern "C" uint32_t pmem_read(uint32_t offset) {
  return (uint32_t)pmem[offset]
       | (uint32_t)pmem[offset + 1] << 8
       | (uint32_t)pmem[offset + 2] << 16
       | (uint32_t)pmem[offset + 3] << 24;
}


extern "C" void npc_trap(int code){//程序结束并且检查寄存器0是不是0
  if(code==0){
    printf("\033[1;35m hello my npc!!!\n");
    printf("\033[1;32m HIT GOOD TRAP\n\n\n");
  }
  else{
    printf("\033[1;35m hello my npc!!!\n");
    printf("\033[1;31m HIT BAD TRAP\n\n\n");
  }
  keep = false;
}
static int deep = 0;
extern "C" void itrace(uint32_t pc, uint32_t ins) {
  uint8_t code[4] = {
    static_cast<uint8_t>(ins),
    static_cast<uint8_t>(ins >> 8),
    static_cast<uint8_t>(ins >> 16),
    static_cast<uint8_t>(ins >> 24)
  };

  cs_insn *decoded = nullptr;
  size_t count = cs_disasm(
      disasm_handle,
      code,
      sizeof(code),
      pc,
      1,
      &decoded);
  
  if (count == 1) {
    if((ins & 0x00000fff )== 0x000000ef){
      deep ++;
      for (int i = 0 ;i < deep;i++){
        printf("  ");
      }
      printf("\033[1;32m[ftrace](deep=%d)(call)\033[0m\n", deep);
    }
    if(ins == 0x00008067){
      for (int i = 0 ;i < deep;i++){
        printf("  ");
      }
      printf("\033[1;33m[ftrace](deep=%d)(ret)\033[0m\n", deep);
      deep --;
    }
    for (int i = 0 ;i < deep;i++){
      printf("  ");
    }
    printf("[itrace] %08x: %08x  %-8s %s\n",
           pc,
           ins,
           decoded[0].mnemonic,
           decoded[0].op_str);

    cs_free(decoded, count);
  } else {
    printf("[itrace] %08x: %08x  <unknown>\n",
           pc, ins);
  }
}
/*
//功能
void run_step(int times){//推进一个周期
  for(int i = 0; i < times; i++) {
    if(top->rst!=1){
      uint32_t current_pc = top->out_pc;
      if (current_pc < 0x80000000 || current_pc >= 0x80000000 + PMEM_SIZE - 4) {
        printf("[PC越界] PC越界或不对齐: 0x%08x\n", current_pc);
        keep = false;
        return; // 强行跳出循环，保存波形
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
  }
}
*/
//寄存器
/*
const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
*/
// ================================================================
// 调试器命令
// ================================================================
/*
static int cmd_si(char *args){//单步执行命令
  int n = args ? atoi(args) : 1;
  if (n <= 0) n = 1;

  run_step(n);
  return 0;
}
  */
/*
uint32_t isa_reg_display(char *args){
  for (int i = 0; i < 32;i++){
    if (strcmp(regs[i],args)==0){
      uint32_t value = top->rootp->ysyx_26010032_npc__DOT__u_ysyx_26010032_GPR_rs1__DOT__rf[i];
      printf("%s value is 0x%08x",args,value);
      printf("\n");
      return value;
    }

  }
    printf("reg no find!\n");
    return 0;
}
*/
/*
static int cmd_info(char *args){
  char *type = strtok(args, " \t\n");
  bool *success;
  if(*type=='r'){
    char *reg = type + strlen(type) + 1;
    isa_reg_display(reg,&success);
  }
  return 0;
}

static int cmd_c(char *args){
  while(keep){
    run_step(1);
  }
  return 0;
}
*/
/*
static int cmd_x(char *args){
  char *num = strtok(args, " \t\n");
  uint32_t addr = (uint32_t)strtoul(num + strlen(num) + 1, NULL, 0);
  for (int i = 0; i < atoi(num); i++){
    uint32_t offset = addr - 0x80000000;
    uint32_t value = (uint32_t)pmem[offset]
               | (uint32_t)pmem[offset + 1] << 8
               | (uint32_t)pmem[offset + 2] << 16
               | (uint32_t)pmem[offset + 3] << 24;
    printf("addr:0x%08x   vaule:0x%08x",addr,value);
    addr=addr+4;
    printf("\n");
  }
  return 0;
}
*/
/*
static int cmd_p(char *args){
  bool success=true;
  expr(args,&success);
  return 0;
}
*/
static struct{
  const char *name;
  const char *description;
  int (*handler)(char *args);
}cmd_table [] = {
  {"si","单步执行指令",cmd_si},
  {"info","打印信息",cmd_info},
  {"c","持续执行",cmd_c},
  {"x","扫描内存",cmd_x},
  {"p","表达式求值",cmd_p},
  {"w","添加监视点",cmd_w},
  {"d","删除监视点",cmd_d},
};
// ================================================================
// 程序入口
// ================================================================
int main(int argc,char** argv) {
  printf("Hello, ysyx!\n");
  init_disasm();
  init_wp_pool();
  init_regex();
  if(argc < 2){
    printf("没有传入指令文件\n");
    return -1;
  }

  char *image_path = argv[1];//读取文件的地址
  if (!load_image(image_path)) {
    return -1;
  }

//    const uint32_t img[]{
//        0b00000000010100000000000010010011, //addi x1 x0 5
//        0b00000000000100000000000100010011, //addi x2 x0 1
//        0b00000000001000000000000100010011, //addi x2 x0 2
//        0b00000000010100001000000100010011, //addi x2 x1 5
//        0b00000000000100000000000001110011  //ebreak
//    };
  sim_init();

  top->rst = 1;
  top->clk = 0;
  // 推进几个时间步
  run_step(5);
  top->rst = 0; // 松开复位，开始工作

  //diff初始化
  CPU_state initial_state = {};
  initial_state.pc = 0x80000000;
  init_difftest(pmem, img_size,&initial_state);

  // 4. 主循环 (模拟时钟和内存行为)
  char *str;
  while (keep && (str = rl_gets()) != NULL) {
    char *str_end = str + strlen(str);
    char *cmd = strtok(str, " \t\n");
    if(cmd == NULL){
      continue;
    }

    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

    int i;
    for (i = 0; i < 7; i++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) {
          return -1;
        }
        break;
      }
    }

    if (i == 7) {
      printf("Unknown command '%s'\n", cmd);
      continue;
    }
    if (!keep) {break;}//防止程序结束后还发送指令
  }

  // 5. 结束仿真
  sim_exit();
  return 0;
}
