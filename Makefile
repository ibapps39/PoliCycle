CC = g++
CFLAGS = -O3 -Wall -std=c++11 -Wno-missing-braces
LDFLAGS = -L/opt/homebrew/Cellar/raylib/5.5/lib -lraylib
INCLUDES = -I/opt/homebrew/Cellar/raylib/5.5/include

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = main

# Default target to build the program
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS) || (make clean && exit 1)  # Clean on linking failure
	rm -f $(OBJ)  # Clean up object files after building

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@ || (make clean && exit 1)  # Clean on compilation failure

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean