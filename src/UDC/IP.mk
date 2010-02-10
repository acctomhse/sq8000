local_dir 	:= src/UDC
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= usbctrl.c
lib_src		:= 

include build/common.mk
