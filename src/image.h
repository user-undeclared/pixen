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

//TODO: change function name format from VERB_image to image_VERB
bool load_image(const char* image_filepath, struct Image* result);
void free_image(const struct Image image);
int scale_image(struct Image image, unsigned size_multiplier, struct Image* scaled_image);
void write_image(struct Image image, enum ImageType image_type);

#endif //_IMAGE_H
