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

#include "sdb.h"
#include <cstddef>
#include <cstdio>
#include <cassert>
#include "expr.h"

#define NR_WP 32//监视点结构数量

typedef struct watchpoint {
  int NO;//监视点编号
  struct watchpoint *next;//下一个监视点结构体地址

  uint32_t old_val;//旧的值
  char expressions[100];//存放表达式

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {//初始化head free
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

static int point_cnt = 0;
WP* new_wp(){//只是在free中取出一个监视点
  if(free_ != NULL){//判断free_中非空
    WP *new_point = free_;
    free_ = new_point->next;
    new_point->next = NULL;
    point_cnt++;
    new_point->NO = point_cnt;
    if(head == NULL){
      head = new_point;
      return new_point;
    }
    WP *last_point = head;
    while(last_point->next != NULL){
      last_point = last_point-> next;
    }
    last_point-> next = new_point;
    return new_point;
  }
  else{
    printf("free_内部没有监视点拉~\n");
    assert(0);//直接退出
    return NULL;
  }
}


void free_wp(int point_num){
  if(head == NULL){
    printf("现在没有监视点\n");
    return ;
  }
  WP *last_point = head;//初始化 用于遍历寻找上一个节点
  WP *now_point = head->next;//记录当前的点
  if(last_point->NO==point_num){//第一个点就是目标
    head = last_point->next;
    last_point->next = free_;
    free_ = last_point;
    point_cnt--;

    last_point = head;//从第二个开始 修改编号
    while(last_point!=NULL){//修改后续的点的编号 让编号顺序排序
    last_point->NO = last_point->NO-1;
    last_point=last_point->next;
    }
    return ;
  }
  else{
    if(now_point == NULL){//如果只有一个点 目标编号比这个大 那么就会进入这里 
        printf("没有这个监视点\n");
        return ;
    }
    while(now_point->NO!=point_num){//找到这个点
      last_point = now_point;
      now_point = now_point->next;
      if(now_point == NULL){
        printf("没有这个监视点\n");
        return;
      }
    }
  }
  last_point->next = now_point->next;
  now_point->next = free_;
  free_ = now_point;
  point_cnt--;
  
  last_point = last_point->next;
  while(last_point!=NULL){//修改后续的点的编号 让编号顺序排序
    last_point->NO = last_point->NO-1;
    last_point=last_point->next;
  }
  return;
}

bool check_wp(){
  WP *check_position= head;
  bool change=false;
  while(check_position!=NULL){
    bool success=false;
    uint32_t new_val = expr(check_position->expressions,&success);
    if(check_position->old_val==new_val){//比较是否改变
      check_position = check_position->next;
    }
    else{//改变了
      change = true ;
      printf("---------------------\n");
      printf("监视点%d发生了变化 \n原来的值是%08x \n新的值是%08x\n",check_position->NO,check_position->old_val,new_val);
      printf("---------------------\n");
      check_position->old_val=new_val;//更新值
      check_position = check_position->next;
    }
  }
  return change;
}

void isa_watchpoint_diaplay(){
  printf("编号    表达式    值\n");
  WP* point_out = head;
  while(point_out != NULL){
    printf("%d        %s        %u\n",point_out->NO,point_out->expressions,point_out->old_val);
    point_out = point_out->next;
  }
}
