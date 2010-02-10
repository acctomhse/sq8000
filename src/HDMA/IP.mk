local_dir 	:= src/HDMA
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= hdma-ctrl.c hdma-menu.c
lib_src		:= 

include build/common.mk
