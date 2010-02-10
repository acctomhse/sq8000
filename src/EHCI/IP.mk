local_dir	:= src/EHCI
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= ehcictrl.c ehcimem.c ehci-menu.c
lib_src		:= 

include build/common.mk
