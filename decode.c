#include <stdio.h>
#include "types.h"
#include "common.h"
#include "decode.h"
#include <string.h>
#include <stdlib.h>

//Function to read and validate decode args
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{

	//To check if the stego image file has a .bmp extension
	if(strcmp(strstr(argv[2],"."),".bmp") == 0)
	{

		decInfo->stego_image_fname=argv[2];

	}
	else
	{

		printf("INFO:ERROR ,Enter correct file name");

		return e_failure;

	}

	//To check if an output image file name is provided, if not use "output"
	if(argv[3] == NULL)
	{
	
		strcpy(decInfo->output_image_fname,"output");
	
	}
	else
	{
	
		strcpy(decInfo->output_image_fname,argv[3]);
	
	}
	
	return e_success;

}

//Function to open files for decoding
Status d_open_files(DecodeInfo *decInfo)
{

	//Stego Image file
	//To open the stego image file for reading
	decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname,"r");

	//Do Error handling
	if (decInfo->fptr_stego_image == NULL)
	{
	
		perror("fopen");
	
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

		return e_failure;
	
	}
	
	return e_success;

}

//Function to decode magic string
Status decode_magic_string(DecodeInfo *decInfo)
{

	char my_magic_string[3]={0};

	char buffer[8];

	//Seek  the location to 54 byte
	fseek(decInfo->fptr_stego_image,54,SEEK_SET);

	for(int i = 0;i < 2 ; i++)
	{

		//To read and decode each byte of the magic string
		fread(buffer, 1, 8, decInfo -> fptr_stego_image);

		my_magic_string[i]=decode_byte_from_lsb(my_magic_string[i],buffer);

	}

	my_magic_string[3]='\0';

	//To compare decoded magic string with original magic string
	if(strcmp(my_magic_string,MAGIC_STRING)==0)
	{

		return e_success;

	}
	else
	{

		return e_failure;

	}

}

//Function to decode byte from lsb
char decode_byte_from_lsb(char data,char *image_buffer)
{
  
  	//To decode a byte from the least significant bits of an image buffer
	for (int i = 0; i < 8; i++)
	{

		if (image_buffer[i] & 1)
		{

			data = data | (1 << (7 - i));

		}
		else
		{

			data = data & ~(1 << (7 - i));

		}

	}

	return data;

}

//Function to decode secret file extn
Status decode_secret_file_extn(int size,DecodeInfo *decInfo)
{

	char store_file_extn[size+1];

	char buffer[8];

	for(int i = 0;i < size;i++)
	{

		store_file_extn[i]=0;

		fread(buffer, 1, 8, decInfo -> fptr_stego_image);

		store_file_extn[i]=decode_byte_from_lsb(store_file_extn[i],buffer);

	}

	store_file_extn[size]='\0';

	//Append the decoded file extension to the output image file name
	strcat(decInfo->output_image_fname,store_file_extn);

	//Open the output image file for writing
	decInfo->output_image_fpointer=fopen(decInfo->output_image_fname,"w");

	if(decInfo->output_image_fpointer == NULL)
	{

		perror("fopen");

		fprintf(stderr, "ERROR : Unable to open file %s\n", decInfo -> output_image_fname);

		return e_failure;

	}
	else
	{

		printf("INFO:Output Image file is Created successfully\n");

	}

	return e_success;

}

//Function to decode secret file extn size
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{

	char buffer[32];

	int length;

	fread(buffer,32,1,decInfo->fptr_stego_image);

	length=decode_size_from_lsb(buffer);

	decInfo->extn_size=length;

	//To check if the decoded extension size matches known extensions
	if(length == strlen(".txt"))
	{

		return e_success;

	}
	else if(length == strlen(".sh"))
	{

		return e_success;

	}
	else if(length == strlen(".c"))
	{

		return e_success;

	}

}

//Function to decode size from lsb
int decode_size_from_lsb(char *buffer)
{

	int size=0;
	
	//To decode an integer from the least significant bits of a buffer
	for (int i = 0; i < 32; i++)
	{

		if (buffer[i] & 1)
		{

			size = size | (1 << (31 - i));

		}
		else
		{

			size = size & ~(1 << (31 - i));

		}

	}

	return size;

}

//Function to decode secret file  size
Status decode_secret_file_size(DecodeInfo *decInfo)
{

	char buffer[32];

	//To decode the secret file size from the stego image
	fread(buffer,1,32,decInfo->fptr_stego_image);

	decInfo->size_stego_file=decode_size_from_lsb(buffer);

	return e_success;

}

//Function to decode secret file data
Status decode_secret_file_data(int size,DecodeInfo *decInfo)
{

	char buffer[8];

	char secret_file_data[size];

	for(int i =0 ;i < size ;i++)
	{

		secret_file_data[i]=0;

		fread(buffer,8,1,decInfo->fptr_stego_image);

		secret_file_data[i]=decode_byte_from_lsb(secret_file_data[i],buffer);

	}

	//To write the decoded secret file data to the output image fil
	fwrite(secret_file_data,size,1,decInfo->output_image_fpointer);

	return e_success;

}

//Function to do decoding
Status do_decoding(DecodeInfo *decInfo)
{

	//To open the file for decoding
	if(d_open_files(decInfo) == e_success )
	{

		printf("INFO:Open files is a successfully\n");

	}
	else
	{

		printf("INFO:ERROR ,Not open files successfully\n");

		return e_failure;

	}

	//To decode the magic string
	if(decode_magic_string(decInfo) == e_success)
	{

		printf("INFO:Decoded magic string Successfully\n");

	}
	else
	{

		printf("INFO:ERROR ,Not decoded magic string successfully\n");

		return e_failure;

	}

	//To decode the file extension size
	if(decode_secret_file_extn_size(decInfo) == e_success)
	{

		printf("INFO:Decoded file extension size Successfully\n");

	}
	else
	{

		printf("INFO:ERROR ,Not decoded file extension size Succesfully\n");

		return e_failure;

	}

	//To decode secret the file extension
	if(decode_secret_file_extn(decInfo->extn_size,decInfo)==e_success)
	{

		printf("INFO:Decoded file Extension successfully\n");

	}
	else
	{

		printf("INFO:ERROR ,Not decoded file extension successfully\n");

	}

	//To decode the secret file size
	if(decode_secret_file_size(decInfo)==e_success)
	{

		printf("INFO:Secret file size Decoded successfully\n");

	}
	else
	{

		printf("INFO:ERROR ,Not decoded Secret file size successfully\n");

	}
	
	//To decode the secret file data
    if(decode_secret_file_data(decInfo->size_stego_file,decInfo)==e_success)
	{
	
		printf("INFO:Secret file data decoded successfully\n");
	
	}
	else
	{
	
		printf("INFO:ERROR ,Not decoded Secret data successfuly\n");
	
		return e_failure;
	
	}

	return e_success;

}

