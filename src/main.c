/**
 * Reads an OTF font.
 * Spec: https://docs.microsoft.com/en-us/typography/opentype/spec/otff
 */

#include <stdlib.h>
#include <stdio.h>

#include "cff-table.h"
#include "cmap-table.h"
#include "table-directory.h"

void read_font();
void dump_cff_table(FILE *font_file);

int main(void) {
    read_font();
}

void read_font() {
    FILE *font_file = fopen("./original.otf", "r");

    struct table_directory td;
    read_table_directory(font_file, &td);
    print_table_directory(&td);

    if (seek_table(font_file, "CFF ", &td) < 0) {
        printf("ERROR: could not find CFF table\n");
        exit(1);
    }

    dump_cff_table(font_file);

    destroy_table_directory(&td);
    fclose(font_file);
}

void dump_cff_table(FILE *font_file) {
    struct cff_table_header header;
    struct cff_table_index name_index;
    struct cff_table_index top_dict_index;
    struct cff_table_index string_index;
    struct cff_table_index_entry top_dict_index_entry;
    struct cff_table_dict_entry *entries = 0;

    read_cff_table_header(font_file, &header);
    print_cff_table_header(&header);

    read_cff_table_index(font_file, &name_index);
    print_cff_table_name_index(&name_index);

    read_cff_table_index(font_file, &top_dict_index);
    get_cff_table_index_entry(&top_dict_index, 0, &top_dict_index_entry); 
    read_cff_table_dict(top_dict_index_entry.object_data, top_dict_index_entry.size, &entries);
    print_cff_table_dict(entries);

    read_cff_table_index(font_file, &string_index);
    print_cff_table_string_index(&string_index);

    destroy_cff_table_index(&name_index);
}
