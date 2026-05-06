#include "chip-8.h"
#include "utilites.h"
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

 

int execute(CHIP* chip,u32 clock)
{
    while (clock > 0) 
    {
        
        // Shifting left by 8 is exactly the same as multiplying by 256
        uint16_t opcode = ( chip->memory.Data[chip->cpu.PC] << 8) | 
                   chip->memory.Data[chip->cpu.PC + 1];    
        uint8_t high_nibble = (opcode & 0xF000) >> 12;
        //printf("(BEFORE EXECUTION)PC: 0x%03X | Opcode: 0x%04X | Nibble: 0x%X\n", chip->cpu.PC, opcode, high_nibble);
        switch (high_nibble) 
        {
            case OP_JP:{
                chip->cpu.PC = opcode &0x0FFF;
                clock--;
                 
            }break;
            case OP_JP_V0:{
                chip->cpu.PC = (opcode &0x0FFF) + chip->cpu.V[0];
                clock--;
            }break;

            case OP_CALL:{
                chip->memory.Stack[chip->cpu.SP] = chip->cpu.PC;
                chip->cpu.SP++;
                chip->cpu.PC = opcode &0x0FFF;
                clock--;
                 
            }break;

            case OP_SE_BYTE:{
                Byte x = (opcode& 0x0f00)>> 8;
                Byte kk = opcode & 0x00ff;
                if (chip->cpu.V[x] == kk) {
                    // Skip the next instruction (which is 2 bytes long)
                    chip->cpu.PC += 4;
                } else {
                    // Proceed to the next instruction normally
                    chip->cpu.PC += 2;
                }
                 
                clock--;
            }break;

            case OP_SNE_BYTE:{
                Byte x = (opcode& 0x0f00)>> 8;
                Byte kk = opcode & 0x00ff;
                if (chip->cpu.V[x] != kk) {
                    // Skip the next instruction (which is 2 bytes long)
                    chip->cpu.PC += 4;
                } else {
                    // Proceed to the next instruction normally
                    chip->cpu.PC += 2;
                }
                 
                clock--;
            }break;
            case OP_SE_REG:{
                Byte x = (opcode& 0x0f00) >> 8;
                Byte y = (opcode & 0x00f0)>>4;
                if (chip->cpu.V[x] == chip->cpu.V[y]) {
                    // Skip the next instruction (which is 2 bytes long)
                    chip->cpu.PC += 4;
                } else {
                    // Proceed to the next instruction normally
                    chip->cpu.PC += 2;
                }
                 
                clock--;
            }break;
            case OP_LD_BYTE:{
                uint8_t x = (opcode & 0x0F00) >> 8;
                // Extract the full last byte (KK)
                uint8_t byte_value = (opcode & 0x00FF); 
                
                chip->cpu.V[x] = byte_value;
                chip->cpu.PC += 2;
                clock--;
            }break;

            case OP_ADD_BYTE:{
                uint8_t x = (opcode & 0x0F00) >> 8;
                uint8_t byte_value = (opcode & 0x00FF); 
                chip->cpu.V[x] = chip->cpu.V[x] + byte_value;
                chip->cpu.PC += 2;
                clock--;    
            }break;

            case OP_SNE_REG:{
                Byte x = (opcode& 0x0f00)>> 8;
                Byte y = (opcode & 0x00f0) >> 4;
                if (chip->cpu.V[x] != chip->cpu.V[y]) {
                    // Skip the next instruction (which is 2 bytes long)
                    chip->cpu.PC += 4;
                } else {
                    // Proceed to the next instruction normally
                    chip->cpu.PC += 2;
                }
                 
                clock--;
                
            }break;
            case 0xE: {
                Byte x = (opcode & 0x0F00) >> 8;
                switch (opcode & 0x00FF) {
                    case 0x9E: // Ex9E: Skip next instruction if key in Vx IS pressed
                        if (chip->InputState[chip->cpu.V[x]]) {
                            chip->cpu.PC += 2;
                        }
                        break;

                    case 0xA1: // ExA1: Skip next instruction if key in Vx is NOT pressed
                        if (!chip->InputState[chip->cpu.V[x]]) {
                            chip->cpu.PC += 2;
                        }
                        break;
                }
                chip->cpu.PC += 2;
                clock--;
            } break;
            case OP_LD_I:{
                uint16_t nnn = (opcode & 0x0FFF);
                chip->cpu.I = nnn;
                clock--;
                chip->cpu.PC += 2;
            }break;
            case OP_RND:{
                uint8_t x = (opcode & 0x0F00) >> 8;
                uint8_t kk = (opcode & 0x00FF);
                
                // Generate random number 0-255 and AND it with kk
                chip->cpu.V[x] = (rand() % 256) & kk;
                
                chip->cpu.PC += 2;
                clock--;
            }break;
            //todo
            case OP_DRW:{
                //0xDxyn
                Byte x = (opcode & 0x0F00) >> 8;
                Byte y = (opcode & 0x00F0) >> 4;
                Byte n = (opcode & 0x000F);

                Byte SpriteData[n][8]; // n rows, 8 pixels per row

               // 1. Get coordinates from registers
                Byte xc = chip->cpu.V[x] % 64; 
                Byte yc = chip->cpu.V[y] % 32;

                // 2. Reset collision flag to 0 before starting
                chip->cpu.V[0xF] = 0; 

                for (int y = 0; y < n; y++) {
                    // 3. Fetch the byte for this row from memory
                    Byte rowByte = chip->memory.Data[chip->cpu.I + y];

                    for (int x = 0; x < 8; x++) {
                        // 4. Extract the specific bit (pixel) from the byte
                        Byte spritePixel = (rowByte & (0x80 >> x));

                        if (spritePixel != 0) {
                            // Calculate actual screen coordinates
                            int screenX = (xc + x)  ;
                            int screenY = (yc + y)  ;

                            if (screenX < 64 && screenY < 32) {
                                if (chip->Display[screenY][screenX] == 1) {
                                    chip->cpu.V[0xF] = 1;
                                }
                                chip->Display[screenY][screenX] ^= 1;
                            }
                        }
                    }
                }
                clock--;
                chip->cpu.PC += 2;
            }break;

            case GRP_0: {
                if (opcode == 0x00E0) {
                    memset(chip->Display, 0, sizeof(chip->Display));
                    chip->draw_flag = 1; // Tell the UI to refresh
                    chip->cpu.PC += 2;
                } 
                else if (opcode == 0x00EE) {
                    if (chip->cpu.SP > 0) {
                        chip->cpu.SP--;
                        chip->cpu.PC = chip->memory.Stack[chip->cpu.SP];
                        chip->cpu.PC += 2;
                    } else {
                        printf("Stack Underflow!\n");
                        return 0; 
                    }
                }
                else if (opcode == 0x0000) {
                    printf("End of program (0x0000). Shutting down.\n");
                    return 0; 
                }
                else {
                    // This is a 0NNN call. We just skip it.
                    printf("Ignoring RCA 1802 call: 0x%04X\n", opcode);
                    chip->cpu.PC += 2;
                }
                clock--;
            } break;
           
           case GRP_8: {
                Byte x = (opcode & 0x0F00) >> 8;
                Byte y = (opcode & 0x00F0) >> 4;

                switch (opcode & 0x000F) {
                    case 0x0: // 8xy0: LD Vx, Vy
                        chip->cpu.V[x] = chip->cpu.V[y];
                        break;

                    case 0x1: // 8xy1: OR Vx, Vy
                        chip->cpu.V[x] |= chip->cpu.V[y];
                        break;

                    case 0x2: // 8xy2: AND Vx, Vy
                        chip->cpu.V[x] &= chip->cpu.V[y];
                        break;

                    case 0x3: // 8xy3: XOR Vx, Vy
                        chip->cpu.V[x] ^= chip->cpu.V[y];
                        break;

                    case 0x4: { // 8xy4: ADD Vx, Vy (Sets Vf carry)
                        uint16_t sum = chip->cpu.V[x] + chip->cpu.V[y];
                        chip->cpu.V[0xF] = (sum > 0xFF) ? 1 : 0;
                        chip->cpu.V[x] = (Byte)(sum & 0xFF);
                    } break;

                    case 0x5: { // 8xy5: SUB Vx, Vy (Vf is 1 if NO borrow)
                        chip->cpu.V[0xF] = (chip->cpu.V[x] > chip->cpu.V[y]) ? 1 : 0;
                        chip->cpu.V[x] -= chip->cpu.V[y];
                    } break;

                    case 0x6: // 8xy6: SHR Vx (Shift Right)
                        // Note: Modern CHIP-8 shifts Vx. Older ones set Vx = Vy then shift.
                        chip->cpu.V[0xF] = (chip->cpu.V[x] & 0x1);
                        chip->cpu.V[x] >>= 1;
                        break;

                    case 0x7: { // 8xy7: SUBN Vx, Vy (Vy - Vx)
                        chip->cpu.V[0xF] = (chip->cpu.V[y] > chip->cpu.V[x]) ? 1 : 0;
                        chip->cpu.V[x] = chip->cpu.V[y] - chip->cpu.V[x];
                    } break;

                    case 0xE: // 8xyE: SHL Vx (Shift Left)
                        chip->cpu.V[0xF] = (chip->cpu.V[x] & 0x80) >> 7;
                        chip->cpu.V[x] <<= 1;
                        break;
                }
                chip->cpu.PC += 2;
                clock--;
            } break;

            case GRP_F: {
                Byte x = (opcode & 0x0F00) >> 8;

                switch (opcode & 0x00FF) {
                    case 0x07: // Fx07: LD Vx, DT (Get Delay Timer)
                        chip->cpu.V[x] = chip->cpu.delay_timer;
                        break;

                    case 0x1E: // Fx1E: ADD I, Vx
                        chip->cpu.I += chip->cpu.V[x];
                        break;

                    case 0x55: // Fx55: LD [I], Vx (Store registers V0 through Vx in memory)
                        for (int i = 0; i <= x; i++) {
                            chip->memory.Data[chip->cpu.I + i] = chip->cpu.V[i];
                        }
                        // Historical note: some interpreters increment I here, modern ones don't.
                        break;
                        
                    case 0x65: // Fx65: LD Vx, [I] (Read registers V0 through Vx from memory)
                        for (int i = 0; i <= x; i++) {
                            chip->cpu.V[i] = chip->memory.Data[chip->cpu.I + i];
                        }
                        break;
                }
                chip->cpu.PC += 2;
                clock--;
            } break;

            default:break;
        }
    }
    return 1;
}


void reset(CHIP* chip)
{
    chip->cpu.PC = 0x200;
    chip->cpu.I = 0;
    chip->cpu.SP = 0;  // in stacks region  0x0 -> 0x15 stack size 
    for(int i =0 ; i < KB(4);i++){
        chip->memory.Data[i] = 0x0;
    }
    for(int i =0 ; i < 16;i++){
        chip->memory.Stack[i] = 0x0;
         chip->cpu.V[i] = 0;
    }
    
    const Byte font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,      // 0
        0x20, 0x60, 0x20, 0x20, 0x70,      // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    memcpy(&chip->memory.Data[0x000], font, sizeof(font)); // Load font starting at address 0x000
    // CORRECT
for(int y = 0; y < 32; y++) {
    for(int x = 0; x < 64; x++) {
        chip->Display[y][x] = 0;
    }
}
    srand(time(NULL));
}