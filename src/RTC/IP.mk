local_dir 	:= src/RTC
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= rtc-ctrl.c rtc-menu.c
lib_src		:= 

include build/common.mk
