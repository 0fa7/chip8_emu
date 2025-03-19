#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct chip_8_cpu
{
    int8_t v[16]; // registers
    int16_t sp;

} Chip8Cpu;

void parse_instructions(uint8_t* buffer, long buffer_sz);
void seg_fault_handler(int s);
void instr_0nnn(Chip8Cpu* cpu, uint16_t nnn);
void instr_00e0(Chip8Cpu* cpu);
void instr_00ee(Chip8Cpu* cpu);
void instr_1nnn(Chip8Cpu* cpu, uint16_t nnn);
void instr_2nnn(Chip8Cpu* cpu, uint16_t nnn);
void instr_3xnn(Chip8Cpu* cpu, uint8_t x, uint8_t nn);
void instr_4xnn(Chip8Cpu* cpu, uint8_t x, uint8_t nn);
void instr_5xy0(Chip8Cpu* cpu, uint8_t x, uint8_t y);
void instr_6xnn(Chip8Cpu* cpu, uint8_t x, uint8_t nn);
void instr_7xnn(Chip8Cpu* cpu, uint8_t x, uint8_t nn);
void instr_8xy0(Chip8Cpu* cpu, uint8_t x, uint8_t y);
void instr_8xy1(Chip8Cpu* cpu, uint8_t x, uint8_t y);
void instr_8xy2(Chip8Cpu* cpu, uint8_t x, uint8_t y);
void instr_8xy3(Chip8Cpu* cpu, uint8_t x, uint8_t y);
void instr_8xy4(Chip8Cpu* cpu, uint8_t x, uint8_t y);
void instr_8xy5(Chip8Cpu* cpu, uint8_t x, uint8_t y);
void instr_8xy6(Chip8Cpu* cpu, uint8_t x, uint8_t y);
void instr_8xy7(Chip8Cpu* cpu, uint8_t x, uint8_t y);
void instr_8xye(Chip8Cpu* cpu, uint8_t x, uint8_t y);
void instr_9xy0(Chip8Cpu* cpu, uint8_t x, uint8_t y);
void instr_annn(Chip8Cpu* cpu, uint16_t nnn);
void instr_bnnn(Chip8Cpu* cpu, uint16_t nnn);
void instr_cxnn(Chip8Cpu* cpu, uint8_t x, uint8_t nn);
void instr_dxyn(Chip8Cpu* cpu, uint8_t x, uint8_t y, uint8_t n);
void instr_ex9e(Chip8Cpu* cpu, uint8_t x);
void instr_exa1(Chip8Cpu* cpu, uint8_t x);
void instr_fx07(Chip8Cpu* cpu, uint8_t x);
void instr_fx0a(Chip8Cpu* cpu, uint8_t x);
void instr_fx15(Chip8Cpu* cpu, uint8_t x);
void instr_unrecognized(uint8_t h, uint8_t l);

int main(int argc, char** argv)
{
    signal(SIGSEGV, seg_fault_handler);

    if(argc != 2)
    {
        printf("usage:\n");
        printf("  disassembler <input.rom>");
        return EXIT_SUCCESS;
    }

    FILE* fp = fopen(argv[1], "rb");
    fseek(fp, 0L, SEEK_END);
    long file_sz = ftell(fp);
    rewind(fp);
    
    printf("size: %d\n", file_sz);

    char* input_buffer = (char*)malloc(file_sz + 1);
    long read_sz = fread(input_buffer, 1, file_sz, fp);
    fclose(fp);

    if(file_sz != read_sz)
    {
        printf("failed to read file into memory\n");
        return EXIT_SUCCESS;
    }

    parse_instructions(input_buffer, file_sz);

    free(input_buffer);
    return EXIT_SUCCESS;
}

void parse_instructions(uint8_t* buffer, long buffer_sz)
{
    Chip8Cpu* cpu = (Chip8Cpu*)malloc(sizeof(Chip8Cpu));

    for(long i = 0; i < buffer_sz; i += 2)
    {

        printf("0x%02X%02X\n", buffer[i], buffer[i + 1]);
        uint8_t h = buffer[i];
        
        uint8_t hh = h >> 4;
        
        //printf("inspecting %02X\n", hh);

        //printf("0x%x\n", h);

        uint16_t op_code = buffer[i] << 4 | buffer[i + 1];

        if(op_code == 0x00E0)
        {
            instr_00e0(cpu);
            continue;
        }
        else if(op_code == 0x00EE)
        {
            instr_00ee(cpu);
            continue;
        }

        switch(hh)
        {
            case 0:
            {
                uint16_t nnn = (buffer[i] << 4 ) | buffer[i + 1];
                instr_0nnn(cpu, nnn);
                exit(1);
                break;
            }
            case 1:
            {
                uint16_t nnn = (0x0F & buffer[i]) << 8 | buffer[i + 1];
                instr_1nnn(cpu, nnn);
                break;
            }
            case 2:
            {
                uint16_t nnn = (0x0F & buffer[i]) << 8 | buffer[i + 1];
                instr_2nnn(cpu, nnn);
                break;
            }
            case 3:
            {
                uint8_t hl = h & 0x0F;
                uint8_t l = buffer[i + 1];
                instr_3xnn(cpu, hl, l);
                break;
            }
            case 4:
            {
                uint8_t hl = h & 0x0F;
                uint8_t l = buffer[i + 1];
                instr_4xnn(cpu, hl, l);
                break;
            }
            case 5:
            {
                uint8_t ll = 0x0F & buffer[i + 1];

                if(ll != 0)
                {
                    instr_unrecognized(buffer[i], buffer[i + 1]);
                    break;
                }
                
                uint8_t x = 0x0F & buffer[i];
                uint8_t y = 0x0F & buffer[i + 1] >> 4;
                instr_5xy0(cpu, x, y);
                break;
            }
            case 6:
            {
                uint8_t hl = h & 0x0F;
                uint8_t l = buffer[i + 1];
                instr_6xnn(cpu, hl, l);
                break;
            }
            case 7:
            {
                uint8_t l = buffer[i + 1];
                uint8_t hl = h & 0x0F;
                instr_7xnn(cpu, hl, l);
                break;
            }
            case 8:
            {
                uint8_t ll = buffer[i + 1] & 0x0F;
                uint8_t x = buffer[i] & 0x0F;
                uint8_t y = buffer[i + 1] >> 4;

                switch(ll)
                {
                    case 0:
                    {
                        instr_8xy0(cpu, x, y);
                        break;
                    }
                    case 1:
                    {
                        instr_8xy1(cpu, x, y);
                        break;
                    }
                    case 2:
                    {
                        instr_8xy2(cpu, x, y);
                        break;
                    }
                    case 3:
                    {
                        instr_8xy3(cpu, x, y);
                        break;
                    }
                    case 4:
                    {
                        instr_8xy4(cpu, x, y);
                        break;
                    }
                    case 5:
                    {
                        instr_8xy5(cpu, x, y);
                        break;
                    }
                    case 6:
                    {
                        instr_8xy6(cpu, x, y);
                        break;
                    }
                    case 7:
                    {
                        instr_8xy7(cpu, x, y);
                        break;
                    }
                    case 0xE:
                    {
                        instr_8xye(cpu, x, y);
                        break;
                    }
                    default:
                    {
                        instr_unrecognized(buffer[i], buffer[i + 1]);
                        break;
                    }
                }

                break;
            }
            case 0x9:
            {
                uint8_t ll = buffer[i + 1] & 0x0F;

                switch(ll)
                {
                    case 0:
                    {
                        uint8_t x = buffer[i] & 0x0F;
                        uint8_t y = buffer[i + 1] >> 4;
                        instr_9xy0(cpu, x, y);
                        break;
                    }
                    default:
                    {
                        instr_unrecognized(buffer[i], buffer[i + 1]);
                        break;
                    }
                }
                break;
            }
            case 0xA:
            {
                uint16_t nnn = (0x0F & buffer[i]) << 8 | buffer[i + 1];
                instr_annn(cpu, nnn);
                break;
            }
            case 0xB:
            {
                uint16_t nnn = (0x0F & buffer[i]) << 8 | buffer[i + 1];
                instr_bnnn(cpu, nnn);
                break;
            }
            case 0xC:
            {
                uint8_t nn = buffer[i + 1];
                uint8_t x = buffer[i] & 0x0F;
                instr_cxnn(cpu, x, nn);
                break;
            }
            case 0xD:
            {
                uint8_t hl = h & 0x0F;
                uint8_t y = buffer[i + 1] >> 4;
                uint8_t n = buffer[i + 1] && 0x0F;
                instr_dxyn(cpu, hl, y, n);
                break;
            }
            case 0xE:
            {
                uint8_t l = buffer[i + 1];
                uint8_t hl = h & 0x0F;

                switch(l)
                {
                    case 0x9E:
                    {
                        instr_ex9e(cpu, hl);
                        break;
                    }
                    case 0xA1:
                    {
                        instr_exa1(cpu, hl);
                        break;
                    }
                    default:
                    {
                        instr_unrecognized(buffer[i], buffer[i + 1]);
                        break;
                    }
                }

                break;
            }
            case 0xF:
            {
                uint8_t l = buffer[i + 1];

                switch(l)
                {
                    case 0x07:
                    {
                        uint8_t x = h & 0x0F;
                        instr_fx07(cpu, x);
                        break;
                    }
                    case 0x0A:
                    {
                        uint8_t x = h & 0x0F;
                        instr_fx0a(cpu, x);
                        break;
                    }
                    case 0x15:
                    {
                        uint8_t x = h & 0x0F;
                        instr_fx15(cpu, x);
                        break;
                    }
                    case 0x1E:
                    {
                        uint8_t x = h & 0x0F;
                        printf("0xF%01X1E ", x);
                        printf("Add the value stored in register v[%01X] im register I", x);
                        break;
                    }
                    default:
                    {
                        instr_unrecognized(buffer[i], buffer[i + 1]);
                        break;
                    }
                }

                break;
            }
            default:
            {
                instr_unrecognized(buffer[i], buffer[i + 1]);
                break;
            }
        };
    }

    free(cpu);
}

void seg_fault_handler(int s)
{
    printf( "Segmentation Fault\n" );
    exit(EXIT_FAILURE);
}

// 0x0NNN
void instr_0nnn(Chip8Cpu* cpu, uint16_t nnn)
{
    printf("0x0%03X Execute the machine language subroutine at address %03X\n", nnn, nnn);
}

// 0x00E0
void instr_00e0(Chip8Cpu* cpu)
{
    printf("0x00E0 Clear the screen\n");
}

// 0x00EE
void instr_00ee(Chip8Cpu* cpu)
{
    printf("0x00EE Clear the screen\n");
}

// 0x1NNN
void instr_1nnn(Chip8Cpu* cpu, uint16_t nnn)
{
    printf("0x%x%03X ", 1, nnn);
    printf("Jump to address 0x%03X\n", nnn);
}

// 0x2NNN
void instr_2nnn(Chip8Cpu* cpu, uint16_t nnn)
{
    printf("0x%x%03X ", 2, nnn);
    printf("Execute subroutine at address 0x%03X\n", nnn);
}

// 0x3XNN
void instr_3xnn(Chip8Cpu* cpu, uint8_t x, uint8_t nn)
{
    printf("0x3%x%02X ", x, nn);
    printf("Skip the next instruction if v[%01X] is equal to 0x%02X\n", x, nn);
}

// 0x4XNN
void instr_4xnn(Chip8Cpu* cpu, uint8_t x, uint8_t nn)
{
    printf("0x4%x%02X ", x, nn);
    printf("Skip the next instruction if v[%01X] is not equal to 0x%02X\n", x, nn);
}

// 0x5XY0
void instr_5xy0(Chip8Cpu* cpu, uint8_t x, uint8_t y)
{
    printf("0x5%01x%01X0 ", x, y);
    printf("Skip the following instruction if register v[%01X] is equal to the register v[%01X]\n", x, y);
}

// 0x6XNN
void instr_6xnn(Chip8Cpu* cpu, uint8_t x, uint8_t nn)
{
    printf("0x6%x%02X ", x, nn);
    printf("Load value 0x%02X into register v[%01X]\n", nn, x);
}

// 0x7XNN
void instr_7xnn(Chip8Cpu* cpu, uint8_t x, uint8_t nn)
{
    printf("0x7%01X%02X ", x, nn);
    printf("Add value 0x%02X into register v[%01X]\n", nn, x);
}

// 0x8XY0
void instr_8xy0(Chip8Cpu* cpu, uint8_t x, uint8_t y)
{
    printf("0x8%01X%01X0 ", x, y);
    printf("Store the value of register v[%01X] in register v[%01X]\n", y, x);
}

// 0x8XY1
void instr_8xy1(Chip8Cpu* cpu, uint8_t x, uint8_t y)
{
    printf("0x8%01X%01X1 ", x, y);
    printf("Set v[%01X] to (v[%01X] OR v[%01X])\n", x, x, y);
}

// 0x8XY2
void instr_8xy2(Chip8Cpu* cpu, uint8_t x, uint8_t y)
{
    printf("0x8%01X%01X2 ", x, y);
    printf("Set v[%01X] to (v[%01X] AND v[%01X])\n", x, x, y);
}

// 0x8XY3
void instr_8xy3(Chip8Cpu* cpu, uint8_t x, uint8_t y)
{
    printf("0x8%01X%01X3 ", x, y);
    printf("Set v[%01X] to (v[%01X] XOR v[%01X])\n", x, x, y);
}

// 0x8XY4
void instr_8xy4(Chip8Cpu* cpu, uint8_t x, uint8_t y)
{
    printf("0x8%01X%01X4 ", x, y);
    printf("Add the value of v[%01X] to v[%01X].  Set v[F] to 0x01 if carry occurs.  Set v[F] to 0x00 if carry does not occur.\n", y, x);
}

// 0x8XY5
void instr_8xy5(Chip8Cpu* cpu, uint8_t x, uint8_t y)
{
    printf("0x8%01X%01X5 ", x, y);
    printf("Subtract the value of v[%01X] from v[%01X].  Set v[F] to 0x00 if a borrow occurs.  Set v[F] to 0x01 if borrow does not occur.\n", y, x);
}

// 0x8XY6
void instr_8xy6(Chip8Cpu* cpu, uint8_t x, uint8_t y)
{
    printf("0x8%01X%01X6 ", x, y);
    printf("Store the value of register v[%01X] shifted right one bit in v[%01X].  Set v[F] to the lsb prior to the shift.\n", y, x);
}

// 0x8XY7
void instr_8xy7(Chip8Cpu* cpu, uint8_t x, uint8_t y)
{
    printf("0x8%01X%01X7 ", x, y);
    printf("Set register v[%01X] to the value v[%01X] minus v[%01X].  Set v[F] to 0x0 if a borrow occures.  Set v[F] to 0x1 if borrow does not occur.\n", x, y, x);
}

// 0x8XYE
void instr_8xye(Chip8Cpu* cpu, uint8_t x, uint8_t y)
{
    printf("0x8%01X%01XE ", x, y);
    printf("Store the value of register v[%01X] shifted left one bit in v[%01X].  Set v[F] to the msb prior to the shift.\n", y, x);
}

// 0x9XY0
void instr_9xy0(Chip8Cpu* cpu, uint8_t x, uint8_t y)
{
    printf("0x9%01X%01X0 ", x, y);
    printf("Skip the following instruction if the value of register v[%01X] is not equal to the value of register v[%01X]\n", x, y);
}

// 0xANNN
void instr_annn(Chip8Cpu* cpu, uint16_t nnn)
{
    printf("0xA%03X ", nnn);
    printf("Load the value 0x%03X in register I\n", nnn);
}

// 0xBNNN
void instr_bnnn(Chip8Cpu* cpu, uint16_t nnn)
{
    printf("0xB%03X ", nnn);
    printf("Jump to address 0x%03X + v[0x0]\n", nnn);
}

// 0xCXNN
void instr_cxnn(Chip8Cpu* cpu, uint8_t x, uint8_t nn)
{
    printf("0xC%01X%02X ", x, nn);
    printf("Set v[0x%01X] to a random number with a mask of 0x%02X", x, nn);
}

// 0xDXYN
void instr_dxyn(Chip8Cpu* cpu, uint8_t x, uint8_t y, uint8_t n)
{
    printf("0xD%01X%01X%01X ", x, y, n);
    printf("Draw a %01X byte sprite starting at memory location stored in register I at v[%01X] v[%01X]\n", x , y, n);
}

// 0xEX9E
void instr_ex9e(Chip8Cpu* cpu, uint8_t x)
{
    printf("0xE%01X9E ", x);
    printf("Skip the following instruction if the key corresponding to the hex value currently stored in register v[%01X] is pressed\n", x);
}

// 0xEXA1
void instr_exa1(Chip8Cpu* cpu, uint8_t x)
{
    printf("0xE%01XA1 ", x);
    printf("Skip the following instruction if the key corresponding to the hex value currently stored in register v[%01X] is not pressed\n", x);
}

// 0xFX07
void instr_fx07(Chip8Cpu* cpu, uint8_t x)
{
    printf("0xF%x07 ", x);
    printf("Store the current value of the delay timer in register v[%01X]\n", x);
}

// 0xFX0A
void instr_fx0a(Chip8Cpu* cpu, uint8_t x)
{
    printf("0xF%x0A ", x);
    printf("Wait for input and store result in register v[%01X]\n", x);
}

// 0xFX15
void instr_fx15(Chip8Cpu* cpu, uint8_t x)
{
    printf("0xF%x15 ", x);
    printf("Set the delay timer to the value stored in register v[%01X]\n", x);
}

// Unrecognized
void instr_unrecognized(uint8_t h, uint8_t l)
{
    printf("0x%02X%02X ", h, l);
    printf("Unrecognized instruction.");
    exit(EXIT_SUCCESS);
}