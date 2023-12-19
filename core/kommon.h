#ifndef _KOMMON_H_
#define _KOMMON_H_

#if defined(_SH3) || defined(_SH4)
#   ifndef fx9860
#       define fx9860
#   endif
#endif

typedef unsigned long K_DWORD;
typedef unsigned char K_BYTE;

#define KB_FLOAT                        float
#define KB_ERROR_MESSAGE_MAX            200
#define KB_TOKEN_LENGTH_MAX             50
#define KB_CONTEXT_VAR_MAX              32
#define KB_CONTEXT_STRING_BUFFER_MAX    300


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

// Command

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

extern const char *_KOCODE_NAME[];

// Klock watchface header

#define HEADER_MAGIC_FLAG 0x424B

typedef struct {
    K_DWORD start;
    K_DWORD width;
    K_DWORD height;
} KLOCK_IMAGE_META;

#define KB_IMG_GET_PITCH(w)         ((w) / 8 + ((w) % 8 ? 1 : 0))
#define KB_IMG_GET_BYTE_SIZE(w, h)  (KB_IMG_GET_PITCH(w) * (h))

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
#ifdef fx9860
    KLOCK_IMAGE_META image_meta[1]; // metadata, length should be 0, but
									// will not be accepted by fx-9860 SDK,
									// so set length to 1 and make it happy
#else
    KLOCK_IMAGE_META image_meta[0];
#endif
} KLOCK_WATCHFACE_HEADER;

void DBG_print_header (KLOCK_WATCHFACE_HEADER *);

#define endian_swap_dword(l) (unsigned long)((((l) & 0x000000ff) << 24) | (((l) & 0x0000ff00) << 8) | (((l) & 0xff000000) >> 24) | (((l) & 0x00ff0000) >> 8))

#endif