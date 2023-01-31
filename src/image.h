#ifndef _IMAGE_H
#define _IMAGE_H

#include <stdbool.h>

enum ImageType {
    IMAGETYPE_PNG,
    IMAGETYPE_BMP,
    IMAGETYPE_TGA,
    IMAGETYPE_JPG,
    IMAGETYPE_HDR
};

bool image_type_from_string(const char* file_extention, enum ImageType* result);

struct Image {
    unsigned width, height, components;
    size_t size;
    uint8_t* data;
};

bool image_load(FILE* image_file, struct Image* result);
void image_free(const struct Image image);
int image_scale(struct Image image, unsigned size_multiplier, struct Image* image_scaled);
void print_image(struct Image image, enum ImageType image_type);

#endif //_IMAGE_H
