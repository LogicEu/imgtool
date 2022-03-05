#ifndef IMGTOOL_H
#define IMGTOOL_H

#ifdef __cplusplus
extern "C" {
#endif

/*================================================
>>>>>>>>>>>>>>>  IMGTOOL HEADER   >>>>>>>>>>>>>>>>
C API to easily load, save and transform different
image formats such as PNG, JPEG, PPM and GIF.
====================================== @eulogic */

#include <stdint.h>

typedef enum {
    IMG_NULL,
    IMG_G,              // Greyscale                    1 channel
    IMG_GA,             // Greyscale & Alpha            2 channels
    IMG_RGB,            // Red, Green & Blue            3 channels
    IMG_RGBA            // Red, Green, Blue & Alpha     4 channels
} img_channel_enum;

typedef enum {
    IMG_FORMAT_NULL,    // Undefined
    IMG_FORMAT_PNG,
    IMG_FORMAT_JPG,
    IMG_FORMAT_PPM,
    IMG_FORMAT_GIF
} img_format_enum;

typedef uint8_t* px_t;

typedef struct {
    unsigned int width, height, channels;
    uint8_t* pixels;
} bmp_t;

typedef struct {
    unsigned int size, used, width, height;
    uint8_t** frames;
    uint8_t background[3];
} gif_t;

/***********************
 -> img save and load <- 
***********************/

uint8_t* img_file_load(const char* path, unsigned int* width, unsigned int* height, unsigned int* out_channels);
void img_file_write(const char* path, const uint8_t* img, const unsigned int width, const unsigned int height, const unsigned int in_channels);

void img_set_jpeg_quality(const int quality);
uint8_t* img_jcompress(const uint8_t* img, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int quality);
uint8_t* img_transform_buffer(const uint8_t* buffer, const unsigned int width, const unsigned int height, const unsigned int src, const unsigned int dest);

/***********************
 -> PNG save and load <- 
***********************/

uint8_t* png_file_load(const char* path, unsigned int* width, unsigned int* height);
void png_file_write(const char* path, const uint8_t* data, const unsigned int width, const unsigned int height);

/************************
 -> JPEG save and load <- 
************************/

uint8_t* jpeg_file_load(const char* path, unsigned int* w, unsigned int* h);
void jpeg_file_write(const char* path, const uint8_t* data, const unsigned int width, const unsigned int height, const int quality);
uint8_t* jpeg_compress(const uint8_t* data, unsigned int* size, const unsigned int width, const unsigned height, const int quality);
uint8_t* jpeg_decompress(const uint8_t* data, const unsigned int size);

/************************
 -> PPM save and load  <- 
************************/

uint8_t* ppm_file_load(const char* path, unsigned int* width, unsigned int* height);
void ppm_file_write(const char* path, const uint8_t* img, const unsigned int width, const unsigned int height);

/*************************
 -> GIF save and load  <- 
*************************/

gif_t* gif_file_load(const char* path);
uint8_t* gif_file_load_frame(const char* path, unsigned int* width, unsigned int* height);
void gif_free(gif_t* gif);
void gif_file_write(const char* path, const gif_t* input);
void gif_file_write_frame(const char* path, const uint8_t* img, const unsigned int width, const unsigned int height);

bmp_t* gif_to_bmp(const gif_t* gif, unsigned int* count);
gif_t* bmp_to_gif(const bmp_t* bitmaps, const unsigned int count);

/*****************************
 -> Greyscale, RGB and RGBA <-
 ****************************/

uint8_t* rgba_to_greyscale(const uint8_t* buffer, const unsigned int width, const unsigned int height);
uint8_t* rgb_to_greyscale(const uint8_t* buffer, const unsigned int width, const unsigned int height);
uint8_t* rgb_to_rgba(const uint8_t* buffer, const unsigned int width, const unsigned int height);
uint8_t* rgba_to_rgb(const uint8_t* buffer, const unsigned int width, const unsigned int height);

/***************************
 -> Bitmap Data Structure <-
 **************************/

uint8_t* px_at(const bmp_t* bitmap, const unsigned int x, const unsigned int y);
bmp_t bmp_new(const unsigned int width, const unsigned int height, const unsigned int channels);
bmp_t bmp_color(const unsigned int width, const unsigned int height, const unsigned int channels, const uint8_t* color);
bmp_t bmp_load(const char* path);
void bmp_write(const char* path, const bmp_t* bitmap);
bmp_t bmp_copy(const bmp_t* bmp);
void bmp_free(bmp_t* bitmap);

/**************************************
 -> Bitmap algorithms and operations <-
 *************************************/

bmp_t bmp_negative(const bmp_t* bitmap);
bmp_t bmp_flip_vertical(const bmp_t* bitmap);
bmp_t bmp_flip_horizontal(const bmp_t* bitmap);
bmp_t bmp_black_and_white(const bmp_t* bitmap);
bmp_t bmp_greyscale(const bmp_t* bitmap);
bmp_t bmp_rotate(const bmp_t* bitmap);
bmp_t bmp_scale(const bmp_t* bitmap);
bmp_t bmp_white_to_transparent(const bmp_t* bitmap);
bmp_t bmp_cut(const bmp_t* bitmap);
bmp_t bmp_reduce(const bmp_t* bitmap);
bmp_t bmp_clear_to_transparent(const bmp_t* bitmap, const uint8_t sensibility);
bmp_t bmp_transform(const bmp_t* bitmap, const unsigned int channels);
bmp_t bmp_jcompress(const bmp_t* bitmap, const unsigned int quality);

bmp_t bmp_resize_width(const bmp_t* bmp, const unsigned int target_width);
bmp_t bmp_resize_height(const bmp_t* bmp, const unsigned int target_height);
bmp_t bmp_scale_lerp(const bmp_t* bmp, const float f);

#ifdef __cplusplus
}
#endif
#endif
