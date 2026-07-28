#include "difftest.h"
#include <dlfcn.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };//difftest-def



static void (*difftest_memcpy_pt)(uint32_t addr, void *buf, size_t n, bool direction)=NULL;

//difftest_memcpy


//difftest_regcpy
static void (*difftest_regcpy_pt)(void *dut, bool direction)=NULL;


//difftest_exec
static void (*difftest_exec_pt)(uint64_t n)=NULL;


//difftest_init
static void (*difftest_init_pt)(int port)=NULL;



void init_difftest(void *mem_buf,long img_size,void *reg_buf){//程序大小
    void* h =dlopen("../../../nemu/build/riscv32-nemu-interpreter-so",RTLD_LAZY);
    if (h == NULL) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        exit(1);
    }

    
    difftest_memcpy_pt = (decltype(difftest_memcpy_pt)) (dlsym(h,"difftest_memcpy"));//转化类型
    if (difftest_memcpy_pt == NULL) {
        fprintf(stderr, "dlsym difftest_memcpy failed: %s\n", dlerror());
        exit(1);
    }

    difftest_regcpy_pt = (decltype(difftest_regcpy_pt))(dlsym(h,"difftest_regcpy"));
    if (difftest_regcpy_pt == NULL) {
        fprintf(stderr, "dlsym difftest_regcpy failed: %s\n", dlerror());
        exit(1);
    }

    difftest_exec_pt = (decltype(difftest_exec_pt))(dlsym(h,"difftest_exec"));
    if (difftest_exec_pt == NULL) {
        fprintf(stderr, "dlsym difftest_exec failed: %s\n", dlerror());
        exit(1);
    }

    difftest_init_pt = (decltype(difftest_init_pt))(dlsym(h,"difftest_init"));
    if (difftest_init_pt == NULL) {
        fprintf(stderr, "dlsym difftest_init failed: %s\n", dlerror());
        exit(1);
    }

    difftest_init_pt(0);
    difftest_memcpy_pt(0x80000000,mem_buf,img_size,DIFFTEST_TO_REF);
    difftest_regcpy_pt(reg_buf,DIFFTEST_TO_REF);
}

bool difftest_single_step(const CPU_state *dut_state){//执行一步之后比较双方pc以及寄存器 返回结果
    CPU_state ref_state = {};
    difftest_exec_pt(1);
    difftest_regcpy_pt(&ref_state,DIFFTEST_TO_DUT);
    if(dut_state->pc != ref_state.pc){
        printf("\033[1;31m[difftest] PC no same\n");
        printf("dut PC:%08x\n",dut_state->pc);
        printf("ref PC:%08x\n",ref_state.pc);
        printf("\033[0m\n");
        return false;
    }
    for (int i = 0 ;i<32;i++){
         if (dut_state->gpr[i] != ref_state.gpr[i]){
            printf("\033[1;31m[difftest] gpr no same\n");
            printf("dut gpr%d:%08x\n",i,dut_state->gpr[i]);
            printf("ref gpr%d:%08x\n",i,ref_state.gpr[i]);
            printf("\033[0m\n");
            return false;
        }
    }
    return true;
}
