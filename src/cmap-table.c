#include "cmap-table.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "fread-endian.h"

static void read_cmap_encoding_record(FILE *font_file, struct cmap_encoding_record *record) {
    record->platform_id = fread_big_endian_uint16_t(font_file);
    record->encoding_id = fread_big_endian_uint16_t(font_file);
    record->subtable_offset = fread_big_endian_uint32_t(font_file);
}

void read_cmap_table_header(FILE *font_file, struct cmap_table_header *header) {
    header->version = fread_big_endian_uint16_t(font_file);
    header->num_tables = fread_big_endian_uint16_t(font_file);

    header->records = calloc(sizeof(struct cmap_encoding_record), header->num_tables);
    size_t i;
    for (i = 0; i < header->num_tables; i++) {
        read_cmap_encoding_record(font_file, header->records + i);
    }
}

void destroy_cmap_table_header(struct cmap_table_header *header) {
    free(header->records);
}

void print_cmap_table_header(struct cmap_table_header *header) {
    printf("Cmap table header\n");
    printf("version: %" PRIu16 "\n", header->version);
    printf("num_tables: %" PRIu16 "\n", header->num_tables);
}
