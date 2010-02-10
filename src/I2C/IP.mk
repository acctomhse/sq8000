local_dir	:= src/I2C
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= i2c.c i2c-ctrl.c i2c-menu.c TVP5145.c CH700X.c tps62353.c tps62353-ctrl.c tps62353-menu.c Si4703-ctrl.c Si4703-menu.c
lib_src		:= 

include build/common.mk
