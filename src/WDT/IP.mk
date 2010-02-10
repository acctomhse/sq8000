local_dir 	:= src/WDT
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= wdtctrl.c
lib_src		:= 

include build/common.mk
