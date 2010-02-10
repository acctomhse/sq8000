# Makefile for Socle Diagnostic Program
#
# Copyright (C) 2003-2007 Socle Technology Corp.
#

Q		= @
REVISION	= $(shell cat .revision)

obj_dir		:= build/obj
objs		:= src/main.o src/main-menu.o
srcs		:= src/main.c src/main-menu.c
libs		:= 
depend		= $(subst .c,.d,$(srcs))
cp_obj_dir	:= src CPU/arch platform/arch #lib #lib is for old version make
clean_dir	:= src CPU platform lib
objs_grp	= $(shell find $(obj_dir) -name '*.o')

SED		= sed
MV		= mv -f
RM		= rm -f
ARFLAGS		= qsv

all:

# Tool Chain
# ---------------------------------------------------------------------------
CROSS		= armv5te-softfloat-linux-
#CROSS		= arm-none-linux-gnueabi-
#CROSS		= arm-linux-

NM		= $(CROSS)nm
AR		= $(CROSS)ar
LD		= $(CROSS)ld
ASM		= $(CROSS)as
CC		= $(CROSS)gcc
OBJCOPY		= $(CROSS)objcopy
OBJDUMP		= $(CROSS)objdump
ASMCPP		= $(CROSS)cpp
RANLIB		= $(CROSS)ranlib

LDFLAGS		= -L$(dir $(shell $(CC) -print-libgcc-file-name)) -lgcc
# ---------------------------------------------------------------------------


include .config
sinclude .config.cmd
include build/platform.in
include build/CPU.in
include build/IP.in
sinclude CPU/arch/CPU.mk
sinclude platform/arch/platform.mk
sinclude lib/lib.mk


# CFlags parameter
# ---------------------------------------------------------------------------
# Emdian
ifeq "y" "$(CONFIG_BIG_ENDIAN)"
SYS_ENDIAN	= SYS_CPU_BIG
endif
ifeq "y" "$(CONFIG_LITTLE_ENDIAN)"
SYS_ENDIAN	= SYS_CPU_LITTLE
endif

# SemiHost
ifeq "$(CPU_ARCH)" "CPU_ARCH_MIPS"
SEMI_HOST	= SEMI_HOST_FUNCTION
endif
ifeq "$(CPU_ARCH)" "CPU_ARCH_ARM"
SEMI_HOST	= SEMI_HOST_FUNCTION_ARM
endif

include_dir	:= ./src ./include ./include/lib ./include/platform/arch
INCLUDE		:= $(addprefix -I,$(include_dir)) -include ./include/diag_config.h
# ---------------------------------------------------------------------------


# Flags
# ---------------------------------------------------------------------------


ASFLAGS		= -x assembler-with-cpp -D_ASSEMBLER_

# Endian
ifeq "$(SYS_ENDIAN)" "SYS_CPU_BIG"
CFLAGS		+= -mlong-calls -Wall -gstabs3 -O0 -G0 -EB -msoft-float -fno-builtin -D$(SYS_ENDIAN) -D$(CPU_ARCH) -D$(SEMI_HOST) $(INCLUDE)
LDFLAGS		+= -e start -N -n -EB -Map $@.map
endif


ifeq "$(SYS_ENDIAN)" "SYS_CPU_LITTLE"

# CPU
ifeq "$(CPU_ARCH)" "CPU_ARCH_MIPS"
CFLAGS		+= -mlong-calls -Wall -gstabs3 -O0 -G0 -EL -mips32 -mcpu=4kc -fno-builtin -D$(SYS_ENDIAN) -D$(CPU_ARCH) -D$(SEMI_HOST) $(INCLUDE)
LDFLAGS		+= -e start -N -n -EL -Map $@.map
endif
ifeq "$(CPU_ARCH)" "CPU_ARCH_ARM"
CFLAGS		+= -mapcs-frame -mpoke-function-name -mlong-calls -Wall -O0 -march=armv4 -msoft-float -fno-builtin -D$(SYS_ENDIAN) -D$(CPU_ARCH) -D$(SEMI_HOST) $(INCLUDE) #-Wstrict-prototypes
LDFLAGS		+= -N -n -EL
endif

ifeq "y" "$(CONFIG_ARM7_HI)"
CFLAGS		+= -DTEXT_BASE=0xF0000000
LDFLAGS		+= -Ttext 0xF0000000
else
CFLAGS		+= -DTEXT_BASE=0x00000000
LDFLAGS		+= -Ttext 0x00000000
endif

# Debuger
ifeq "y" "$(CONFIG_SEMIHOST)"
CFLAGS		+= -gdwarf-2
endif
ifeq "y" "$(CONFIG_GDB)"
CFLAGS		+= -gstabs
endif

endif #endif "$(SYS_ENDIAN)" "SYS_CPU_LITTLE"


# UART display
ifeq "y" "$(CONFIG_UART_DISP)"
CFLAGS		+= -DUART_DEBUG
endif
# ---------------------------------------------------------------------------

# That's our default target when none is given on the command line
all: build-ln $(platform_arch)_$(cpu)_v$(REVISION).out 

.PHONY: menuconfig help clean release distclean

ld_file		= build/ELF.ld


# Help
# ---------------------------------------------------------------------------
help:
	@echo "  clean			- delete temporary and dependence files created by build"
	@echo ""
	@echo "  distclean		- delete .obj files created by build"
	@echo ""
	@echo "Configuration:"
	@echo "  menuconfig		- interactive curses-based configurator"
	@echo ""
# ---------------------------------------------------------------------------

# Configuration
# ---------------------------------------------------------------------------
scripts/config/mconf: scripts/config/Makefile
	@$(MAKE) -s -C scripts/config ncurses conf mconf
	-@if [ ! -f .config ] ; then \
		touch .config; \
	fi

menuconfig: scripts/config/mconf
	@$< build/config.in
	@$(MAKE) -s build-ln
# ---------------------------------------------------------------------------

# Clean
# ---------------------------------------------------------------------------
clean:
	@echo -n "Cleaning temporary and dependence files... "
	@find $(clean_dir) -name '*.o' | xargs $(RM)
	@find ./ -name '*.d' | xargs $(RM)
	@find ./ -name '*.*~' | xargs $(RM)
	@find ./ -name '*.tmp' | xargs $(RM)
	@$(RM) *.out *.text *.srec *.bin lib/libio.a
	@$(MAKE) -s -C scripts/config $@
	@echo "finish!"
# ---------------------------------------------------------------------------

# Distclean
# ---------------------------------------------------------------------------
distclean: clean
	@echo -n "Cleaning *.o in $(obj_dir)/... "
	@$(RM) $(obj_dir)/*.o $(platform_arch)_$(cpu)*
	@echo "finish!"
# ---------------------------------------------------------------------------

# target
# ---------------------------------------------------------------------------
$(platform_arch)_$(cpu)_v$(REVISION).out: $(objs) $(libs) cp_objs $(objs_grp)
	@echo LD $(ld_file)
	$(Q)$(LD) -T$(ld_file) $(objs_grp) --start-group  $(libs) --end-group $(LDFLAGS) -o $@
ifeq "$(CPU_ARCH)" "CPU_ARCH_MIPS"
	$(Q)$(OBJDUMP) -h -S -l -m mips --show-raw-insn -EL $@ > $*.text
endif
ifeq "$(CPU_ARCH)" "CPU_ARCH_ARM"
	$(Q)$(OBJDUMP) -h -S -l -m arm --show-raw-insn -EL $@ > $*.text
endif
	$(Q)$(OBJCOPY) -O srec $@ $*.srec
	$(Q)$(OBJCOPY) -O binary $@ $*.bin
	@echo Output files: $@ $*.text $*.srec $*.bin
# ---------------------------------------------------------------------------

#$(warning cyli print: CPU = $(cpu), platform_arch = $(platform_arch), objs = $(objs))

cp_objs:
	@for d in $(cp_obj_dir);	\
	do						\
		find $$d/ -name '*.o' | xargs cp --target-directory=$(obj_dir)/;	\
	done

build-ln:
	@$(RM) include/platform/arch CPU/arch platform/arch
	@ln -fsn $(platform_arch) include/platform/arch
	@ln -fsn $(cpu) CPU/arch
	@ln -fsn $(platform_arch) platform/arch

ifneq "$(MAKECMDGOALS)" "menuconfig"
ifneq "$(MAKECMDGOALS)" "help"
ifneq "$(MAKECMDGOALS)" "clean"
ifneq "$(MAKECMDGOALS)" "distclean"
sinclude $(depend)
endif
endif
endif
endif

%.o: %.c
	@echo CC $@
	$(Q)$(CC) $(CFLAGS) -MD -c $< -o $@

%.o: %.s
	@echo CC $@
	$(Q)$(CC) $(CFLAGS) $(ASFLAGS) -c $< -o $@

# ---------------------------------------------------------------------------
# copy to tftp server
pub:
	cp $(platform_arch)_$(cpu)_v$(REVISION).bin /tftpboot/cyli

