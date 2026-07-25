#include <stdint.h>
#include <stdio.h>
#define PMEM_SIZE (128*1024*1024)
uint8_t pmem[PMEM_SIZE];

bool load_image(const char *image_path) {
  FILE *fp = fopen(image_path, "rb");//打开文件
  if(fp==NULL){
    printf("文件打开失败");
    return false;
  }

  fseek(fp,0,SEEK_END);
  long size = ftell(fp);
  fseek(fp,0,SEEK_SET);

  if (fread(pmem, size, 1, fp));
  fclose(fp);
  return true;
}

