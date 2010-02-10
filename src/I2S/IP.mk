local_dir	:= src/I2S
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= i2s-ctrl.c i2s-menu.c uda1342ts.c ms6335.c codec-menu.c codec-ctrl.c
lib_src		:= 

include build/common.mk
