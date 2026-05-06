#ifndef __CHIP_8_H
#define __CHIP_8_H
#include "utilites.h"


typedef struct 
{
    Byte Data[KB(4)]; // 4 * 1025 -> 4096
    u16 Stack[16];
}Memory;


typedef struct
{
    Byte V[16]; // 16 general purpose 8-bit(1 Byte) registers
    u16 PC; //programe counter
    u16 SP; // Stack Pointer
    u16 I; //index resitor
    Byte delay_timer;
    Byte sound_timer;
    
}CPU;

typedef enum {
    // Unique Opcodes (Identified by high nibble 0x0-0x7, 0x9-0xD)
    
    OP_JP = 0x1,
    OP_CALL     = 0x2, // 2NNN
    OP_SE_BYTE  = 0x3, // 3XKK
    OP_SNE_BYTE = 0x4, // 4XKK
    OP_SE_REG   = 0x5, // 5XY0
    OP_LD_BYTE  = 0x6, // 6XKK
    OP_ADD_BYTE = 0x7, // 7XKK
    OP_SNE_REG  = 0x9, // 9XY0
    OP_LD_I     = 0xA, // ANNN
    OP_JP_V0    = 0xB, // BNNN
    OP_RND      = 0xC, // CXKK
    OP_DRW      = 0xD, // DXYN

    // Maskable Groups (Multiple instructions share these high nibbles)
    GRP_0       = 0x0, // Includes CLS (00E0) and RET (00EE)
    GRP_8       = 0x8, // All Arithmetic (8XY0 - 8XYE)
    GRP_E       = 0xE, // Keyboard Skips (EX9E, EXA1)
    GRP_F       = 0xF  // Timers and Memory (FX07 - FX65)
} ISA;

typedef struct
{
    Memory memory;
    CPU cpu;
    Byte Display[32][64];
    Byte draw_flag;
   
    Byte InputState[16];
}CHIP;

int execute(CHIP* chip,u32 clock);
void reset(CHIP* chip);

#endif