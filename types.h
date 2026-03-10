#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef unsigned int uint;

/* Generic return status */
typedef enum {
    SUCCESS,
    FAILURE
} Status;

/* Operation selection */
typedef enum {
    ENCODE,
    DECODE,
    INVALID_OP
} OperationType;

#endif /* TYPES_H */
