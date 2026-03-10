#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode.h"
#include "common.h"

/* Decide operation from argv[1] */
OperationType choose_operation(char *argv[])
{
    if (!argv[1]) return INVALID_OP;
    if (strcmp(argv[1], "-e") == 0) return ENCODE;
    if (strcmp(argv[1], "-d") == 0) return DECODE;
    return INVALID_OP;
}

/* Read and validate encode arguments: ./encode -e input.bmp secret.txt output.bmp */
Status read_encode_args(char *argv[], EncodeData *enc)
{
    if (!argv[2] || !argv[3] || !argv[4]) return FAILURE;
    enc->input_image = argv[2];
    enc->secret_file = argv[3];
    enc->output_image = argv[4];
    return SUCCESS;
}

/* Open files in binary mode */
Status open_encode_files(EncodeData *enc)
{
    enc->fptr_input = fopen(enc->input_image, "rb");
    if (!enc->fptr_input) { perror("Opening input image"); return FAILURE; }

    enc->fptr_secret = fopen(enc->secret_file, "rb");
    if (!enc->fptr_secret) { perror("Opening secret file"); fclose(enc->fptr_input); return FAILURE; }

    enc->fptr_output = fopen(enc->output_image, "wb");
    if (!enc->fptr_output) { perror("Creating output image"); fclose(enc->fptr_input); fclose(enc->fptr_secret); return FAILURE; }

    return SUCCESS;
}

/* Read width/height from BMP header (offset 18) and return capacity = w*h*3 */
uint get_bmp_image_capacity(FILE *fptr)
{
    uint32_t width=0, height=0;
    fseek(fptr, 18, SEEK_SET);
    fread(&width, sizeof(uint32_t), 1, fptr);
    fread(&height, sizeof(uint32_t), 1, fptr);
    /* restore file pointer not necessary here */
    return width * height * 3u;
}

/* Return file size */
uint get_file_size(FILE *fptr)
{
    long cur = ftell(fptr);
    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    fseek(fptr, cur, SEEK_SET);
    return (uint)size;
}

/* Check if image has enough capacity to store secret and overhead */
Status check_storage_capacity(EncodeData *enc)
{
    enc->image_capacity = get_bmp_image_capacity(enc->fptr_input);
    /* Overhead: magic (length) + 4 bytes size + extension not stored here (we keep simple) */
    uint needed_bits = (strlen(MAGIC_PATTERN) + 4 + enc->secret_size) * 8u;
    if (enc->image_capacity >= needed_bits) return SUCCESS;
    return FAILURE;
}

/* Copy BMP header (54 bytes) */
Status copy_bmp_header(FILE *src, FILE *dest)
{
    unsigned char header[54];
    rewind(src);
    if (fread(header, 1, 54, src) != 54) return FAILURE;
    if (fwrite(header, 1, 54, dest) != 54) return FAILURE;
    return SUCCESS;
}

/* Hide one byte into 8 bytes (LSBs) of image buffer */
Status hide_byte_to_lsb(unsigned char data, unsigned char *img_buf)
{
    for (int i = 0; i < 8; ++i) {
        img_buf[i] &= 0xFEu;                          /* clear LSB */
        img_buf[i] |= (unsigned char)((data >> (7 - i)) & 0x1u); /* set LSB to bit */
    }
    return SUCCESS;
}

/* Hide sequence of bytes to image: for each byte read 8 image bytes, hide, write */
Status hide_data_to_image(const unsigned char *data, int data_len, FILE *src, FILE *dest)
{
    unsigned char img_buf[8];
    for (int i = 0; i < data_len; ++i)
    {
        if (fread(img_buf, 1, 8, src) != 8) return FAILURE;
        hide_byte_to_lsb(data[i], img_buf);
        if (fwrite(img_buf, 1, 8, dest) != 8) return FAILURE;
    }
    return SUCCESS;
}

/* Hide magic string */
Status hide_magic(const char *magic, EncodeData *enc)
{
    return hide_data_to_image((const unsigned char *)magic, (int)strlen(magic), enc->fptr_input, enc->fptr_output);
}

/* Hide secret size (store as 4 bytes big-endian) */
Status hide_secret_size(long size, EncodeData *enc)
{
    unsigned char size_bytes[4];
    size_bytes[0] = (unsigned char)((size >> 24) & 0xFF);
    size_bytes[1] = (unsigned char)((size >> 16) & 0xFF);
    size_bytes[2] = (unsigned char)((size >> 8) & 0xFF);
    size_bytes[3] = (unsigned char)(size & 0xFF);
    return hide_data_to_image(size_bytes, 4, enc->fptr_input, enc->fptr_output);
}

/* Hide secret file bytes */
Status hide_secret_data(EncodeData *enc)
{
    rewind(enc->fptr_secret);
    unsigned char ch;
    while (fread(&ch, 1, 1, enc->fptr_secret) == 1)
    {
        if (hide_data_to_image(&ch, 1, enc->fptr_input, enc->fptr_output) == FAILURE) return FAILURE;
    }
    return SUCCESS;
}

/* Copy remaining source image bytes to output (after encoding done) */
Status copy_remaining_data(FILE *src, FILE *dest)
{
    unsigned char buf;
    while (fread(&buf, 1, 1, src) == 1)
    {
        if (fwrite(&buf, 1, 1, dest) != 1) return FAILURE;
    }
    return SUCCESS;
}

/* High-level encoding flow */
Status perform_encoding(EncodeData *enc)
{
    if (open_encode_files(enc) == FAILURE) return FAILURE;

    /* determine secret size */
    fseek(enc->fptr_secret, 0, SEEK_END);
    enc->secret_size = ftell(enc->fptr_secret);
    rewind(enc->fptr_secret);

    if (check_storage_capacity(enc) == FAILURE) {
        printf("ERROR: image does not have enough capacity\n");
        fclose(enc->fptr_input);
        fclose(enc->fptr_secret);
        fclose(enc->fptr_output);
        return FAILURE;
    }

    /* copy header and then start hiding (src pointer now at 54) */
    if (copy_bmp_header(enc->fptr_input, enc->fptr_output) == FAILURE) goto fail;

    if (hide_magic(MAGIC_PATTERN, enc) == FAILURE) goto fail;
    if (hide_secret_size(enc->secret_size, enc) == FAILURE) goto fail;
    if (hide_secret_data(enc) == FAILURE) goto fail;

    /* After embedding, copy all remaining bytes as-is */
    if (copy_remaining_data(enc->fptr_input, enc->fptr_output) == FAILURE) goto fail;

    /* close files */
    fclose(enc->fptr_input);
    fclose(enc->fptr_secret);
    fclose(enc->fptr_output);
    return SUCCESS;

fail:
    fclose(enc->fptr_input);
    fclose(enc->fptr_secret);
    fclose(enc->fptr_output);
    return FAILURE;
}
