#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>

#include "string_utils.h"
#include "image.h"

#define ERROR_PREFIX "ERROR - "
#define USAGE "usage: pixen MULTIPLIER IMAGES..."

#define MAX_SIZE_MULTIPLIER 128

bool parse_long(const char* string, long* result) {
    char* endptr;

    errno = 0;
    const long parsed_long = strtol(string, &endptr, 0);

    if(errno == ERANGE) {
        if(parsed_long == LONG_MIN) fprintf(stderr, "ERROR: %s is too small to parse\n", string);
        else fprintf(stderr, ERROR_PREFIX "%s is too big to parse\n", string);
        return false;
    }

    if(*endptr != '\0') {
        fprintf(stderr, ERROR_PREFIX "\"%s\" is not a number\n", string);
        return false;
    }

    *result = parsed_long;
    return true;
}

bool parse_size_multiplier(const char* string, unsigned* result) {
    long size_multiplier;
    if(!parse_long(string, &size_multiplier)) return false;

    if(size_multiplier < 0) {
        fputs(ERROR_PREFIX "size multiplier cannot be negative\n", stderr);
        return false;
    }
    if(size_multiplier > MAX_SIZE_MULTIPLIER) {
        fprintf(stderr, ERROR_PREFIX "scale multiplier too big (max is %i)\n", MAX_SIZE_MULTIPLIER);
        return false;
    }
    
    *result = (unsigned) size_multiplier;
    return true; 
}

int main(int argc, char* argv[]) {
    if(argc < 3) {
        fputs(USAGE "\n" ERROR_PREFIX "too few arguments provided\n", stderr);
        return 1;
    }

    unsigned size_multiplier;
    if(!parse_size_multiplier(argv[1], &size_multiplier)) return 1;

    unsigned image_filename_count = argc - 2;
    char** image_filepath_array = &argv[2];

    for(unsigned index = 0; index < image_filename_count; index++) {
        const char* image_filepath = image_filepath_array[index];
        const char* image_file_extention = get_filename_extention(image_filepath);

        FILE* image_file = fopen(image_filepath, "rb");
        if(image_file == NULL) {
            fprintf(stderr, ERROR_PREFIX "failed to open file \"%s\": %s\n", image_filepath, strerror(errno));
            return 1;
        }

        struct Image image;
        bool loaded_image = image_load(image_file, &image);
        fclose(image_file);
        if(!loaded_image) {
            fprintf(stderr, ERROR_PREFIX "\"%s\" is not an image\n", image_filepath);
            return 1;
        }

        enum ImageType image_type;
        if(!image_type_from_string(image_file_extention, &image_type)) {
            fprintf(stderr, ERROR_PREFIX "unsupported image type \"%s\"\n", image_file_extention);
            continue;
        }

        if(size_multiplier == 1) {
            print_image(image, image_type);
            image_free(image);
            continue;
        }

        struct Image scaled_image;
        int error = image_scale(image, size_multiplier, &scaled_image);
        image_free(image);
        if(error != 0) {
            fprintf(stderr, ERROR_PREFIX "failed to scale up \"%s\": %s\n", image_filepath, strerror(error));
            continue;
        }

        print_image(scaled_image, image_type);
        free(scaled_image.data);
    }

    return 0;
}
