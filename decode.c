#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "common.h"

Status read_decode_args(int argc, char *argv[], DecodeData *dec)
{
    if (argc != 4 || strcmp(argv[1], "-d") != 0)
        return FAILURE;
    dec->stego_image = argv[2];
    dec->output_file = argv[3];
    return SUCCESS;
}

Status open_decode_files(DecodeData *dec)
{
    dec->fptr_stego  = fopen(dec->stego_image, "rb"); /* binary! */
    if (!dec->fptr_stego) { perror("Opening stego image"); return FAILURE; }

    dec->fptr_output = fopen(dec->output_file, "wb"); /* binary! */
    if (!dec->fptr_output) { perror("Creating output file"); fclose(dec->fptr_stego); return FAILURE; }

    return SUCCESS;
}

/* Take 8 image bytes, reconstruct one data byte from their LSBs (MSB-first) */
unsigned char decode_byte_from_lsb(const unsigned char *img8)
{
    unsigned char out = 0;
    for (int i = 0; i < 8; ++i)
        out = (unsigned char)((out << 1) | (img8[i] & 1u));
    return out;
}

/* Read n_bytes by consuming 8*n_bytes image bytes */
Status extract_bytes_from_image(FILE *f, int n_bytes, unsigned char *out)
{
    unsigned char buf[8];
    for (int i = 0; i < n_bytes; ++i) {
        if (fread(buf, 1, 8, f) != 8) return FAILURE;
        out[i] = decode_byte_from_lsb(buf);
    }
    return SUCCESS;
}

/* Verify magic marker */
Status read_magic(DecodeData *dec)
{
    const int mlen = (int)strlen(MAGIC_PATTERN);
    unsigned char tmp[16]; /* MAGIC_PATTERN is tiny */
    if (extract_bytes_from_image(dec->fptr_stego, mlen, tmp) == FAILURE) return FAILURE;
    tmp[mlen] = '\0';
    return (strcmp((char*)tmp, MAGIC_PATTERN) == 0) ? SUCCESS : FAILURE;
}

/* Read 4-byte big-endian size written by encoder */
Status read_secret_size_be32(DecodeData *dec)
{
    unsigned char sz[4];
    if (extract_bytes_from_image(dec->fptr_stego, 4, sz) == FAILURE) return FAILURE;
    dec->secret_size = ((long)sz[0] << 24) | ((long)sz[1] << 16) | ((long)sz[2] << 8) | (long)sz[3];
    return SUCCESS;
}

/* Stream out payload */
Status read_secret_payload(DecodeData *dec)
{
    unsigned char buf[8];
    for (long i = 0; i < dec->secret_size; ++i) {
        if (fread(buf, 1, 8, dec->fptr_stego) != 8) return FAILURE;
        unsigned char ch = decode_byte_from_lsb(buf);
        if (fwrite(&ch, 1, 1, dec->fptr_output) != 1) return FAILURE;
    }
    return SUCCESS;
}
