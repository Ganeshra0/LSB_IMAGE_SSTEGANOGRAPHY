/*
Name            :E.Ganesh
Date            :15-02-2023
Description     :LSB image steganography
Sample execution:
For encoding:
./a.out -e beautiful.bmp secret.txt stego.bmp
<---------INFO:Selected encoding--------->
detected encoding
U can do this
INFO:Encoding started
INFO:Open files success
width = 1024
height = 768
INFO:Check capacity success
INFO:Header copied successfully
INFO:Encoded magic string successfully
INFO:Encoded secret file extn size successfully
INFO:Encoded secret file extension successfully
INFO:Encoded secret file size successfully
INFO:Encoded secret file data successfully
INFO:Copied remaining data successfully
<---------INFO:Encoding success--------->
For decoding:
./a.out -d stego.bmp output.txt
<---------INFO:Selected decoding--------->
INFO:Read and validate success
INFO:Open files is a successfully
INFO:Decoded magic string Successfully
INFO:Decoded file extension size Successfully
INFO:Output Image file is Created successfully
INFO:Decoded file Extension successfully
INFO:Secret file size Decoded successfully
INFO:Secret file data decoded successfully
<---------INFO:decoding success--------->
*/
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
	DecodeInfo decInfo;
	int y;
	y=check_operation_type(argv);
	if(y == e_unsupported)
		printf("Invalid with the usage\n");
	if(y == e_encode)
	{
		printf("detected encoding\n");
		if(argc >= 4 || argc <=5)
		{
			printf("U can do this");
			read_and_validate_encode_args(argv,&encInfo);
			do_encoding(&encInfo);
		}
		else
		{
			printf("Check no. of files passed");
		}
	}
	if(y == e_decode)
	{
		printf("detected decoding\n");
		printf("<---------INFO:Selected decoding--------->\n");
        /*CLA count validation*/
        if(argc >=3 && argc <= 4)
        {
	         /*Checking CLA arguments is gets passed in proper extensions*/
	         if(read_and_validate_decode_args(argv,&decInfo) == e_success)
	         {
	         	printf("INFO:Read and validate success \n");
	         }
	         else
	         {
		        printf("INFO:Error, Read and validate failure \n");
		        return e_failure;
	         }
	         //To perform decoding operation
	         if(do_decoding(&decInfo) == e_success)
	         {
		       printf("<---------INFO:decoding success--------->\n");
	         }
           	else
	        {  
		      printf("<---------INFO:decoding failure--------->\n");
	       	  return e_failure;
			}
		}
	}

		return 0;
}

OperationType check_operation_type(char *argv[])
{
	if((argv[1] == NULL))
		return e_unsupported;
	
	else if(strcmp(argv[1],"-e")==0)
	    return e_encode;
	
	else if(strcmp(argv[1],"-d")==0)
		return e_decode;

	else
		return e_unsupported;
}
