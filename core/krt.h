#ifndef _KRT_H_
#define _KRT_H_

// check is sh3 of fx-9860 SDK
#ifdef _SH3
#define fx9860
#endif

#ifdef fx9860
#   include "../core/kommon.h"
#else
#   include "kommon.h"
#endif

typedef enum {
    KBRE_NO_ERROR,
    KBRE_OPERAND_TYPE_MISMATCH,
    KBRE_STACK_OVERFLOW,
    KBRE_INVALID_IMAGE_INDEX,
    KBRE_UNKNOWN_BUILT_IN_FUNC,
    KBRE_UNKNOWN_CMD
} KB_RT_ERROR_TYPE;

typedef enum {
    RVT_NIL = 0,
    RVT_NUMBER,
    RVT_STRING
} KB_RT_VALUE_TYPE;

typedef struct {
    int type;
    union {
        char *sz;
        KB_FLOAT num;
    } data;
} kb_rt_value;

void rtvalue_destroy(kb_rt_value * val);

typedef struct {
    KLOCK_WATCHFACE_HEADER *header; // pointer of raw
    vlist_t*                stack; // k_rt_value
    kb_op_command_t *       cmd_ptr;
    unsigned char *         raw;
    kb_rt_value **          variables;

    // graph api provider
    int (*graph_clr)();
    int (*graph_set_pixel)(int x, int y, int dot);
    int (*graph_plot_line)(int x1, int y1, int x2, int y2, int dot);
    int (*graph_draw_image)(int x, int y, int w, int h, int rev, const unsigned char *raw);
} kb_machine_t;

typedef struct {
    int type;
    int cmd_op;
    char message[KB_ERROR_MESSAGE_MAX];
} kb_runtime_error_t;

kb_machine_t *  machine_create              (unsigned char * raw);
void            machine_command_reset       (kb_machine_t* machine);
int             machine_exec                (kb_machine_t* machine, kb_runtime_error_t *error_ret);
void            machine_destroy             (kb_machine_t* machine);
int             machine_var_assign_num      (kb_machine_t* machine, int var_index, KB_FLOAT num);
void            format_exec_error           (const kb_runtime_error_t *error_ret, char *message, int message_length);

#endif