#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

/**
 *
 * This API will copy the data from position the source location
 * to the destinationt location.
 * 
 * @param num
 *   This is a pointer to the value that the function will
 *   set the bit for.
 * 
 * @param bit_value
 *   This is the bit position at which the bit needs to be set.
 *   This will range from 0-63.
 * 
 * @return
 *   <0 on error. Standard linux errno is set on errors.
 *   0 on success. A success means that the bit requested by
 *   the user is set in the value pointer passed in.
 */

int mem_copy_deep(void *dest, const void *source, size_t numbyte);
