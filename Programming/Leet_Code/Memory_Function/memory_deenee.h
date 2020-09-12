#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

/**
 *
 * This API will copy the data from  the source location
 * to the destination location.
 * 
 * @param dest
 *   This is a pointer to the destination address to where the data will be copied  
 *   
 * 
 * @param source
 *   This is a pointer to the source address from  where the data need to  be copied  
 *   to the destination
 * 
 * @param numbyte
 *   This is the size of the data in bytes which need to be copied from source to destination 
 * 
 * @return
 *   <0 on error. Standard linux errno is set on errors.
 *   0 on success. A success means that the bit requested by
 *   the user is set in the value pointer passed in.
 */
int mem_copy_deep(void *dest, const void *source, size_t numbyte);

/**
 *
 * This API will move the data from  the source location
 * to the destination location. mem_copy_deep will not work properly if the 
 * location of the source and the destination are overlapping.
 * 
 * @param dest
 *   This is a pointer to the destination address to where the data will be copied  
 *   
 * 
 * @param source
 *   This is a pointer to the source address from  where the data need to  be copied  
 *   to the destination
 * 
 * @param numbyte
 *   This is the size of the data in bytes which need to be copied from source to destination 
 * 
 * @return
 *   <0 on error. Standard linux errno is set on errors.
 *   0 on success. A success means that the bit requested by
 *   the user is set in the value pointer passed in.
 */
int mem_move_deep(void *dest, const void *source, size_t numbyte);
