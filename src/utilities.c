#include <imgtool.h>
#include <stdlib.h>
#include <stdio.h>

/*****************************
 -> Greyscale, RGB and RGBA <-
 ****************************/

uint8_t* rgba_to_greyscale(uint8_t* buffer, unsigned int width, unsigned int height)
{
    printf("Converting from RGBA to greyscale color format\n");
    uint8_t* ret = (uint8_t*)malloc(width * height);
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            unsigned int m = 0;
            m += buffer[(width * y + x) * 4 + 0];
            m += buffer[(width * y + x) * 4 + 1];
            m += buffer[(width * y + x) * 4 + 2];
            ret[(width * y + x)] = (uint8_t)(m / 3);
        }
    }
    printf("Process completed\n");
    return ret;
}

uint8_t* rgb_to_greyscale(uint8_t* buffer, unsigned int width, unsigned int height)
{
    printf("Converting from RGB to greyscale color format\n");
    uint8_t* ret = (uint8_t*)malloc(width * height);
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            unsigned int m = 0;
            m += buffer[(width * y + x) * 3 + 0];
            m += buffer[(width * y + x) * 3 + 1];
            m += buffer[(width * y + x) * 3 + 2];
            ret[(width * y + x)] = (uint8_t)(m / 3);
        }
    }
    printf("Process completed\n");
    return ret;
}

uint8_t* rgb_to_rgba(uint8_t* buffer, unsigned int width, unsigned int height)
{
    printf("Converting from RGB to RGBA color format\n");
    uint8_t* ret = (uint8_t*)malloc(width * height * 4);
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            ret[(width * y + x) * 4 + 0] = buffer[(width * y + x) * 3 + 0];
            ret[(width * y + x) * 4 + 1] = buffer[(width * y + x) * 3 + 1];
            ret[(width * y + x) * 4 + 2] = buffer[(width * y + x) * 3 + 2];
            ret[(width * y + x) * 4 + 3] = 255;
        }
    }
    printf("Process completed\n");
    return ret;
}

uint8_t* rgba_to_rgb(uint8_t* buffer, unsigned int width, unsigned int height)
{
    printf("Converting from RGBA to RGB color format\n");
    uint8_t* ret = (uint8_t*)malloc(width * height * 3);
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            ret[(width * y + x) * 3 + 0] = buffer[(width * y + x) * 4 + 0];
            ret[(width * y + x) * 3 + 1] = buffer[(width * y + x) * 4 + 1];
            ret[(width * y + x) * 3 + 2] = buffer[(width * y + x) * 4 + 2];
        }
    }
    printf("Process completed\n");
    return ret;
}