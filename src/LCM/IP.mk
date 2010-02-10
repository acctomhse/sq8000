local_dir	:= src/LCM
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= lcm-ctrl.c lcm-menu.c lcm-t6963c.c lcm-eink-ctrl.c lcm-eink-menu.c
lib_src		:= 

include build/common.mk
