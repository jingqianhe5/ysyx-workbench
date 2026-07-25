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

//#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include "expr.h"

#include <assert.h>
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reg.h"
#include "sdb.h"
//#include <memory/paddr.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_DEC,TK_NEQ,TK_AND,TK_MULT,TK_DERE,TK_HEX,TK_REG,//十进制数字类型

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"-",'-'}  ,           // sub
  {"\\*",'*'} ,          // multiplied / 解引用
  {"/",'/'}    ,         // div
  {"\\(",'('}   ,        // left
  {"\\)",')'}    ,       // right
  {"0x[0-9a-fA-F]+",TK_HEX},//HEX
  {"[0-9]+",TK_DEC},     // DEC_number  
  {"\\&\\&" ,TK_AND }   ,    //&&
  {"\\!=" ,TK_NEQ},        //no equal
  {"\\$[0-9a-zA-Z]+",TK_REG},//REG

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
        fprintf(stderr, "regex compilation failed: %s\n%s\n",
        error_msg, rules[i].regex);
        exit(EXIT_FAILURE);
    }
  }
}
//token结构体
typedef struct token {
  int type;//记录token类型 存放==或者是其他的两个字符的运算
  char str[32];//记录数字大小 长度是有限的
} Token;

static Token tokens[65536] __attribute__((used)) = {};//用于顺序存放已经识别出来的token信息
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {//识别表达式的token
  int position = 0;//表示当前识别到的位置
  int tokens_position = 0;//数组当前记录到的位置
  int i;
  regmatch_t pmatch;

  nr_token = 0;//已经识别出来的token数量

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {//正则匹配
        //char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //    i, rules[i].regex, position, substr_len, substr_len, substr_start);//输出识别成功的信息
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.//我还需要记录我的识别结果到我的tokens数组里面
         */
         if(rules[i].token_type!=TK_NOTYPE){//空格不要
          nr_token += 1;//识别成功之后 总数需要加一  但是同时希望不要添加空格进入

          tokens[tokens_position].type = rules[i].token_type;//储存类型的数字
          if(rules[i].token_type == TK_DEC||rules[i].token_type == TK_HEX||rules[i].token_type == TK_REG){//十六进制 十进制 寄存器
            strncpy(tokens[tokens_position].str, e + position - substr_len, substr_len);
            tokens[tokens_position].str[substr_len] = '\0';
            }
          
          tokens_position +=1;
        }

        switch (rules[i].token_type) {
          case(TK_NOTYPE):break;
          case('+'):break;
          case(TK_EQ):break;
          case('-'):break;
          case('*'):break;
          case('/'):break;
          case('('):break;
          case(')'):break;
          case(TK_HEX):break;
          case(TK_REG):break;
          case(TK_DEC):break;
          case(TK_AND):break;
          case(TK_NEQ):break;
          case(TK_DERE):break;
          default: printf("暂时无法识别你的表达式\n");
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  //识别输出内容
  //for(int k=0;k<nr_token;k++){
  //  printf("类型编号是%d   内容是：%s\n",tokens[k].type,tokens[k].str);
//
  //}
  //用于输出识别的内容

  return true;
}
//--------------------------------
//括号检查
//--------------------------------
static bool check_parentheses(int p,int q){
  int flag=0;
  int flag_0=0;
  if(tokens[p].type=='('&&tokens[q].type==')'){
    for (int i=p;i<=q;i++){
      if(tokens[i].type=='('){flag++;}
      if(tokens[i].type==')'){flag--;}
      if(flag<0){
        printf("表达式不满足数学要求\n");
        assert(0);
      }
      if(flag==0&&i!=q){flag_0=1;}
    }
    if(flag>0){
        printf("表达式不满足数学要求\n");
        assert(0);
    }
    if(flag_0){return false;}
    return true;
  }
  else {
    for (int i=p;i<=q;i++){
      if(tokens[i].type=='('){flag++;}
      if(tokens[i].type==')'){flag--;}
      if(flag<0){
        printf("表达式不满足数学要求\n");
        assert(0);
      }
    }
    if(flag>0){
        printf("表达式不满足数学要求\n");
        assert(0);
    }
      return false;
  }
}
//--------------------------------
//嵌套计算函数
//--------------------------------
static uint32_t eval(int p, int q) {
  if (p > q) {
    printf("出现计算范围反常 错误发生时 p(左边)为%d q(右边)为%d\n",p,q);
    return 999;
  }
  else if (p == q) {//单字情况
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
        if(tokens[p].type == TK_HEX){//十六进制数据
           return strtol(tokens[p].str,NULL,16);
        }
        else if(tokens[p].type == TK_DEC){//十进制数据
          uint32_t number=0;
          for (int i = 0;tokens[p].str[i]!='\0';i++){ 
            number=number*10+(tokens[p].str[i]-'0');//char转换为数字
          }
          return number;
        }
        else {//寄存器数据
          bool success = false;
          return isa_reg_display(tokens[p].str,&success);
        }
  }
  else if (check_parentheses(p, q) == true) {//1 检查括号是否满足数学上的标准 2 检查最左边和最右边是不是括号（是否需要去括号）
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */


    return eval(p + 1, q - 1);//去括号
  }
  else {
    int op=p;
    int op_type=0;//设定一个初始值 用来判断运算的类型
    int  in_pare=0;//判断是不是在括号内部
    for(int i = p;i<=q;i++){//找出主运算符  也就是判断运算的优先级
      if(tokens[i].type=='('){in_pare ++;}
      if(tokens[i].type==')'){in_pare --;}//判断是不是在括号里面

      if((tokens[i].type!='('&&tokens[i].type!=')'&&tokens[i].type!=TK_DEC&&tokens[i].type!=TK_HEX&&tokens[i].type!=TK_REG)&&!in_pare){
        if(tokens[i].type=='*'){//如果是* 首先判断是乘法还是解引用 只有在这作区分
          if(i==0||(tokens[i-1].type!=TK_DEC&&tokens[i-1].type!=TK_HEX&&tokens[i-1].type!=TK_REG && tokens[i-1].type!=')')){//如果这个符号前面是空的或者不是一个数字 那么就是解引用
            if(op_type!='/'&&op_type!='*'&&op_type!='+' && op_type!='-' && op_type!=TK_EQ && op_type!=TK_NEQ && op_type!=TK_AND){
              op = i;
              op_type = TK_DERE;//解引用
            }
          }
          else{//乘法
            if(op_type!='+' && op_type!='-' && op_type!=TK_EQ && op_type!=TK_NEQ && op_type!=TK_AND){
              op = i;
              op_type = TK_MULT;
            }
          }

        }

        if(tokens[i].type==TK_AND){//最低级运算 &&
          op = i;
          op_type = tokens[i].type;
        }
        else if ((tokens[i].type == TK_EQ ||tokens[i].type == TK_NEQ)&& op_type!=TK_AND){//高一级运算 不等于和等于
          op = i;
          op_type = tokens[i].type;
        }
        else if ((tokens[i].type=='+'||tokens[i].type=='-')&&(op_type!=TK_EQ && op_type!=TK_NEQ && op_type!=TK_AND)){//在高一级运算 加减法
          op = i;
          op_type = tokens[i].type;
        }
        else if((tokens[i].type=='/')&&(op_type!='+' && op_type!='-' && op_type!=TK_EQ && op_type!=TK_NEQ && op_type!=TK_AND)){//在高一级运算 除法
          op = i;
          op_type = tokens[i].type;
        }
      }
    }
    uint32_t val1 = 0;
    uint32_t val2 = eval(op + 1, q);

    if(op_type!=TK_DERE){
      val1 = eval(p, op - 1);//只有不是解引用才计算val1的值
    }

    switch (op_type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case TK_MULT: return val1 * val2;
      case '/': if(val2 == 0){return 0;}return val1 / val2;
      case TK_DERE:return pmem_read(val2,4);//右结合 解引用
      case TK_EQ:return val1==val2;
      case TK_NEQ:return val1!=val2;
      case TK_AND:return val1&&val2;
      default : return 0;
    }
  }
}


uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  uint32_t result=eval(0,nr_token-1);
  /* TODO: Insert codes to evaluate the expression. */



  //这里我需要实现一个计算结果的功能 暂时注释TODO（）
  //TODO();
  *success = true;
  return result;
}
