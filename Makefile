CXX = m68k-unknown-linux-gnu-g++
LD = m68k-unknown-linux-gnu-ld
OBJCOPY = m68k-unknown-linux-gnu-objcopy
# -Wno-array-bounds due to "array subscript 0 is outside array bounds" gcc bug (at least afaik it's a gcc bug)
CXXFLAGS = -I./include -Os -std=c++11 -march=68000 -nostdlib -nostdinc -fno-pie -fno-pic -fno-stack-protector -ffreestanding -fno-threadsafe-statics -fno-exceptions -Wall -Wpedantic -g -Wno-array-bounds

HEADERS_CPP = $(shell find ./src -name "*.h") # fuck it, lets just do this..

SOURCES_CPP = $(shell find ./src -name "*.cpp")

OBJECTS_CPP = $(patsubst ./src/%, ./obj/%,$(patsubst %.cpp, %.o, $(SOURCES_CPP)))

SOURCES_ASM = $(shell find ./src -name "*.S" -o -name "*.s")

OBJECTS_ASM = $(patsubst ./src/%, ./obj/%,$(patsubst %.S, %.o, $(patsubst %.s, %.o, $(SOURCES_ASM))))

all: 
	@mkdir -p obj
	@$(MAKE) startup

startup: $(OBJECTS_ASM) $(OBJECTS_CPP)
	@$(LD) -T linker.ld -nostdlib $^ -o startup.o
	@$(OBJCOPY) -O binary startup.o startup
	@truncate -s 12288 startup

obj/%.o: src/%.cpp $(HEADERS_CPP)
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
	@rm -f startup startup.o
	@find . -name '*.o' -type f -delete
