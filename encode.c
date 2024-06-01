#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "common.h"
#include "types.h"

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status read_and_validate_encode_args(char *argv[],EncodeInfo *encinfo)
{
//check src file is .bmp or not
	if(strcmp(strstr(argv[2],"."),".bmp") == 0)
	{
		/* Storing source file name into the structure member*/
		encinfo->src_image_fname = argv[2];
	}
	else
	{
		printf("INFO:ERROR,Please enter the .bmp file\n");
		return e_failure;
	}
	//check secret file
	if(strcmp(strstr(argv[3],"."),".txt") == 0)
	{
		/*Storing secret file name into structure member*/
		encinfo->secret_fname = argv[3];

		/*Storing secret file extensions*/
		strcpy(encinfo->extn_secret_file,".txt");
	}
	else if(strcmp(strstr(argv[3],"."),".c") == 0)
	{
		/*Storing secret file name into structure member*/
		encinfo->secret_fname = argv[3];

		/*Storing secret file extensions*/
		strcpy(encinfo->extn_secret_file,".c");

	}
	else if(strcmp(strstr(argv[3],"."),".sh") == 0)
	{
		/*Storing secret file name into structure member*/
		encinfo->secret_fname = argv[3];

		/*Storing secret file extensions*/
		strcpy(encinfo->extn_secret_file,".sh");

	}
	else
	{
		return e_failure;
	}
	/*checking stego image(output) file*/
	if(argv[4] == NULL)
	{
		//If file is not present creating own file
		encinfo->stego_image_fname="stego.bmp";
	}
	else
	{
		if(strcmp(strstr(argv[4],"."),".bmp") == 0)
		{
			//Storing stego file name into structure member
			encinfo->stego_image_fname=argv[4];
		}
	}
	return e_success;
}

/*Get the file size*/
uint get_file_size(FILE *fptr)
{
	fseek(fptr,0,SEEK_END);
	uint s_ize = ftell(fptr);
	fseek(fptr,0,SEEK_SET);
	return s_ize;

}

/*Calculate image capacity and check if it has sufficient space for encoding*/
Status check_capacity(EncodeInfo *encinfo)
{
	//To find the size of .bmp file
	encinfo->image_capacity=get_image_size_for_bmp(encinfo->fptr_src_image);

	//To find the size of the secret file
	encinfo->size_secret_file=get_file_size(encinfo->fptr_secret);

	//To check capacity
	if(encinfo->image_capacity-54 >= ((strlen(MAGIC_STRING)+4+strlen(encinfo->extn_secret_file)+sizeof(encinfo->size_secret_file)+encinfo->size_secret_file)*8))
	{
 		return e_success;
	}
	else
	{
		return e_failure;
	}
}
/*Copy 54 bytes header to destination file*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{

	char str[54];

	fseek(fptr_src_image,0,SEEK_SET);

	fread(str,54,1,fptr_src_image);

	fwrite(str,54,1,fptr_dest_image);

	return e_success;

}
/*Function to encode magic string*/
Status encode_magic_string(char *magic_string,EncodeInfo *encinfo)
{

   	encode_data_to_image(magic_string, 2, encinfo->fptr_src_image, encinfo->fptr_stego_image,encinfo);

   	return e_success;

}

/*Function to encode data to image*/
Status encode_data_to_image(char *data,int size,FILE *fptr_src_image,FILE *fptr_stego_image,EncodeInfo *encinfo)
{

	for(int i = 0; i < size ; i++)
	{

		//Read 8 bytes
		fread(encinfo->image_data,8,1,fptr_src_image);

		encode_byte_to_lsb(data[i],encinfo->image_data);

		fwrite(encinfo->image_data,8,1,fptr_stego_image);

	}

	return e_success;
}

/* Function to encode 1byte into lsb of 8bytes*/
Status encode_byte_to_lsb(char data,char *image_buffer)
{

	for(int i = 0 ; i < 8 ; i++)
	{

		if(data & (1 << (7 - i)))
        {

		  	image_buffer[i] = image_buffer[i] | (1 << 0);

		}
        else
        {

			image_buffer[i] = image_buffer[i] & ( ~(1 << 0));

		}
    }
}

/*Function to encode secret file extension*/
Status encode_secret_file_extn(char *file_extn,EncodeInfo *encinfo)
{

	encode_data_to_image(file_extn, strlen(file_extn) ,encinfo->fptr_src_image ,encinfo->fptr_stego_image ,encinfo);

	return e_success;

}

/*Function to encode secret file extension size*/
Status encode_secret_file_extn_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{

 	char str[32];

 	fread (str, 32, 1, fptr_src_image);

 	encode_size_to_lsb(size, str);

 	fwrite(str, 32, 1, fptr_stego_image);

 	return e_success;
}


/* Function to encode size into lsb of 8bytes*/
Status encode_size_to_lsb(int size, char *image_buffer)
{

	for(int i = 0 ;i < 32 ;i++)
     {

		 if( size & (1 << (31 - i)))
        {

			image_buffer[i] = image_buffer[i] | (1 << 0);

		}
        else
        {

			image_buffer[i] = image_buffer[i] & (~(1 << 0));

		}
    }

}

/*Function to encode secret file size*/
Status encode_secret_file_size(int size ,EncodeInfo *encinfo)
{

	char str[32];

	fread (str, 32, 1, encinfo->fptr_src_image);

	encode_size_to_lsb(size, str);

	fwrite(str, 32, 1, encinfo->fptr_stego_image);

	return e_success;

}

/*Function tncode secret file data into the destination image*/
Status encode_secret_file_data(EncodeInfo *encinfo)
{

	fseek(encinfo->fptr_secret,0,SEEK_SET);

	//Declering array of size of secreat file
	char str[encinfo->size_secret_file];

	fread(str,encinfo->size_secret_file,1,encinfo->fptr_secret);

	encode_data_to_image(str,strlen(str),encinfo->fptr_src_image,encinfo->fptr_stego_image,encinfo);

	return e_success;

}

/*Copy remaing data into destination image*/
Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{

	long current_position,end_position;

	//To find the current position
	current_position=ftell(fptr_src);

	//To set the offset at the end
	fseek(fptr_src,0,SEEK_END);

	//To find the end position
	end_position=ftell(fptr_src);

	//Bring the pointer to the previous location
	fseek(fptr_src,current_position,SEEK_SET);

	//declearing array
	char str[ end_position - current_position ];

	fread( str , end_position -current_position , 1 , fptr_src );

	fwrite( str , end_position - current_position, 1 , fptr_dest );

	return e_success;

}

/*To perform encoding*/
Status do_encoding(EncodeInfo *encinfo)
{

	printf("INFO:Encoding started\n");

	//To open input and output files
	if(open_files(encinfo) == e_success)
	{

		printf("INFO:Open files success\n");

	}
	else
	{

		printf("INFO:ERROR ,Open files failure\n");

		return e_failure;

	}

	//To check image capacity
	if(check_capacity(encinfo) == e_success)
	{

		printf("INFO:Check capacity success\n");

	}
	else
	{

		printf("INFO:ERROR ,Check capacity failure\n");

		return e_failure;

	}

	//To copy BMP header
	if(copy_bmp_header(encinfo->fptr_src_image,encinfo->fptr_stego_image) == e_success)
	{

		printf("INFO:Header copied successfully\n");

	}
	else
	{

		printf("INFO:ERROR ,Header not copied successfully\n");

		return e_failure;

	}

	 //To encode magic string
	if(encode_magic_string(MAGIC_STRING,encinfo) == e_success)
	{

		printf("INFO:Encoded magic string successfully\n");

	}
	else
	{

		printf("INFO:ERROR ,magic string not encoded successfully\n");

		return e_failure;

	}

	//TO encode secret file extension size
	if(encode_secret_file_extn_size(strlen(encinfo->extn_secret_file),encinfo->fptr_src_image,encinfo->fptr_stego_image) == e_success)
			{

		printf("INFO:Encoded secret file extn size successfully\n");

	}
	else
	{

		printf("INFO:ERROR ,Failed to encode secret file extn\n");

		return e_failure;

	}

	//To encode secret file extension
	if(encode_secret_file_extn(encinfo->extn_secret_file, encinfo) == e_success)
	{

		printf("INFO:Encoded secret file extension successfully\n");

	}
	else
	{

		printf("INFO:ERROR secret file extension not encoded successfully\n");

		return e_failure;

	}

	//To encode secret file size
	if(encode_secret_file_size(encinfo->size_secret_file,encinfo) == e_success)
	{

		printf("INFO:Encoded secret file size successfully\n");

	}
	else
	{

		printf("INFO:ERROR ,Failed to encode secret file size\n");

		return e_failure;

	}

	//to encode secret file data
	if(encode_secret_file_data(encinfo) == e_success)
	{

		printf("INFO:Encoded secret file data successfully\n");

	}
	else
	{

		printf("INFO:ERROR ,Failed to encode secret file data");

		return e_failure;

	}

	//To copy remaining image data
	if(copy_remaining_img_data(encinfo->fptr_src_image,encinfo->fptr_stego_image) == e_success)
	{

		printf("INFO:Copied remaining data successfully\n");

	}
	else
	{

		printf("INFO:ERROR ,Failed to copy remaining data successfully\n");

		return e_failure;
	}

	return e_success;

}

