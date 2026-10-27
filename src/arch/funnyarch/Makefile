CXXFLAGS += -munaligned-access -fno-merge-constants

startup.bin: startup.o
	@$(OBJCOPY) -O binary startup.o startup.bin
