#include "page_walk.h"

/**
 * \brief Sets the nth bit (flag) to 1 in a 64-bit integer.
 */
uint64_t stud_set_bit(uint64_t value, unsigned int flag) {
    return value | (1ULL << flag);
}

/**
 * \brief Tests whether the nth bit is 1.
 */
bool stud_test_bit(uint64_t value, unsigned int flag) {
    return ((value >> flag) & 1ULL) != 0;
}

/**
 * \brief Clears the nth bit (sets it to 0).
 */
uint64_t stud_clear_bit(uint64_t value, unsigned int flag) {
    return value & ~(1ULL << flag);
}

/**
 * \brief Extracts the index into the level-1 page table.
 *
 * We have 4 levels, each with LEVEL_INDEX_BITS bits; the topmost
 * index sits above 3⋅LEVEL_INDEX_BITS+OFFSET_BITS.
 */
uint64_t stud_index_page_level_1(uint64_t va) {
    return (va >> (OFFSET_BITS + 3 * LEVEL_INDEX_BITS))
         & ((1ULL << LEVEL_INDEX_BITS) - 1);
}

/**
 * \brief Extracts the index into the level-2 page table.
 */
uint64_t stud_index_page_level_2(uint64_t va) {
    return (va >> (OFFSET_BITS + 2 * LEVEL_INDEX_BITS))
         & ((1ULL << LEVEL_INDEX_BITS) - 1);
}

/**
 * \brief Extracts the index into the level-3 page table.
 */
uint64_t stud_index_page_level_3(uint64_t va) {
    return (va >> (OFFSET_BITS + 1 * LEVEL_INDEX_BITS))
         & ((1ULL << LEVEL_INDEX_BITS) - 1);
}

/**
 * \brief Extracts the index into the level-4 page table (the PTE index).
 */
uint64_t stud_index_page_level_4(uint64_t va) {
    return (va >> OFFSET_BITS)
         & ((1ULL << LEVEL_INDEX_BITS) - 1);
}

/**
 * \brief Converts a PTE to a full physical address.
 *
 * Checks the PRESENT bit; if it’s not set, we signal a fault.
 * Otherwise we grab the 40-bit frame number from bits [12..51] of
 * the PTE and re-append the low OFFSET_BITS from the virtual address.
 */
uint64_t stud_pte_to_physical(uint64_t pte, uint64_t va) {
    if (!stud_test_bit(pte, PRESENT)) {
        fprintf(stderr, "Page fault: PTE not present\n");
        return 0;
    }
    // extract the 40-bit page-frame number
    uint64_t frame = (pte >> OFFSET_BITS) & ((1ULL << 40) - 1);
    // concatenate frame << OFFSET_BITS with the page-offset bits of va
    return (frame << OFFSET_BITS) | (va & ((1ULL << OFFSET_BITS) - 1));
}
