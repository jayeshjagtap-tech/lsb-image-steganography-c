#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "encode.h"

int main(int argc, char *argv[])
{
    /* Expected usage: ./encode -e cover.bmp secret.txt stego.bmp */
    if (argc != 5) {
        printf("Usage: %s -e <cover_image.bmp> <secret_file> <stego_image.bmp>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-e") != 0) {
        printf("Use -e for encoding.\n");
        return 1;
    }

    EncodeData enc;
    if (read_encode_args(argv, &enc) == FAILURE) {
        printf("Invalid arguments\n");
        return 1;
    }

    if (perform_encoding(&enc) == SUCCESS) {
        printf("INFO: Secret encoded into '%s'\n", enc.output_image);
        return 0;
    } else {
        printf("ERROR: Encoding failed\n");
        return 1;
    }
}
