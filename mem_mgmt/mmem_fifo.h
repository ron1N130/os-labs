#ifndef MMEM_FIFO_H__
#define MMEM_FIFO_H__

// You do not need to change anything here

#include "mmem.h"
#include <stdlib.h>

/**
 * \brief Initializes the list with the page frames
 * 
 * \return Returns a pointer to the created memory struct
 */
memory* stud_fifo_init_list();


/**
 * \brief maps a page to a page frame
 * 
 * \param mem - the memory struct in which the page should be mapped in
 * \param virtual_address - the virtual address of the page
 */
void stud_fifo_map_page(memory* mem, uint64_t virtual_address);


/**
 * \brief function is called, when a page is accessed
 * The page that is accessed may or may not be in a page frame
 * 
 * \param virtual_address - the virtual address of the page
 * \param mem - the memory struct in which the page should be accessed
 */
void stud_fifo_access_page(memory* mem, uint64_t virtual_address);

#endif