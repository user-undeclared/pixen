#include <errno.h>
#include <assert.h>
#include <ctype.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "image.h" 

bool string_equal_ignore_case(const char* string1, const char* string2) {
    if(string1 == NULL || string2 == NULL) return false;

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
    int image_type = -1;

    if(string_equal_ignore_case(file_extention, "png"))
        image_type = IMAGETYPE_PNG;
    else if(string_equal_ignore_case(file_extention, "bmp"))
        image_type = IMAGETYPE_BMP;
    else if(string_equal_ignore_case(file_extention, "tga"))
        image_type = IMAGETYPE_TGA;
    else if(string_equal_ignore_case(file_extention, "hdr"))
        image_type = IMAGETYPE_HDR;
    else if(string_equal_ignore_case(file_extention, "jpg") || string_equal_ignore_case(file_extention, "jpeg"))
        image_type = IMAGETYPE_JPG;

    if(image_type == -1) return false;
    *result = image_type;
    return true;
}

bool image_load(FILE* image_file, struct Image* result) {
    int width, height, components;
    uint8_t* imagedata = stbi_load_from_file(image_file, &width, &height, &components, 0);
    if(imagedata == NULL) return false;
    
    *result = (struct Image) {
        .width = width,
        .height = height,
        .components = components,
        .size = width * height * components,
        .data = imagedata
    };
    return true;
}

void image_free(const struct Image image) {
    stbi_image_free(image.data);
}

int image_scale(struct Image image, unsigned size_multiplier, struct Image* scaled_image) {
    unsigned scaled_width = image.width * size_multiplier;
    unsigned scaled_height = image.height * size_multiplier;
    size_t scaled_image_size = scaled_width * scaled_height * image.components;

    uint8_t* scaled_imagedata = malloc(scaled_image_size);
    if(scaled_imagedata == NULL) return errno;

    for(unsigned column = 0; column < scaled_width; column++) {
        for(unsigned row = 0; row < scaled_height; row++) {
            unsigned data_index = ((row / size_multiplier) * image.width + (column / size_multiplier)) * image.components;
            unsigned scaled_index = (row * scaled_width + column) * image.components;
            for(unsigned component = 0; component < image.components; component++) {
                scaled_imagedata[scaled_index + component] = image.data[data_index + component];
            }
        }
    }
        
    *scaled_image = (struct Image) {
        .width = scaled_width,
        .height = scaled_height,
        .components = image.components,
        .size = scaled_image_size,
        .data = scaled_imagedata
    };
    return 0;
}

void print_buffer(void* file_pointer, void* data, int size) {
    FILE* file = (FILE*) file_pointer;
    fwrite(data, 1, size, file);
}

void print_image(struct Image image, enum ImageType image_type, unsigned jpg_quality) {
    void* context = (void*) stdout;

    switch(image_type) {
        case IMAGETYPE_PNG:
            stbi_write_png_to_func(print_buffer, context, image.width, image.height, image.components, image.data, 0);
            return;
        case IMAGETYPE_BMP:
            stbi_write_bmp_to_func(print_buffer, context, image.width, image.height, image.components, image.data);
            return;
        case IMAGETYPE_TGA:
            stbi_write_tga_to_func(print_buffer, context, image.width, image.height, image.components, image.data);
            return;
        case IMAGETYPE_JPG:
            stbi_write_jpg_to_func(print_buffer, context, image.width, image.height, image.components, image.data, jpg_quality);
            return;
        case IMAGETYPE_HDR:
            stbi_write_bmp_to_func(print_buffer, context, image.width, image.height, image.components, image.data);
            return;
        default:
            assert(0 && "UNREACHABLE");
    }
}

