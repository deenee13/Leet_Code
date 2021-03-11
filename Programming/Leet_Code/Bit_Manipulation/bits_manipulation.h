/* gcc -o set_clear_toggle set_clear_toggle.c */
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

/**
 * In this approach we only reverse the bits where there 1 present where in the above approach
 * we were shifting whether the bit is 0 or 1.
 * 
 * @param num
 * This is the pointer to the number whose bits are to be reversed
 * 
 * @return 
 * <0 on error. Standard linux errno is set on errors.
 * 0 on success. A success means that the number is successfully
 * reversed and returned as value pointer passed in.
 * **/
int reverse_bits_differentway(unsigned int *num);

/**
 * In this approach we only reverse the bits where there 1 present where in the above approach
 * we were shifting whether the bit is 0 or 1.
 * 
 * @param num
 * This is the pointer to the number whose bits are to be reversed
 * 
 * @return 
 * <0 on error. Standard linux errno is set on errors.
 * 0 on success. A success means that the number is successfully
 * reversed and returned as value in the pointer passed in.
 * **/
int reverse_bits(unsigned int *num);

/**
 * This function will encode the 32-bit number such that lower
 * 16 bits will be untouched and the upper 16 bits will be reversed 
 * 
 * @param num
 * This is the pointer to the number which is to be encoded
 * 
 * @param bit_value
 * It specifies the bit location from where the bits needs to be encoded
 * which means need to be reversed. It ranges from 0 to 31
 * 
 * @return
 * <0 on error. Standard linux errno is set on errors.
 * 0 on success. A success means that the number is successfully encoded
 * and returned as value in the pointer passed in 
 * **/
int encoding_bits(unsigned int *num, unsigned int bit_value);
