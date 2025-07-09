#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PHYSICAL_MEMORY_SIZE 65536 // 64KB

// Segment definitions
typedef struct {
    unsigned int base;
    unsigned int size;
} Segment;

Segment codeSegment = {32768, 2048}; // 32KB, 2KB
Segment heapSegment = {34816, 3072}; // 34KB, 3KB
Segment stackSegment = {28672, 2048}; // 28KB, 2KB

void translateAddress(char *logicalAddress) {
    unsigned int address = (unsigned int)strtol(logicalAddress, NULL, 16);
    unsigned int segment = (address & 0xC000) >> 14; // Segment Mask and Shift
    printf("Segment: %X\n", segment);
    unsigned int offset = address & 0x3FFF; // Offset Mask
    unsigned int physicalAddr;

    if (segment == 0x0) { // Code Segment
        printf("Code Segment\n");
        if (offset < codeSegment.size) {
            physicalAddr = codeSegment.base + offset;
            printf("Physical Address: %04X\n", physicalAddr);
        } else {
            printf("Segmentation Fault\n");
        }
    } else if (segment == 0x1) { // Heap Segment
        printf("Heap Segment\n");
        if (offset < heapSegment.size) {
            physicalAddr = heapSegment.base + offset;
            printf("Physical Address: %04X\n", physicalAddr);
        } else {
            printf("Segmentation Fault\n");
        }
    } else if (segment == 0x2) { // Stack Segment
        printf("Stack Segment\n");
        int negativeOffset = offset - 4096;
        if (negativeOffset <= stackSegment.size) {
            physicalAddr = stackSegment.base + negativeOffset;
            printf("Physical Address: %04X\n", physicalAddr);
        } else {
            printf("Segmentation Fault\n");
        }
    } else {
        printf("Invalid Segment\n");
    }
}

int main() {
    char logicalAddress[5];
    while (1) {
        printf("Enter a 16-bit logical address in hex format (or 'q' to quit): ");
        scanf("%4s", logicalAddress);
        if (logicalAddress[0] == 'q' || logicalAddress[0] == 'Q') {
            break;
        }
        translateAddress(logicalAddress);
    }
    return 0;
}
