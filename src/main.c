#include <stddef.h>
#include <stdio.h>
#include "chip-8.h"
#include "utilites.h"
#include <raylib.h>
#include <string.h>

size_t MaxRom = (KB(4) - 0x200);

int main(int argc , char** argv)
{
    char* rom_file;
    Color color = GREEN;
    if(argc < 1){
        return 0;
    }else {
        rom_file = argv[1];
        if (argc  >= 2) {
           if(strcmp(argv[2], "Z") == 0)
           {
                color = ORANGE;
           }else if(strcmp(argv[2], "W") == 0)
           {
                color = WHITE;
           }
        }
    }
    printf("Hello CHIP-8, emulating useing Elite's CHIP-8 emulator\n");
    CHIP chip;
    reset(&chip);

    FILE* file = fopen( rom_file, "rb");
    if(file == NULL) 
        return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    if (size >  MaxRom) {
        printf("ROM too large!\n");
        fclose(file);
        return -1;
    }
    rewind(file);
    Byte rom[MaxRom];
    size_t rom_size = fread(rom, sizeof(Byte),size , file);
    if (rom_size != (size_t)size) {
        printf("Error reading ROM\n");
        return -1;
    }
    for (int i = 0; i < rom_size; i++) {
        chip.memory.Data[0x200 + i] = rom[i];
    }
    printf("Running Emulation For Given ROM %s\n",rom_file);
    
    const int SCREEN_WIDTH = 64;
    const int SCREEN_HEIGHT = 32;
    const int SCALE = 15;  

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE, "CHIP-8");
    SetTargetFPS(60); // Standard 60Hz loop
   
    while (!WindowShouldClose()) {
        int cycles_per_frame = 10; 
        for (int i = 0; i < cycles_per_frame; i++) {
            execute(&chip, 1);
        }
        if (chip.cpu.delay_timer > 0) chip.cpu.delay_timer--;
        if (chip.cpu.sound_timer > 0) {
            chip.cpu.sound_timer--;
        }
        {
            chip.InputState[0x1] = IsKeyDown(KEY_ONE);   chip.InputState[0x2] = IsKeyDown(KEY_TWO);
            chip.InputState[0x3] = IsKeyDown(KEY_THREE); chip.InputState[0xC] = IsKeyDown(KEY_C);

            chip.InputState[0x4] = IsKeyDown(KEY_FOUR);     chip.InputState[0x5] = IsKeyDown(KEY_FIVE);
            chip.InputState[0x6] = IsKeyDown(KEY_SIX);     chip.InputState[0xD] = IsKeyDown(KEY_D);

            chip.InputState[0x7] = IsKeyDown(KEY_SEVEN);     chip.InputState[0x8] = IsKeyDown(KEY_EIGHT);
            chip.InputState[0x9] = IsKeyDown(KEY_NINE);     chip.InputState[0xE] = IsKeyDown(KEY_E);

            chip.InputState[0xA] = IsKeyDown(KEY_A);     chip.InputState[0x0] = IsKeyDown(KEY_ZERO);
            chip.InputState[0xB] = IsKeyDown(KEY_B);     chip.InputState[0xF] = IsKeyDown(KEY_F);
        }
        
        BeginDrawing();
        ClearBackground(BLACK);

        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                if (chip.Display[y][x]) {
                    DrawRectangle(x * SCALE, y * SCALE, SCALE, SCALE, color);
                }
            }
        }
        DrawFPS(0, 0);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
 