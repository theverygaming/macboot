CXXFLAGS += -march=68000
LDFLAGS += -nostdlib

kernel.bin: kernel.o
	@$(OBJCOPY) -O binary kernel.o kernel.bin
