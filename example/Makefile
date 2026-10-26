ifndef ARCH
$(error ARCH is not set)
endif
CXX = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
CXXFLAGS = -I./include -Os -nostdlib -nostdinc -fno-pie -fno-pic -fno-stack-protector -ffreestanding -fno-threadsafe-statics -fno-exceptions -Wall

_FIND_WITH_ARCH = find ./src -path './src/arch/*' ! -path './src/arch/$(ARCH)' ! -path './src/arch/$(ARCH)/*' -prune -o
SOURCES_CPP = $(shell $(_FIND_WITH_ARCH) -name "*.cpp" -print)

OBJECTS_CPP = $(patsubst ./src/%, ./obj/%,$(patsubst %.cpp, %.o, $(SOURCES_CPP)))

include ./src/arch/$(ARCH)/Makefile

all:
	@mkdir -p obj
	@$(MAKE) kernel

kernel: $(OBJECTS_CPP)
	@$(LD) -T ./src/arch/$(ARCH)/linker.ld $(LDFLAGS) $^ -o kernel.o
	@$(OBJCOPY) -O binary kernel.o kernel

obj/%.o: src/%.cpp
	@echo "    CXX $<"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -f kernel
	@find . -name '*.o' -type f -delete
