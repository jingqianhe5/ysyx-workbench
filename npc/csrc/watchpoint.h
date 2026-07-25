#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__


#define NR_WP 32


typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  uint32_t old_val;      // 监视点旧值
  char expressions[100]; // 监视点表达式字符串

} WP;



void init_wp_pool();



WP* new_wp();

void free_wp(int point_num);
void isa_watchpoint_diaplay();

bool check_wp();

#endif