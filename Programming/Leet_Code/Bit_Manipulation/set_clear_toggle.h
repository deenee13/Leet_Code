/* gcc -o set_clear_toggle set_clear_toggle.c*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>


/**
 *
 * This API will set the bit position in the pointer passed in
 * the function.
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
int set_bit(unsigned int* num, uint8_t bit_value);


/**
 *
 * This API will clear the bit position in the pointer passed in
 * the function.
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
int clear_bit(unsigned int* num, uint8_t bit_value);


/**
 *
 * This API will toggle the bit position in the pointer passed in
 * the function.
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
int toggle_bit(unsigned int* num, uint8_t bit_value);
