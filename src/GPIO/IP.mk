local_dir	:= src/GPIO
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= gpio.c gpio-ctrl.c gpio-menu.c gpio-kpd-ctrl.c
lib_src		:= 

include build/common.mk
