#ifndef PAGER_H_
#define PAGER_H_

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

const __uint32_t PAGE_SIZE = 4096;
#define TABLE_MAX_PAGES 100

typedef struct
{
    int file_descriptor;
    __uint32_t file_length;
    void *pages[TABLE_MAX_PAGES];
} Pager;

Pager *pager_open(const char *filename)
{
    int fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    if (fd == -1)
    {
        printf("Unable to open file.\n");
        exit(EXIT_FAILURE);
    }

    off_t file_length = lseek(fd, 0, SEEK_END);

    Pager *pager = (Pager *)malloc(sizeof(Pager));
    pager->file_descriptor = fd;
    pager->file_length = file_length;

    for (__uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        pager->pages[i] = NULL;
    }

    return pager;
}

void pager_flush(Pager *pager, __uint32_t page_num, __uint32_t size)
{
    if (pager->pages[page_num] == NULL)
    {
        printf("Tried to flush null page.\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
    if (offset == -1)
    {
        printf("Error seeking: %d.\n", errno);
        exit(EXIT_FAILURE);
    }

    __ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], size);
    if (bytes_written == -1)
    {
        printf("Error writing: %d.\n", errno);
        exit(EXIT_FAILURE);
    }
}

void *get_page(Pager *pager, __uint32_t page_num)
{
    if (page_num > TABLE_MAX_PAGES)
    {
        printf("Tried to fetch page number out of bounds. %d > %d.\n", page_num, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[page_num] == NULL)
    {
        // cache miss, allocate memory and load from file.
        void *page = malloc(PAGE_SIZE);
        __uint32_t num_pages = pager->file_length / PAGE_SIZE;

        // we might save a partial page at the end of file
        if (pager->file_length % PAGE_SIZE)
        {
            num_pages += 1;
        }

        if (page_num <= num_pages)
        {
            lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
            __ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
            if (bytes_read == -1)
            {
                printf("Error reading file: %d.\n", errno);
                exit(EXIT_FAILURE);
            }
        }

        pager->pages[page_num] = page;
    }

    return pager->pages[page_num];
}

#endif