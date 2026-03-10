#ifndef ENCODE_H
#define ENCODE_H

#include <stdio.h>
#include "types.h"

#define EXT_SIZE 4    /* we will store up to 3 chars + '\0' (we can store fewer) */

/* Container for encoding operation */
typedef struct {
    char *input_image;    /* cover image name */
    FILE *fptr_input;

    char *secret_file;    /* secret file name */
    FILE *fptr_secret;
    long secret_size;

    char *output_image;   /* stego image name */
    FILE *fptr_output;

    uint image_capacity;  /* bytes available (width*height*3) */
} EncodeData;

/* Prototypes */
OperationType choose_operation(char *argv[]);
Status read_encode_args(char *argv[], EncodeData *enc);
Status open_encode_files(EncodeData *enc);
uint get_bmp_image_capacity(FILE *fptr);
uint get_file_size(FILE *fptr);
Status check_storage_capacity(EncodeData *enc);
Status copy_bmp_header(FILE *src, FILE *dest);
Status hide_byte_to_lsb(unsigned char data, unsigned char *img_buf);
Status hide_data_to_image(const unsigned char *data, int data_len, FILE *src, FILE *dest);
Status hide_magic(const char *magic, EncodeData *enc);
Status hide_secret_size(long size, EncodeData *enc);
Status hide_secret_data(EncodeData *enc);
Status copy_remaining_data(FILE *src, FILE *dest);
Status perform_encoding(EncodeData *enc);

#endif /* ENCODE_H */
