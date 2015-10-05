#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// Struct def for capexe commands:
struct command {
    uint32_t code;
    uint32_t *arg[3];
};

// Struct def for capos memory:
struct memory {
    uint32_t vari[8192];
    uint32_t cons[8192];
    uint32_t i; // Stores location in constant memory.
};

// Stores variables and constants in memory, and returns a pointer to them:
uint32_t *pointrify(
    uint32_t x,
    struct memory *mem
) {
    uint32_t firstBitMask = 0x80000000;
    // Check if variable or constant:
    if(x & firstBitMask) {
        // Zero-initialize variables:
        mem->vari[x ^ firstBitMask] = 0;
        if((x ^ firstBitMask) >= 8192) {
            printf("Variable memory overflow error.\n");
            return NULL;
        }
        return &mem->vari[x ^ firstBitMask];
    } else {
        if(mem->i >= 8192) {
            printf("Constant memory overflow error.\n");
            return NULL;
        }
        mem->cons[mem->i] = x;
        return &mem->cons[mem->i++];
    }
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: ./capos file.capexe\n");
        return 1;
    }
    uint32_t size;
    struct memory mem;
    mem.i = 0;
    struct command *command;
    {
        FILE *fp;
        fp = fopen(argv[1], "rb");
        if(fp == NULL) {
            printf("Error: Cannot open file '%s'.\n", argv[1]);
            return 2;
        }
        // Determine exe size:
        fseek(fp, 0, SEEK_END);
        size = ftell(fp) / 16;
        fseek(fp, 0, SEEK_SET);
        // Read exe to buffer:
        uint32_t *readBuff = malloc(size * 16);
        if(readBuff == NULL) {
            printf("Memory error.\n");
            free(readBuff);
            return 3;
        }
        if(fread(readBuff, 4, size * 4, fp) < size * 4) {
            printf("File read error.\n");
            free(readBuff);
            return 4;
        }
        command = malloc(size * sizeof(struct command));
        if(command == NULL) {
            printf("Memory error.\n");
            free(readBuff);
            free(command);
            return 5;
        }
        // Parse exe into command list:
        uint32_t n = 0;
        while(n < size) {
            uint32_t m = n * 4;
            command[n].code = readBuff[m];
            command[n].arg[0] = pointrify(readBuff[m + 1], &mem);
            command[n].arg[1] = pointrify(readBuff[m + 2], &mem);
            command[n].arg[2] = pointrify(readBuff[m + 3], &mem);
            if(
                command[n].arg[0] == NULL ||
                command[n].arg[1] == NULL ||
                command[n].arg[2] == NULL
            ) {
                free(command);
                return 6;
            }
            n++;
        }
        fclose(fp);
        free(readBuff);
    }
    // Iterate through the list of commands:
    uint32_t n = 0;
    while(n < size) {
        uint32_t **arg = command[n].arg;
        // Use the opcode to jump to the relevant operation:
        switch(command[n].code) {
        case 0:
            *arg[2] = *arg[0] + *arg[1];
            break;
        case 1:
            *arg[2] = *arg[0] - *arg[1];
            break;
        case 2:
            *arg[2] = *arg[0] | *arg[1];
            break;
        case 3:
            *arg[2] = *arg[0] & *arg[1];
            break;
        case 4:
            *arg[2] = *arg[0] ^ *arg[1];
            break;
        case 5:
            *arg[2] = ~ *arg[0];
            break;
        case 6:
            *arg[2] = *arg[0] >> *arg[1];
            break;
        case 7:
            *arg[2] = *arg[0] << *arg[1];
            break;
        case 8:
            printf("0x%x\n", *arg[0]);
            break;
        case 9:
            printf(
                "%c%c%c%c",
                (*arg[0] >> 24) & 0xFF,
                (*arg[0] >> 16) & 0xFF,
                (*arg[0] >> 8) & 0xFF,
                *arg[0] & 0xFF
            );
            break;
        case 10:
            if(*arg[0] > *arg[1]) {
                // Subtract 1 to compensate for n++ below:
                n = *arg[2] - 1;
            }
            break;
        case 11:
            if(*arg[0] < *arg[1]) {
                n = *arg[2] - 1;
            }
            break;
        case 12:
            if(*arg[0] == *arg[1]) {
                n = *arg[2] - 1;
            }
            break;
        case 13:
            if(
                *arg[0] == 0 &&
                *arg[1] == 0 &&
                *arg[2] == 0
            ) {
                // Exits loop:
                n = size;
            }
            break;
        default:
            printf("Error: Unrecognized opcode.\n");
            free(command);
            return 7;
        }
        n++;
    }
    free(command);
    return 0;
}
