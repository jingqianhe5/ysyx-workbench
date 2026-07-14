#include "verilated.h"
#include "verilated_fst_c.h"
#include "Vysyx_26010032_npc.h"//引入对应的v文件
#include <stdio.h>
//------------------------------------------------------
//框架代码
//
//
//------------------------------------------------------
VerilatedContext* contextp = NULL;
VerilatedFstC* tfp = NULL; 
static Vysyx_26010032_npc* top;//对应的V文件

void step_and_dump_wave(){//推进方针时间
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
//------------------------------------------------------
//main函数  主要仿真代码
//
//
//------------------------------------------------------
bool keep = true;
extern "C" void npc_trap(int code){
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

#define PMEM_SIZE (128*1024*1024)
uint8_t pmem[PMEM_SIZE];
int main(int argc,char** argv) {
    if(argc < 2){
        printf("没有传入指令文件\n");
        return -1;
    }

    char *image_path = argv[1];//读取文件的地址
    FILE *fp = fopen(image_path,"rb");//打开文件
    if(fp==NULL){
        printf("文件打开失败");
        return -1;
    }
    fseek(fp,0,SEEK_END);
    long size = ftell(fp);
    fseek(fp,0,SEEK_SET);

    if (fread(pmem, size, 1, fp));
    fclose(fp);


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
    for(int i = 0; i < 5; i++) {
        step_and_dump_wave();
        top->clk = 1; 
        step_and_dump_wave();
        top->clk = 0; 
    }
    top->rst = 0; // 松开复位，开始工作

    // 4. 主循环 (模拟时钟和内存行为)
    while (keep) {

        uint32_t current_pc = top->out_pc; 
        
        if (current_pc < 0x80000000 || current_pc >= 0x80000000 + PMEM_SIZE - 4) {
            printf("[异常停机] PC越界或不对齐: 0x%08x\n", current_pc);
            break; // 强行跳出循环，保存波形
        }


        top->ins = ((uint32_t)pmem[(current_pc-0x80000000)]|(uint32_t)pmem[((current_pc-0x80000000)+1)]<<8|(uint32_t)pmem[((current_pc-0x80000000)+2)]<<16|(uint32_t)pmem[((current_pc-0x80000000)+3)]<<24);
        printf("给指令：%08x\n",top->ins);
        top->eval();


        top->clk = 1;
                step_and_dump_wave();


        top->clk = 0;
                step_and_dump_wave();
        
    }

    // 5. 结束仿真
    sim_exit();
    return 0;
}