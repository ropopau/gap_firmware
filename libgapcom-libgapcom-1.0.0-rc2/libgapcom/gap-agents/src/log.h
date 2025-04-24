#ifndef LOG_H
#define LOG_H



#include <stdio.h>



#ifndef GAP_AGENTS_EOL
#pragma error GAP_AGENTS_EOL must be defined to give End-Of-Line termination character set
#endif
#define log(Format, ...)        printf(Format GAP_AGENTS_EOL __VA_OPT__(,) __VA_ARGS__)
#define log_cli(Format, ...)    log("[GAPCLI] " Format, __VA_ARGS__)



#endif /* LOG_H */
