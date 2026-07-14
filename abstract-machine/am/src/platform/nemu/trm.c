#include <am.h>
#include <nemu.h>

extern char _heap_start;
int main(const char *args);

Area heap = RANGE(&_heap_start, PMEM_END);
static const char mainargs[MAINARGS_MAX_LEN] = TOSTRING(MAINARGS_PLACEHOLDER); // defined in CFLAGS

void putch(char ch) {//输出一个字符
  outb(SERIAL_PORT, ch);
}

void halt(int code) {//结束程序的运行
  nemu_trap(code);

  // should not reach here
  while (1);
}

void _trm_init() {//初始化TRM
  int ret = main(mainargs);
  halt(ret);
}
