# STM32F4 ADC DMA Project Makefile

# Compiler and flags
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Target
TARGET = stm32f4_adc_dma

# Directories
SRC_DIR = .
BUILD_DIR = build

# CPU and FPU settings
CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16
FLOAT_ABI = -mfloat-abi=hard

# Architecture flags
ARCH_FLAGS = $(CPU) $(FPU) $(FLOAT_ABI) -mthumb

# Compiler flags
CFLAGS = $(ARCH_FLAGS)
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -g3 -O0
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fno-builtin
CFLAGS += -std=c11

CXXFLAGS = $(ARCH_FLAGS)
CXXFLAGS += -Wall -Wextra -Wpedantic
CXXFLAGS += -g3 -O0
CXXFLAGS += -ffunction-sections -fdata-sections
CXXFLAGS += -fno-builtin -fno-rtti -fno-exceptions
CXXFLAGS += -std=c++20

# Linker flags
LDFLAGS = $(ARCH_FLAGS)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--print-memory-usage
LDFLAGS += --specs=nano.specs
LDFLAGS += --specs=nosys.specs

# Include paths
INCLUDES = -I.

# Source files
C_SOURCES = $(wildcard $(SRC_DIR)/*.c)
CXX_SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Object files
C_OBJECTS = $(C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
CXX_OBJECTS = $(CXX_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
OBJECTS = $(C_OBJECTS) $(CXX_OBJECTS)

# Default target
all: $(BUILD_DIR)/$(TARGET).elf

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile C++ files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Link
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@
	$(SIZE) $@

# Generate binary
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Generate hex
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Flash (using st-flash)
flash: $(BUILD_DIR)/$(TARGET).bin
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000

# Debug (using openocd and gdb)
debug: $(BUILD_DIR)/$(TARGET).elf
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg &
	arm-none-eabi-gdb $(BUILD_DIR)/$(TARGET).elf -ex "target extended-remote localhost:3333"

# Phony targets
.PHONY: all clean flash debug

# Show help
help:
	@echo "Available targets:"
	@echo "  all    - Build the project"
	@echo "  clean  - Clean build files"
	@echo "  flash  - Flash binary to STM32"
	@echo "  debug  - Start debugging session"
	@echo "  help   - Show this help"