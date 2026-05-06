CC = clang
CFLAG = -g -Wall -std=c99 -Wno-unused-variable
SRC = src/*.c 
BIN = bin/chip_8emu
LIB_PATH = lib/
LIB = -lraylib -framework OpenGL -framework IOKit -framework Cocoa
INC_PATH = lib/include
all:
	@mkdir -p bin
	@${CC} ${CFLAG} ${SRC} -o ${BIN} -L ${LIB_PATH} -I ${INC_PATH} ${LIB}