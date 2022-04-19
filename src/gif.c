#include <imgtool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/************************
 -> GIF save and load  <- 
*************************/

#include "gif/gifenc.h"
#include "gif/gifdec.h"

#define absi(i) ((i) * ((i) >= 0) - (i) * ((i) < 0))
#define px3_at(buff, width, x, y) (buff + (((y) * (width)) + (x)) * 3)

extern uint8_t vga[0x30];

static uint8_t rgb_palette_256(const uint8_t* restrict rgb)
{
    int dif[256], i = 0;
    for (int j = 0; j < 16; j++) {
        dif[i] = absi((int)rgb[0] - (int)vga[i * 3]);
        dif[i] += absi((int)rgb[1] - (int)vga[i * 3 + 1]);
        dif[i] += absi((int)rgb[2] - (int)vga[i * 3 + 2]);
        i++;
    }

    for (int r = 0; r < 6; r++) {
        for (int g = 0; g < 6; g++) {
            for (int b = 0; b < 6; b++) {
                dif[i] = absi((int)rgb[0] - r * 51);
                dif[i] += absi((int)rgb[1] - g * 51);
                dif[i] += absi((int)rgb[2] - b * 51);
                i++;
            }
        }
    }

    for (int j = 1; j <= 24; j++) {
        int v = j * 0xFF / 25;
        dif[i] = absi((int)rgb[0] - v);
        dif[i] += absi((int)rgb[1] - v);
        dif[i] += absi((int)rgb[2] - v);
        i++;
    }

    int dif_mark = 100000;
    uint8_t mark = 0;
    for (int j = 0; j < 256; j++) {
        if (dif[j] < dif_mark) {
            dif_mark = dif[j];
            mark = (uint8_t)j;
        }
    }
    return mark;
}

static gif_t* gif_new(const unsigned int width, const unsigned int height, const uint8_t* restrict background)
{
    gif_t* gif = malloc(sizeof(gif_t));
    gif->width = width;
    gif->height = height;
    memcpy(gif->background, background, 3);
    
    gif->used = 0;
    gif->size = 1;
    gif->frames = malloc(gif->size * sizeof(uint8_t*));
    return gif;
}

static void gif_push_frame(gif_t* gif, uint8_t* frame)
{
    if (gif->used >= gif->size) {
        gif->size *= 2;
        gif->frames = realloc(gif->frames, gif->size * sizeof(uint8_t*));
    }
    gif->frames[gif->used++] = frame;
}

void gif_free(gif_t* gif)
{
    for (unsigned int i = 0; i < gif->used; i++) {
        free(gif->frames[i]);
    }
    free(gif->frames);
}

gif_t* gif_file_load(const char* restrict path)
{
    gd_GIF* gif = gd_open_gif(path);
    if (!gif) {
        fprintf(stderr, "imgtool could not open GIF file '%s'\n", path);
        return NULL;
    }

    gif_t* ret = gif_new(gif->width, gif->height, (uint8_t*)&gif->gct.colors[gif->bgindex * 3]);

    int rc;
    while ((rc = gd_get_frame(gif)) != 0) {
        if (rc == -1) break;
        uint8_t* frame = (uint8_t*)malloc(ret->width * ret->height * 3);
        gd_render_frame(gif, frame);
        gif_push_frame(ret, frame);
    }
    gd_close_gif(gif);
    return ret;
}

void gif_file_write(const char* restrict path, const gif_t* restrict input)
{
    ge_GIF *gif = ge_new_gif(path, input->width, input->height, NULL, 8, 0);

    for (unsigned int i = 0; i < input->used; i++) {
        for (unsigned int y = 0; y < input->height; y++) {
            for (unsigned int x = 0; x < input->width; x++) {
                gif->frame[y * input->width + x] = rgb_palette_256(px3_at(input->frames[i], input->width, x, y));
            }
        }
        ge_add_frame(gif, 10);
    }
    ge_close_gif(gif);
}

void gif_file_write_frame(const char* restrict path, const uint8_t* restrict img, const unsigned int width, const unsigned int height)
{
    ge_GIF *gif = ge_new_gif(path, width, height, NULL, 8, 0);

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            gif->frame[y * width + x] = rgb_palette_256(px3_at(img, width, x, y));
        }
    }

    ge_add_frame(gif, 10);
    ge_close_gif(gif);
}

uint8_t* gif_file_load_frame(const char* restrict path, unsigned int* width, unsigned int* height)
{
    gd_GIF* gif = gd_open_gif(path);
    if (!gif) {
        fprintf(stderr, "imgtool could not open GIF file '%s'\n", path);
        return NULL;
    }

    if (gd_get_frame(gif) == -1) {
        fprintf(stderr, "imgtool had a problem loading GIF file '%s'\n", path);
        return NULL;
    }
    
    *width = gif->width;
    *height = gif->height;
    uint8_t* frame = (uint8_t*)malloc(gif->width * gif->height * 3);
    
    gd_render_frame(gif, frame);
    gd_close_gif(gif);

    return frame;
}

bmp_t* gif_to_bmp(const gif_t* restrict gif, unsigned int* count)
{
    const unsigned int size = gif->used;
    bmp_t* ret = malloc(size * sizeof(bmp_t));
    const unsigned int width = gif->width, height = gif->height, channels = 3;
    for (unsigned int i = 0; i < size; i++) {
        ret[i].width = width;
        ret[i].height = height;
        ret[i].channels = channels;
        ret[i].pixels = (uint8_t*)malloc(width * height * channels);
        memcpy(ret[i].pixels, gif->frames[i], width * height * channels);
    }
    *count = size;
    return ret;
}

gif_t* bmp_to_gif(const bmp_t* restrict bitmaps, const unsigned int count)
{
    static const uint8_t white[3] = {255};
    const unsigned int channels = bitmaps->channels;

    gif_t* gif = gif_new(bitmaps->width, bitmaps->height, &white[0]);
    for (unsigned int i = 0; i < count; i++) {
        if (channels == 4) {
            bmp_t b = bmp_transform(&bitmaps[i], 3);
            gif_push_frame(gif, b.pixels);
            bmp_free(&b);
        } else gif_push_frame(gif, bitmaps[i].pixels);
    }
    return gif;
}
