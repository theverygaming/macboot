CXXFLAGS += -munaligned-access -fno-merge-constants

kernel.bin: kernel.o
	@$(OBJCOPY) -O binary kernel.o kernel.bin
