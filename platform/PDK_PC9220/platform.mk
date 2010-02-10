local_dir 	:= platform/arch
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= tlb_mapping.s irq.c scu.c
lib_src		:= 

include build/common.mk
