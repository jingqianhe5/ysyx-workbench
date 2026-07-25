#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include <cstring>
#include <stdint.h>
#include "sim.h"
#include "reg.h"
#include "expr.h"
#include "watchpoint.h"



int cmd_x(char *args){
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

uint32_t pmem_read(uint32_t addr,int len){
  uint32_t value =0;
  for (int i = 0; i < len; i++){
    uint32_t offset = addr - 0x80000000;
    if (i==0){
        value = (uint32_t)pmem[offset] ;
    }
    else{
        value  |= (uint32_t)pmem[offset + i] << 8*i;
    }
  }
  return value;
}

int cmd_si(char *args){//单步执行命令
  int n = args ? atoi(args) : 1;
  if (n <= 0) n = 1;

  run_step(n);
  return 0;
}

int cmd_info(char *args){
  char *type = strtok(args, " \t\n");
  bool success=false;
  if(*type=='r'){
    char *reg = type + strlen(type) + 1;
    uint32_t value = isa_reg_display(reg,&success);
    if(success){
      printf("%s value is 0x%08x\n",reg,value);
    }
    else{
      printf("reg no find!\n");
    }
  }
  if(*type=='w'){
    isa_watchpoint_diaplay();
  }
  return 0;
}

int cmd_c(char *args){
  while(keep){
    if(run_step(1)){
      break;
    }
  }
  return 0;
}
int cmd_p(char *args){
  bool success=true;
  uint32_t result= expr(args,&success);
  printf("计算结果是：%u\n",result);
  return 0;
}

int cmd_w(char *args){
  bool success = false;
  uint32_t value = expr(args, &success);
  if (!success) {
    printf("无效表达式\n");
    return 0;
  }
  WP *wp = new_wp();
    strncpy(wp->expressions, args, sizeof(wp->expressions) - 1);
  wp->expressions[sizeof(wp->expressions) - 1] = '\0';
  wp->old_val = value;
  printf("make a new watchpoint: %d\n", wp->NO);
  return 0;
}
int cmd_d(char *args){
  free_wp(atoi(args));
  printf("delete a watchpoint!\n");
  return 0;
}
