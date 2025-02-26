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
void instr_2nnn(Chip8Cpu* cpu, int16_t nnn);
void instr_4xnn(Chip8Cpu* cpu, int8_t x, int8_t nn);
void instr_6xnn(Chip8Cpu* cpu, int8_t x, int8_t nn);
void instr_fx0a(Chip8Cpu* cpu, int8_t x);

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

    for(long i = 0; i < 12; i += 2)
    {
        printf("i: %d\n", i);
        uint8_t h = buffer[i];
        
        uint8_t hh = h >> 4;
        
        printf("inspecting %02x\n", hh);

        printf("0x%x\n", h);


        switch(hh)
        {
            // 0x6XNN - Store value NN in register VX
            case 2:
            {
                uint16_t nnn = (0x0F & buffer[i]) << 8 | buffer[i + 1];
                instr_2nnn(cpu, nnn);
                break;
            }
            case 4:
            {
                uint8_t hl = h & 0x0F;
                uint8_t l = buffer[i + 1];
                instr_4xnn(cpu, hl, l);
                break;
            }
            case 6:
            {
                uint8_t hl = h & 0x0F;
                uint8_t l = buffer[i + 1];
                instr_6xnn(cpu, hl, l);
                break;
            }
            case 0xF:
            {
                uint8_t l = buffer[i + 1];

                switch(l)
                {
                    case 0x0A:
                    {
                        uint8_t x = h & 0x0F;
                        instr_fx0a(cpu, x);
                        break;
                    }
                    default:
                    {
                        printf("Unrecognized instruction 0xF\n");
                        //break;
                    }
                }

                break;
            }
            default:
            {
                printf("Unrecognized instruction\n");
                //break;
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

// 0x2NNN
void instr_2nnn(Chip8Cpu* cpu, int16_t nnn)
{
    printf("0x%x%03x ", 2, nnn);
    printf("Execute subroutine at address (%03x)\n", nnn);
    cpu->sp = nnn;
}

// 0x4XNN
void instr_4xnn(Chip8Cpu* cpu, int8_t x, int8_t nn)
{
    printf("0x4%x%02x ", 4, x, nn);
    printf("Skip the next instruction if v[%d] is not equal to 0x%02x\n", x, nn);
}

// 0x6XNN
void instr_6xnn(Chip8Cpu* cpu, int8_t x, int8_t nn)
{
    printf("0x6%x%02x ", x, nn);
    printf("Load value 0x%02x into register v[%d]\n", nn, x);
    cpu->v[x] = nn;
}

// 0xFX0A
void instr_fx0a(Chip8Cpu* cpu, int8_t x)
{
    printf("0xF%x0A ", x);
    printf("Wait for input and store result in register v[%d]\n", x);
}