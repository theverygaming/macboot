ifndef ARCH
$(error ARCH is not set)
endif
CXX = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
CXXFLAGS = -I./include -I./src/arch/$(ARCH)/include -Os -nostdlib -nostdinc -fno-pie -fno-pic -fno-stack-protector -ffreestanding -fno-threadsafe-statics -fno-exceptions -Wall

_FIND_WITH_ARCH = find ./src -path './src/arch/*' ! -path './src/arch/$(ARCH)' ! -path './src/arch/$(ARCH)/*' -prune -o
SOURCES_CPP = $(shell $(_FIND_WITH_ARCH) -name "*.cpp" -print)

OBJECTS_CPP = $(patsubst ./src/%, ./obj/%,$(patsubst %.cpp, %.o, $(SOURCES_CPP)))

all:
	@mkdir -p obj
	@$(MAKE) kernel.bin module1.bin module2.bin

include ./src/arch/$(ARCH)/Makefile

kernel.o: $(OBJECTS_CPP)
	@$(LD) -T ./src/arch/$(ARCH)/linker.ld $(LDFLAGS) $^ -o kernel.o

module1.bin module2.bin:
	echo -en "module 1\0" > module1.bin
	echo -en "this is module 2\0" > module2.bin

obj/%.o: src/%.cpp
	@echo "    CXX $<"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -f kernel.bin module1.bin module2.bin
	@find . -name '*.o' -type f -delete
