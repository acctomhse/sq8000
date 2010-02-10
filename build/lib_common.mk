local_src	:= $(addprefix $(local_dir)/,$(local_src))
local_obj	:= $(subst .c,.o,$(filter %.c,$(local_src)))	\
				$(subst .s,.o,$(filter %.s,$(local_src)))

lib_src		:= $(addprefix $(local_dir)/,$(lib_src))
lib_obj		:= $(subst .c,.o,$(filter %.c,$(lib_src)))	\
				$(subst .s,.o,$(filter %.s,$(lib_src)))

srcs 	+= $(filter %.c,$(local_src) $(lib_src))
objs 	+= $(local_obj)
#libs	+= $(local_lib)

# $(call create-lib, lib_name, lib_obj)
define create-lib
$1: $2
	$(Q)$(RM) $$@
	$(Q)$(AR) $(ARFLAGS) $$@ $$^
endef

$(eval $(call create-lib,$(local_lib),$(lib_obj)))
