# Compiler: Use the Linux-GNU toolchain for standard library support
CC = riscv64-linux-gnu-gcc

# Flags:
# -march=rv64gcv: Target RISC-V 64-bit with Vector extensions
# -static: Required for QEMU user-mode emulation
CFLAGS = -march=rv64gcv -mabi=lp64d -O3 -static -I./include

# Emulator Configuration
EMU = qemu-riscv64
EMU_FLAGS = -cpu rv64,v=true,vlen=128

TARGET = test_axpy
SRCS = src/q15_axpy.c tests/test_q15_axpy.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

run: $(TARGET)
	$(EMU) $(EMU_FLAGS) ./$(TARGET)

clean:
	rm -f $(TARGET)