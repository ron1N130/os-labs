#ifndef PAGE_WALK_H__
#define PAGE_WALK_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// The number of bits used to represent the level index in a page table entry.
#define LEVEL_INDEX_BITS 9

// The number of bits used to represent the page offset in a virtual address.
#define OFFSET_BITS 12


// Define the possible flags for a page table entry
enum flags {
  PRESENT,        // 1 if page frame present in memory
  READWRITE,      // 1 if read/write, 0 if readonly
  USERSUPERVISOR, // 1 if accessible in user/supervisor, 0 if only supervisor mode
};

enum operation {
	READ,
	WRITE,
};

enum mode {
	USER,
	SUPERVISOR,
};

/**
 * \brief Sets the nth bit (flag) to 1 in a 64-bit integer.
 *
 * \param value The 64-bit integer to modify.
 * \param flag The bit position to set (0-63).
 *
 * \returns The modified 64-bit integer with the nth bit set to 1.
 */
uint64_t stud_set_bit(uint64_t value, unsigned int flag);

/**
 * \brief Tests the nth (flag) bit in a 64-bit integer.
 *
 * \param value The 64-bit integer to test.
 * \param flag The bit position to test (0-63).
 *
 * \returns true if the nth bit is set to 1, false otherwise.
 */
bool stud_test_bit(uint64_t value, unsigned int flag);

/**
 * \brief Clears the nth (flag)  bit in a 64-bit integer.
 *
 * \param value The 64-bit integer to modify.
 * \param flag The bit position to clear (0-63).
 *
 * \returns The modified 64-bit integer with the nth bit cleared.
 */
uint64_t stud_clear_bit(uint64_t value, unsigned int flag);

/**
 * \brief Calculates the offset of the levels in the  page table
 *
 * @param virtual_address: virtual address associated
 *
 * @return offset of the levels in the page table
 */
uint64_t stud_index_page_level_1(uint64_t virtual_address);
uint64_t stud_index_page_level_2(uint64_t virtual_address);
uint64_t stud_index_page_level_3(uint64_t virtual_address);
uint64_t stud_index_page_level_4(uint64_t virtual_address);

/**
 * \brief Converts a PTE to physical address
 *
 * @param pte: page table entry
 * @param virtual_address:  virtual address associated with the PTE
 *
 * @return the physical address
 */
uint64_t stud_pte_to_physical(uint64_t pte, uint64_t virtual_address);


#endif // PAGE_WALK_H__
