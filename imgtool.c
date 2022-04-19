#include <imgtool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUFF_SIZE 1024

typedef enum {
    IMG_COMMAND_NULL,
    IMG_COMMAND_BLACK_AND_WHITE,
    IMG_COMMAND_NEGATIVE,
    IMG_COMMAND_CUT,
    IMG_COMMAND_ROTATE,
    IMG_COMMAND_FLIP_HORIZONTAL,
    IMG_COMMAND_FLIP_VERTICAL,
    IMG_COMMAND_SCALE_UP,
    IMG_COMMAND_SCALE_DOWN,
    IMG_COMMAND_WHITE_TO_TRANSPARENT,
    IMG_COMMAND_WHITE_SENSIBILITY,
    IMG_COMMAND_JCOMPRESS,
    IMG_COMMAND_DUMP,
    IMG_COMMAND_FRAME_DUMP,
    IMG_COMMAND_RESIZE_WIDTH,
    IMG_COMMAND_RESIZE_HEIGHT,
    IMG_COMMAND_RESIZE_F
} imgtool_command_enum;

static unsigned int jcompress_quality = 100;
static unsigned int sensibility = 255;
static unsigned int resize_x, resize_y;
static float resize_scale;

#define bmp_swap(func, bmp)                 \
do {                                        \
    bmp_t b = func(bmp);                    \
    bmp_free(bmp);                          \
    memcpy(bmp, &b, sizeof(bmp_t));         \
} while (0)

#define bmp_swap_param(func, bmp, val)      \
do {                                        \
    bmp_t b = func(bmp, val);               \
    bmp_free(bmp);                          \
    memcpy(bmp, &b, sizeof(bmp_t));         \
} while (0)

static void imgtool_open_at_exit(int check, const char* path)
{
    if (!check) return;
    char open_str[BUFF_SIZE];
#ifdef __APPLE__
    strcpy(open_str, "open ");
#else
    strcpy(open_str, "xdg-open ");
#endif
    strcat(open_str, path);
    system(open_str);
}                           

static void imgtool_command(unsigned int command, bmp_t* bitmap)
{
    switch (command) {
        case IMG_COMMAND_BLACK_AND_WHITE: {
            bmp_swap(bmp_black_and_white, bitmap);
            break;
        }
        case IMG_COMMAND_NEGATIVE: {
            bmp_swap(bmp_negative, bitmap);
            break;
        }
        case IMG_COMMAND_FLIP_HORIZONTAL: {
            bmp_swap(bmp_flip_horizontal, bitmap);
            break;
        }
        case IMG_COMMAND_FLIP_VERTICAL: {
            bmp_swap(bmp_flip_vertical, bitmap);
            break;
        }
        case IMG_COMMAND_ROTATE: {
            bmp_swap(bmp_rotate, bitmap);
            break;
        }
        case IMG_COMMAND_SCALE_UP: {
            bmp_swap(bmp_scale, bitmap);
            break;
        }
        case IMG_COMMAND_SCALE_DOWN: {
            bmp_swap(bmp_reduce, bitmap);
            break;  
        }
        case IMG_COMMAND_WHITE_TO_TRANSPARENT: {
            bmp_swap(bmp_white_to_transparent, bitmap);
            break;
        }
        case IMG_COMMAND_WHITE_SENSIBILITY: {
            bmp_swap_param(bmp_clear_to_transparent, bitmap, (uint8_t)sensibility);
            break;
        }
        case IMG_COMMAND_CUT: {
            bmp_swap(bmp_cut, bitmap);
            break;
        }
        case IMG_COMMAND_JCOMPRESS: {
            bmp_swap_param(bmp_jcompress, bitmap, jcompress_quality);
            break;
        }
        case IMG_COMMAND_RESIZE_WIDTH: {
            bmp_swap_param(bmp_resize_width, bitmap, resize_x);
            break;
        }
        case IMG_COMMAND_RESIZE_HEIGHT: {
            bmp_swap_param(bmp_resize_height, bitmap, resize_y);
            break;
        }
        case IMG_COMMAND_RESIZE_F: {
            bmp_swap_param(bmp_scale_lerp, bitmap, resize_scale);
            break;
        }
    }
}

static char* imgtool_output_strnum(const char* output_path, unsigned int num)
{
    const unsigned int size = strlen(output_path);
    if (size < 5) return NULL;

    char* ret = (char*)malloc(BUFF_SIZE);
    memcpy(ret, output_path, size - 4);
    ret[size - 4] = '\0';

    char num_str[16];
    sprintf(num_str, "%03d", num);
    
    strcat(ret, num_str);
    strcat(ret, &output_path[size - 4]);
    return ret;
}

static long imgtool_file_size(const char* path)
{
    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "imgtool could not open file '%s'\n", path);
        return 0;
    }
    fseek(file, 0, SEEK_END);
    long ret = ftell(file);
    fclose(file);
    return ret;
}

static void imgtool_dump_data(unsigned char* img, unsigned int width, unsigned int height, unsigned int channels)
{
    fprintf(stdout, "--------------------------------------------------\n");
    fprintf(stdout, "Width: %u, Height: %u, Channels: %u\n", width, height, channels);
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            fprintf(stdout, "(%u", *(img + (width * y + x) * channels));
            for (unsigned int i = 1; i < channels; i++) {
                fprintf(stdout, " %u ", *(img + (width * y + x) * channels + i));
            }
            fprintf(stdout, ") ");
        }
        fprintf(stdout, "\n");
    }
}

static void imgtool_dump_file(bmp_t* bitmap, const char* path)
{
    if (!strlen(path)) return;
    long size = imgtool_file_size(path);
    fprintf(stdout, "--------------------------------------------------\n");
    fprintf(stdout, "File:\t\t'%s'\n", path);
    fprintf(stdout, "Size:\t\t%ldKb\t\t (%ld bytes)\n", size >> 10, size);
    fprintf(stdout, "Width:\t\t%u px\n", bitmap->width);
    fprintf(stdout, "Height:\t\t%u px\n", bitmap->height);
    fprintf(stdout, "Channels:\t%u\n", bitmap->channels);
}

static void imgtool_help()
{
    fprintf(stdout, "\n**** IMGTOOL: COMMAND LINE HANDY IMAGE TOOL ****\n\n");
    fprintf(stdout, "Enter any number of image files and commands to execute.\n");
    fprintf(stdout, "Each command or operation is applied to input images secuentially.\n");
    fprintf(stdout, "Supported formats are PNG, JPG, GIF and PPM.\n");
    fprintf(stdout, "Here is a simple use case example:\n\n");
    fprintf(stdout, "$ imgtool input.png -o output.jpg\n\n");
    fprintf(stdout, "This creates a copy of the input PNG in a JPG image format.\n");
    fprintf(stdout, "Another useful example is:\n\n");
    fprintf(stdout, "$ imgtool *.png -I -s -bw\n\n");
    fprintf(stdout, "In this case we take all PNG files in the current directory and apply\n");
    fprintf(stdout, "a scale-down and a grey scale effect to the original input files.\n");
    fprintf(stdout, "The following are the commands and flags supported.\n\n");

    fprintf(stdout, "-o:\t\tWrite the output image file to the path and format following this flag.\n");
    fprintf(stdout, "-I:\t\tWrite the output to the same file path and format as input.\n");
    fprintf(stdout, "-n\t\tNo output.\n");
    fprintf(stdout, "-d:\t\tDump main info about image file.\n");
    fprintf(stdout, "-D:\t\tDump image frame buffer as RGB/A values.\n");
    fprintf(stdout, "-j:\t\tCompress image using JPEG compression (lossy).\n");
    fprintf(stdout, "-bw:\t\tTransform to black and white.\n");
    fprintf(stdout, "-N:\t\tTransform to negative RGB values.\n");
    fprintf(stdout, "-cut:\t\tCut corners of the image when they are transparent.\n");
    fprintf(stdout, "-r:\t\tRotate by 90 degrees.\n");
    fprintf(stdout, "-S:\t\tScale up image by factor of two (nearest).\n");
    fprintf(stdout, "-s:\t\tScale down image by factor of two (linear).\n");
    fprintf(stdout, "-fh:\t\tFlip the image horizontally.\n");
    fprintf(stdout, "-fv\t\tFlip the image vertically.\n");
    fprintf(stdout, "-Rx:\t\tResize width of image to specified width.\n");
    fprintf(stdout, "-Ry:\t\tResize height of image to specified height.\n");
    fprintf(stdout, "-R:\t\tResize scale of image to specified floating point number.\n");
    fprintf(stdout, "-t\t\tSet white to transparent. Needs alpha channel present.\n");
    fprintf(stdout, "-T\t\tSet clear colors to transparent with a sensibility between 0 and 255.\n");
    fprintf(stdout, "-q:\t\tSet quality for JPEG compression output when writing to JPG.\n");
    fprintf(stdout, "-to-gif:\tWrite output images to a single output GIF file.\n");
    fprintf(stdout, "-from-gif:\tTake every frame of input GIF file as input images.\n");
    fprintf(stdout, "-open:\t\tOpen the first output image after process is completed.\n");
}

int main(int argc, char** argv)
{
    const int INPUT_SIZE = argc;
    char input_path[INPUT_SIZE][BUFF_SIZE], output_path[BUFF_SIZE];
    unsigned int commands[INPUT_SIZE], command_count = 0;
    unsigned int output_count = 0, input_count = 0, output_to_gif = 0, input_from_gif = 0;
    unsigned int output_to_input = 0, open_at_exit = 0, missing_output = 1;

    if (argc <= 1) {
        fprintf(stderr, "Missing arguments. Use -help to see instructions.\n");
        return EXIT_FAILURE;
    }

    /* parse arguments -> input files and commands */

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-help") || !strcmp(argv[i], "-h")) {
            imgtool_help();
            return EXIT_SUCCESS;
        }

        if (!strcmp(argv[i], "-version") || !strcmp(argv[i], "-v")) {
            fprintf(stdout, "imgtool version 0.1.0\n");
            return EXIT_SUCCESS;
        } 
        
        if (!strcmp(argv[i], "-o") && i + 1 < argc) {
            output_count++;
            missing_output = 0;
            strcpy(output_path, argv[++i]);
            command_count++;
        }
        else if (!strcmp(argv[i], "-q") && i + 1 < argc) {
            img_set_jpeg_quality(atoi(argv[++i]));
        }
        else if (!strcmp(argv[i], "-Rx") && i + 1 < argc) {
            commands[command_count++] = IMG_COMMAND_RESIZE_WIDTH;
            resize_x = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-Ry") && i + 1 < argc) {
            commands[command_count++] = IMG_COMMAND_RESIZE_HEIGHT;
            resize_y = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-R") && i + 1 < argc) {
            commands[command_count++] = IMG_COMMAND_RESIZE_F;
            resize_scale = atof(argv[++i]);
        }
        else if (!strcmp(argv[i], "-T") && i + 1 < argc) {
            commands[command_count++] = IMG_COMMAND_WHITE_SENSIBILITY;
            sensibility = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-n")) {
            missing_output = 0;
            output_count = 0;
            command_count++;
        }
        else if (!strcmp(argv[i], "-N")) {
            commands[command_count++] = IMG_COMMAND_NEGATIVE;
        }
        else if (!strcmp(argv[i], "-I")) {
            missing_output = 0;
            output_to_input = 1;
        }
        else if (!strcmp(argv[i], "-cut")) {
            commands[command_count++] = IMG_COMMAND_CUT;
        }
        else if (!strcmp(argv[i], "-d")) {
            missing_output = 0;
            commands[command_count++] = IMG_COMMAND_DUMP;
        } 
        else if (!strcmp(argv[i], "-D")) {
            missing_output = 0;
            commands[command_count++] = IMG_COMMAND_FRAME_DUMP;
        }
        else if (!strcmp(argv[i], "-bw")) {
            commands[command_count++] = IMG_COMMAND_BLACK_AND_WHITE;
        }
        else if (!strcmp(argv[i], "-t")) {
            commands[command_count++] = IMG_COMMAND_WHITE_TO_TRANSPARENT;
        }
        else if (!strcmp(argv[i], "-r")) {
            commands[command_count++] = IMG_COMMAND_ROTATE;
        }
        else if (!strcmp(argv[i], "-S")) {
            commands[command_count++] = IMG_COMMAND_SCALE_UP;
        }
        else if (!strcmp(argv[i], "-s")) {
            commands[command_count++] = IMG_COMMAND_SCALE_DOWN;
        }
        else if (!strcmp(argv[i], "-to-gif")) {
            output_to_gif = 1;
        }
        else if (!strcmp(argv[i], "-from-gif")) {
            input_from_gif = 1;
        }
        else if (!strcmp(argv[i], "-open")) {
            missing_output = 0;
            open_at_exit = 1;
        }
        else if (!strcmp(argv[i], "-j") && i + 1 < argc) {
            commands[command_count++] = IMG_COMMAND_JCOMPRESS;
            jcompress_quality = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-fh")) {
            commands[command_count++] = IMG_COMMAND_FLIP_HORIZONTAL;
        }
        else if (!strcmp(argv[i], "-fv")) {
            commands[command_count++] = IMG_COMMAND_FLIP_VERTICAL;
        }  else strcpy(input_path[input_count++], argv[i]);

        if (command_count == 255 || input_count == 255) break;
    }

    /* check if parsed arguments meet requirements */
    
    if (!input_count) {
        fprintf(stderr, "Missing input image file. See -help for more information.\n");
        return EXIT_FAILURE;
    }
    if (missing_output) {
        fprintf(stderr, "Missing output image file. See -help for more information.\n");
        return EXIT_FAILURE;
    }
    if (!command_count) {
        if (open_at_exit) {
            imgtool_open_at_exit(open_at_exit, input_path[0]);
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "Missing command to execute. See -help for more info.\n");
            return EXIT_FAILURE;
        }
    }

    /* load input image files */

    bmp_t* bitmaps;
    if (input_from_gif) {
        gif_t* g = gif_file_load(input_path[0]);
        if (g == NULL) return EXIT_FAILURE;
        bitmaps = gif_to_bmp(g, &input_count);
        gif_free(g);
    } else {
        bitmaps = (bmp_t*)malloc(input_count * sizeof(bmp_t));
        int miss = 0;
        for (unsigned int i = 0; i < input_count; i++) {
            if (input_count > 1) fprintf(stdout, "imgtool is loading images... ( %d / %d )\t'%s'\n", i + 1, input_count, input_path[i]);
            bitmaps[i - miss] = bmp_load(input_path[i]);
            if (bitmaps[i - miss].pixels == NULL) miss++;
        }
        input_count -= miss;
    }

    if (bitmaps[0].pixels == NULL) {
        fprintf(stderr, "imgtool could not load any image file\n");
        return EXIT_FAILURE;
    }

    /* apply commands & operations */

    for (unsigned int i = 0; i < input_count; i++) {
        for (unsigned int j = 0; j < command_count; j++) {
            if (commands[j] == IMG_COMMAND_DUMP) imgtool_dump_file(&bitmaps[i], input_path[i]);
            else if (commands[j] == IMG_COMMAND_FRAME_DUMP) imgtool_dump_data(bitmaps[i].pixels, bitmaps[i].width, bitmaps[i].height, bitmaps[i].channels);
            else imgtool_command(commands[j], &bitmaps[i]);
        }
    }

    /* write to output and open */

    if (output_to_gif) {
        gif_t* g = bmp_to_gif(bitmaps, input_count);
        gif_file_write(output_path, g);
        gif_free(g);
        imgtool_open_at_exit(open_at_exit, output_path);
    }
    else if (output_to_input) {
        for (unsigned int i = 0; i < input_count; i++) {
            bmp_write(input_path[i], &bitmaps[i]);
            bmp_free(&bitmaps[i]);
        }
        imgtool_open_at_exit(open_at_exit, input_path[0]);
    } 
    else if (output_count) {
        if (input_count > 1) {
            for (unsigned int i = 0; i < input_count; i++) {
                char* output_path_num = imgtool_output_strnum(output_path, i);
                bmp_write(output_path_num, &bitmaps[i]);
                bmp_free(&bitmaps[i]);
            }
            imgtool_open_at_exit(open_at_exit, imgtool_output_strnum(output_path, 0));
        } else {
            bmp_write(output_path, bitmaps);
            bmp_free(bitmaps);
            imgtool_open_at_exit(open_at_exit, output_path);
        }
    } 
    
    free(bitmaps);
    return EXIT_SUCCESS;
}
