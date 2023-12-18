#ifndef _KB_H_
#define _KB_H_

#include "kommon.h"

typedef enum {
    KBE_NO_ERROR = 0,
    KBE_SYNTAX_ERROR,
    KBE_UNDEFINED_IDENTIFIER,
    KBE_UNDEFINED_FUNC,
    KBE_INVALID_NUM_ARGS,
    KBE_STRING_NO_SPACE,
    KBE_TOO_MANY_VAR,
    KBE_DUPLICATED_LABEL,
    KBE_UNDEFINED_LABEL,
    KBE_OTHER
} KB_BUILD_ERROR;

typedef struct tagkb_build_error_t {
    int error_type;
    int error_pos;
    char message[200];
} kb_build_error_t;

typedef struct {
    K_DWORD     width;
    K_DWORD     height;
    K_DWORD     size;
    K_BYTE      data[0];
} kb_image_t;

typedef struct {
    K_DWORD     pos;
    char        name[0];
} kb_label_t;

typedef struct {
    char        var_list[KB_CONTEXT_VAR_MAX][KB_TOKEN_LENGTH_MAX];
    int         num_var;
    char        string_buffer[KB_CONTEXT_STRING_BUFFER_MAX];
    char    *   string_buffer_ptr;
    vlist_t *   command_list;   // kb_op_command_t
    vlist_t *   image_list;     // kb_image_t
    vlist_t *   label_list;     // kb_label_t
    // for assets loader
    int (*bmp_load_convert)(const char* filename, int* p_width,int* p_height, unsigned char** p_raw);
} kb_context_t;

kb_context_t*   context_create();
void            context_destroy (kb_context_t *context);
int             context_set_variable(kb_context_t *context, const char * var_name);

int kb_format_build_error(const kb_build_error_t *error_val, char *str_buffer, int str_length_max);

kb_context_t *  kb_compile_start    ();
int             kb_scan_label       (const char * line, kb_context_t *context, kb_build_error_t *error_ret);
int             kb_compile_line     (const char * line_content, kb_context_t *context, kb_build_error_t *p_error_ret);
int             kb_compile_end      (kb_context_t *context);
int             kb_serialize        (const kb_context_t* context, unsigned char ** p_raw, int * p_byte_length);

void DBG_print_context_command_list (const kb_context_t *context);
void DBG_print_context_command_list (const kb_context_t *context);
void DBG_print_context_variables    (const kb_context_t *context);
void DBG_print_context_list_text    (const kb_context_t *context);
void DBG_print_context_list_image   (const kb_context_t *context);
void DBG_print_context_list_label   (const kb_context_t *context);

#endif
