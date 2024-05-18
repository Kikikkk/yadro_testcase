CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Werror
SRC_DIR := src
OBJ_DIR := obj
INCLUDE_DIR := include
EXECUTABLE := task

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
INCLUDES := -I$(INCLUDE_DIR)

.PHONY: all clean

all: $(OBJ_DIR) $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(EXECUTABLE)
