CXX = m68k-elf-g++
LD = m68k-elf-ld
OBJCOPY = m68k-elf-objcopy
CXXFLAGS = -I./include -Os -march=68000 -nostdlib -nostdinc -fno-pie -fno-pic -fno-stack-protector -ffreestanding -fno-threadsafe-statics -fno-exceptions -Wall -g

HEADERS_CPP = $(shell find ./src -name "*.h") # fuck it, lets just do this..

SOURCES_CPP = $(shell find ./src -name "*.cpp")

OBJECTS_CPP = $(patsubst ./src/%, ./obj/%,$(patsubst %.cpp, %.ocpp, $(SOURCES_CPP)))

SOURCES_ASM = $(shell find ./src -name "*.S")

OBJECTS_ASM = $(patsubst ./src/%, ./obj/%,$(patsubst %.S, %.oS, $(SOURCES_ASM)))

all: 
	@mkdir -p obj
	@$(MAKE) kernel

kernel: $(OBJECTS_ASM) $(OBJECTS_CPP)
	@$(LD) -T linker.ld -nostdlib $^ -o kernel.o
	@$(OBJCOPY) -O binary kernel.o kernel

obj/%.ocpp: src/%.cpp $(HEADERS_CPP) 
	@echo "    CXX $<"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.oS: src/%.S
	@echo "    AS $<"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -f kernel kernel.o
	@find . \( -name '*.oS' -o -name '*.ocpp' \) -type f -delete

proper: clean
