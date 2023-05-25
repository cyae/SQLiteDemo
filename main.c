#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "core/interface/REPL.h"
#include "core/virtual_machine/meta.h"
#include "core/virtual_machine/executor.h"
#include "core/compiler/compiler.h"
#include "backend/indexing.h"

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Must supply a database filename.\n");
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    Table *table = db_open(filename);

    InputBuffer *input_buffer = new_input_buffer();

    while (true)
    {
        print_prompt();
        read_input(input_buffer);

        // Non-SQL meta commands starts with a dot
        if (input_buffer->buffer[0] == '.')
        {
            switch (do_meta_command(input_buffer, table))
            {
            case META_COMMAND_SUCCESS:
                continue;
            case META_COMMAND_UNRECOGNIZED_COMMAND:
                printf("Unrecognized command '%s'.\n", input_buffer->buffer);
                continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement))
        {
        case PREPARE_SUCCESS:
            break;
        case PREPARE_NEGATIVE_ID:
            printf("ID must be positive.\n");
            continue;
        case PREPARE_STRING_TOO_LONG:
            printf("String is too long.\n");
            continue;
        case PREPARE_SYNTAX_ERROR:
            printf("Syntax error. Could not parse statement.\n");
            continue;
        case PREPARE_UNRECOGNIZED_STATEMENT:
            printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
            continue;
        }

        switch (execute_statement(&statement, table))
        {
        case EXECUTE_SUCCESS:
            printf("Executed.\n");
            break;
        case EXECUTE_TABLE_FULL:
            printf("Error: Table Full.\n");
            break;
        }
    }

    return 0;
}
