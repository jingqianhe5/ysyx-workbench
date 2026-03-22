#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {//个人实现
  //panic("Not implemented");
  char *process = dst;
  while (*src!='\0'){
    *process = *src;
    process++;
    src++;
  }
  *process = *src;
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {//个人实现
  //panic("Not implemented");
  char *process = dst;
  while(*process!='\0'){
    process++;
  }
  while(*src!='\0'){
    *process = *src;
    process++;
    src++;
  }
  *process = *src;
  return dst;
}

int strcmp(const char *s1, const char *s2) {//个人实现
  //panic("Not implemented");
  while(*s1==*s2){
    if(*s1=='\0'){
      return 0;
    }
    s1 +=1;
    s2 +=1;
  }
  return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {//个人实现
  //panic("Not implemented");
  char *process = s;
  for (int i=0;i<n;i++){
    *(process+i)=c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  //panic("Not implemented");
  uint8_t *dst_1 = (uint8_t*)dst;
  const uint8_t *src_1 = (const uint8_t*)src;

  if(dst_1>src_1&&src_1+n>dst_1){
    dst_1 = dst_1 + n - 1;
    src_1 = src_1 + n - 1;
    for (size_t i = 0;i<n;i++){
      *dst_1 = *src_1;
      dst_1--;
      src_1--;
    }
  }
  else{
    for(size_t i = 0;i<n;i++){
      *dst_1 = *src_1;
      dst_1++;
      src_1++;
    }
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  //panic("Not implemented");
  uint8_t *d = (uint8_t *)out;
  const uint8_t *s = (const uint8_t *)in;

  for(size_t i = 0 ;i < n ;i++){
    *d=*s;
    d++;
    s++;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {//个人实现
  //panic("Not implemented");
  const unsigned char *process1 = s1;
  const unsigned char *process2 = s2;
  for (int i = 0; i<n;i++){
    if(*(process1+i) != *(process2+i)){
      return (*(process1+i)-*(process2+i));
    }
  }
  return 0;
}

#endif
