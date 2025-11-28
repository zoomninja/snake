CC = clang
SRC = src/main.c src/shader.c src/mesh.c
OUT = main

# Default target: build and run
all: run

# Build rule
$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) -L/opt/homebrew/lib -Llib -lglad -lglfw -lassimp -Iinclude

# Run rule
run: $(OUT)
	./$(OUT)

# Clean rule
clean:
	rm -f $(OUT)

.PHONY: all run clean
