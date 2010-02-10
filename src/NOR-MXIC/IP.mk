local_dir 	:= src/NOR-MXIC
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= nor-mxic-ctrl.c nor-mxic-menu.c
lib_src		:= 

include build/common.mk
