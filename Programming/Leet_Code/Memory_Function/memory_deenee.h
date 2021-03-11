#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

/**
 * We are implementing as macro and not as function because sizeof need to 
 * take variable of any type and thus we cannot do that in c hence it is defined
 * as macro. In short c dont support function overloading.
 * 
 *  In this function I am first finding the address of the variable and then Incrementing
 *  it with 1 which will give me the next address where I can store the value of same type.
 *  After obtaining it I am subtracting it from the address of the varibale. Which will give
 *  me the actual bytes of the variable.
 * 
 * The reason I am typecasting it in Char* is because I want answer in number of bytes
 * 
 * // https://www.geeksforgeeks.org/implement-your-own-sizeof/
 */
#define sizeofdeep(type) (char*)(&type+1) - (char*)(&type)



/**
 *
 * This API will compare the  string 1 with string 2 and by calculating the
 * total of both the string
 * 
 * 
 * @param s1
 *   This is a pointer to the string address of the string 1
 *   
 * 
 * @param s2
 *   This is a pointer to the string address of the string 2
 * 
 * @param numbyte
 *   It specifies the number of character to comapre
 * 
 * @return
 *   if string 1 total is greater than string 2 it will return positive value
 *   if it is less than it will return negative value else if both are equal 
 *   than it will return 0
 */
int mem_cmp_deep(const void *s1, const void *s2, size_t numbyte);

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
