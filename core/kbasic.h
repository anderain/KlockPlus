#ifndef _KB_H_
#define _KB_H_

#ifdef _SH3
#define IS_BIG_ENDIAN
#endif

typedef unsigned long K_DWORD;
typedef unsigned char K_BYTE;

#define KB_FLOAT                        float
#define KB_ERROR_MESSAGE_MAX            200
#define KB_TOKEN_LENGTH_MAX             50
#define KB_CONTEXT_VAR_MAX              32
#define KB_CONTEXT_STRING_BUFFER_MAX    300

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
} KB_ERROR;

typedef struct tagKB_error_t {
    int error_type;
    int error_pos;
    char message[200];
} kb_error_t;

// Node of List
typedef struct tag_vlist_node_t {
    struct tag_vlist_node_t *prev, *next;
    void *data;
} vlist_node_t;

// List
typedef struct {
    vlist_node_t *head, *tail;
    int size;
} vlist_t;

// List API
vlist_t*    vl_new_list     ();
vlist_t*    vl_push_back    (vlist_t* _self, void *data);
void*       vl_pop_front    (vlist_t* _self);
void*       vl_pop_back     (vlist_t* _self);
void        vl_destroy      (vlist_t* _self, void (* releaseData)(void *));

// Queue API
typedef vlist_t vqueue_t;
#define vq_new_queue                vl_new_list
#define vq_push                     vl_push_back
#define vq_pop                      vl_pop_front
#define vq_empty(q)                 ((q)->size <= 0)
#define vq_destroy(q, releaseData)  vl_destroy(q, releaseData)

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
    const char *asset_root_path;
} kb_context_t;

kb_context_t*   context_create();
void            context_destroy (kb_context_t *context);
int             context_set_variable(kb_context_t *context, const char * var_name);

typedef struct {
    unsigned long   op;
    union {
        KB_FLOAT        number;
        unsigned long   index;
        void *          ptr;
    } param;
} kb_op_command_t;

#define op_command_free free

typedef enum {
    KBO_NUL = 0,
    KBO_PUSH_VAR,       // push / pop actions
    KBO_PUSH_NUM,
    KBO_PUSH_STR,
    KBO_POP,
    KBO_OPR_NEG,        // operators
    KBO_OPR_CONCAT,
    KBO_OPR_ADD,
    KBO_OPR_SUB,
    KBO_OPR_MUL,
    KBO_OPR_DIV,
    KBO_OPR_INTDIV,
    KBO_OPR_MOD,
    KBO_OPR_NOT,
    KBO_OPR_AND,
    KBO_OPR_OR,
    KBO_OPR_EQUAL,
    KBO_OPR_NEQ,
    KBO_OPR_GT,
    KBO_OPR_LT,
    KBO_OPR_GTEQ,
    KBO_OPR_LTEQ,
    KBO_CALL_BUILT_IN,  // call built-in function
    KBO_ASSIGN_VAR,     // assign to variable
    KBO_GOTO,           // goto commands
    KBO_IFGOTO,
    KBO_STOP            // stop command
} KB_OPCODE;

int kb_format_error(const kb_error_t *error_val, char *str_buffer, int str_length_max);

kb_context_t *  kb_compile_start    ();
int             kb_scan_label       (const char * line, kb_context_t *context, kb_error_t *error_ret);
int             kb_compile_line     (const char * line_content, kb_context_t *context, kb_error_t *p_error_ret);
int             kb_compile_end      (kb_context_t *context);
int             kb_serialize        (const kb_context_t* context, unsigned char ** p_raw, int * p_byte_length);

void DBG_print_context_command_list (const kb_context_t *context);
void DBG_print_context_command_list (const kb_context_t *context);
void DBG_print_context_variables    (const kb_context_t *context);
void DBG_print_context_list_text    (const kb_context_t *context);
void DBG_print_context_list_image   (const kb_context_t *context);
void DBG_print_context_list_label   (const kb_context_t *context);


#define HEADER_MAGIC_FLAG 0x424B

typedef struct {
    K_DWORD start;
    K_DWORD width;
    K_DWORD height;
} KLOCK_IMAGE_META;

typedef struct {
    // file header
    K_DWORD header_magic;

    // number of variables
    K_DWORD variables_num;

    // cmd block def
    K_DWORD cmd_block_start;        // cmd start position
    K_DWORD cmd_num;                // number of cmd

    // string block def
    K_DWORD string_block_start;     // string start position
    K_DWORD string_block_length;    // length of string block

    // image block def
    K_DWORD image_block_start;      // image start position
    K_DWORD image_num;              // number of images
    KLOCK_IMAGE_META image_meta[0]; // metadata
} KLOCK_WATCHFACE_HEADER;

void DBG_print_header(KLOCK_WATCHFACE_HEADER *);

#endif
