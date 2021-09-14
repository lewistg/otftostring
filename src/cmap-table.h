#ifndef CMAP_TABLE_H
#define CMAP_TABLE_H

#include <stdint.h>
#include <stdio.h>

struct cmap_encoding_record {
    uint16_t platform_id;
    uint16_t encoding_id;
    uint32_t subtable_offset;
};

struct cmap_table_header {
    uint16_t version;
    uint16_t num_tables;
    struct cmap_encoding_record *records;
};

void read_cmap_table_header(FILE *font_file, struct cmap_table_header *header);
void destroy_cmap_table_header(struct cmap_table_header *header);
void print_cmap_table_header(struct cmap_table_header *header);

#endif
