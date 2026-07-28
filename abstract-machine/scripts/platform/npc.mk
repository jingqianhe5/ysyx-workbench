AM_SRCS := riscv/npc/start.S \
           riscv/npc/trm.c \
           riscv/npc/ioe.c \
           riscv/npc/timer.c \
           riscv/npc/input.c \
           riscv/npc/cte.c \
           riscv/npc/trap.S \
           platform/dummy/vme.c \
           platform/dummy/mpe.c

CFLAGS    += -fdata-sections -ffunction-sections
LDSCRIPTS += $(AM_HOME)/scripts/linker.ld
LDFLAGS   += --defsym=_pmem_start=0x80000000 --defsym=_entry_offset=0x0
LDFLAGS   += --gc-sections -e _start

MAINARGS_MAX_LEN = 64
MAINARGS_PLACEHOLDER = the_insert-arg_rule_in_Makefile_will_insert_mainargs_here
CFLAGS += -DMAINARGS_MAX_LEN=$(MAINARGS_MAX_LEN) -DMAINARGS_PLACEHOLDER=$(MAINARGS_PLACEHOLDER)

insert-arg: image
	@python $(AM_HOME)/tools/insert-arg.py $(IMAGE).bin $(MAINARGS_MAX_LEN) $(MAINARGS_PLACEHOLDER) "$(mainargs)"

image: image-dep
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin


NPC_HOME := $(abspath $(AM_HOME)/../npc)
NPC_V    := $(wildcard $(NPC_HOME)/vsrc/*.v)
NPC_C   := $(wildcard $(NPC_HOME)/csrc/*.c)
NPC_CPP := $(wildcard $(NPC_HOME)/csrc/*.cpp)
NPC_SRC := $(NPC_C) $(NPC_CPP)
NPC_EXEC := obj_dir/Vysyx_26010032_npc
CAPSTONE_HOME := $(abspath $(AM_HOME)/../nemu/tools/capstone/repo)
CAPSTONE_INC  := $(CAPSTONE_HOME)/include
CAPSTONE_LIB  := $(CAPSTONE_HOME)/libcapstone.so.5
NEMU_HOME := $(abspath $(AM_HOME)/../nemu)
NEMU_REF  := $(NEMU_HOME)/build/riscv32-nemu-interpreter-so


run: insert-arg
	verilator --trace-fst --cc --exe --build -j 0 \
		-Wall -Wno-fatal -Wno-DECLFILENAME -Wno-UNUSEDPARAM \
		-CFLAGS "-I$(CAPSTONE_INC) -I$(NEMU_HOME)/include" \
		-LDFLAGS "-lreadline -ldl $(CAPSTONE_LIB) -Wl,-rpath,$(CAPSTONE_HOME)" \
		--top-module ysyx_26010032_npc \
		-I$(NPC_HOME) $(NPC_SRC) $(NPC_V)
	$(NPC_EXEC) $(IMAGE).bin $(NEMU_REF)

.PHONY: insert-arg
