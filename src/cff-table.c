#include "cff-table.h"

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "fread-endian.h"

#define OPERAND_BUFFER_LENGTH 256

// static const to scope to file
static const size_t NUM_STANDARD_STRINGS = 391;

static bool is_operand(const unsigned char *operand) {
    return (*operand >= 28 && *operand <= 30) || (*operand >= 32 && *operand <= 254);
}

static bool is_operator(const unsigned char *operator) {
    return *operator >= 0 && *operator <= 21;
}

static void reverse_linked_list(void **head, void * (*get_next)(void *), void (*set_next)(void *, void *)) {
    void *reversed = NULL;
    void *current_node = *head;
    while (current_node != NULL) {
        void *next_node = get_next(current_node);
        set_next(current_node, reversed);
        reversed = current_node;
        current_node = next_node;
    }
    *head = reversed;
}

static struct cff_table_dict_operand *get_next_operand(struct cff_table_dict_operand *operand) {
    return operand->next_operand;
}

static struct cff_table_dict_operand *set_next_operand(struct cff_table_dict_operand *operand, struct cff_table_dict_operand *next_operand) {
    return operand->next_operand = next_operand;
}

static void reverse_operands(struct cff_table_dict_operand **operands) {
    reverse_linked_list(
        (void **) operands, 
        (void * (*)(void *)) get_next_operand, 
        (void (*)(void *, void *)) set_next_operand
    );
}

static void operand_to_string(struct cff_table_dict_operand *operand, char *operand_string, size_t max_length) {
    switch (operand->type) {
        case INT8:
            snprintf(operand_string, max_length, "%" PRIi8, *(int8_t *) operand->data);
            break;
        case INT16:
            snprintf(operand_string, max_length, "%" PRIi16, *(int16_t *) operand->data);
            break;
        case INT64:
            snprintf(operand_string, max_length, "%" PRIi64, *(int64_t *) operand->data);
            break;
        case DOUBLE:
            snprintf(operand_string, max_length, "%lf", *(double *) operand->data);
            break;
        default:
            snprintf(operand_string, max_length, "unknown operand\n");
    }
}

size_t get_index_object_size(struct cff_table_index *index, size_t i) {
    return index->offsets[i + 1] - index->offsets[i];
}

static struct cff_table_dict_entry *get_next_entry(struct cff_table_dict_entry *entry) {
    return entry->next_entry;
}

static struct cff_table_dict_entry *set_next_entry(struct cff_table_dict_entry *entry, struct cff_table_dict_entry *next_entry) {
    return entry->next_entry = next_entry;
}

static void reverse_entries(struct cff_table_dict_entry **entrys) {
    reverse_linked_list(
        (void **) entrys, 
        (void * (*)(void *)) get_next_entry, 
        (void (*)(void *, void *)) set_next_entry
    );
}

/*static void reverse_operands(struct cff_table_dict_operand **operands) {
    struct cff_table_dict_operand *reversed_operands = NULL;
    struct cff_table_dict_operand *operand = *operands;
    while (operand != NULL) {
        struct cff_table_dict_operand *next_operand = operand->next_operand;
        operand->next_operand = reversed_operands;
        reversed_operands = operand;
        operand = next_operand;
    }
    *operands = reversed_operands;
}*/

/*static void reverse_entries(struct cff_table_dict_entry **entries) {
    struct cff_table_dict_operand *reversed_entries = NULL;
    struct cff_table_dict_operand *entry = *entries;
    while (entry != NULL) {
        struct cff_table_dict_operand *cff_table_dict_entry = operand->next_operand;
        operand->next_operand = reversed_operands;
        reversed_operands = operand;
        operand = next_operand;
    }
    *operands = reversed_operands;
}*/

static size_t read_operator(unsigned char *operator, struct cff_table_dict_entry **entries, struct cff_table_dict_operand *operands) {
    assert(is_operator(operator));
    struct cff_table_dict_entry *next_entry = malloc(sizeof(struct cff_table_dict_entry));
    size_t operator_size;
    if (*operator == 0xc) {
        next_entry->operator = (*(operator + 0) << 8) | (*(operator + 1) << 0);
        operator_size = 2;
    } else {
        next_entry->operator = *operator;
        operator_size = 1;
    }
    next_entry->next_entry = *entries;
    next_entry->operands = operands;
    *entries = next_entry;
    return operator_size;
}

static const char *cff_table_dict_operator_to_str(enum cff_table_dict_operator_type operator) {
    switch (operator) {
        case VERSION:
            return "version";
        case NOTICE:
            return "notice";
        case COPYRIGHT:
            return "copyright";
        case FULL_NAME:
            return "full name";
        case FAMILY_NAME:
            return "family name";
        case WEIGHT:
            return "weight";
        case IS_FIXED_PITCH:
            return "is fixed pitch";
        case ITALIC_ANGLE:
            return "italic angle";
        case UNDERLINE_POSITION:
            return "underline position";
        case UNDERLINE_THICKNESS:
            return "underlind thickness";
        case PAINT_TYPE:
            return "paint type";
        case CHARSTRING_TYPE:
            return "charstring type";
        case FONT_MATRIX:
            return "font matrix";
        case UNIQUE_ID:
            return "unique id";
        case FONT_BBOX:
            return "font bbox";
        case STROKE_WIDTH:
            return "stroke width";
        case XUID:
            return "xuid";
        case CHARSET:
            return "charset";
        case ENCODING:
            return "encoding";
        case CHAR_STRINGS:
            return "char strings";
        case PRIVATE:
            return "private";
        case SYNTHETIC_BASE:
            return "synthetic base";
        case POST_SCRIPT:
            return "post script";
        case BASE_FONT_NAME:
            return "base font name";
        case BASE_FONT_BLEND:
            return "base font blend";
        case ROS:
            return "ros";
        case CID_FONT_VERSION:
            return "cid font version";
        case CID_FONT_REVISION:
            return "cid font revision";
        case CID_FONT_TYPE:
            return "cid font type";
        case CID_COUNT:
            return "cid count";
        case UID_BASE:
            return "uid base";
        case FD_ARRAY:
            return "fd array";
        case FD_SELECT:
            return "fd select";
        case FONT_NAME:
            return "font name";
        default:
            return "unknown operator";
    }
}

void read_cff_table_header(FILE* font_file, struct cff_table_header *header) {
     fread(&header->major, sizeof(uint8_t), 1, font_file);
     fread(&header->minor, sizeof(uint8_t), 1, font_file);
     fread(&header->header_size, sizeof(uint8_t), 1, font_file);
     fread(&header->offset_size, sizeof(uint8_t), 1, font_file);
}

void print_cff_table_header(struct cff_table_header *header) {
    printf("'cff' table header\n");
    printf("\tmajor version: %" PRIu8 "\n", header->major);
    printf("\tminor version: %" PRIu8 "\n", header->minor);
}

int read_cff_table_index(FILE *font_file, struct cff_table_index *index) {
    index->count = fread_big_endian_uint16_t(font_file);
    fread(&index->offset_size, sizeof(uint8_t), 1, font_file);

    size_t offsets_count = index->count + 1;
    index->offsets = calloc(offsets_count, sizeof(uint32_t));

    size_t i;
    for (i = 0; i < offsets_count; i++) {
        if (index->offset_size == 1) {
            fread(index->offsets + i, sizeof(uint8_t), 1, font_file);
        } else if (index->offset_size == 2) {
            index->offsets[i] = fread_big_endian_uint16_t(font_file);
        } else if (index->offset_size == 3) {
            index->offsets[i] = fread_big_endian_uint24_t(font_file);
        } else if (index->offset_size == 4) {
            index->offsets[i] = fread_big_endian_uint32_t(font_file);
        } else {
            return -1;
        }
    }

    size_t data_size = index->offsets[offsets_count - 1] - 1;
    index->object_data = calloc(data_size, sizeof(uint8_t));
    fread(index->object_data, sizeof(uint8_t), data_size, font_file);

    return 0;
}

void get_cff_table_index_entry(struct cff_table_index *index, size_t entry_index, struct cff_table_index_entry *entry) {
    entry->size = index->offsets[entry_index + 1] - index->offsets[entry_index];
    entry->object_data = index->object_data + (index->offsets[entry_index] - 1); // Recall: Offsets are 1-indexed
}

void destroy_cff_table_index(struct cff_table_index *index) {
    free(index->offsets);
    free(index->object_data);
}

void print_cff_table_name_index(struct cff_table_index *name_index) {
    printf("Name index\n");
    printf("\tCount: %" PRIu16 "\n", name_index->count);
    printf("\tLanguages:\n");
    size_t i;
    for (i = 0; i < name_index->count; i++) {
        size_t string_length = name_index->offsets[i + 1] - name_index->offsets[i];
        printf("\t\t");
        fwrite(
            name_index->object_data + (name_index->offsets[i] - 1), 
            sizeof(uint8_t),
            string_length,
            stdout
        );
        printf("\n");
    }
}

void print_cff_table_string_index(struct cff_table_index *string_index) {
    printf("String index\n");
    printf("\tCount: %" PRIu16 "\n", string_index->count);
    printf("\tSID / string:\n");
    size_t i;
    for (i = 0; i < string_index->count; i++) {
        size_t string_length = string_index->offsets[i + 1] - string_index->offsets[i];
        size_t sid = NUM_STANDARD_STRINGS + i;
        printf("\t\t");
        printf("%zu \t", sid);
        fwrite(
            string_index->object_data + (string_index->offsets[i] - 1), 
            sizeof(uint8_t),
            string_length,
            stdout
        );
        printf("\n");
    }
}

void read_cff_table_dict(unsigned char *dict_data, size_t dict_data_size, struct cff_table_dict_entry **entries) {
    unsigned char *current_byte = dict_data;
    struct cff_table_dict_operand *operands = NULL;
    *entries = NULL;
    while (current_byte < dict_data + dict_data_size) {
        if (is_operand(current_byte)) {
            current_byte += read_operand(current_byte, &operands);
        } else if (is_operator(current_byte)) {
            reverse_operands(&operands);
            current_byte += read_operator(current_byte, entries, operands);
            operands = NULL;
        }
    }
    reverse_entries(entries);
}

void print_cff_table_dict(struct cff_table_dict_entry *head_entry) {
    printf("Top DICT\n");
    struct cff_table_dict_entry *entry;
    char operand_string[OPERAND_BUFFER_LENGTH];
    for (entry = head_entry; entry; entry = entry->next_entry) {
        printf("\toperator: %s\n", cff_table_dict_operator_to_str(entry->operator));
        printf("\toperands:");
        struct cff_table_dict_operand *operand;
        for (operand = entry->operands; operand; operand = operand->next_operand) {
            operand_to_string(operand, operand_string, OPERAND_BUFFER_LENGTH);
            printf(" %s", operand_string);
        }
        printf("\n\n");
    }
}

/**
 * @return The number of read bytes
 */
size_t read_operand(unsigned char *operand, struct cff_table_dict_operand **operands) {
    assert(is_operand(operand));

    size_t bytes_read = 0;

    struct cff_table_dict_operand *next_operand = malloc(sizeof(struct cff_table_dict_operand));
    if (*operand >= 32 && *operand <= 246) {
        next_operand->type = INT8;
        next_operand->data = malloc(sizeof(int8_t));
        *((uint8_t *) next_operand->data) = *operand - 139;
        bytes_read = 1;
    } else if (*operand >= 247 && *operand <= 250) {
        next_operand->type = INT16;
        next_operand->data = malloc(sizeof(int16_t));
        *((uint16_t *) next_operand->data) = (*operand - 247) * 256 + (*(operand + 1) + 108);
        bytes_read = 2;
    } else if (*operand >= 251 && *operand <= 254) {
        next_operand->type = INT16;
        next_operand->data = malloc(sizeof(int16_t));
        *((uint16_t *) next_operand->data) = -(*operand - 251) * 256 - *(operand + 1) - 108;
        bytes_read = 2;
    } else if (*operand == 28) {
        next_operand->type = INT16;
        next_operand->data = malloc(sizeof(int16_t));
        *((uint16_t *) next_operand->data) = (*(operand + 1) << 8)  | *(operand + 2);
        bytes_read = 3;
    } else if (*operand == 29) {
        next_operand->type = INT64;
        next_operand->data = malloc(sizeof(int64_t));
        *((uint64_t * )next_operand->data) = (*(operand + 1) << 24)  | (*(operand + 2) << 16) | (*(operand + 3) << 8) | (*(operand + 4) << 0);
        bytes_read = 5;
    } else if (*operand == 30) {
        size_t float_str_index = 0;
        char float_str[128];
        unsigned char *c = operand + 1; // Move past prefix
        bytes_read = 1;

        unsigned char nibble_mask_index = 0;
        unsigned char nibble_masks[2] = {0xf0, 0x0f};

        while (true) {
            char nibble = *c & nibble_masks[nibble_mask_index];
            if (nibble_mask_index == 0) {
                // Shift the nibble if it is the most significant nibble
                nibble = (nibble >> 4) & 0x0f;
            }
            if (nibble == 0xf) {
                break;
            }
            assert(nibble != 0xd); // 0xd is reserved
            if (nibble >= 0 && nibble <= 9) {
                float_str[float_str_index++] = nibble + '0';
            } else if (nibble == 0xa) {
                float_str[float_str_index++] = '.';
            } else if (nibble == 0xb) {
                float_str[float_str_index++] = 'E';
            } else if (nibble == 0xc) {
                float_str[float_str_index++] = 'E';
                float_str[float_str_index++] = '-';
            } else if (nibble == 0xe) {
                float_str[float_str_index++] = '-';
            } else {
                return -1;
            }

            if (nibble_mask_index == 1) {
                nibble_mask_index = 0;
                c += 1; // Advance to the next byte
            } else {
                nibble_mask_index = 1;
            }
        }
        bytes_read += c - operand;
        float_str[float_str_index] = 0;

        next_operand->type = DOUBLE;
        next_operand->data = malloc(sizeof(double));
        *((double *) next_operand->data) = strtod(float_str, NULL);
    }

    next_operand->next_operand = *operands;
    *operands = next_operand;

    return bytes_read;
}

void print_operand(const struct cff_table_dict_operand *operand) {
    switch (operand->type) {
        case INT8:
            printf("%" PRIi8 "\n", *(int8_t *) operand->data);
            break;
        case INT16:
            printf("%" PRIi16 "\n", *(int16_t *) operand->data);
            break;
        case INT64:
            printf("%" PRIi64 "\n", *(int64_t *) operand->data);
            break;
        case DOUBLE:
            printf("%lf\n", *(double *) operand->data);
            break;
        default:
            printf("unknown operand\n");
    }

struct cff_table_dict_operand {
    enum cff_table_dict_operand_type type;
    void *data;
    struct cff_table_dict_operand *next_operand;
};
}
