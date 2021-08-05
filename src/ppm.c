#include <imgtool.h>
#include <stdio.h>

/*
--------------------------
 -> Basic PPM File I/O  <- 
--------------------------
*/

unsigned char* ppm_file_load(const char* path, unsigned int* width, unsigned int* height)
{
    FILE* file = fopen(path, "rb");
    if (!file) {
        printf("Could not open file '%s'\n", path);
        return NULL;
    }

    char header[256];
    fgets(header, 256, file);
    sscanf(header, "P6 %d %d 255", width, height);

    unsigned char* ret = (unsigned char*)malloc(*width * *height * 3);
    fread(ret, 3, *width * *height, file);
    fclose(file);
    return ret;
}

void ppm_file_write(const char* path, unsigned char* img, unsigned int width, unsigned int height)
{
    FILE* file = fopen(path, "wb");
    if (!file) {
        printf("Could not write file '%s'\n", path);
        return;
    }

    char buff[256];
    int rc = sprintf(buff, "P6 %d %d 255\n", width, height);
    fwrite(buff, rc, 1, file);
    fwrite(img, 3, width * height, file);
    fclose(file);
    printf("Succesfully writed PPM file '%s'\n", path);
}