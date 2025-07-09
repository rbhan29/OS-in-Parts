# Q2
## Assumptions:

- The system has a 64KB physical memory.

- Segment Definitions:
    - The system has three segments: Code, Heap, and Stack.
    - Each segment has a base address and a size.
    - Segment Details:
        - Code Segment - Base Address: 32KB, Size: 2KB
        - Heap Segment - Base Address: 34KB, Size: 3KB
        - Stack Segment - Base Address: 28KB, Size: 2KB

The user inputs a 16-bit logical address in hexadecimal format.
The top 2 bits of the logical address are used to determine the segment:
- 00 (binary) or 0x0 (hex): Code Segment
- 01 (binary) or 0x1 (hex): Heap Segment
- 10 (binary) or 0x2 (hex): Stack Segment

Offset Calculation:
- The offset is determined by the remaining 14 bits of the logical address.
For the stack segment, to obtain the correct negative offset, we must subtract the maximum segment size from the offset (assumed, a segment can be 4KB). We simply add the negative offset to the base to arrive at the correct physical address. This is done to account for the stack growing downwards.


# Q3
## Assumptions

- The system has a 64KB physical memory.

- Page Size: Each page is 4KB in size.

- Number of Pages:The total number of pages is calculated as MEMORY_SIZE / PAGE_SIZE, which equals 16 pages.

- Translation Lookaside Buffer (TLB): The TLB can hold 4 entries.

- Page Table:
    - The page table has a simple 1:1 mapping for this simulation.
    - Each page table entry contains:
        - Page Frame Number (PFN)
        - Valid bit
        - Protection bits (all access allowed in this simulation)

- Logical Address Format:
    - The logical address is a 16-bit value.
    - The top 4 bits (bits 12-15) are used as the Virtual Page Number (VPN).
    - The remaining 12 bits (bits 0-11) are used as the offset within the page.
