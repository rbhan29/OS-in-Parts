#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define PAGE_SIZE 4096
#define MEMORY_SIZE 65536 // 64KB
#define NUM_PAGES (MEMORY_SIZE / PAGE_SIZE)
#define TLB_SIZE 4 // Small TLB that can't hold all pages
#define VPN_MASK 0xF000
#define OFFSET_MASK 0x0FFF
#define VPN_SHIFT 12
#define PFN_SHIFT 12

typedef struct {
    uint8_t pfn;
    bool valid;
    uint8_t protect_bits;
} PageTableEntry;

typedef struct {
    uint8_t vpn;
    uint8_t pfn;
    bool valid;
    uint8_t protect_bits;
} TLBEntry;

PageTableEntry page_table[NUM_PAGES];
TLBEntry tlb[TLB_SIZE];
uint8_t next_tlb_entry = 0;
uint32_t tlb_hits = 0, tlb_misses = 0;

void init_page_table() {
    for (int i = 0; i < NUM_PAGES; i++) {
        page_table[i].pfn = i; // Simple 1:1 mapping for this simulation
        page_table[i].valid = true;
        page_table[i].protect_bits = 0xFF; // All access allowed
    }
}

void init_tlb() {
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].valid = false;
    }
}

bool check_tlb(uint8_t vpn, uint8_t *pfn, uint8_t *protect_bits) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid && tlb[i].vpn == vpn) {
            *pfn = tlb[i].pfn;
            *protect_bits = tlb[i].protect_bits;
            tlb_hits++;
            return true;
        }
    }
    tlb_misses++;
    return false;
}

void update_tlb(uint8_t vpn, uint8_t pfn, uint8_t protect_bits) {
    tlb[next_tlb_entry].vpn = vpn;
    tlb[next_tlb_entry].pfn = pfn;
    tlb[next_tlb_entry].valid = true;
    tlb[next_tlb_entry].protect_bits = protect_bits;
    next_tlb_entry = (next_tlb_entry + 1) % TLB_SIZE;
}

uint16_t translate_address(uint16_t logical_addr, bool *tlb_hit) {
    uint8_t vpn = (logical_addr & VPN_MASK) >> VPN_SHIFT;
    uint16_t offset = logical_addr & OFFSET_MASK;
    uint8_t pfn, protect_bits;

    if (check_tlb(vpn, &pfn, &protect_bits)) {
        // TLB Hit
        *tlb_hit = true;
        if ((protect_bits & 0xFF) == 0) {
            printf("Protection Fault (TLB)\n");
            exit(1);
        }
    } else {
        // TLB Miss
        *tlb_hit = false;
        PageTableEntry pte = page_table[vpn];
        if (!pte.valid) {
            printf("Segmentation Fault\n");
            exit(1);
        }
        if ((pte.protect_bits & 0xFF) == 0) {
            printf("Protection Fault (Page Table)\n");
            exit(1);
        }
        pfn = pte.pfn;
        protect_bits = pte.protect_bits;
        update_tlb(vpn, pfn, protect_bits);
    }

    return (pfn << PFN_SHIFT) | offset;
}

int main() {
    init_page_table();
    init_tlb();

    uint16_t logical_addr;
    char input[5]; // 4 hex digits + null terminator
    uint16_t previous_addresses[100]; // Array to store previously translated addresses
    int address_count = 0;

    while (1) {
        printf("Enter a 16-bit logical address in hex (or 'q' to quit): ");
        scanf("%4s", input);
        if (input[0] == 'q' || input[0] == 'Q') {
            break;
        }

        logical_addr = (uint16_t)strtol(input, NULL, 16);
        bool tlb_hit = false;

        // Convert logical address to decimal and check if it has been translated before
        uint16_t logical_addr_decimal = logical_addr;
        for (int i = 0; i < address_count; i++) {
            if (previous_addresses[i] == logical_addr_decimal) {
                tlb_hit = true;
                break;
            }
        }

        if (!tlb_hit) {
            previous_addresses[address_count++] = logical_addr_decimal;
        }

        uint16_t physical_addr = translate_address(logical_addr, &tlb_hit);

        printf("Logical address: 0x%04X\n", logical_addr);
        printf("Physical address: 0x%04X\n", physical_addr);
        printf("TLB %s\n", tlb_hit ? "Hit" : "Miss");
    }

    printf("\nSimulation complete.\n");
    printf("TLB Hits: %u\n", tlb_hits);
    printf("TLB Misses: %u\n", tlb_misses);
    printf("Hit Rate: %.2f%%\n", (float)tlb_hits / (tlb_hits + tlb_misses) * 100);

    return 0;
}
