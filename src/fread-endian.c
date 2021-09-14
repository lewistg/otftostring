#include "fread-endian.h"

// These methods utilize the method described
// here: https://commandcenter.blogspot.com/2012/04/byte-order-fallacy.html 

uint32_t fread_little_endian_uint32_t(FILE *file) {
    unsigned char bytes[4];
    fread(&bytes, sizeof(unsigned char), 4, file);
    return (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | (bytes[0] << 0);
}

uint16_t fread_little_endian_uint16_t(FILE *file) {
    unsigned char bytes[2];
    fread(&bytes, sizeof(unsigned char), 2, file);
    return (bytes[1] << 8) | (bytes[0] << 0);
}

uint32_t fread_big_endian_uint32_t(FILE *file) {
    unsigned char bytes[4];
    fread(&bytes, sizeof(unsigned char), 4, file);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3] << 0);
}

uint16_t fread_big_endian_uint16_t(FILE *file) {
    unsigned char bytes[2];
    fread(&bytes, sizeof(unsigned char), 2, file);
    return (bytes[0] << 8) | (bytes[1] << 0);
}

uint32_t fread_big_endian_uint24_t(FILE *file) {
    unsigned char bytes[2];
    fread(&bytes, sizeof(unsigned char), 2, file);
    return (bytes[0] << 8) | (bytes[1] << 0);
}
