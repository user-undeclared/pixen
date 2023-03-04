#ifndef _IMAGE_H
#define _IMAGE_H

#include <stdbool.h>

/* these correspond to the image types supported by both stb_image and stb_image_write */
enum image_type {
    IMAGETYPE_PNG = 0,
    IMAGETYPE_BMP,
    IMAGETYPE_TGA,
    IMAGETYPE_JPG,
    IMAGETYPE_HDR
};

bool image_type_from_string(const char* file_extention, enum image_type* result);

struct image {
    unsigned width, height, components;
    uint8_t* data;
};

bool image_load(FILE* image_file, struct image* result);
void image_free(struct image image);
int image_scale(const struct image source_image, unsigned size_multiplier, struct image* scaled_image);
void image_write_to_file(const struct image image, FILE* file, enum image_type image_type, unsigned jpg_quality);

#endif /* _IMAGE_H */
