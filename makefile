# VEX V5 Makefile
# This makefile is used to compile VEX V5 C++ projects

# Project settings
PROJ_NAME = VexV5-OmniDrive

# Toolchain settings
TOOLCHAIN = arm-none-eabi-
CC = $(TOOLCHAIN)gcc
CXX = $(TOOLCHAIN)g++
AS = $(TOOLCHAIN)as
LD = $(TOOLCHAIN)ld
OBJCOPY = $(TOOLCHAIN)objcopy

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# V5 SDK paths (update these based on your installation)
V5_SDK = /usr/local/vexcode-sdk
V5_INCLUDE = $(V5_SDK)/include
V5_LIB = $(V5_SDK)/lib

# Compiler flags
CFLAGS = -std=c11 -Wall -Wextra -I$(INC_DIR) -I$(V5_INCLUDE)
CXXFLAGS = -std=c++17 -Wall -Wextra -I$(INC_DIR) -I$(V5_INCLUDE)
LDFLAGS = -L$(V5_LIB)

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# Default target
all: $(BIN_DIR)/$(PROJ_NAME).bin

# Create directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile C++ files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link
$(BIN_DIR)/$(PROJ_NAME).elf: $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ -o $@

# Create binary
$(BIN_DIR)/$(PROJ_NAME).bin: $(BIN_DIR)/$(PROJ_NAME).elf
	$(OBJCOPY) -O binary $< $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean
