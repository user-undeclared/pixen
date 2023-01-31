#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#define UNREACHABLE assert(0 && "UNREACHABLE");

#define ERROR_PREFIX "ERROR - "
#define USAGE "usage: pixen MULTIPLIER IMAGES..."

#define MAX_SIZE_MULTIPLIER 128
static_assert(MAX_SIZE_MULTIPLIER < UINT_MAX);

#define JPG_QUALITY 75

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

struct Image {
    unsigned width, height, components;
    size_t size;
    uint8_t* data;
};

enum ImageType {
    IMAGETYPE_PNG,
    IMAGETYPE_BMP,
    IMAGETYPE_TGA,
    IMAGETYPE_JPG,
    IMAGETYPE_HDR
};

bool load_image(const char* image_filepath, struct Image* result) {
    FILE* image_file = fopen(image_filepath, "rb");
    if(image_file == NULL) {
        fprintf(stderr, ERROR_PREFIX "failed to open file \"%s\": %s\n", image_filepath, strerror(errno));
        return false;
    }

    int width, height, components;
    uint8_t* imagedata = stbi_load_from_file(image_file, &width, &height, &components, 0);
    fclose(image_file);

    if(imagedata == NULL) {
        fprintf(stderr, ERROR_PREFIX "\"%s\" is not an image\n", image_filepath);
        return false;
    }
    
    *result = (struct Image) {
        .width = width,
        .height = height,
        .components = components,
        .size = width * height * components,
        .data = imagedata
    };
    return true;
}

const char* get_filename_extention(const char* filepath) {
    for(size_t index = strlen(filepath); index > 0; index--) {
        if(filepath[index - 1] == '.') {
            return &filepath[index];
        }
    }
    return NULL;
}

#if 0
char* append_to_filename(const char* filepath, const char* filename_suffix) {
    (void) filepath;
    (void) filename_suffix;

    return NULL;
}

int main(int argc, char* argv[]) {
    if(argc < 1) return 1;
    for(int index = 1; index < argc; index++) {
        
    }
    return 0;
}
#endif

int scale_image(struct Image image, unsigned size_multiplier, struct Image* scaled_image) {
    size_t scaled_image_size = image.width * image.height * image.components * size_multiplier;
    uint8_t* scaled_imagedata = malloc(scaled_image_size);
    if(scaled_imagedata == NULL) return errno;

    memcpy(scaled_imagedata, image.data, image.width * image.height * image.components);
    *scaled_image = (struct Image) {
        .width = image.width * size_multiplier,
        .height = image.height * size_multiplier,
        .components = image.components,
        .size = scaled_image_size,
        .data = scaled_imagedata
    };
    return 0;
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

bool image_type_from_string(const char* file_extention, enum ImageType* result) {
    if(string_equal_ignore_case(file_extention, "png")) {
        *result = IMAGETYPE_PNG;
        return true;
    }
    if(string_equal_ignore_case(file_extention, "bmp")) {
        *result = IMAGETYPE_BMP;
        return true;
    }
    if(string_equal_ignore_case(file_extention, "tga")) {
        *result = IMAGETYPE_TGA;
        return true;
    }
    if(string_equal_ignore_case(file_extention, "hdr")) {
        *result = IMAGETYPE_HDR;
        return true;
    }
    if(string_equal_ignore_case(file_extention, "jpg") || string_equal_ignore_case(file_extention, "jpeg")) {
        *result = IMAGETYPE_JPG;
        return true;
    }

    return false;
}

void write_to_file(void* file_pointer, void* data, int size) {
    FILE* file = (FILE*) file_pointer;
    fwrite(data, 1, size, file);
}

void write_image(struct Image image, enum ImageType image_type) {
    void* context = (void*) stdout;

    switch(image_type) {
        case IMAGETYPE_PNG:
            stbi_write_png_to_func(write_to_file, context, image.width, image.height, image.components, image.data, 0);
            return;
        case IMAGETYPE_BMP:
            stbi_write_bmp_to_func(write_to_file, context, image.width, image.height, image.components, image.data);
            return;
        case IMAGETYPE_TGA:
            stbi_write_tga_to_func(write_to_file, context, image.width, image.height, image.components, image.data);
            return;
        case IMAGETYPE_JPG:
            stbi_write_jpg_to_func(write_to_file, context, image.width, image.height, image.components, image.data, JPG_QUALITY);
            return;
        case IMAGETYPE_HDR:
            stbi_write_bmp_to_func(write_to_file, context, image.width, image.height, image.components, image.data);
            return;
        default:
            UNREACHABLE;
    }
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

        enum ImageType image_type;
        if(!image_type_from_string(image_file_extention, &image_type)) {
            fprintf(stderr, ERROR_PREFIX "unsupported image type \"%s\"\n", image_file_extention);
            continue;
        }

        struct Image image;
        if(!load_image(image_filepath, &image)) continue;

        struct Image scaled_image;
        int error = scale_image(image, size_multiplier, &scaled_image);
        stbi_image_free(image.data);
        if(error != 0) {
            fprintf(stderr, ERROR_PREFIX "failed to scale up \"%s\": %s\n", image_filepath, strerror(error));
            continue;
        }

        write_image(scaled_image, image_type);
        free(scaled_image.data);
    }

    return 0;
}
