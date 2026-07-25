#ifndef __MEMORY_H__
#define __MEMORY_H__
#include <stdint.h>
#define PMEM_SIZE (128*1024*1024)
bool load_image(const char *image_path);
extern uint8_t pmem[PMEM_SIZE];

#endif