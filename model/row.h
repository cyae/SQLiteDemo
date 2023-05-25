#ifndef ROW_H_
#define ROW_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// hard-coded row
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef struct
{
    __uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1]; // +1 for ending marker in C string
    char email[COLUMN_EMAIL_SIZE + 1];       // same as above
} Row;

void print_row(Row *row)
{
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

// compact representation of a row
#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)

const __uint32_t ID_SIZE = size_of_attribute(Row, id);
const __uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const __uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const __uint32_t ID_OFFSET = 0;
const __uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const __uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const __uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

void serialize_row(Row *source, void *destination)
{
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    strncpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
    strncpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void deserialize_row(void *source, Row *destination)
{
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

#endif