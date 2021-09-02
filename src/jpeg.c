#include <imgtool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <jpeglib.h>

/************************
 -> JPEG save and load <- 
************************/

void jpeg_file_write(const char* path, uint8_t* data, unsigned int width, unsigned int height, int quality) 
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    JSAMPROW row_pointer[1];

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    FILE* file = fopen(path, "wb");
    if (!file) {
        printf("imgtool could not write JPEG file '%s'\n", path);
        return;
    }

    jpeg_stdio_dest(&cinfo, file);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;	
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    int row_stride = width * 3;
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &data[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);

    fclose(file);
    jpeg_destroy_compress(&cinfo);
    printf("succesfully writed JPG file '%s'\n", path);
}

uint8_t* jpeg_file_load(const char* path, unsigned int* w, unsigned int* h)
{
	struct stat file_info;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	unsigned long bmp_size;
	uint8_t *bmp_buffer;
	int row_stride, width, height, pixel_size;

    int rc = stat(path, &file_info);
    if (rc) {
        printf("imgtool could not get info about JPEG file '%s'\n", path);
        return NULL;
    }

	unsigned long jpg_size = file_info.st_size;
	uint8_t* jpg_buffer = (uint8_t*)malloc(jpg_size + 100);

	FILE* file = fopen(path, "rb");
    if (!file) {
        printf("imgtool could not open JPEG file '%s'\n", path);
        return NULL;
    } 

	fread(jpg_buffer, jpg_size, 1, file);
	fclose(file);  

	cinfo.err = jpeg_std_error(&jerr);	
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);
	rc = jpeg_read_header(&cinfo, TRUE);

	if (rc != 1) {
		printf("file '%s' does not seem to be a normal JPEG.\n", path);
		return NULL;
	}

	jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
	height = cinfo.output_height;
	pixel_size = cinfo.output_components;

	bmp_size = width * height * pixel_size;
	bmp_buffer = (uint8_t*)malloc(bmp_size);
	row_stride = width * pixel_size;

	while (cinfo.output_scanline < cinfo.output_height) {
		uint8_t *buffer_array[1];
		buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(jpg_buffer);

    *w = width;
    *h = height;
    return bmp_buffer;
}

uint8_t* jpeg_compress(uint8_t* data, unsigned int* size, unsigned int width, unsigned height, int quality)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    JSAMPROW row_pointer[1];

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    unsigned long s;
    uint8_t* ret;
    jpeg_mem_dest(&cinfo, &ret, &s);
    *size = (unsigned int)s;

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;	
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    int row_stride = width * 3;
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &data[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    return ret;   
}

uint8_t* jpeg_decompress(uint8_t* data, unsigned int size)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	unsigned long bmp_size;
	uint8_t *bmp_buffer;
	int row_stride, width, height, pixel_size;

    cinfo.err = jpeg_std_error(&jerr);	
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, data, size);
	int rc = jpeg_read_header(&cinfo, TRUE);

	if (rc != 1) {
		printf("image data buffer does not seem to be a normal JPEG\n");
		return NULL;
	}

    jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
	height = cinfo.output_height;
	pixel_size = cinfo.output_components;

	bmp_size = width * height * pixel_size;
	bmp_buffer = (uint8_t*)malloc(bmp_size);
	row_stride = width * pixel_size;

	while (cinfo.output_scanline < cinfo.output_height) {
		uint8_t *buffer_array[1];
		buffer_array[0] = bmp_buffer + (cinfo.output_scanline) * row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
    return bmp_buffer;
}
