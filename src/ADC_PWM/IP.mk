local_dir 	:= src/ADC_PWM
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= adc_pwmt-ctrl.c adc_pwmt-menu.c adc-ctrl.c pwmt-ctrl.c
lib_src		:= 

include build/common.mk
