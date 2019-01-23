#ifndef JPEG_H
#define JPEG_H

extern "C" {
#include <jpeglib.h>
}
#include <setjmp.h>

struct Image
{
    JSAMPLE * image_buffer; /* Points to large array of R,G,B-order data */
    int image_height; /* Number of rows in image */
    int image_width; /* Number of columns in image */
};

struct my_error_mgr {
    struct jpeg_error_mgr pub;

    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

int read_JPEG_file(char * filename);
void my_error_exit(j_common_ptr cinfo);
int write_JPEG_file(char * filename, int quality);

#endif
