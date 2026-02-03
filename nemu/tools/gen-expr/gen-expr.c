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
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURpos_nemuE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf_nemu[65536] = {};
static char buf_gcc[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"#include <stdint.h>\n"
"int main() { "
"  uint32_t result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static uint32_t pos_nemu = 0;
static uint32_t pos_gcc = 0;
static uint32_t choose(int num){
  return rand()%num;
}


static void gen(char c){//填写一个字符进入buf
  buf_nemu[pos_nemu]=c;
  buf_gcc[pos_gcc]=c;
  pos_gcc++;
  pos_nemu++;
}

static void gen_num(){//生成一个随机数字
  uint32_t num = choose(7);//数字不大于
  char str_nemu[32];
  char str_gcc[32];
  sprintf(str_gcc,"%uu",num);//现在对于每一个数字我都需要进行无符号运算
  sprintf(str_nemu,"%u",num);//现在对于每一个数字我都需要进行无符号运算
  int len_gcc =strlen(str_gcc);
  int len_nemu =strlen(str_nemu);
  strcpy(buf_nemu+pos_nemu,str_nemu);
  strcpy(buf_gcc+pos_gcc,str_gcc);
  pos_gcc+=len_gcc; 
  pos_nemu+=len_nemu;
  switch (choose(2)){//有一半的概率生成空格
    case 0 :gen(' ');break;
    default:
  }
}

static void gen_rand_op(){//生成运算符号
  switch (choose(4)) {
    case 0:gen('+');break;
    case 1:gen('-');break;
    case 2:gen('*');break;
    case 3:gen('/');break;
    default:
  }
  switch (choose(2)){//有一半的概率生成空格
    case 0 :gen(' ');break;
    default:
  }
}



static void gen_rand_expr(int n) {//随机生成的表达式  设置递归深度 防止太大
  if(n==0){
    gen_num();
    return ;
  }
  switch (choose(3)) {
    case 0: gen_num(); break;//数字生成
    case 1: gen('('); gen_rand_expr(n-1); gen(')'); break;//括号生成
    default: gen_rand_expr(n-1); gen_rand_op(); gen_rand_expr(n-1); break;//符号生成
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);//生成随机数
  srand(seed);
  int loop = 1;//循环次数
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);//输入循环次数
  }
  int i;
  for (i = 0; i < loop; i ++) {//开始循环
    pos_nemu = 0;//初始化开始存储的buf位置
    pos_gcc = 0;//初始化开始存储的buf位置
    gen_rand_expr(10);//设置嵌套递归深度为10

    buf_nemu[pos_nemu]='\0';//递归结束之后设置结束符
    buf_gcc[pos_gcc]='\0';//递归结束之后设置结束符

    sprintf(code_buf, code_format, buf_gcc);//把buf的内容填写到模板的%s处 最终的code_buf就是最后生成的代码

    FILE *fp = fopen("/tmp/.code.c", "w");//打开临时文件 写
    assert(fp != NULL);//断言 可以打开否则终止程序
    fputs(code_buf, fp);//把生成的代码写进文件
    fclose(fp);//关闭文件

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");//在shell执行命令 编译生成的C文件
    if (ret != 0) {
      i=i-1;
      continue;//如果编译失败 跳过本次循环
    }

    fp = popen("/tmp/.expr", "r");//启动子进程 运行expr 并且读取输出
    assert(fp != NULL);//失败就终止程序

    uint32_t result;
    ret = fscanf(fp, "%u", &result);//从子程序输出中读取结果
    pclose(fp);//关闭程序

    if (ret != 1) {
      i=i-1;
      continue;//失败就关闭 
      }

    printf("%u %s\n ", result, buf_nemu);//输出 结果以及buf内容 输出到input文件之中
  }
  
  return 0;
}
