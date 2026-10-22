CXX = m68k-unknown-linux-gnu-g++
LD = m68k-unknown-linux-gnu-ld
OBJCOPY = m68k-unknown-linux-gnu-objcopy
CXXFLAGS = -I./include -Os -march=68000 -nostdlib -nostdinc -fno-pie -fno-pic -fno-stack-protector -ffreestanding -fno-threadsafe-statics -fno-exceptions -Wall -g

HEADERS_CPP = $(shell find ./src -name "*.h") # fuck it, lets just do this..

SOURCES_CPP = $(shell find ./src -name "*.cpp")

OBJECTS_CPP = $(patsubst ./src/%, ./obj/%,$(patsubst %.cpp, %.o, $(SOURCES_CPP)))

all:
	@mkdir -p obj
	@$(MAKE) kernel

kernel: $(OBJECTS_CPP)
	@$(LD) -T linker.ld -nostdlib $^ -o kernel.o
	@$(OBJCOPY) -O binary kernel.o kernel

obj/%.o: src/%.cpp $(HEADERS_CPP)
	@echo "    CXX $<"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -f kernel kernel.o
	@find . -name '*.o' -type f -delete
