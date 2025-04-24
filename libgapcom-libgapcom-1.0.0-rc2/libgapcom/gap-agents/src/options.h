#ifndef OPTIONS_H
#define OPTIONS_H



#define OPT_PATH_MAX_LEN 256

typedef struct options_t
{
    char com_node_path[OPT_PATH_MAX_LEN];
} options_t;

options_t* options_parse_cmdline(int* argc, const char** argv[]);
void options_destroy_cmdline(options_t* options);



#endif /* OPTIONS_H */
