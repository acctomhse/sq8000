local_dir 	:= lib/io_lib
lib_dir		:= lib
lib_name	:= libio.a

local_lib	:= $(lib_dir)/$(lib_name)

# compile source
local_src	:= 
lib_src		:= getchar.c scanf.c vsprintf.c

include build/lib_common.mk
