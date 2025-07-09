#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define PAGE_SIZE 4096
#define PAGE_TABLE_ENTRIES 1024
#define PTE_SIZE 4 // 4 bytes per Page Table Entry

typedef struct {
    uint32_t frame_number : 20;
    uint32_t valid : 1;
    uint32_t unused : 11;
} PageTableEntry;

typedef struct {
    PageTableEntry *page_table;
} PageDirectoryEntry;

typedef struct PhysicalMemoryNode {
    uint32_t address;
    uint8_t value;
    struct PhysicalMemoryNode *next;
} PhysicalMemoryNode;

PageDirectoryEntry page_directory[PAGE_TABLE_ENTRIES];
PhysicalMemoryNode *physical_memory = NULL;

uint32_t page_faults = 0;
uint32_t page_hits = 0;

uint32_t allocate_frame() {
    static uint32_t next_frame = 0;
    return next_frame++;
}

void handle_page_fault(uint32_t pd_index, uint32_t pt_index) {
    page_faults++;
    
    if (page_directory[pd_index].page_table == NULL) {
        page_directory[pd_index].page_table = calloc(PAGE_TABLE_ENTRIES, sizeof(PageTableEntry));
        printf("Allocated new page table for PD index %u\n", pd_index);
    }
    
    if (!page_directory[pd_index].page_table[pt_index].valid) {
        uint32_t frame = allocate_frame();
        page_directory[pd_index].page_table[pt_index].frame_number = frame;
        page_directory[pd_index].page_table[pt_index].valid = 1;
        printf("Allocated new frame %u for PT index %u\n", frame, pt_index);
    }
}

uint32_t translate_address(uint32_t virtual_address) {
    uint32_t pd_index = (virtual_address >> 22) & 0x3FF;
    uint32_t pt_index = (virtual_address >> 12) & 0x3FF;
    uint32_t offset = virtual_address & 0xFFF;
    
    if (page_directory[pd_index].page_table == NULL || !page_directory[pd_index].page_table[pt_index].valid) {
        handle_page_fault(pd_index, pt_index);
    } else {
        page_hits++;
    }
    
    uint32_t frame = page_directory[pd_index].page_table[pt_index].frame_number;
    return (frame << 12) | offset;
}

uint8_t load(uint32_t va) {
    uint32_t pa = translate_address(va);
    PhysicalMemoryNode *current = physical_memory;
    while (current != NULL) {
        if (current->address == pa) {
            return current->value;
        }
        current = current->next;
    }
    return 0; // Return 0 if address not found
}

void store(uint32_t va, uint8_t value) {
    uint32_t pa = translate_address(va);
    PhysicalMemoryNode *current = physical_memory;
    while (current != NULL) {
        if (current->address == pa) {
            current->value = value;
            return;
        }
        current = current->next;
    }
    // If not found, add new node
    PhysicalMemoryNode *new_node = malloc(sizeof(PhysicalMemoryNode));
    new_node->address = pa;
    new_node->value = value;
    new_node->next = physical_memory;
    physical_memory = new_node;
}

void print_statistics() {
    printf("Page faults: %u\n", page_faults);
    printf("Page hits: %u\n", page_hits);
    printf("Hit ratio: %.2f%%\n", (float)page_hits / (page_hits + page_faults) * 100);
    
    size_t pd_size = sizeof(PageDirectoryEntry) * PAGE_TABLE_ENTRIES;
    size_t pt_size = sizeof(PageTableEntry) * PAGE_TABLE_ENTRIES * page_faults;
    printf("Page Directory size: %zu bytes\n", pd_size);
    printf("Total Page Table size: %zu bytes\n", pt_size);
}


int main() {
    uint32_t va;
    uint8_t value;
    char operation;

    while (1) {
        printf("\nEnter operation (l for load, s for store, q to quit): ");
        scanf(" %c", &operation);

        if (operation == 'q') {
            break;
        }

        printf("Enter 32-bit virtual address in hexadecimal: ");
        scanf("%x", &va);

        if (operation == 'l') {
            value = load(va);
            printf("Value at 0x%08X: %u\n", va, value);
        } else if (operation == 's') {
            printf("Enter 8-bit value to store: ");
            scanf("%hhu", &value);
            store(va, value);
            printf("Stored %u at 0x%08X\n", value, va);
        }
    }

    print_statistics();

    // Clean up
    for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        free(page_directory[i].page_table);
    }
    while (physical_memory != NULL) {
        PhysicalMemoryNode *temp = physical_memory;
        physical_memory = physical_memory->next;
        free(temp);
    }

    return 0;
}
/* test_addresses
CCC0FFEE
12345678
DEADBEEF
CCC0FFEE
87654321
*/
