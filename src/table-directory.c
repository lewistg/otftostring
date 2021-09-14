#include "table-directory.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "fread-endian.h"

void read_table_directory(FILE *font_file, struct table_directory *td) {
    td->sfnt_version = fread_big_endian_uint32_t(font_file);
    td->num_tables = fread_big_endian_uint16_t(font_file);
    td->search_range = fread_big_endian_uint16_t(font_file);
    td->entry_selector = fread_big_endian_uint16_t(font_file);
    td->range_shift = fread_big_endian_uint16_t(font_file);
    td->table_records = calloc(sizeof(struct table_record), td->num_tables);

    uint16_t i;
    for (i = 0; i < td->num_tables; i++) {
        read_table_record(font_file, td->table_records + i);
    }
}

void destroy_table_directory(struct table_directory *td) {
    free(td->table_records);
}

void print_table_directory(struct table_directory *td) {
    printf("Table directory\n");
    printf("sfnt_version: 0x%" PRIx32 "\n", td->sfnt_version);
    printf("num_tables: %" PRIu16 "\n", td->num_tables);

    /*size_t i;
    for (i = 0; i < td->num_tables; i++) {
        print_table_record(td->table_records + i);
    }*/
}

void print_table_record(struct table_record *tr) {
    printf("Table record\n");
    char tag[5] = {
        tr->tag[0],
        tr->tag[1],
        tr->tag[2],
        tr->tag[3],
        0
    };
    printf("tag: %s\n", tag);
    printf("offset: %" PRIx32 "\n", tr->offset);
}

void read_table_record(FILE *font_file, struct table_record *tr) {
    fread(&tr->tag, sizeof(uint8_t), 4, font_file);
    tr->checksum = fread_big_endian_uint32_t(font_file);
    tr->offset = fread_big_endian_uint32_t(font_file);
    tr->length = fread_big_endian_uint32_t(font_file);
}

struct table_record *find_table_record(const char *tag, struct table_directory *td) {
    size_t i;
    for (i = 0; i < td->num_tables; i++) {
        struct table_record *record = td->table_records + i;
        if (memcmp(tag, record, TAG_LENGTH) == 0) {
            return record;
        }
    }
    return NULL;
}

int seek_table(FILE *font_file, const char *tag, struct table_directory *td) {
    struct table_record *record = find_table_record(tag, td);
    if (record == NULL) {
        return -1;
    }
    fseek(font_file, record->offset, SEEK_SET);
    return 0;
}
