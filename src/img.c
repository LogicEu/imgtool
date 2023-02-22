#include <imgtool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/***********************
 -> img save and load <- 
***********************/

static int jpeg_quality = 100;

static char* img_parse_suffix(const char* restrict path)
{
    int size = strlen(path), found = -1;
    for (int i = 0; i < size; i++) {
        if (path[i] == '.') {
            found = i;
            break;
        }
    }
    
    if (found == -1) {
        fprintf(stderr, "imgtool needs a file extension for '%s'\n", path);
        return NULL;
    }

    unsigned int suffix_size = size - found;
    char* suffix = (char*)malloc(suffix_size + 1);
    strcpy(suffix, &path[found]);
    return suffix;
}

static img_channel_enum img_parse_channels(const img_format_enum format)
{
    if (!format) return IMG_NULL;
    if (format == IMG_FORMAT_PNG) return IMG_RGBA;
    return IMG_RGB;
}

static img_format_enum img_parse_format(const char* restrict suffix)
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

static uint8_t* img_file_load_any(const char* restrict path, unsigned int* width, unsigned int* height, const img_format_enum format)
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

static void img_file_write_any(const char* restrict path, const uint8_t* restrict img, const unsigned int width, const unsigned int height, const img_format_enum format)
{
    if (format == IMG_FORMAT_PNG) {
        png_file_write(path, img, width, height);
    } else if (format == IMG_FORMAT_JPG) {
        jpeg_file_write(path, img, width, height, jpeg_quality);
    } else if (format == IMG_FORMAT_PPM) {
        ppm_file_write(path, img, width, height);
    } else if (format == IMG_FORMAT_GIF) {
        gif_file_write_frame(path, img, width, height);
    } else fprintf(stderr, "imgtool cannot write specified file extension.\n");
}

uint8_t* img_transform_buffer(const uint8_t* restrict buffer, const unsigned int width, const unsigned int height, const unsigned int src, const unsigned int dest)
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

uint8_t* img_file_load(const char* restrict path, unsigned int* width, unsigned int* height, unsigned int* out_channels)
{
    char* suffix = img_parse_suffix(path);
    if (!suffix) return NULL;

    img_format_enum format = img_parse_format(suffix);
    *out_channels = img_parse_channels(format);
    if (!format || !*out_channels) {
        fprintf(stderr, "imgtool does not recognize file extension '%s'\n", suffix);
        free(suffix);
        return NULL;
    }
    free(suffix);

    return img_file_load_any(path, width, height, format);
}

void img_file_write(const char* restrict path, const uint8_t* restrict img, const unsigned int width, const unsigned int height, const unsigned int in_channels)
{
    char* suffix = img_parse_suffix(path);
    if (!suffix) return;

    img_format_enum format = img_parse_format(suffix);
    img_channel_enum parse_channel = img_parse_channels(format);
    if (!format || !parse_channel) {
        fprintf(stderr, "imgtool does not recognize file extension '%s'\n", suffix);
        free(suffix);
        return;
    }
    free(suffix);

    if (in_channels != parse_channel) {
        uint8_t* buffer = img_transform_buffer(img, width, height, in_channels, parse_channel);
        if (!buffer) {
            fprintf(stderr, "imgtool could not transform file '%s'\n", path);
            return;
        }
        img_file_write_any(path, buffer, width, height, format);
        free(buffer);
    } else img_file_write_any(path, img, width, height, format);
}

uint8_t* img_jcompress(const uint8_t* restrict img, const unsigned int width, const unsigned int height, unsigned int channels, unsigned int quality)
{
    uint8_t* buffer;
    if (channels != IMG_RGB) {
        buffer = img_transform_buffer(img, width, height, channels, IMG_RGB);
        if (!buffer) {
            fprintf(stderr, "imgtool could not transform image\n");
            return NULL;
        }
    } else buffer = (uint8_t*)(size_t)img;

    unsigned int size;
    uint8_t* compress = jpeg_compress(buffer, &size, width, height, quality);
    uint8_t* decompress = jpeg_decompress(compress, size);

    if (channels != IMG_RGB) free(buffer);
    free(compress);
    return decompress;
}

void img_set_jpeg_quality(const int quality)
{
    jpeg_quality = quality;
}

