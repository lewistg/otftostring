#ifndef FREAD_ENDIAN_H
#define FREAD_ENDIAN_H

#include <stdint.h>
#include <stdio.h>

uint32_t fread_little_endian_uint32_t(FILE *file);
uint16_t fread_little_endian_uint16_t(FILE *file);

uint32_t fread_big_endian_uint32_t(FILE *file);
uint16_t fread_big_endian_uint16_t(FILE *file);
uint32_t fread_big_endian_uint24_t(FILE *file);

#endif
