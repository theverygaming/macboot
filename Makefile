ifndef ARCH
$(error ARCH is not set)
endif
CXX = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
# -Wno-array-bounds due to "array subscript 0 is outside array bounds" gcc bug (at least afaik it's a gcc bug)
CXXFLAGS = -I./include -Os -std=c++11 -nostdlib -nostdinc -fno-pie -fno-pic -fno-stack-protector -ffreestanding -fno-threadsafe-statics -fno-exceptions -Wall -Wpedantic -Wno-array-bounds

_FIND_WITH_ARCH = find ./src -path './src/arch/*' ! -path './src/arch/$(ARCH)' ! -path './src/arch/$(ARCH)/*' -prune -o
SOURCES_CPP = $(shell $(_FIND_WITH_ARCH) -name "*.cpp" -print)

OBJECTS_CPP = $(patsubst ./src/%, ./obj/%,$(patsubst %.cpp, %.o, $(SOURCES_CPP)))

SOURCES_ASM = $(shell $(_FIND_WITH_ARCH) \( -name "*.S" -o -name "*.s" \) -print)

OBJECTS_ASM = $(patsubst ./src/%, ./obj/%,$(patsubst %.S, %.o, $(patsubst %.s, %.o, $(SOURCES_ASM))))

include ./src/arch/$(ARCH)/Makefile

all: 
	@mkdir -p obj
	@$(MAKE) startup

startup: $(OBJECTS_ASM) $(OBJECTS_CPP)
	@$(LD) -T ./src/arch/$(ARCH)/linker.ld -nostdlib $^ -o startup.o
	@$(OBJCOPY) -O binary startup.o startup
	@truncate -s 12288 startup

obj/%.o: src/%.cpp
	@echo "    CXX $<"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.S
	@echo "    AS  $<"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.s
	@echo "    AS  $<"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -f startup
	@find . -name '*.o' -type f -delete
