CC = clang
SRC = main.c
OUT = main

# Default target: build and run
all: run

# Build rule
$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) -I/opt/homebrew/include -Iinclude -L/opt/homebrew/lib -Llib -lglad -lglfw -framework Cocoa -framework IOKit -framework CoreVideo

# Run rule
run: $(OUT)
	./$(OUT)

# Clean rule
clean:
	rm -f $(OUT)

.PHONY: all run clean
