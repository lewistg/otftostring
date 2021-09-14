#ifndef CFF_TABLE_H
#define CFF_TABLE_H

#include <stdio.h>
#include <stdint.h>

// See: https://wwwimages2.adobe.com/content/dam/acom/en/devnet/font/pdfs/5176.CFF.pdf


struct cff_table_header {
    uint8_t major;
    uint8_t minor;
    uint8_t header_size;
    uint8_t offset_size;
};

struct cff_table_index {
    uint16_t count;
    uint8_t offset_size;
    uint32_t *offsets;
    uint8_t *object_data;
};

struct cff_table_index_entry {
    size_t size;
    uint8_t *object_data;
};

enum cff_table_dict_operand_type {
	INT8,
    INT16,
    INT64,
    DOUBLE,
    BOOLEAN,
    SID,
    ARRAY,
    DELTA,
};

enum cff_table_dict_operator_type {
	// Top DICT Operator Entries
	VERSION = 0x0000,
	NOTICE = 0x0001,
	COPYRIGHT = 0x0c00,
	FULL_NAME = 0x0002,
	FAMILY_NAME = 0x0003,
	WEIGHT = 0x0004,
	IS_FIXED_PITCH = 0x0c01,
	ITALIC_ANGLE = 0x0c02,
	UNDERLINE_POSITION = 0x0c03,
	UNDERLINE_THICKNESS = 0x0c04,
	PAINT_TYPE = 0x0c05,
	CHARSTRING_TYPE = 0x0c06,
	FONT_MATRIX = 0x0c07,
	UNIQUE_ID = 0x000d,
	FONT_BBOX = 0x0005,
	STROKE_WIDTH = 0x0c08,
	XUID = 0x000e,
	CHARSET = 0x000f,
	ENCODING = 0x0010,
	CHAR_STRINGS = 0x0011,
	PRIVATE = 0x0012,
	SYNTHETIC_BASE = 0x0c14,
	POST_SCRIPT = 0x0c15,
	BASE_FONT_NAME = 0x0c16,
	BASE_FONT_BLEND = 0x0c17,
	// CIDFont Operator Extensions
	ROS = 0x0c1e,
	CID_FONT_VERSION = 0x0c1f,
	CID_FONT_REVISION = 0x0c20,
	CID_FONT_TYPE = 0x0c21,
	CID_COUNT = 0x0c22,
	UID_BASE = 0x0c23,
	FD_ARRAY = 0x0c24,
	FD_SELECT = 0x0c25,
	FONT_NAME = 0x0c26,
};

struct cff_table_dict_operand {
    enum cff_table_dict_operand_type type;
    void *data;
    struct cff_table_dict_operand *next_operand;
};

struct cff_table_dict_entry {
    uint16_t operator;
    struct cff_table_dict_operand *operands;
    struct cff_table_dict_entry *next_entry;
};

void read_cff_table_header(FILE *font_file, struct cff_table_header *header);
void print_cff_table_header(struct cff_table_header *header);

int read_cff_table_index(FILE *font_file, struct cff_table_index *index);
void get_cff_table_index_entry(struct cff_table_index *index, size_t entry_index, struct cff_table_index_entry *entry);
void destroy_cff_table_index(struct cff_table_index *index);

void read_cff_table_dict(unsigned char *dict_data, size_t dict_data_size, struct cff_table_dict_entry **entries);
void print_cff_table_dict(struct cff_table_dict_entry *head_entry);

void print_cff_table_name_index(struct cff_table_index *name_index);

void print_cff_table_string_index(struct cff_table_index *string_index);

size_t read_operand(unsigned char *operand, struct cff_table_dict_operand **operands);
void print_operand(const struct cff_table_dict_operand *operand);

#endif
