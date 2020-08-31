#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>


/**
 *
 * This API will convert the Decimal number in to Binary.
 * 
 * 
 * @param num
 *   This is the value in decimal which need to convert in to binary.
 *   This will range from 0-63.
 * 
 * @return
 *   <0 on error. Standard linux errno is set on errors.
 *   0 on success. A success means that number is converted into
 *   binary
 */
int decimal_binary(const long int num);