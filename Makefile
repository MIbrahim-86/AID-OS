FILES = ./build/kernel.asm.o ./build/kernel.o ./build/pic.o ./build/keyboard.o ./build/keyboard_isr.o ./build/idt.o ./build/idt_asm.o ./build/isr.o ./build/isr_asm.o ./build/shell.o ./build/time.o ./build/random.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=512 count=100 >> ./bin/os.bin

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

./build/kernel.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o

./build/pic.o: ./src/pic.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/pic.c -o ./build/pic.o

./build/keyboard.o: ./src/keyboard.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/keyboard.c -o ./build/keyboard.o

./build/keyboard_isr.o: ./src/keyboard_isr.asm
	nasm -f elf -g ./src/keyboard_isr.asm -o ./build/keyboard_isr.o

./build/idt.o: ./src/idt.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/idt.c -o ./build/idt.o

./build/idt_asm.o: ./src/idt.asm
	nasm -f elf -g ./src/idt.asm -o ./build/idt_asm.o

./build/isr.o: ./src/isr.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/isr.c -o ./build/isr.o

./build/isr_asm.o: ./src/isr.asm
	nasm -f elf -g ./src/isr.asm -o ./build/isr_asm.o

./build/shell.o: ./src/shell.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/shell.c -o ./build/shell.o

./build/time.o: ./src/time.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/time.c -o ./build/time.o

./build/random.o: ./src/random.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/random.c -o ./build/random.o

clean:
	rm -rf ./bin/boot.bin
	rm -rf ./bin/kernel.bin
	rm -rf ./bin/os.bin
	rm -rf ${FILES}
	rm -rf ./build/kernelfull.o