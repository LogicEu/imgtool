#include <imgtool.h>
#include <stdlib.h>
#include <stdio.h>

/************************
 -> PPM save and load  <- 
************************/

uint8_t* ppm_file_load(const char* restrict path, unsigned int* width, unsigned int* height)
{
    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "imgtool could not open PPM file '%s'\n", path);
        return NULL;
    }

    char header[256];
    fgets(header, 256, file);
    sscanf(header, "P6 %u %u 255", width, height);

    uint8_t* ret = (uint8_t*)malloc(*width * *height * 3);
    fread(ret, 3, *width * *height, file);
    fclose(file);
    
    return ret;
}

void ppm_file_write(const char* restrict path, const uint8_t* restrict img, const unsigned int width, const unsigned int height)
{
    FILE* file = fopen(path, "wb");
    if (!file) {
        fprintf(stderr, "imgtool could not write PPM file '%s'\n", path);
        return;
    }

    char buff[256];
    int rc = sprintf(buff, "P6 %u %u 255\n", width, height);
    fwrite(buff, rc, 1, file);
    fwrite(img, 3, width * height, file);
    
    fclose(file);
}

