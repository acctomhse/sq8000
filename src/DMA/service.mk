local_dir 	:= src/DMA
lib_dir		:= $(local_dir)
lib_name	:= #??.

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= dma.c panther7-hdma.c socle-a2a.c socle-hdma.c
lib_src		:= 

include build/common.mk
