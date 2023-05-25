#ifndef CURSOR_H_
#define CURSOR_H_

#include "indexing.h"

// abstracting cursor out of table so that we can interact with table without concerning about how the table is stored.
typedef struct
{
    Table *table;
    __uint32_t row_num;
    bool end_of_table;
} Cursor;

Cursor *table_start(Table *table)
{
    Cursor *cursor = (Cursor *)malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = 0;
    cursor->end_of_table = (table->num_rows == 0);

    return cursor;
}

Cursor *table_end(Table *table)
{
    Cursor *cursor = (Cursor *)malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = table->num_rows;
    cursor->end_of_table = true;

    return cursor;
}

// locate where to read/write in memory for a particular row using cursor
void *cursor_value(Cursor *cursor)
{
    __uint32_t row_num = cursor->row_num;
    __uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = get_page(cursor->table->pager, page_num);
    __uint32_t row_offset = row_num % ROWS_PER_PAGE;
    __uint32_t byte_offset = row_offset * ROW_SIZE;

    return page + byte_offset;
}

void cursor_advance(Cursor *cursor)
{
    cursor->row_num += 1;
    if (cursor->row_num >= cursor->table->num_rows)
    {
        cursor->end_of_table = true;
    }
}

#endif