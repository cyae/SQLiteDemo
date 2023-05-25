#ifndef INDEXING_H_
#define INDEXING_H_

#include "../model/row.h"
#include "pager.h"

const __uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const __uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

typedef struct
{
    Pager *pager;
    __uint32_t num_rows;
} Table;

// table initialization
Table *db_open(const char *filename)
{
    Pager *pager = pager_open(filename);
    __uint32_t num_rows = pager->file_length / ROW_SIZE;

    Table *table = (Table *)malloc(sizeof(Table));
    table->pager = pager;
    table->num_rows = num_rows;

    return table;
}

// table destruction
void db_close(Table *table)
{
    Pager *pager = table->pager;
    __uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

    for (__uint32_t i = 0; i < num_full_pages; i++)
    {
        if (pager->pages[i] == NULL)
        {
            continue;
        }
        pager_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    // there may be a partial page to write to the end of the file
    // this should not be needed after we switch to a B-tree
    __uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0)
    {
        __uint32_t page_num = num_full_pages;
        if (pager->pages[page_num] != NULL)
        {
            pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }

    int result = close(pager->file_descriptor);
    if (result == -1)
    {
        printf("Eoor closing db file.\n");
        exit(EXIT_FAILURE);
    }

    for (__uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        void *page = pager->pages[i];
        if (page)
        {
            free(page);
            pager->pages[i] = NULL;
        }
    }

    free(pager);
    free(table);
}

// locate where to read/write in memory for a particular row
void *row_slot(Table *table, __uint32_t row_num)
{
    __uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = get_page(table->pager, page_num);
    __uint32_t row_offset = row_num % ROWS_PER_PAGE;
    __uint32_t byte_offset = row_offset * ROW_SIZE;

    return page + byte_offset;
}

#endif