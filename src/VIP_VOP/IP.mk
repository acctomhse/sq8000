local_dir 	:= src/VIP_VOP
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= vip-vop-menu.c vip.c vop.c vip-vop-ctrl.c  
lib_src		:= 

include build/common.mk
