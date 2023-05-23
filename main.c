#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "REPL.h"

int main(int argc, char const *argv[])
{
    InputBuffer *input_buffer = new_input_buffer();
    while (true)
    {
        print_prompt();
        read_input(input_buffer);

        if (strcmp(input_buffer->buffer, ".exit") == 0)
        {
            close_input_buffer(input_buffer);
            exit(EXIT_SUCCESS);
        }
        else
        {
            printf("Unrecognized commaind '%s'.\n", input_buffer->buffer);
        }
    }

    return 0;
}
