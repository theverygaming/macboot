CXXFLAGS += -march=68000
LDFLAGS += -nostdlib

startup.bin: startup.o
	@$(OBJCOPY) -O binary startup.o startup.bin
	@truncate -s 12288 startup.bin
