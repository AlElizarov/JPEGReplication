#include <iostream>
#include "JPEGReplication.h"

Image image;

int main(int argc, char **argv)
{
    std::cout << "Program for working with JPEG images" << std::endl << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    if (argc != 3)
    {
        std::cout << "Incorrect number of argumnets!" << std::endl;
        std::cin.get();
        return 0;
    }
    
    if (read_JPEG_file(argv[1]))
    {
        std::cout << "Error reading " << argv[0] << " file!" << std::endl;
        std::cin.get();
        return 0;
    }
    if (write_JPEG_file(argv[2], 70))
    {
        std::cout << "Error writing " << argv[1] << " file!" << std::endl;
        std::cin.get();
        return 0;
    }

    std::cout << "Program completed successfully! Image was replecated from " << argv[1] << " to " << argv[2] <<std::endl;
    std::cin.get();
    return 0;
}

int write_JPEG_file(char * filename, int quality)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    FILE * outfile;
    if ((outfile = fopen(filename, "wb")) == NULL) 
        return 1;
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = image.image_width;
    cinfo.image_height = image.image_height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);

    jpeg_set_quality(&cinfo, quality, TRUE);

    jpeg_start_compress(&cinfo, TRUE);
    int row_stride = image.image_width * 3;

    // encode
    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &image.image_buffer[cinfo.next_scanline * row_stride];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // write
    jpeg_finish_compress(&cinfo);
    // release resources
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
    return 0;
}

int read_JPEG_file(char * filename)
{
    // read
    FILE * infile;
    if ((infile = fopen(filename, "rb")) == NULL) 
        return 1;

    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return 1;
    }
    jpeg_create_decompress(&cinfo);

    // fill jpeg_decompress_struct
    jpeg_stdio_src(&cinfo, infile); 
    (void)jpeg_read_header(&cinfo, TRUE);
    (void)jpeg_start_decompress(&cinfo);

    // physical row width in output buffer 
    int row_stride = cinfo.output_width * cinfo.output_components; 

    // allocate output buffer
    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

    // allocate input buffer
    image.image_buffer = new unsigned char[cinfo.output_width * cinfo.output_height * cinfo.output_components];

    // decode
    int counter = 0;
    while (cinfo.output_scanline < cinfo.output_height) {
        (void)jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(image.image_buffer + counter, buffer[0], row_stride);
        counter += row_stride;
    }

    image.image_height = cinfo.output_height;
    image.image_width = cinfo.output_width;

    // release resources
    (void)jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return 0;
}

void my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}
