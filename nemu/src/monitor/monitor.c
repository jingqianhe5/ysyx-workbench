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

#include <isa.h>
#include <memory/paddr.h>
#include <elf.h>

void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm();

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ANSI_FMT(str(__GUEST_ISA__), ANSI_FG_YELLOW ANSI_BG_RED));
  printf("For help, type \"help\"\n");
  //Log("Exercise: Please remove me in the source code and compile NEMU again.");
  //assert(0);你不是很厉害吗？怎么一下子就比我找到了？小样还以为我找不到吗？
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>

void sdb_set_batch_mode();

static char *log_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;

static long load_img() {
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    {"help"     , no_argument      , NULL, 'h'},
    {"elf"      , required_argument, NULL, 'e'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  char*elf_file=NULL;
  void ftrace_init(char *elf_file);
  while ( (o = getopt_long(argc, argv, "-bhl:d:p:e:", table, NULL)) != -1) {
    switch (o) {
      case 'b': sdb_set_batch_mode(); break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
      case 'e': elf_file = optarg;ftrace_init(elf_file);break;
      case 1: img_file = optarg; return 0;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\t-e,--elf=FILE           run ELF FILE for ftrace\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}




typedef struct {
    char name[64];
    uint32_t addr;
    uint32_t size;
} FuncSymbol;

FuncSymbol func_table[1024]; 
int func_cnt = 0;            

void ftrace_init(char *elf_file){
  #ifdef CONFIG_FTRACE_COND
    if (!FTRACE_COND||elf_file==NULL) { 
        return ;
    }
    else {
      FILE *fp = fopen(elf_file,"rb");
      assert(fp != NULL);
      Elf32_Ehdr ehdr;
      if(fread(&ehdr,sizeof(Elf32_Ehdr),1,fp)<=0){
        assert(0);
      }
      assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);

      Elf32_Shdr *shdrs = malloc(sizeof(Elf32_Shdr) * ehdr.e_shnum);
      fseek(fp,ehdr.e_shoff,SEEK_SET);
      if (fread(shdrs, sizeof(Elf32_Shdr), ehdr.e_shnum, fp) != ehdr.e_shnum) {
          assert(0); 
      }

      Elf32_Shdr *symtab_hdr = NULL;
      Elf32_Shdr *strtab_hdr = NULL;
      for(int i = 0 ;i < ehdr.e_shnum;i++){
        if(shdrs[i].sh_type == SHT_SYMTAB){
          symtab_hdr = &shdrs[i];//符号表
          strtab_hdr = &shdrs[symtab_hdr ->sh_link];//字符串表
          break;
        }
      }
      char *strtab = malloc(strtab_hdr ->sh_size);
      fseek(fp,strtab_hdr -> sh_offset,SEEK_SET);
      if (fread(strtab, 1, strtab_hdr->sh_size, fp) != strtab_hdr->sh_size) {
          assert(0);
      }

      int sym_num = symtab_hdr->sh_size / symtab_hdr->sh_entsize; 
      Elf32_Sym *syms = malloc(symtab_hdr->sh_size);
      fseek(fp, symtab_hdr->sh_offset, SEEK_SET);
      if (fread(syms, sizeof(Elf32_Sym), sym_num, fp) != sym_num) {
          assert(0);
      }

      for(int i = 0;i<sym_num;i++){
        if(ELF32_ST_TYPE(syms[i].st_info)==STT_FUNC){
          char*func_name = strtab + syms[i].st_name;
          uint32_t func_addr = syms[i].st_value;
          uint32_t func_size = syms[i].st_size;
          if (func_cnt < 1024) {
                strncpy(func_table[func_cnt].name, func_name, 63);
                func_table[func_cnt].name[63] = '\0'; // 确保安全结束
                func_table[func_cnt].addr = func_addr;
                func_table[func_cnt].size = func_size;
                func_cnt++;
          } 
        }
      }
      free(shdrs);
      free(strtab);
      free(syms);
      fclose(fp);
    }
  #endif
}
int deep = 0;
void find_name(int rd,int rs1,uint32_t current_pc,uint32_t target_pc){
    if(rd == 1){
      for (int i = 0;i < func_cnt;i++){
      uint32_t start = func_table[i].addr;
      uint32_t end   = start + func_table[i].size;

        if(target_pc >= start && target_pc <end){
          log_write("Call:0x%08x----->0x%08x %*sname:%s\n", current_pc,target_pc,deep*2,"  ",func_table[i].name);
          deep++;      
          break;  
        }
      }
    }
    else if(rd==0 && rs1 ==1){
      deep--;
      for (int i = 0;i < func_cnt;i++){
      uint32_t start = func_table[i].addr;
      uint32_t end   = start + func_table[i].size;

        if(current_pc >= start && current_pc <end){
          log_write("RET:0x%08x----->0x%08x %*sname:%s\n", current_pc,target_pc,deep*2,"  ",func_table[i].name);   
          break;  
        }
      }
    }
}




void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Set random seed. */
  init_rand();

  /* Open the log file. */
  init_log(log_file);

  /* Initialize memory. */
  init_mem();

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device());

  /* Perform ISA dependent initialization. */
  init_isa();

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Initialize the simple debugger. */
  init_sdb();

  IFDEF(CONFIG_ITRACE, init_disasm());

  /* Display welcome message. */
  welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();
  init_mem();
  init_isa();
  load_img();
  IFDEF(CONFIG_DEVICE, init_device());
  welcome();
}
#endif
