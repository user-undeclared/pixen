#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "image.h"

#define MAX_SIZE_MULTIPLIER 128
#define JPG_QUALITY 75

#define USAGE "usage: pixen MULTIPLIER IMAGE"
#define ERROR_PREFIX "error: "

const char* filename_extention(const char* filepath) {
    for(size_t index = strlen(filepath); index > 0; index--) {
        if(filepath[index - 1] == '.') {
            return &filepath[index];
        }
    }
    return NULL;
}

bool parse_size_multiplier(const char* string, unsigned* result) {
    char* end_pointer;
    errno = 0;
    long parsed_value = strtol(string, &end_pointer, 0);

    if(errno == ERANGE) {
        if(parsed_value == LONG_MIN) fprintf(stderr, "ERROR: %s is too small to parse\n", string);
        else fprintf(stderr, ERROR_PREFIX "%s is too big to parse\n", string);
        return false;
    }

    if(*end_pointer != '\0') {
        fprintf(stderr, ERROR_PREFIX "\"%s\" is not a number\n", string);
        return false;
    }

    if(parsed_value > MAX_SIZE_MULTIPLIER) {
        fprintf(stderr, ERROR_PREFIX "scale multiplier too big (max is %i)\n", MAX_SIZE_MULTIPLIER);
        return false;
    }
    
    if(parsed_value < 2) {
        fputs(ERROR_PREFIX "size multiplier must be greater than one\n", stderr);
        return false;
    }

    *result = (unsigned) parsed_value;
    return true; 
}

int main(int argc, char* argv[]) {
    if(argc < 3) {
        fputs(USAGE "\n" ERROR_PREFIX "too few arguments provided\n", stderr);
        return 1;
    }

    unsigned size_multiplier;
    if(!parse_size_multiplier(argv[1], &size_multiplier)) return 1;

    const char* image_filepath = argv[2];
    const char* image_file_extention = filename_extention(image_filepath);

    FILE* image_file = fopen(image_filepath, "rb");
    if(image_file == NULL) {
        fprintf(stderr, ERROR_PREFIX "failed to open file \"%s\": %s\n", image_filepath, strerror(errno));
        return 1;
    }

    enum image_type original_image_type;
    if(!image_type_from_string(image_file_extention, &original_image_type)) {
        fprintf(stderr, ERROR_PREFIX "unsupported image type \"%s\"\n", image_file_extention);
        return 1;
    }

    struct image original_image;
    bool image_was_loaded = image_load(image_file, &original_image);
    fclose(image_file);
    if(!image_was_loaded) {
        fprintf(stderr, ERROR_PREFIX "\"%s\" is not an image\n", image_filepath);
        return 1;
    }

    struct image scaled_image;
    int error = image_scale(original_image, size_multiplier, &scaled_image);
    image_free(original_image);
    if(error != 0) {
        fprintf(stderr, ERROR_PREFIX "failed to scale up \"%s\": %s\n", image_filepath, strerror(error));
        return 1;
    }

    image_write_to_file(scaled_image, stdout, original_image_type, JPG_QUALITY);

    free(scaled_image.data);
    return 0;
}
