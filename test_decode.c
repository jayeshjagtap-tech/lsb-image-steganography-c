#include <stdio.h>
#include <string.h>
#include "decode.h"

int main(int argc, char *argv[])
{
    DecodeData dec;

    /* Usage for decoding (this was missing/inconsistent) */
    if (read_decode_args(argc, argv, &dec) == FAILURE) {
        printf("Usage: %s -d <stego_image.bmp> <recovered_file>\n", argv[0]);
        return 1;
    }

    if (open_decode_files(&dec) == FAILURE) return 1;

    /* Always skip 54-byte BMP header before reading embedded LSBs */
    fseek(dec.fptr_stego, 54, SEEK_SET);

    if (read_magic(&dec) != SUCCESS) {
        printf("ERROR: Not a valid stego image (magic mismatch).\n");
        fclose(dec.fptr_stego); fclose(dec.fptr_output);
        return 1;
    }

    if (read_secret_size_be32(&dec) != SUCCESS) {
        printf("ERROR: Failed to read embedded size.\n");
        fclose(dec.fptr_stego); fclose(dec.fptr_output);
        return 1;
    }

    if (read_secret_payload(&dec) != SUCCESS) {
        printf("ERROR: Failed while extracting payload.\n");
        fclose(dec.fptr_stego); fclose(dec.fptr_output);
        return 1;
    }

    printf("Decoding successful → saved to '%s'\n", dec.output_file);
    fclose(dec.fptr_stego);
    fclose(dec.fptr_output);
    return 0;
}
