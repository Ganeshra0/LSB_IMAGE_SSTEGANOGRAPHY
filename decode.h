#ifndef DECODE_H
#define DECODE_H
#include "types.h"

/*
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */
typedef struct _DecodeInfo
{
    //Stego image Info
	char *stego_image_fname;
	FILE *fptr_stego_image;
	uint extn_size;
	uint size_stego_file;

    //output_file Info
	char output_image_fname[20];
	FILE *output_image_fpointer;

}DecodeInfo;


/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status d_open_files(DecodeInfo *decInfo);

/* Store Magic String from the source image */
Status decode_magic_string(DecodeInfo *decInfo);

/* decode secret file extenstion size from the source image*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* decode secret file extn from the source image*/
Status decode_secret_file_extn(int size, DecodeInfo *decInfo);

/* decode secret file size from the source image*/
Status decode_secret_file_size(DecodeInfo *decInfo);

/* decode secret file data*/
Status decode_secret_file_data(int size,DecodeInfo *decInfo);

/* decode a byte into LSB of image data array */
char decode_byte_from_lsb(char data,char *buffer);

/* Copy remaining image bytes from src to stego image after encoding */
int decode_size_from_lsb(char *buffer);

#endif





