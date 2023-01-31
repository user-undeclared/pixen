#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "string_utils.h"

char* concat(const char* string1, const char* string2) {
    if(string1 == NULL) string1 = "";
    if(string2 == NULL) string2 = "";

    size_t string1_length = strlen(string1);
    size_t string2_length = strlen(string2);

    char* result_string = malloc(string1_length + string2_length + sizeof(char));
    if(result_string == NULL) return NULL;

    strcpy(result_string, string1);
    strcat(result_string, string2);
    return result_string;
}

const char* get_filename_extention(const char* filepath) {
    for(size_t index = strlen(filepath); index > 0; index--) {
        if(filepath[index - 1] == '.') {
            return &filepath[index];
        }
    }
    return NULL;
}

bool string_equal_ignore_case(const char* string1, const char* string2) {
    size_t string1_length = strlen(string1);
    size_t string2_length = strlen(string2);
    if(string1_length != string2_length) return false;

    for(size_t index = 0; index < string1_length; index++) {
        if(tolower(string1[index]) != tolower(string2[index])) {
            return false;
        }
    }

    return true;
}

