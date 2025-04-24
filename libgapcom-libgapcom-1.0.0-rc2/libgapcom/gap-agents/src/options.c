#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "options.h"



/***************************
 *** Private definitions ***
 ***************************/

static bool options_is_option(const char* str)
{
    return strlen(str) > 1 && (str[0] == '-');
}

/**************************
 *** Public definitions ***
 **************************/

options_t* options_parse_cmdline(int* argc, const char** argv[])
{
    options_t* options;
    const char* opt_str;
    const char* opt_val_str;

    options = calloc(1, sizeof(*options));
    if (!options)
        return NULL;

    while (*argc >= 2 && options_is_option((*argv)[0]))
    {
        opt_str = (*argv)[0];
        opt_val_str = (*argv)[1];

        if (strcmp(opt_str, "-com") == 0)
            strcpy(options->com_node_path, opt_val_str);
        else
        {
            free(options);
            return NULL;
        }

        *argc -= 2;
        *argv += 2;
    }
    /* Having no options is not an error. The options_t is designed
     * so that zeroed-out values make sense for each option.
     */
    return options;
}

void options_destroy_cmdline(options_t* options)
{
    if (options)
        free(options);
}
