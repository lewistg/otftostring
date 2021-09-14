#ifndef TABLE_DIRECTORY_H
#define TABLE_DIRECTORY_H

#include <stdio.h>
#include <stdint.h>

#define TAG_LENGTH 4

struct table_record {
    uint8_t tag[TAG_LENGTH];
    uint32_t checksum;
    uint32_t offset;
    uint32_t length;
};

struct table_directory {
    uint32_t sfnt_version;
    uint16_t num_tables;
    uint16_t search_range;
    uint16_t entry_selector;
    uint16_t range_shift;
    struct table_record *table_records;
};

void read_table_directory(FILE *font_file, struct table_directory *td);
void destroy_table_directory(struct table_directory *td);
void print_table_directory(struct table_directory *td);

void read_table_record(FILE *font_file, struct table_record *tr);
void print_table_record(struct table_record *tr);

struct table_record *find_table_record(const char *tag, struct table_directory *td);
int seek_table(FILE *font_file, const char *tag, struct table_directory *td);

#endif
