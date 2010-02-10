local_dir 	:= lib/socle_lib
lib_dir		:= lib
lib_name	:= libsocle.a

local_lib	:= $(lib_dir)/$(lib_name)

# compile source
local_src	:= 
lib_src		:= test_item.c board.c

include build/lib_common.mk
