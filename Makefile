CC = g++
CFLAGS = -O3 -Wall -std=c++11 -Wno-missing-braces
LDFLAGS = -L/opt/homebrew/Cellar/raylib/5.5/lib -lraylib 
INCLUDES = -I/opt/homebrew/Cellar/raylib/5.5/include

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)
	rm $(OBJ)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: make clean