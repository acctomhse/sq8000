local_dir 	:= CPU/arch
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= startup_arm.s
lib_src		:= 

include build/common.mk
