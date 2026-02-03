/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <common.h>
word_t expr(char *e, bool *success); 


void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

void test(){
  int cnt=1;//测试次数初始化
  FILE *fp = fopen("tools/gen-expr/input","r");//打开文件
  assert(fp != NULL);//打不开直接结束
  char buf [65536];//放置表达式
  uint32_t result_nemu;//nemu计算出来的值
  uint32_t result_real;//真实值
  bool success;//用于传入expr函数
  while(fgets(buf,sizeof(buf),fp)!=NULL){
    char *p = strchr(buf,'\n');
    if (p != NULL) {
            *p = '\0';//最后一行可能没有换行符
    }//去掉最后的换行 换成结束符号

    sscanf(buf,"%u",&result_real);//提取出真实值

    char *buf_p = buf;
    if(*buf_p==' '){
      buf_p++;//跳过刚开始的空格
    }
    while(*buf_p >= '0'&& *buf_p <= '9'){
      buf_p++;
    }//跳过一开始的数字 结果（）
    buf_p++;//跳过一个空格
    result_nemu = expr(buf_p,&success);
    if(result_real==result_nemu){
      printf("第%d次循环正确\n",cnt);
    }
    else {
      printf("第%d次循环错误\n",cnt);
      assert(0);
    }
    cnt++;
  }
  printf("all pass!!!\n");

}

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  test();
  /* Start engine. */
  engine_start();


  return is_exit_status_bad();
}
