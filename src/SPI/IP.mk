local_dir 	:= src/SPI
lib_dir		:= $(local_dir)
lib_name	:= #??.a

local_lib	:= #$(lib_dir)/$(lib_name)

# compile source
local_src	:= spi-master.c eeprom-test.c marvel-wifi-test.c tsc2000-test.c socle-spi-slave-test.c spi-master-menu.c spi-master-ctrl.c spi-slave-ctrl.c spi-slave-menu.c
lib_src		:= 

include build/common.mk
