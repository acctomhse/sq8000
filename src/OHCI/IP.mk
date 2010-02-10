local_dir	:= src/OHCI
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= ohcictrl.c ohcimem.c ohci-menu.c
lib_src		:= 

include build/common.mk
