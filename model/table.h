#ifndef TABLE_H_
#define TABLE_H_

#include "row.h"

const __uint32_t PAGE_SIZE = 4096;
#define TABLE_MAX_PAGES 100
const __uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const __uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

typedef struct
{
    __uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} Table;

// table initialization
Table *new_table()
{
    Table *table = (Table *)malloc(sizeof(Table));
    table->num_rows = 0;
    for (__uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        table->pages[i] = NULL;
    }

    return table;
}

// table destruction
void free_table(Table *table)
{
    for (__uint32_t i = 0; table->pages[i]; i++)
    {
        free(table->pages[i]);
    }
    free(table);
}

// locate where to read/write in memory for a particular row
void *row_slot(Table *table, __uint32_t row_num)
{
    __uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = table->pages[page_num];
    if (page == NULL)
    {
        // allocate memory only when we try to access page
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }
    __uint32_t row_offset = row_num % ROWS_PER_PAGE;
    __uint32_t byte_offset = row_offset * ROW_SIZE;

    return page + byte_offset;
}

#endif