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
void img_file_write(const char* path, uint8_t* img, unsigned int width, unsigned int height, unsigned int in_channels);

void img_set_jpeg_quality(int quality);
uint8_t* img_jcompress(uint8_t* img, unsigned int width, unsigned int height, unsigned int channels, unsigned int quality);
uint8_t* img_transform_buffer(uint8_t* buffer, unsigned int width, unsigned int height, unsigned int src, unsigned int dest);

/***********************
 -> PNG save and load <- 
***********************/

uint8_t* png_file_load(const char* path, unsigned int* width, unsigned int* height);
void png_file_write(const char* path, uint8_t* data, unsigned int width, unsigned int height);

/************************
 -> JPEG save and load <- 
************************/

uint8_t* jpeg_file_load(const char* path, unsigned int* w, unsigned int* h);
void jpeg_file_write(const char* path, uint8_t* data, unsigned int width, unsigned int height, int quality);
uint8_t* jpeg_compress(uint8_t* data, unsigned int* size, unsigned int width, unsigned height, int quality);
uint8_t* jpeg_decompress(uint8_t* data, unsigned int size);

/************************
 -> PPM save and load  <- 
************************/

uint8_t* ppm_file_load(const char* path, unsigned int* width, unsigned int* height);
void ppm_file_write(const char* path, uint8_t* img, unsigned int width, unsigned int height);

/*************************
 -> GIF save and load  <- 
*************************/

gif_t* gif_file_load(const char* path);
uint8_t* gif_file_load_frame(const char* path, unsigned int* width, unsigned int* height);
void gif_free(gif_t* gif);
void gif_file_write(const char* path, gif_t* input);
void gif_file_write_frame(const char* path, uint8_t* img, unsigned int width, unsigned int height);

bmp_t* gif_to_bmp(gif_t* gif, unsigned int* count);
gif_t* bmp_to_gif(bmp_t* bitmaps, unsigned int count);

/*****************************
 -> Greyscale, RGB and RGBA <-
 ****************************/

uint8_t* rgba_to_greyscale(uint8_t* buffer, unsigned int width, unsigned int height);
uint8_t* rgb_to_greyscale(uint8_t* buffer, unsigned int width, unsigned int height);
uint8_t* rgb_to_rgba(uint8_t* buffer, unsigned int width, unsigned int height);
uint8_t* rgba_to_rgb(uint8_t* buffer, unsigned int width, unsigned int height);

/***************************
 -> Bitmap Data Structure <-
 **************************/

uint8_t* px_at(bmp_t* bitmap, unsigned int x, unsigned int y);
bmp_t bmp_new(unsigned int width, unsigned int height, unsigned int channels);
bmp_t bmp_color(unsigned int width, unsigned int height, unsigned int channels, uint8_t* color);
bmp_t bmp_load(const char* path);
void bmp_write(const char* path, bmp_t* bitmap);
void bmp_free(bmp_t* bitmap);
bmp_t bmp_copy(bmp_t* bmp);

/**************************************
 -> Bitmap algorithms and operations <-
 *************************************/

bmp_t bmp_negative(bmp_t* bitmap);
bmp_t bmp_flip_vertical(bmp_t* bitmap);
bmp_t bmp_flip_horizontal(bmp_t* bitmap);
bmp_t bmp_black_and_white(bmp_t* bitmap);
bmp_t bmp_greyscale(bmp_t* bitmap);
bmp_t bmp_rotate(bmp_t* bitmap);
bmp_t bmp_scale(bmp_t* bitmap);
bmp_t bmp_white_to_transparent(bmp_t* bitmap);
bmp_t bmp_cut(bmp_t* bitmap);
bmp_t bmp_reduce(bmp_t* bitmap);
bmp_t bmp_clear_to_transparent(bmp_t* bitmap, uint8_t sensibility);
bmp_t bmp_transform(bmp_t* bitmap, unsigned int channels);
bmp_t bmp_jcompress(bmp_t* bitmap, unsigned int quality);

bmp_t bmp_resize_width(bmp_t* bmp, unsigned int target_width);
bmp_t bmp_resize_height(bmp_t* bmp, unsigned int target_height);
bmp_t bmp_scale_lerp(bmp_t* bmp, float f);

#ifdef __cplusplus
}
#endif
#endif
