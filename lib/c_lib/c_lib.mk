local_dir 	:= lib/c_lib
lib_dir		:= lib
lib_name	:= libc.a

local_lib	:= $(lib_dir)/$(lib_name)

# compile source
local_src	:= 
lib_src		:= atob.c buffer.c genlib.c isdigit.c qsort.c sprintf.c str_fmt.c strcspn.c strichr.c

include build/lib_common.mk
