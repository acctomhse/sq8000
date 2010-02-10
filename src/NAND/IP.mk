local_dir	:= src/NAND
lib_dir		:= $(local_dir)/rscode-1.0_socle
lib_name	:= libecc.a

local_lib	:= $(lib_dir)/$(lib_name)

# compile source
local_src	:= nand-flsh-ctrl.c nand-flsh-menu.c
lib_src		:= rs.c galois.c berlekamp.c

include build/common.mk
