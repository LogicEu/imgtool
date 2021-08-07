#include <imgtool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/***********************
 -> img save and load <- 
***********************/

static int jpeg_quality = 100;

static char* img_parse_suffix(const char* path)
{
    unsigned int size = strlen(path);
    if (strlen(path) < 5) {
        printf("Invalid path for img file '%s'\n", path);
        return NULL;
    }
    char* suffix = (char*)malloc(5);
    for (int i = 0; i < 4; i++) {
        suffix[i] = path[size - 4 + i];
    }
    suffix[4] = '\0';
    return suffix;
}

static img_channel_enum img_parse_channels(img_format_enum format)
{
    if (!format) return IMG_NULL;
    if (format == IMG_FORMAT_PNG) return IMG_RGBA;
    return IMG_RGB;
}

static img_format_enum img_parse_format(const char* suffix)
{
    if (!strcmp(suffix, ".jpg") || 
        !strcmp(suffix, "jpeg") ||
        !strcmp(suffix, "JPEG") ||
        !strcmp(suffix, ".JPG")) {
        return IMG_FORMAT_JPG;
    }
    if (!strcmp(suffix, ".png") ||
        !strcmp(suffix, ".PNG")) { 
        return IMG_FORMAT_PNG;
    }
    if (!strcmp(suffix, ".ppm") ||
        !strcmp(suffix, ".PPM")) {
        return IMG_FORMAT_PPM;
    }
    if (!strcmp(suffix, ".gif") || 
        !strcmp(suffix, ".GIF")) { 
        return IMG_FORMAT_GIF;
    }
    return IMG_FORMAT_NULL;
}

static uint8_t* img_file_load_any(const char* path, unsigned int* width, unsigned int* height, img_format_enum format)
{
    if (format == IMG_FORMAT_PNG) {
        return png_file_load(path, width, height);
    } else if (format == IMG_FORMAT_JPG) {
        return jpeg_file_load(path, width, height);
    } else if (format == IMG_FORMAT_PPM) {
        return ppm_file_load(path, width, height);
    } else if (format == IMG_FORMAT_GIF) {
        return gif_file_load_frame(path, width, height);
    } 
    return NULL;
}

static void img_file_write_any(const char* path, uint8_t* img, unsigned int width, unsigned int height, img_format_enum format)
{
    if (format == IMG_FORMAT_PNG) {
        png_file_write(path, img, width, height);
    } else if (format == IMG_FORMAT_JPG) {
        jpeg_file_write(path, img, width, height, jpeg_quality);
    } else if (format == IMG_FORMAT_PPM) {
        ppm_file_write(path, img, width, height);
    } else if (format == IMG_FORMAT_GIF) {
        gif_file_write_frame(path, img, width, height);
    } else printf("Format is not supported to write.\n");
}

uint8_t* img_transform_buffer(uint8_t* buffer, unsigned int width, unsigned int height, unsigned int src, unsigned int dest)
{
    uint8_t* ret = NULL;
    if (src == IMG_RGB && dest == IMG_RGBA) {
        ret = rgb_to_rgba(buffer, width, height);
    } else if (src == IMG_RGBA && dest == IMG_RGB) {
        ret = rgba_to_rgb(buffer, width, height);
    } else if (src == IMG_RGB && dest == IMG_G) {
        ret = rgb_to_greyscale(buffer, width, height);
    } else if (src == IMG_RGBA && dest == IMG_G) {
        ret = rgba_to_greyscale(buffer, width, height);
    }
    return ret;
}

uint8_t* img_file_load(const char* path, unsigned int* width, unsigned int* height, unsigned int* out_channels)
{
    char* suffix = img_parse_suffix(path);
    if (!suffix) return NULL;

    img_format_enum format = img_parse_format(suffix);
    *out_channels = img_parse_channels(format);
    if (!format || !*out_channels) {
        printf("File extension '%s' is not recognized.\n", suffix);
        return NULL;
    }

    uint8_t* buffer = img_file_load_any(path, width, height, format);
    if (!buffer) {
        printf("There was a problem loading file '%s'\n", path);
        return NULL;
    }
    return buffer;
}

void img_file_write(const char* path, uint8_t* img, unsigned int width, unsigned int height, unsigned int in_channels)
{
    char* suffix = img_parse_suffix(path);
    if (!suffix) {
        printf("Invalid suffix '%s' for output file\n", suffix);
        return;
    }
    img_format_enum format = img_parse_format(suffix);
    img_channel_enum parse_channel = img_parse_channels(format);
    if (!format || !parse_channel) {
        printf("File extension '%s' is not supported.\n", suffix);
        return;
    }

    if (in_channels != parse_channel) {
        uint8_t* buffer = img_transform_buffer(img, width, height, in_channels, parse_channel);
        if (!buffer) {
            printf("There was a problem transforming file '%s'\n", path);
            return;
        }
        img_file_write_any(path, buffer, width, height, format);
        free(buffer);
    } else img_file_write_any(path, img, width, height, format);
}

uint8_t* img_jcompress(uint8_t* img, unsigned int width, unsigned int height, unsigned int channels, unsigned int quality)
{
    uint8_t* buffer;
    if (channels != IMG_RGB) {
        buffer = img_transform_buffer(img, width, height, channels, IMG_RGB);
        if (!buffer) {
            printf("There was a problem transforming RGBA buffer to RGB\n");
            return NULL;
        }
    } else buffer = img;

    unsigned int size;
    uint8_t* compress = jpeg_compress(buffer, &size, width, height, quality);
    uint8_t* decompress = jpeg_decompress(compress, size);

    if (channels != IMG_RGB) free(buffer);
    free(compress);
    return decompress;
}

void img_set_jpeg_quality(int quality)
{
    jpeg_quality = quality;
}