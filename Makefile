# Simple Makefile for User Management API
# This is an alternative to CMake build system
# Note: You'll need to manually download dependencies

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -O2
INCLUDES = -I./include -I./external/httplib -I./external/json/include -I./external/catch2/include

# Platform-specific settings
ifeq ($(OS),Windows_NT)
    LDFLAGS = -lws2_32
    EXE_EXT = .exe
else
    LDFLAGS = 
    EXE_EXT = 
endif

# Targets
all: user_api$(EXE_EXT)

user_api$(EXE_EXT): src/main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o user_api$(EXE_EXT) src/main.cpp -lpthread $(LDFLAGS)

test_api$(EXE_EXT): tests/test_main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o test_api$(EXE_EXT) tests/test_main.cpp

test: test_api$(EXE_EXT)
	./test_api$(EXE_EXT)

run: user_api$(EXE_EXT)
	./user_api$(EXE_EXT)

clean:
	rm -f user_api$(EXE_EXT) test_api$(EXE_EXT)

.PHONY: all test run clean

# Note: This Makefile requires you to manually download dependencies:
# 1. Download cpp-httplib to ./external/httplib/
#    git clone https://github.com/yhirose/cpp-httplib.git external/httplib
# 2. Download nlohmann/json to ./external/json/
#    git clone https://github.com/nlohmann/json.git external/json
# 3. Download Catch2 to ./external/catch2/
#    git clone https://github.com/catchorg/Catch2.git external/catch2
#
# For automatic dependency management, use the CMake build system instead.