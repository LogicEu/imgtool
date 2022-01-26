#include <imgtool.h>
#include <stdlib.h>
#include <string.h>

/***************************
 -> Bitmap Data Structure <-
 **************************/

uint8_t* px_at(bmp_t* bmp, unsigned int x, unsigned int y)
{
    return bmp->pixels + (bmp->width * y + x) * bmp->channels;
}

bmp_t bmp_new(unsigned int width, unsigned int height, unsigned int channels)
{
    bmp_t bitmap;
    bitmap.pixels = (uint8_t*)calloc(width * height * channels, 1);
    bitmap.channels = channels;
    bitmap.height = height;
    bitmap.width = width;
    return bitmap;
}

bmp_t bmp_copy(bmp_t* bmp)
{
    bmp_t ret;
    ret.width = bmp->width;
    ret.height = bmp->height;
    ret.channels = bmp->channels;
    size_t size = ret.width * ret.height * ret.channels;
    ret.pixels = (uint8_t*)malloc(size);
    memcpy(ret.pixels, bmp->pixels, size);
    return ret;
}

bmp_t bmp_color(unsigned int width, unsigned int height, unsigned int channels, const uint8_t* restrict color)
{
    bmp_t bitmap = bmp_new(width, height, channels);
    for (unsigned int y = 0; y < height; y ++) {
        for (unsigned int x = 0; x < width; x ++) {
            memcpy(bitmap.pixels + (width * y + x) * bitmap.channels, color, channels);
        }
    }
    return bitmap;
}

bmp_t bmp_load(const char* path) 
{
    bmp_t bitmap;
    bitmap.pixels = img_file_load(path, &bitmap.width, &bitmap.height, &bitmap.channels);
    return bitmap;
}

void bmp_write(const char* path, bmp_t* bitmap) 
{
    img_file_write(path, bitmap->pixels, bitmap->width, bitmap->height, bitmap->channels);
}

void bmp_free(bmp_t* bitmap)
{
    if (bitmap->pixels != NULL) {
        free(bitmap->pixels);
    }
}
