local_dir 	:= src/SDMMC
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= sdmmc-ctrl.c sdmmc-menu.c
lib_src		:= 

include build/common.mk
