#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

typedef struct {
    char *stego_image;
    FILE *fptr_stego;

    char *output_file;
    FILE *fptr_output;

    long secret_size;
} DecodeData;

/* CLI + file ops */
Status read_decode_args(int argc, char *argv[], DecodeData *dec);
Status open_decode_files(DecodeData *dec);

/* LSB helpers */
unsigned char decode_byte_from_lsb(const unsigned char *img8);
Status extract_bytes_from_image(FILE *f, int n_bytes, unsigned char *out);

/* Stego stream readers (must be called after fseek 54) */
Status read_magic(DecodeData *dec);
Status read_secret_size_be32(DecodeData *dec);
Status read_secret_payload(DecodeData *dec);

#endif /* DECODE_H */
