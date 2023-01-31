#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H

#include <stdbool.h>

char* concat(const char* string1, const char* string2);
const char* get_filename_extention(const char* filepath);
bool string_equal_ignore_case(const char* string1, const char* string2);

#endif //_STRING_UTILS_H
