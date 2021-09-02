#include <imgtool.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>

/***********************
 -> PNG save and load <- 
***********************/

uint8_t* png_file_load(const char* path, unsigned int* width, unsigned int* height)
{
    FILE *file = fopen(path, "rb");
    if (!file) {
        printf("imgtool could not open PNG file '%s'\n", path);
        return NULL;
    }
    
    png_byte bit_depth;
    png_byte color_type;
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        printf("imgtool had a problem trying to read PNG file '%s'\n", path);
        return NULL;
    }
    png_infop info = png_create_info_struct(png);
    if (!info || setjmp(png_jmpbuf(png))) {
        printf("imgtool detected a problem reading the PNG file '%s'\n", path);
        return NULL;
    }

    png_init_io(png, file);
    png_read_info(png, info);
    const unsigned int w = png_get_image_width(png, info);
    const unsigned int h = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16) png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY ||
            color_type == PNG_COLOR_TYPE_PALETTE) png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png);
    png_read_update_info(png, info);

    png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * h);
    for (unsigned int y = 0; y < h; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
    }
    png_read_image(png, row_pointers);

    uint8_t* data = (uint8_t*)malloc(h * w * 4);
    unsigned int i = 0;
    for (unsigned int y = 0; y < h; y++) {
        png_bytep row = row_pointers[y];
        for (unsigned int x = 0; x < w; x++) {
            png_bytep px = &row[x * 4];
            memcpy(&data[i], &px[0], 4);
            i += 4;
        }
    }

    for (unsigned int y = 0; y < h; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);
    fclose(file);
    *width = w;
    *height = h;
    return data;
}

void png_file_write(const char* path, uint8_t* data, unsigned int width, unsigned int height) 
{
    FILE* file = fopen(path, "wb");
    if (!file) {
        printf("imgtool could not write PNG file '%s'\n", path);
        return;
    }
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        printf("imgtool had a problem writing PNG file '%s'\n", path);
        return;
    }
    png_infop info = png_create_info_struct(png);
    if (!info || setjmp(png_jmpbuf(png))) {
        printf("imgtool detected a problem writing PNG file '%s'\n", path);
        return;
    }

    png_init_io(png, file);
    png_set_IHDR(
        png, 
        info, 
        width, 
        height, 
        8,
        PNG_COLOR_TYPE_RGBA, 
        PNG_INTERLACE_NONE, 
        PNG_COMPRESSION_TYPE_DEFAULT, 
        PNG_FILTER_TYPE_DEFAULT
    );

    png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for (unsigned int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
    }

    unsigned int i = 0;
    for (unsigned int y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for (unsigned int x = 0; x < width; x++) {
            png_bytep px = &(row[x * 4]);
            px[0] = data[i + 0];
            px[1] = data[i + 1];
            px[2] = data[i + 2];
            px[3] = data[i + 3];
            i += 4;
        }
    }

    png_write_info(png, info);
    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    for (unsigned int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);
    fclose(file);
    printf("succesfully writed PNG file '%s'\n", path);
}