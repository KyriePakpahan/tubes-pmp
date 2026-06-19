CC = gcc

CFLAGS = -Wall -Wextra -I./include -I./src

TARGET = build/program_inventaris
TEST_TARGET = build/test_inventaris

ARDUINO_CLI = build/appimage/squashfs-root/resources/app/lib/backend/resources/arduino-cli
ARDUINO_PORT = /dev/ttyACM0
ARDUINO_FQBN = arduino:avr:uno

SRC = main.c src/inventory.c src/memory.c src/validation.c src/lookup.c
TEST_SRC = test/testbench.c src/inventory.c src/memory.c src/validation.c src/lookup.c

all: $(TARGET)

build:
	mkdir -p build

$(TARGET): build $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

testbench: build $(TEST_SRC)
	mkdir -p test/output
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRC)

run: $(TARGET)
	./$(TARGET)

run_test: testbench
	./$(TEST_TARGET)

flash_main:
	$(ARDUINO_CLI) compile --clean --fqbn $(ARDUINO_FQBN) --upload --port $(ARDUINO_PORT) InventarisArduino

clean:
	rm -rf build/program_inventaris build/test_inventaris \
		build/eeprom.bin build/eeprom.hex build/*.hex build/*.bin \
		test/output/*

