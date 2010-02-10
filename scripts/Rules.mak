SED        ?= sed
HOSTCC     = gcc
ifneq ($(findstring s,$(MAKEFLAGS)),)
SECHO := @-false
DISP  := sil
Q     := @
endif
sil_disp_compile.h = true
disp_compile.h     = $($(DISP)_disp_compile.h)
cmd_compile.h      = $(HOSTCC) $(HOSTCFLAGS) $(INCS) -c -o $@ $<
compile.h          = @$(disp_compile.h) ; $(cmd_compile.h)
