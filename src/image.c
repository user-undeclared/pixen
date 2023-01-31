#include <errno.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "string_utils.h"
#include "image.h" 

#define JPG_QUALITY 75

#define UNREACHABLE assert(0 && "UNREACHABLE");

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

bool image_load(const char* image_filepath, struct Image* result) {
    FILE* image_file = fopen(image_filepath, "rb");
    if(image_file == NULL) return false;

    int width, height, components;
    uint8_t* imagedata = stbi_load_from_file(image_file, &width, &height, &components, 0);
    fclose(image_file);

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

void print_buffer(void* file_pointer, void* data, int size) {
    FILE* file = (FILE*) file_pointer;
    fwrite(data, 1, size, file);
}

void print_image(struct Image image, enum ImageType image_type) {
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
            stbi_write_jpg_to_func(print_buffer, context, image.width, image.height, image.components, image.data, JPG_QUALITY);
            return;
        case IMAGETYPE_HDR:
            stbi_write_bmp_to_func(print_buffer, context, image.width, image.height, image.components, image.data);
            return;
        default:
            UNREACHABLE;
    }
}

