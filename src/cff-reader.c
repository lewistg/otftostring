#include <stdio.h>

#include "cff-table.h"

int main() {
    FILE *font_file = fopen("./examples/example-font.cff", "r");

    struct cff_table_header header;
    struct cff_table_index name_index;
    struct cff_table_index top_dict_index;
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

    destroy_cff_table_index(&name_index);
}
