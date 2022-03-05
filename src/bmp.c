#include <imgtool.h>
#include <stdlib.h>
#include <string.h>

/***************************
 -> Bitmap Data Structure <-
 **************************/

uint8_t* px_at(const bmp_t* restrict bmp, const unsigned int x, const unsigned int y)
{
    return bmp->pixels + (bmp->width * y + x) * bmp->channels;
}

bmp_t bmp_new(const unsigned int width, const unsigned int height, const unsigned int channels)
{
    bmp_t bitmap;
    bitmap.pixels = calloc(width * height, channels);
    bitmap.channels = channels;
    bitmap.height = height;
    bitmap.width = width;
    return bitmap;
}

bmp_t bmp_copy(const bmp_t* restrict bmp)
{
    bmp_t ret;
    ret.width = bmp->width;
    ret.height = bmp->height;
    ret.channels = bmp->channels;
    
    const size_t size = ret.width * ret.height * ret.channels;
    ret.pixels = calloc(size, 1);
    memcpy(ret.pixels, bmp->pixels, size);
    return ret;
}

bmp_t bmp_color(const unsigned int width, const unsigned int height, const unsigned int channels, const uint8_t* restrict color)
{
    bmp_t bitmap = bmp_new(width, height, channels);
    for (unsigned int y = 0; y < height; y ++) {
        for (unsigned int x = 0; x < width; x ++) {
            memcpy(bitmap.pixels + (width * y + x) * bitmap.channels, color, channels);
        }
    }
    return bitmap;
}

bmp_t bmp_load(const char* restrict path) 
{
    bmp_t bitmap;
    bitmap.pixels = img_file_load(path, &bitmap.width, &bitmap.height, &bitmap.channels);
    return bitmap;
}

void bmp_write(const char* restrict path, const bmp_t* restrict bitmap) 
{
    img_file_write(path, bitmap->pixels, bitmap->width, bitmap->height, bitmap->channels);
}

void bmp_free(bmp_t* bitmap)
{
    if (bitmap->pixels != NULL) {
        free(bitmap->pixels);
    }
}
