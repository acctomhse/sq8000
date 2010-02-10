local_dir 	:= src/PANTHER7-HDMA
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= panther7-hdma-ctrl.c panther7-hdma-menu.c
lib_src		:= 

include build/common.mk
