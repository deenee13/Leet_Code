#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


/**
 *
 * This API will find the starting location of the substring inside the mainstring
 * and return the starting location of it as a pointer
 * 
 * @param mainstring
 *   This is a pointer to the starting address of the mainstring  
 *   
 * 
 * @param substring
 *   This is a pointer to the starting address of the substring
 * 
 * @return
 * It will return the pointer to the starting location of the substring inside the mainstrig.
 * if the substring is not found inside the mainstring it will return NULL pointer. It will 0 if 
 * any of the two string is empty
 */
const char* strstr_deep(const char *mainstring, const char *substring);
