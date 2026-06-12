# Compiler
CC = gcc

# Flag: nyari header di folder include/ (Flag -g sudah dihapus)
CFLAGS = -Wall -Wextra -I./include

# Target eksekusi masuk ke folder build/
TARGET = build/program_inventaris
TEST_TARGET = build/test_inventaris

# Kumpulin file source
SRC = main.c src/inventory.c src/memory.c src/validation.c
TEST_SRC = test/testbench.c src/inventory.c src/memory.c src/validation.c

# Rule Default
all: $(TARGET)

# Compile Program Utama
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Compile Testbench
testbench: $(TEST_SRC)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRC)

# Jalanin program utama
run: $(TARGET)
	./$(TARGET)

# Jalanin testbench
run_test: testbench
	./$(TEST_TARGET)

# Bersihin file build dan output test
clean:
	rm -rf build/* test/output/*