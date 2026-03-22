#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  //panic("Not implemented");
  char buf[1024];
  va_list args;
  va_start(args,fmt);
  int len = vsprintf(buf,fmt,args);
  va_end(args);

  for (int i=0 ;i<len;i++){
    putch(buf[i]);
  }
  return len;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  //panic("Not implemented");
  const char *process = fmt;
  char *output = out;
  while(*process!='\0'){
    if(*process=='%'){
      process++;
      if(*process=='d'){//如果是d
        char temp[32];
        int i = 0;
        int val = va_arg(ap, int);
        if(val==0){
          *output = '0';
          output++;
        }
        else if(val < 0){
          *output = '-';
          val = -val;
          output++;
        }
        while(val >0){
          temp[i++]=(val%10)+'0';
          val /=10;
        }
        while(i>0){
          i--;
          *output = temp[i];
          output++;
        }
        process++;
      }
      else if(*process=='s'){//如果是s
        char *str = va_arg(ap, char*);
        while(*str!='\0'){
          *output = *str;
          output++;
          str++;
        }
        process++;
      }
      continue;
    }
    *output = *process;
    output++;
    process++;
  }
  *output='\0';
  return (int)(output - out);
}

int sprintf(char *out, const char *fmt, ...) {//个人实现
  //panic("Not implemented");
  va_list args;
  va_start (args,fmt);

  int len =vsprintf(out,fmt,args);
  va_end(args);
  return len;
  
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
