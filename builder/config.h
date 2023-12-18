#ifndef _CONFIG_H_
#define _CONFIG_H_

typedef struct {
    int build_mode;
    int preview_mode;
    const char * in_filename;
    const char * out_filename;
    int use_debug;
} BUILDER_CONFIG;

extern BUILDER_CONFIG builder_config;

#endif