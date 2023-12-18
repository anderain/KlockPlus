#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "kbasic.h"
#include "kasset.h"
#include "utils.h"

// keywords
typedef struct {
    const char *word;
} KEYWORD_DEFINITION;

static const KEYWORD_DEFINITION KEYWORDS[] = {
    { "let"     },
    { "goto"    },
    { "if"      },
    { "image"   },
    { NULL      }
};

// function: defined num of parameters
typedef struct tagFUNC_DEFINITION {
    const char *func_name;
    int func_argc;
} FUNC_DEFINITION;

static const FUNC_DEFINITION FUNC_DEFINITION_LIST[] = {
    // batch one
    { "sin",        1 },    //  0 num
    { "cos",        1 },    //  1 num
    { "tan",        1 },    //  2 num
    { "sqrt",       1 },    //  3 num
    { "exp",        1 },    //  4 num
    { "abs",        1 },    //  5 num
    { "log",        1 },    //  6 num
    { "rand",       0 },    //  7
    { "clr",        0 },    //  8
    { "zeropad",    2 },    //  9 num, digit
    { "printmini",  4 },    // 10 x, y, str, rev
    { "print",      4 },    // 11 x, y, str, rev
    { "gprint",     4 },    // 12 x, y, str, image_index
    { "line",       5 },    // 13 x1, y1, x2, y2, dot
    { "pixel",      3 },    // 14 x, y ,dot
    { "blt",        3 },    // 15 x, y, image_index
    { "p",          1 }     // 16 value
};

#define is_digit(c) ((c) >= '0' && (c) <= '9')
#define is_alpha(c) ((c) >= 'a' && (c) <= 'z' || (c) >= 'A' && (c) <= 'Z' || (c) == '_')
#define is_alnum(c) ((is_digit(c)) || (is_alpha(c)))
#define is_space(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')

typedef enum {
    TOKEN_ERR, TOKEN_END, TOKEN_NUM,
    TOKEN_ID, TOKEN_OPR, TOKEN_FNC,
    TOKEN_BKT, TOKEN_CMA, TOKEN_STR,
    TOKEN_LABEL, TOKEN_KEY,
    TOKEN_UDF
} token_type_t;

const char *DBG_TOKEN_NAME[] = {
    "TOKEN_ERR", "TOKEN_END", "TOKEN_NUM",
    "TOKEN_ID", "TOKEN_OPR", "TOKEN_FNC",
    "TOKEN_BKT", "TOKEN_CMA", "TOKEN_STR",
    "TOKEN_LABEL",
    "TOKEN_UDF"
};

typedef struct tag_token_t {
    int type;
    int source_length;
    char content[KB_CONTEXT_STRING_BUFFER_MAX];
} token_t;

typedef struct tag_expr_node_t {
    int type;
    // 'param' is only for operator
    // to store OPERATOR 
    int param;
    char content[KB_CONTEXT_STRING_BUFFER_MAX];
    struct tag_expr_node_t **children;
    int child_num;
} expr_node_t;

typedef enum {
    OPR_NEG = 0,    // -
    OPR_CONCAT,     // &
    OPR_ADD,        // +
    OPR_SUB,        // -
    OPR_MUL,        // *
    OPR_DIV,        // /
    OPR_INTDIV,     // \  
    OPR_MOD,        // %
    OPR_NOT,        // !
    OPR_AND,        // &&
    OPR_OR,         // ||
    OPR_EQUAL,      // =
    OPR_NEQ,        // <>
    OPR_GT,         // >
    OPR_LT,         // <
    OPR_GTEQ,       // >=
    OPR_LTEQ        // <=
} operator_type_t;

const struct {
    char * opr_str;
    operator_type_t opr_type_val;
} OPERATOR_DEFINITION[] = {
    { "&",  OPR_CONCAT  },
    { "+",  OPR_ADD     },
    { "-",  OPR_SUB     },
    { "*",  OPR_MUL     },
    { "/",  OPR_DIV     },
    { "\\", OPR_INTDIV  },
    { "%",  OPR_MOD     },
    { "!",  OPR_NOT     },
    { "&&", OPR_AND     },
    { "||", OPR_OR      },
    { "=",  OPR_EQUAL   },
    { "<>",  OPR_NEQ    },
    { ">",  OPR_GT      },
    { "<",  OPR_LT      },
    { ">=", OPR_GTEQ    },
    { "<=", OPR_LTEQ    },
    // place at bottom, not in use
    { "-",  OPR_NEG     }
};

const int OPERATOR_DEFINITION_LENGTH = sizeof(OPERATOR_DEFINITION) / sizeof(OPERATOR_DEFINITION[0]);

const struct {
    char *opr_name;
    int priority;
} OPERATOR_META[] = {
    { "OPR_NEG",    500 },
    { "OPR_CONCAT", 90  },
    { "OPR_ADD",    100 },
    { "OPR_SUB",    100 },
    { "OPR_MUL",    200 },
    { "OPR_DIV",    200 },
    { "OPR_INTDIV", 150 },
    { "OPR_MOD",    200 },
    { "OPR_NOT",    50  },
    { "OPR_AND",    40  },
    { "OPR_OR",     30  },
    { "OPR_EQUAL",  50  },
    { "OPR_NEQ",    50  },
    { "OPR_GT",     60  },
    { "OPR_LT",     60  },
    { "OPR_GTEQ",   60  },
    { "OPR_LTEQ",   60  }
};

const char *_KOCODE_NAME[] = {
    "NUL",
    "PUSH_VAR",
    "PUSH_NUM",
    "PUSH_STR",
    "POP",
    "NEG",
    "CONCAT",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "INTDIV",
    "MOD",
    "NOT",
    "AND",
    "OR",
    "EQUAL",
    "NEQ",
    "GT",
    "LT",
    "GTEQ",
    "LTEQ",
    "CALL_BUILT_IN",
    "ASSIGN_VAR",
    "GOTO",
    "IFGOTO",
    "STOP"
};

#define EN_LCHILD(en) ((en)->children[0])
#define EN_RCHILD(en) ((en)->children[1])

token_t* set_token(token_t *token, token_type_t type, const char *content, int source_length) {
    token->type = type;
    STR_COPY(token->content, KB_CONTEXT_STRING_BUFFER_MAX, content);
    token->source_length = source_length;
    // printf("token: %s %s\n", token->content, DBG_TOKEN_NAME[token->type]);
    return token;
}

typedef struct tag_analyzer_t {
    const char *expr;
    const char *eptr;
    token_t token;
} analyzer_t;

vlist_node_t* vln_new_node(void *data) {
    vlist_node_t* n = (vlist_node_t *)malloc(sizeof(vlist_node_t));
    n->prev = n->next = NULL;
    n->data = data;
    return n;
}

vlist_t* vl_new_list() {
    vlist_t *l = (vlist_t *)malloc(sizeof(vlist_t));
    l->head = l->tail = NULL;
    l->size = 0;
    return l;
}

vlist_t* vl_push_back(vlist_t* _self, void *data) {
    vlist_node_t *new_node = vln_new_node(data);

    if (_self->head == NULL) {
        _self->head = _self->tail = new_node;
    }
    else {
        vlist_node_t * tail = _self->tail;
        tail->next = new_node;
        new_node->prev = tail;
        _self->tail = new_node;
    }

    _self->size++;

    return _self;
}

// void* vl_pop_front(vlist_t* _self) {
//     vlist_node_t *head;
//     void *data;

//     head = _self->head;

//     if (head == NULL) return NULL;

//     data = head->data;

//     _self->head = head->next;
//     if (_self->head) {
//         _self->head->prev = NULL;
//     }
//     else {
//         _self->tail = NULL;
//     }

//     free(head);
//     _self->size--;

//     return data;
// }

void* vl_pop_back(vlist_t* _self) {
    vlist_node_t *tail, *prev_tail; 
    void * data;

    if (_self->size <= 0) {
        return NULL;
    }

    tail = _self->tail;
    data = tail->data;

    if (_self->size == 1) {
        _self->head = NULL;
        _self->tail = NULL;
        free(tail);
        _self->size = 0;
        return data;
    }

    prev_tail = tail->prev;
    prev_tail->next = NULL;
    _self->tail = prev_tail;
    free(tail);
    _self->size--;

    return data;
}

void vln_destroy(vlist_node_t* vln, void (* releaseData)(void *)) {
    if (vln->data != NULL && releaseData) {
        releaseData(vln->data);
        vln->data = NULL;
    }
    free(vln);
}

void vl_destroy(vlist_t* _self, void (* releaseData)(void *)) {
    vlist_node_t *n1, *n2;

    n1 = _self->head;
    while (n1) {
        n2 = n1->next;
        vln_destroy(n1, releaseData);
        n1 = n2;
    }

    free(_self);
}

// --

token_t* next_token(analyzer_t *_self) {
    char first_char, second_char;
    char buffer[KB_TOKEN_LENGTH_MAX * 2];
    char *pbuffer = buffer;
    const char *eptr_start;

    while (is_space(*_self->eptr)) {
        _self->eptr++;
    }

    eptr_start = _self->eptr;

    first_char = *_self->eptr;

    // operator 
    switch (first_char) {
    case '+':   case '-':   case '*':   case '/':   case '^':
    case '%':   case '=':   case '!':   case '\\':
        buffer[0] = first_char;
        buffer[1] = '\0';
        _self->eptr++;
        return set_token(&_self->token, TOKEN_OPR, buffer, _self->eptr - eptr_start);
    case '(':   case ')':
        buffer[0] = first_char;
        buffer[1] = '\0';
        _self->eptr++;
        return set_token(&_self->token, TOKEN_BKT, buffer, _self->eptr - eptr_start);
    case ',':
        buffer[0] = first_char;
        buffer[1] = '\0';
        _self->eptr++;
        return set_token(&_self->token, TOKEN_CMA, buffer, _self->eptr - eptr_start);
    case ':':
        buffer[0] = first_char;
        buffer[1] = '\0';
        _self->eptr++;
        return set_token(&_self->token, TOKEN_LABEL, buffer, _self->eptr - eptr_start);
    case '>':
        second_char = *(_self->eptr + 1);
        if (second_char == '=') {
            buffer[0] = first_char;
            buffer[1] = second_char;
            buffer[2] = '\0';
            _self->eptr += 2;
            return set_token(&_self->token, TOKEN_OPR, buffer, _self->eptr - eptr_start);
        } else {
            buffer[0] = first_char;
            buffer[1] = '\0';
            _self->eptr++;
            return set_token(&_self->token, TOKEN_OPR, buffer, _self->eptr - eptr_start);
        }
    case '<':
        second_char = *(_self->eptr + 1);
        if (second_char == '=') {
            buffer[0] = first_char;
            buffer[1] = second_char;
            buffer[2] = '\0';
            _self->eptr += 2;
            return set_token(&_self->token, TOKEN_OPR, buffer, _self->eptr - eptr_start);
        } else if (second_char == '>') {
            buffer[0] = first_char;
            buffer[1] = second_char;
            buffer[2] = '\0';
            _self->eptr += 2;
            return set_token(&_self->token, TOKEN_OPR, buffer, _self->eptr - eptr_start);
        } else {
            buffer[0] = first_char;
            buffer[1] = '\0';
            _self->eptr++;
            return set_token(&_self->token, TOKEN_OPR, buffer, _self->eptr - eptr_start);
        }
    case '&':
        second_char = *(_self->eptr + 1);
        if (second_char == '&') {
            buffer[0] = first_char;
            buffer[1] = second_char;
            buffer[2] = '\0';
            _self->eptr += 2;
            return set_token(&_self->token, TOKEN_OPR, buffer, _self->eptr - eptr_start);
        } else {
            buffer[0] = first_char;
            buffer[1] = '\0';
            _self->eptr++;
            return set_token(&_self->token, TOKEN_OPR, buffer, _self->eptr - eptr_start);
        }
    case '|':
        second_char = *(_self->eptr + 1);
        if (second_char == '|') {
            buffer[0] = first_char;
            buffer[1] = second_char;
            buffer[2] = '\0';
            _self->eptr += 2;
            return set_token(&_self->token, TOKEN_OPR, buffer, _self->eptr - eptr_start);
        } else {
            buffer[0] = first_char;
            buffer[1] = '\0';
            _self->eptr++;
            return set_token(&_self->token, TOKEN_UDF, buffer, _self->eptr - eptr_start);
        }
    }

    // expr end here 
    if (first_char == '\0') {
        return set_token(&_self->token, TOKEN_END, "", 0);
    }
    // numberic
    else if (is_digit(first_char)) {
        while (is_digit(*_self->eptr)) {
            *pbuffer++ = *_self->eptr++;
        }
        // check if is float
        if (*_self->eptr == '.') {
            // write '.' to buffer
            *pbuffer++ = *_self->eptr++;
            // continue read digits
            while (is_digit(*_self->eptr)) {
                *pbuffer++ = *_self->eptr++;
            }
        }

        *pbuffer = '\0';

        // Token error: to long
        if ((pbuffer - buffer) >= KB_CONTEXT_STRING_BUFFER_MAX) {
            return set_token(&_self->token, TOKEN_ERR, "Number too long", _self->eptr - eptr_start);
        }

        return set_token(&_self->token, TOKEN_NUM, buffer, _self->eptr - eptr_start);
    }
    // identifier
    else if (is_alpha(first_char)) {
        int i;
        int is_func = 0;

        while (is_alnum(*_self->eptr)) {
            *pbuffer++ = *_self->eptr++;
        }
        *pbuffer = '\0';

        // check is keyword?
        for (i = 0; KEYWORDS[i].word; ++i) {
            if (STR_EQUAL(KEYWORDS[i].word, buffer)) {
                return set_token(&_self->token, TOKEN_ID, buffer, _self->eptr - eptr_start);
            }
        }
        
        while (is_space(*_self->eptr)) {
            *_self->eptr++;
        }

        // func
        if (*_self->eptr == '(') {
            _self->eptr++;
            is_func = 1;
        }
        

        // Token error: to long
        if ((pbuffer - buffer) >= KB_CONTEXT_STRING_BUFFER_MAX) {
            return set_token(&_self->token, TOKEN_ERR, "Identifier too long", _self->eptr - eptr_start);
        }

        if (is_func) {
            return set_token(&_self->token, TOKEN_FNC, buffer, _self->eptr - eptr_start);
        }
        else {
            return set_token(&_self->token, TOKEN_ID, buffer, _self->eptr - eptr_start);
        }
    }
    // string
    else if (first_char == '"') {
        // skip first '"'
        _self->eptr++;

        while (*_self->eptr != '"') {
            const char currentChar = *_self->eptr;

            // escape sequence
            if (currentChar == '\\') {
                const char nextChar = *++_self->eptr;
                // newline
                if (nextChar == 'n') {
                    *pbuffer++ = '\n';
                }
                // carriage return
                else if (nextChar == 'r') {
                    *pbuffer++ = '\r';
                }
                // tab
                else if (nextChar == 't') {
                    *pbuffer++ = '\t';
                }
                // "
                else if (nextChar == '"') {
                    *pbuffer++ = '\"';
                }
                else {
                    return set_token(&_self->token, TOKEN_ERR, "Invalid escape", _self->eptr - eptr_start);
                }
                _self->eptr++;
            }
            // incomplete
            else if (currentChar == '\0') {
                return set_token(&_self->token, TOKEN_ERR, "Incomplete string", _self->eptr - eptr_start);
            }
            else {
                *pbuffer++ = *_self->eptr++;
            }
        }
        
        // skip last '"'
        _self->eptr++;

        // Token error: to long
        if ((pbuffer - buffer) >= KB_CONTEXT_STRING_BUFFER_MAX) {
            return set_token(&_self->token, TOKEN_ERR, "String too long", _self->eptr - eptr_start);
        }

        *pbuffer = '\0';

        return set_token(&_self->token, TOKEN_STR, buffer, _self->eptr - eptr_start);
    }
    // undefined
    else {
        _self->eptr++;
        buffer[0] = first_char;
        buffer[1] = '\0';
        return set_token(&_self->token, TOKEN_UDF, buffer, _self->eptr - eptr_start);
    }
    return NULL;
}

void rewind_token(analyzer_t *_self) {
    _self->eptr -= _self->token.source_length;
}

void reset_token(analyzer_t *_self) {
    _self->eptr = _self->expr;
}

int match_expr(analyzer_t *_self);

int match_try_next(analyzer_t *_self) {
    token_t *next = next_token(_self);
    if (next->type == TOKEN_OPR) {
        return match_expr(_self);
    }
    else {
        rewind_token(_self);
    }
    return 1;
}

int match_expr(analyzer_t *_self) {
    token_t *token = next_token(_self);
    // printf("[%s] %s\n", DBG_TOKEN_NAME[token->type], token->content);
    if (token->type == TOKEN_ID || token->type == TOKEN_NUM || token->type == TOKEN_STR) {
        return match_try_next(_self);
    }
    else if ((token->type == TOKEN_OPR && STR_EQUAL(token->content, "-"))
            || (token->type == TOKEN_OPR && STR_EQUAL(token->content, "!"))) {
        return match_expr(_self) && match_try_next(_self);
    }
    else if (token->type == TOKEN_FNC) {
        token = next_token(_self);
        if (token->type == TOKEN_BKT && *token->content == ')') {
            // no arg?
        }
        else {
            // at least 1 arg
            rewind_token(_self);
            while (1) {
                int result = match_expr(_self);
                if (!result)
                    return 0;
                token = next_token(_self);
                if (token->type == TOKEN_CMA)
                    continue;
                else if (token->type == TOKEN_BKT && *token->content == ')')
                    break;
                else
                    return 0;
            }
        }
        return match_try_next(_self);
    }
    else if (token->type == TOKEN_BKT && STR_EQUAL(token->content, "(")) {
        int success = match_expr(_self);
        if (!success) return 0;
        token = next_token(_self);
        if (token->type == TOKEN_BKT && STR_EQUAL(token->content, ")")) {
            token_t *next = next_token(_self);
            if (next->type == TOKEN_OPR) {
                return match_expr(_self);
            }
            else {
                rewind_token(_self);
            }
            return 1;
        }
        return 0; //return match_try_next(_self);
    }
    // else {
    //  // Debug
    //  printf("Token = %s, %s\n", token->content, DBG_TOKEN_NAME[token->type]);
    // }
    return 0;
}

void display_syntax_error(analyzer_t *_self) {
    int i;
    rewind_token(_self);
    puts("[Syntax error]");
    puts(_self->expr);
    for (i = 0; i < _self->eptr - _self->expr; ++i) {
        printf(" ");
    }
    printf("^\n");
}

int check_expr(analyzer_t *_self, kb_error_t *error_ret) {
    int result = match_expr(_self);
    // syntax error
    if (!result) {
        // return error
        // display_syntax_error(_self);
        if (error_ret) {
            error_ret->error_type = KBE_SYNTAX_ERROR;
            error_ret->error_pos = _self->eptr - _self->expr;
            if (_self->token.type == TOKEN_ERR) {
                STR_COPY(error_ret->message, KB_CONTEXT_STRING_BUFFER_MAX, _self->token.content);
            }
        }
        return 0;
    }
    else {
        // checkout not end
        token_t *token = next_token(_self);
        if (token->type != TOKEN_END) {
            // return error
            // display_syntax_error(_self);
            if (error_ret) {
                error_ret->error_type = KBE_SYNTAX_ERROR;
                error_ret->error_pos = _self->eptr - _self->expr;
            }
            return 0;
        }
    }
    reset_token(_self);
    return 1;
}

expr_node_t * en_new(const int type, const char *content, int child_num) {
    int i;

    expr_node_t *node = (expr_node_t *)malloc(sizeof(expr_node_t));

    node->type = type;
    node->param = 0;

    if (type == TOKEN_OPR) {
        for (i = 0; i < OPERATOR_DEFINITION_LENGTH; ++i) {
            if (STR_EQUAL(content, OPERATOR_DEFINITION[i].opr_str)) {
                node->param = OPERATOR_DEFINITION[i].opr_type_val;
                break;
            }
        }
    }

    STR_COPY(node->content, KB_CONTEXT_STRING_BUFFER_MAX, content);

    node->child_num = child_num;
    node->children = (expr_node_t **)malloc(child_num * (sizeof(expr_node_t *)));
    for (i = 0; i < node->child_num; ++i) {
        node->children[i] = NULL;
    }

    return node;
}


expr_node_t * en_create_by_token(const token_t * token, int child_num) {
    return en_new(token->type, token->content, child_num);
}

expr_node_t * en_create_operator_negative(const token_t * token) {
    expr_node_t *new_node = en_new(token->type, token->content, 1);

    new_node->param = OPR_NEG;

    return new_node;
}

expr_node_t * en_create_operator_not(const token_t * token) {
    expr_node_t *new_node = en_new(token->type, token->content, 1);

    new_node->param = OPR_NOT;

    return new_node;
}

void en_destroy(expr_node_t *node) {
    int i;

    if (node == NULL) return;

    for (i = 0; i < node->child_num; ++i) {
        if (node->children[i]) free(node->children[i]);
    }
    free(node->children);

    free(node);
}

expr_node_t* build_expr_tree(analyzer_t *_self);

expr_node_t* build_try_next(analyzer_t *_self, expr_node_t *left_node) {
    token_t *token = next_token(_self);

    if (token->type == TOKEN_OPR) {
        expr_node_t *opr_node = en_create_by_token(token, 2);
        expr_node_t *right_node = build_expr_tree(_self);

        EN_LCHILD(opr_node) = left_node;
        EN_RCHILD(opr_node) = right_node;

        return opr_node;
    }
    else {
        rewind_token(_self);
        return left_node;
    }

    return NULL;
}

expr_node_t* build_expr_tree(analyzer_t *_self) {
    token_t *token;

    token = next_token(_self);
    // printf("token = %s,%s\n", DBG_TOKEN_NAME[token->type], token->content);

    if (token->type == TOKEN_ID || token->type == TOKEN_NUM || token->type == TOKEN_STR) {
        expr_node_t *left_node = en_create_by_token(token, 0);
        return build_try_next(_self, left_node);
    }
    else if (token->type == TOKEN_OPR && STR_EQUAL("-", token->content)) { // negative '-'
        expr_node_t* opr_node = en_create_operator_negative(token); // en_create_by_token(token, 1);
        EN_LCHILD(opr_node) = build_expr_tree(_self);
        return build_try_next(_self, opr_node);
    }
    else if (token->type == TOKEN_OPR && STR_EQUAL("!", token->content)) { // not '!'
        expr_node_t* opr_node = en_create_operator_not(token); 
        EN_LCHILD(opr_node) = build_expr_tree(_self);
        return build_try_next(_self, opr_node);
    }
    else if (token->type == TOKEN_FNC) {
        char *func_name = STR_DUMP(token->content);
        expr_node_t *func_node = NULL;
        expr_node_t *child_buf[100];
        int child_num = 0, i;

        token = next_token(_self);
        if (token->type == TOKEN_BKT && *token->content == ')') {
            // no arg
        } else {
            // at least 1 arg
            rewind_token(_self);
            while (1) {
                expr_node_t *child = build_expr_tree(_self);

                child_buf[child_num++] = child;

                token = next_token(_self);
                if (token->type == TOKEN_CMA) {
                    continue;
                }
                else if (token->type == TOKEN_BKT && *token->content == ')') {
                    break;
                }
            }
        }


        func_node = en_new(TOKEN_FNC, func_name, child_num);
        for (i = 0; i < child_num; ++i) {
            func_node->children[i] = child_buf[i];
        }
        free(func_name);

        return build_try_next(_self, func_node);
    }
    else if (token->type == TOKEN_BKT && *token->content == '(') {
        expr_node_t *bkt_node = en_new(TOKEN_BKT, "()", 1);
        bkt_node->children[0] = build_expr_tree(_self);
        next_token(_self); // ignore ')'

        return build_try_next(_self, bkt_node);
    }

    return NULL;
}

void travel_expr(expr_node_t *en, int tab) {
    int i;
    for (i = 0; i < tab; ++i) printf(" ");
    if (en->child_num > 0) {
        if (en->type == TOKEN_OPR) {
            printf("<en text=\"%s\" type=\"%s\" opr=\"%s\">\n", en->content, DBG_TOKEN_NAME[en->type], OPERATOR_META[en->param]);
        }
        else {
            printf("<en text=\"%s\" type=\"%s\">\n", en->content, DBG_TOKEN_NAME[en->type]);
        }

        for (i = 0; i < en->child_num; ++i) {
            travel_expr(en->children[i], tab + 2);
        }

        for (i = 0; i < tab; ++i) printf(" "); printf("</en>\n");
    }
    else {
        if (en->type == TOKEN_OPR) {
            printf("<en type=\"%s\" opr=\"%s\"> %s </en>\n", DBG_TOKEN_NAME[en->type], en->content, OPERATOR_META[en->param]);
        }
        else {
            printf("<en type=\"%s\"> %s </en>\n", DBG_TOKEN_NAME[en->type], en->content);
        }
    }
}

#define kb_format_error_append(new_part) (p += STR_COPY(p, str_length_max - (p - str_buffer), new_part))

int kb_format_error(const kb_error_t *error_val, char *str_buffer, int str_length_max) {
    char *p = str_buffer;

    if (error_val == NULL) {
        str_buffer[0] = 0;
        return 0;
    }
    
    if (error_val->error_type == KBE_NO_ERROR) {
        STR_COPY(str_buffer, str_length_max, "No error");
    }
    else if (error_val->error_type == KBE_SYNTAX_ERROR) {
        char buf[100];
        k_itoa(error_val->error_pos, buf, 10);
        kb_format_error_append("Syntax Error ");
        kb_format_error_append(buf);
        if (!STR_EQUAL("", error_val->message)) {
            kb_format_error_append(error_val->message);
        }
    }
    else if (error_val->error_type == KBE_UNDEFINED_IDENTIFIER) {
        kb_format_error_append("Undefined Identifier: ");
        kb_format_error_append(error_val->message);
    }
    else if (error_val->error_type == KBE_UNDEFINED_FUNC) {
        kb_format_error_append("Undefined Function: ");
        kb_format_error_append(error_val->message);
    }
    else if (error_val->error_type == KBE_INVALID_NUM_ARGS) {
        kb_format_error_append("Wrong number of arguments of function \"");
        kb_format_error_append(error_val->message);
        kb_format_error_append("\"");
    }
    else if (error_val->error_type == KBE_STRING_NO_SPACE) {
        kb_format_error_append("No enough space for string");
    }
    else if (error_val->error_type == KBE_TOO_MANY_VAR) {
        kb_format_error_append("Too many variables. Failed to declare \"");
        kb_format_error_append(error_val->message);
        kb_format_error_append("\"");
    }
    else if (error_val->error_type == KBE_DUPLICATED_LABEL) {
        kb_format_error_append("Duplicated label: \"");
        kb_format_error_append(error_val->message);
        kb_format_error_append("\"");
    }
    else if (error_val->error_type == KBE_UNDEFINED_LABEL) {
        kb_format_error_append("Undefined label: \"");
        kb_format_error_append(error_val->message);
        kb_format_error_append("\"");
    }
    else if (error_val->error_type == KBE_OTHER) {
        kb_format_error_append(error_val->message);
    }

    return 1;
}

expr_node_t* sort_expr_tree(expr_node_t *en, int *ptr_change_flag) {
    // printf("en=%s -> '%s' childs = %d\n", DBG_TOKEN_NAME[en->type], en->content, en->child_num);
    if (en == NULL) {
        return NULL;
    }
    else if (en->type == TOKEN_FNC || en->type == TOKEN_BKT) {
        int i;
        for (i = 0; i < en->child_num; ++i) {
            en->children[i] = sort_expr_tree(en->children[i], ptr_change_flag);
        }
        return en;
    }
    else if (en->type == TOKEN_OPR) {
        expr_node_t *enr, *enrl;
        int i;

        // EN_LCHILD(en) = sort_expr_tree(EN_LCHILD(en), ptr_change_flag);
        // EN_RCHILD(en) = sort_expr_tree(EN_RCHILD(en), ptr_change_flag);
        
        for (i = 0; i < en->child_num; ++i) {
            en->children[i] = sort_expr_tree(en->children[i], ptr_change_flag);
        }
        enr = en->children[en->child_num - 1];

        if (enr->type == TOKEN_OPR) {
            const int enr_priority = OPERATOR_META[enr->param].priority;
            const int en_priority = OPERATOR_META[en->param].priority;

            if (en_priority >= enr_priority) {
                *ptr_change_flag = 1;
            
                enrl = EN_LCHILD(enr);
                en->children[en->child_num - 1] = enrl;
                EN_LCHILD(enr) = en;

                // EN_RCHILD(enr) = sort_expr_tree(EN_RCHILD(enr), ptr_change_flag);
                return enr;
            }
        }
    }
    return en;
}

void sort_expr(expr_node_t ** ptr_en) {
    int change_flag;

    do {
        change_flag = 0;
        *ptr_en = sort_expr_tree(*ptr_en, &change_flag);
        // printf("change_flag: %d\n", change_flag);
    } while (change_flag);
}

kb_context_t* context_create() {
    kb_context_t* context = (kb_context_t *)malloc(sizeof(kb_context_t));

    context->num_var = 0;
    context->string_buffer_ptr = context->string_buffer;
    memset(context->string_buffer, 0, sizeof(context->string_buffer));

    context->command_list = vl_new_list();
    context->image_list = vl_new_list();
    context->label_list = vl_new_list();

    context->bmp_load_convert = NULL;

    return context;
}

void context_destroy (kb_context_t *context) {
    if (!context) {
        return;
    }
    vl_destroy(context->command_list, op_command_free);
    vl_destroy(context->image_list, free);
    vl_destroy(context->label_list, free);

    free(context);
}

int context_set_variable(kb_context_t *context, const char * var_name) {
    if (context->num_var >= KB_CONTEXT_VAR_MAX) {
        return -1;
    }

    STR_COPY(context->var_list[context->num_var], KB_CONTEXT_STRING_BUFFER_MAX, var_name);

    return context->num_var++;
}

int context_get_variable_index (const kb_context_t *context, const char * var_to_find) {
    int i;
    for (i = 0; i < context->num_var; ++i) {
        const char *var_name = context->var_list[i];
        if (STR_EQUAL(var_name, var_to_find)) {
            return i;
        }
    }
    return -1;
}

int context_append_text(kb_context_t *context, const char * string) {
    char *old_buffer_ptr = context->string_buffer_ptr;

    if (context->string_buffer_ptr - context->string_buffer + strlen(string) > KB_CONTEXT_STRING_BUFFER_MAX) {
        return -1;
    }

    while (*context->string_buffer_ptr++ = *string++);

    return old_buffer_ptr - context->string_buffer;
}

kb_label_t* context_get_label(const kb_context_t *context, const char * label_name_to_find) {
    vlist_node_t *node;

    for (node = context->label_list->head; node != NULL; node = node->next) {
        kb_label_t* label = (kb_label_t *)node->data;
        if (STR_EQUAL(label->name, label_name_to_find)) {
            return label;
        }
    }

    return NULL;
}

int context_add_label(const kb_context_t *context, const char * label_name_to_find) {
    kb_label_t* label = NULL;
    
    label = context_get_label(context, label_name_to_find);

    // already exist
    if (label) {
        return 0;
    }

    label = (kb_label_t *)malloc(sizeof(kb_label_t) + strlen(label_name_to_find) + 1);

    label->pos = 0;
    STR_COPY(label->name, KB_TOKEN_LENGTH_MAX, label_name_to_find);

    vl_push_back(context->label_list, label);

    return 1;
}

kb_op_command_t* op_command_create_built_function(int func_index) {
    kb_op_command_t* cmd = (kb_op_command_t*)malloc(sizeof(kb_op_command_t));

    cmd->op = KBO_CALL_BUILT_IN;
    cmd->param.index = func_index;

    return cmd;
}

kb_op_command_t* op_command_create_push_var(int var_index) {
    kb_op_command_t* cmd = (kb_op_command_t*)malloc(sizeof(kb_op_command_t));

    cmd->op = KBO_PUSH_VAR;
    cmd->param.index = var_index;

    return cmd;
}

kb_op_command_t* op_command_create_push_num(KB_FLOAT num) {
    kb_op_command_t* cmd = (kb_op_command_t*)malloc(sizeof(kb_op_command_t));

    cmd->op = KBO_PUSH_NUM;
    cmd->param.number = num;

    return cmd;
}

kb_op_command_t* op_command_create_push_str(int str_index) {
    kb_op_command_t* cmd = (kb_op_command_t*)malloc(sizeof(kb_op_command_t));

    cmd->op = KBO_PUSH_STR;
    cmd->param.index = str_index;

    return cmd;
}

kb_op_command_t* op_command_create_pop() {
    kb_op_command_t* cmd = (kb_op_command_t*)malloc(sizeof(kb_op_command_t));

    cmd->op = KBO_POP;
    cmd->param.index = 0;

    return cmd;
}

kb_op_command_t* op_command_create_assign_var(int var_index) {
    kb_op_command_t* cmd = (kb_op_command_t*)malloc(sizeof(kb_op_command_t));

    cmd->op = KBO_ASSIGN_VAR;
    cmd->param.index = var_index;

    return cmd;
}

kb_op_command_t* op_command_create_operator(operator_type_t opr_type) {
    kb_op_command_t* cmd = (kb_op_command_t*)malloc(sizeof(kb_op_command_t));

    cmd->op = opr_type + KBO_OPR_NEG;
    cmd->param.index = 0;

    return cmd;
}

kb_op_command_t* op_command_create_stop() {
    kb_op_command_t* cmd = (kb_op_command_t*)malloc(sizeof(kb_op_command_t));

    cmd->op = KBO_STOP;
    cmd->param.index = 0;

    return cmd;
}

kb_op_command_t* op_command_create_goto(kb_label_t *label) {
    kb_op_command_t* cmd = (kb_op_command_t*)malloc(sizeof(kb_op_command_t));

    cmd->op = KBO_GOTO;
    cmd->param.ptr = label;

    return cmd;
}

kb_op_command_t* op_command_create_ifgoto(kb_label_t *label) {
    kb_op_command_t* cmd = (kb_op_command_t*)malloc(sizeof(kb_op_command_t));

    cmd->op = KBO_IFGOTO;
    cmd->param.ptr = label;

    return cmd;
}

int compile_expr_tree(kb_context_t *context, expr_node_t *node, kb_error_t *error_ret) {
    int i;

    for (i = 0; i < node->child_num; ++i) {
        int ret;
        ret = compile_expr_tree(context, node->children[i], error_ret);
        if (!ret) return 0;
    }

    // function
    if (node->type == TOKEN_FNC) {
        int                   func_index = 0;
        const FUNC_DEFINITION *target_func = NULL;
        int                   func_length = sizeof(FUNC_DEFINITION_LIST) / sizeof(FUNC_DEFINITION_LIST[0]);
        
        for (i = 0; i < func_length; ++i) {
            const FUNC_DEFINITION *func = FUNC_DEFINITION_LIST + i;
            if (STR_EQUAL(func->func_name, node->content)) {
                func_index = i;
                target_func = func;
                break;
            }
        }

        // undefined function
        if (target_func == NULL) {
            error_ret->error_pos = 0;
            error_ret->error_type = KBE_UNDEFINED_FUNC;
            STR_COPY(error_ret->message, KB_ERROR_MESSAGE_MAX, node->content);
            return 0;
        }

        // invalid number of parameters
        if (target_func->func_argc != node->child_num) {
            error_ret->error_pos = 0;
            error_ret->error_type = KBE_INVALID_NUM_ARGS;
            STR_COPY(error_ret->message, KB_ERROR_MESSAGE_MAX, node->content);
            return 0;
        }

        vl_push_back(context->command_list, op_command_create_built_function(func_index));
    }
    // operator
    else if (node->type == TOKEN_OPR) {
        vl_push_back(context->command_list, op_command_create_operator(node->param));
    }
    // variable
    else if (node->type == TOKEN_ID) {
        int var_index = context_get_variable_index(context, node->content);

        // undefined variable
        if (var_index < 0) {
            error_ret->error_pos = 0;
            error_ret->error_type = KBE_UNDEFINED_IDENTIFIER;
            STR_COPY(error_ret->message, KB_ERROR_MESSAGE_MAX, node->content);
            return 0;
        }

        vl_push_back(context->command_list, op_command_create_push_var(var_index));
    }
    // string
    else if (node->type == TOKEN_STR) {
        int str_index = context_append_text(context, node->content);
        if (str_index < 0) {
            error_ret->error_pos = 0;
            error_ret->error_type = KBE_STRING_NO_SPACE;
            error_ret->message[0] = '\0';
            return 0;
        }
        vl_push_back(context->command_list, op_command_create_push_str(str_index));
    }
    // number
    else if (node->type == TOKEN_NUM) {
        KB_FLOAT num = (KB_FLOAT)k_atof(node->content);
        vl_push_back(context->command_list, op_command_create_push_num(num));
    }
    // bkt
    else if (node->type == TOKEN_BKT) {
        // do nothing here
    }
    else {
        // undefined variable
        error_ret->error_pos = 0;
        error_ret->error_type = KBE_OTHER;
        STR_COPY(error_ret->message, KB_ERROR_MESSAGE_MAX, "unknown express");
        return 0;
    }

    return 1;
}

void DBG_print_context_command(const kb_op_command_t *cmd) {
    printf("%-15s ", _KOCODE_NAME[cmd->op]);

    if (cmd->op == KBO_PUSH_VAR
     || cmd->op == KBO_PUSH_STR
     || cmd->op == KBO_CALL_BUILT_IN
     || cmd->op == KBO_ASSIGN_VAR
     || cmd->op == KBO_GOTO
     || cmd->op == KBO_IFGOTO) {
        printf("%d", cmd->param.index);
    }
    else if (cmd->op == KBO_PUSH_NUM) {
        char sz_num[KB_TOKEN_LENGTH_MAX];
        k_ftoa((float)cmd->param.number, sz_num, DEFAUL_FTOA_PRECISION);
        printf("%s", sz_num);
    }
    else if (cmd->op >= KBO_OPR_NEG && cmd->op <= KBO_OPR_LTEQ || cmd->op == KBO_POP || cmd->op == KBO_STOP) {
        // nothing here
    }
    else {
        printf("ERROR");
    }

    printf("\n");
}

void DBG_print_context_command_list (const kb_context_t *context) {
    vlist_node_t    *node       = context->command_list->head;
    int             line_num    = 0;

    while (node != NULL) {
        printf("%03d ", line_num);
        DBG_print_context_command((kb_op_command_t *)node->data);
        ++line_num;
        node = node->next;
    }
}

void DBG_print_context_list_text(const kb_context_t *context) {
    const char *ptr = context->string_buffer;
    int i = 0;

    while (ptr < context->string_buffer_ptr) {
        printf("String %2i : \"%s\"\n", i, ptr);
        ptr += strlen(ptr) + 1;
        ++i;
    }
}

void DBG_print_context_list_image(const kb_context_t *context) {
    vlist_node_t    *node       = context->image_list->head;
    int             line_num    = 0;

    while (node != NULL) {
        kb_image_t * image = (kb_image_t *)node->data;
        printf("Image %d: w = %d, h = %d, byte size = %d\n", line_num, image->width, image->height, image->size);
        
        if (image->width < 80) {
            const byte_width = image->size / image->height;
            unsigned int x, y;
            for (y = 0; y < image->height; ++y) {
                for (x = 0; x < image->width; ++x) {
                    const K_BYTE eight_pixel = *(image->data + y * byte_width + x / 8);
                    const int dot = (eight_pixel >> (7 - x % 8)) & 1;
                    printf(dot ? "*" : " ");
                }
                printf("\n");
            }
        }

        node = node->next;
        line_num++;
    }
}

void DBG_print_context_list_label(const kb_context_t *context) {
    vlist_node_t *node = context->label_list->head;

    while (node != NULL) {
        kb_label_t* label = (kb_label_t *)node->data;
        printf("%-15s: %03d\n", label->name, label->pos);
        node = node->next;
    }
}

void DBG_print_context_variables(const kb_context_t *context) {
    int i;
    for (i = 0; i < context->num_var; ++i) {
        printf("Variable %2d : %s\n", i, context->var_list[i]);
    }
}

kb_image_t* kb_image_create(int width, int height, K_BYTE *raw) {
    int byte_width = width / 8 + (width % 8 ? 1 : 0);
    int byte_size = byte_width * height;
    kb_image_t * kb_image;

    kb_image = (kb_image_t *)malloc(sizeof(kb_image_t) + byte_size);

    kb_image->width = width;
    kb_image->height = height;
    kb_image->size = byte_size;
    memcpy(kb_image->data, raw, byte_size);

    return kb_image;
}

// #define compile_line_return(ret) NULL;      \
//     if (line) { free(line); line = NULL; }  \
//    return (ret)

#define compile_line_return(ret) return (ret)

#define comiple_line_return_with_error(err_code, msg) NULL;     \
    error_ret->error_type = err_code;                           \
    error_ret->error_pos = analyzer.eptr - analyzer.expr;       \
    STR_COPY(error_ret->message, KB_ERROR_MESSAGE_MAX, msg);    \
    compile_line_return(0)

kb_context_t * kb_compile_start(const char * line) {
    kb_context_t * context;

    context = context_create();
    
    context_set_variable(context, "hh");
    context_set_variable(context, "mm");
    context_set_variable(context, "ss");
    context_set_variable(context, "ms");

    return context;
}

int kb_scan_label(const char * line, kb_context_t *context, kb_error_t *error_ret) {
    analyzer_t      analyzer;
    token_t         *token;
    int             ret;

    analyzer.expr = line;
    reset_token(&analyzer);

    error_ret->error_type = KBE_NO_ERROR;
    error_ret->message[0] = '\0';

    // try first token
    token = next_token(&analyzer);

    // :label_name
    if (token->type == TOKEN_LABEL) {
        char label_name[KB_TOKEN_LENGTH_MAX];

        token = next_token(&analyzer);
        if (token->type != TOKEN_ID) {
            comiple_line_return_with_error(KBE_SYNTAX_ERROR, ", invalid label");
        }

        STR_COPY(label_name, sizeof(label_name), token->content);

        // match end
        token = next_token(&analyzer);
        if (token->type != TOKEN_END) {
            comiple_line_return_with_error(KBE_SYNTAX_ERROR, " in label command");
        }

        ret = context_add_label(context, label_name);
        if (!ret) {
            comiple_line_return_with_error(KBE_DUPLICATED_LABEL, label_name);
        }
    }

    return 1;
}

int kb_compile_line(const char * line, kb_context_t *context, kb_error_t *error_ret) {
    analyzer_t      analyzer;
    expr_node_t     *expr_root;
    token_t         *token;
    int             ret;

    analyzer.expr = line;
    reset_token(&analyzer);

    error_ret->error_type = KBE_NO_ERROR;
    error_ret->message[0] = '\0';

    // try first token
    token = next_token(&analyzer);

    // empty
    if (token->type == TOKEN_END) {
        // nothing to do
    }
    // if command
    else if (token->type == TOKEN_ID && STR_EQUAL("if", token->content)) {
        char            label_name[KB_TOKEN_LENGTH_MAX];
        const char *    expr_start_eptr;
        kb_label_t *    label;

        // save express pos
        expr_start_eptr = analyzer.eptr;

        ret = match_expr(&analyzer);
        if (!ret) {
            comiple_line_return_with_error(KBE_SYNTAX_ERROR, ", invalid express in if statement");
        }

        // match 'goto'
        token = next_token(&analyzer);
        if (!(token->type == TOKEN_ID && STR_EQUAL("goto", token->content))) {
            comiple_line_return_with_error(KBE_SYNTAX_ERROR, " missing 'goto' in if statement");
        }

        // match label
        token = next_token(&analyzer);
        if (token->type != TOKEN_ID) {
            comiple_line_return_with_error(KBE_SYNTAX_ERROR, " missing label in if statement");
        }

        STR_COPY(label_name, sizeof(label_name), token->content);

        // match end
        token = next_token(&analyzer);
        if (token->type != TOKEN_END) {
            comiple_line_return_with_error(KBE_SYNTAX_ERROR, " in if statement");
        }

        // reset and compile expr
        analyzer.eptr = expr_start_eptr;
        expr_root = build_expr_tree(&analyzer);
        sort_expr(&expr_root);
        // printf("if goto label %s:", label_name); travel_expr(expr_root, 0);
        
        // compile expr
        ret = compile_expr_tree(context, expr_root, error_ret);
        if (!ret) {
            compile_line_return(0);
        }

        en_destroy(expr_root);

        // find label
        label = context_get_label(context, label_name);
        if (!label) {
            comiple_line_return_with_error(KBE_UNDEFINED_LABEL, label_name);
        }

        vl_push_back(context->command_list, op_command_create_ifgoto(label));
    }
    // image command
    else if (token->type == TOKEN_ID && STR_EQUAL("image", token->content)) {
        char        filename[KB_CONTEXT_STRING_BUFFER_MAX];
        int         load_ret;
        int         width, height;
        K_BYTE*     raw;
        kb_image_t* image;

        // get filename of image
        token = next_token(&analyzer);
        if (token->type != TOKEN_STR) {
            comiple_line_return_with_error(KBE_SYNTAX_ERROR, " in image command, missing filename");
        }

        STR_COPY(filename, sizeof(filename), token->content);

        // match end
        token = next_token(&analyzer);
        if (token->type != TOKEN_END) {
            comiple_line_return_with_error(KBE_SYNTAX_ERROR, " in image command, too many arguments");
        }

        load_ret = context->bmp_load_convert(filename, &width, &height, &raw);

        if (load_ret != KBMPE_NO_ERROR) {
            char error_buf[KB_ERROR_MESSAGE_MAX];
            char *p = error_buf;

            if (load_ret == KBMPE_FAILED_LOAD) {
                p += STR_COPY(p, sizeof(error_buf) - (p - error_buf), "Failed to open '");
                p += STR_COPY(p, sizeof(error_buf) - (p - error_buf), filename);
                p += STR_COPY(p, sizeof(error_buf) - (p - error_buf), "'");
            }
            else if (load_ret == KBMPE_INVALID_DEPTH) {
                p += STR_COPY(p, sizeof(error_buf) - (p - error_buf), "The color depth of '");
                p += STR_COPY(p, sizeof(error_buf) - (p - error_buf), filename);
                p += STR_COPY(p, sizeof(error_buf) - (p - error_buf), "' is not 24");
            }
            else if (load_ret == KBMPE_TOO_LARGE) {
                p += STR_COPY(p, sizeof(error_buf) - (p - error_buf), "The width of '");
                p += STR_COPY(p, sizeof(error_buf) - (p - error_buf), filename);
                p += STR_COPY(p, sizeof(error_buf) - (p - error_buf), "' exceeds 128 pixels");
            }
            else {
                p += STR_COPY(p, sizeof(error_buf) - (p - error_buf), "Other problems occurred while reading file: ");
                p += STR_COPY(p, sizeof(error_buf) - (p - error_buf), filename);
            }
            comiple_line_return_with_error(KBE_OTHER, error_buf);
        }

        image = kb_image_create(width, height, raw);
        vl_push_back(context->image_list, image);

        if (raw) free(raw);
    }
    // let statement
    else if (token->type == TOKEN_ID && STR_EQUAL("let", token->content)) {
        char        var_name[KB_TOKEN_LENGTH_MAX];
        const char  *expr_start_eptr;
        int         var_index;

        // get name of variable
        token = next_token(&analyzer);
        if (token->type != TOKEN_ID) {
            comiple_line_return_with_error(KBE_SYNTAX_ERROR, " in let statement, missing variable");
        }
        STR_COPY(var_name, sizeof(var_name), token->content);

        // match '='
        token = next_token(&analyzer);
        if (!(token->type == TOKEN_OPR && STR_EQUAL("=", token->content))) {
            comiple_line_return_with_error(KBE_SYNTAX_ERROR, " in let statement, require '='");
        }

        var_index = context_get_variable_index(context, var_name);
        // create var
        if (var_index < 0) {
            var_index = context_set_variable(context, var_name);
            if (var_index < 0) {
                comiple_line_return_with_error(KBE_TOO_MANY_VAR, var_name);
            }
        }

        // remain part is expr
        expr_start_eptr = analyzer.eptr;

        if (!check_expr(&analyzer, error_ret)) {
            compile_line_return(0);
        }

        analyzer.eptr = expr_start_eptr;
        expr_root = build_expr_tree(&analyzer);
        sort_expr(&expr_root);
        // printf("let: '%s' = ", var_name); travel_expr(expr_root, 0);
        
        // compile expr
        ret = compile_expr_tree(context, expr_root, error_ret);
        if (!ret) {
            compile_line_return(0);
        }

        en_destroy(expr_root);

        // add assign command
        vl_push_back(context->command_list, op_command_create_assign_var(var_index));
    }
    // goto command
    else if (token->type == TOKEN_ID && STR_EQUAL("goto", token->content)) {
        char            label_name[KB_TOKEN_LENGTH_MAX];
        kb_label_t *    label;

        // match label
        token = next_token(&analyzer);
        if (token->type != TOKEN_ID) {
            comiple_line_return_with_error(KBE_SYNTAX_ERROR, " missing label in goto statement");
        }

        STR_COPY(label_name, sizeof(label_name), token->content);

        // match end
        token = next_token(&analyzer);
        if (token->type != TOKEN_END) {
            comiple_line_return_with_error(KBE_SYNTAX_ERROR, " in goto statement");
        }

        // find label
        label = context_get_label(context, label_name);
        if (!label) {
            comiple_line_return_with_error(KBE_UNDEFINED_LABEL, label_name);
        }

        vl_push_back(context->command_list, op_command_create_goto(label));
    }
    // label mark, update position
    else if (token->type == TOKEN_LABEL) {
        const char *label_name;
        kb_label_t *label;

        token = next_token(&analyzer);
        if (token->type != TOKEN_ID) {
            comiple_line_return_with_error(KBE_SYNTAX_ERROR, ", invalid label");
        }

        label_name = token->content;

        // update position of label
        label = context_get_label(context, label_name);

        if (!label) {
            comiple_line_return_with_error(KBE_UNDEFINED_LABEL, label_name);
        }

        label->pos = context->command_list->size;
    }
    else {
        rewind_token(&analyzer);

        if (!check_expr(&analyzer, error_ret)) {
            compile_line_return(0);
        }

        expr_root = build_expr_tree(&analyzer);
        sort_expr(&expr_root);
        // printf("expr:"); travel_expr(expr_root, 0);
        
        // compile expr
        ret = compile_expr_tree(context, expr_root, error_ret);
        if (!ret) {
            compile_line_return(0);
        }

        en_destroy(expr_root);

        // pop no use value
        vl_push_back(context->command_list, op_command_create_pop());
    }

    compile_line_return(1);
}

int kb_compile_end(kb_context_t *context) {
    vlist_node_t *node;

    // update position of goto / ifgoto
    for (node = context->command_list->head; node != NULL; node = node->next) { 
        kb_op_command_t *cmd = (kb_op_command_t *)node->data;
        
        if (cmd->op == KBO_IFGOTO || cmd->op == KBO_GOTO) {
            kb_label_t *label = (kb_label_t *)cmd->param.ptr;
            cmd->param.index = label->pos;
        }
    }

    // add assign command
    vl_push_back(context->command_list, op_command_create_stop());
    return 1;
}

int kb_serialize(
    const           kb_context_t* context,
    unsigned char** p_raw,
    int *           p_byte_length
) {
    unsigned char * entire_raw;
    int             byte_length_header;
    int             byte_length_cmd;
    int             num_cmd;
    int             byte_length_string;
    int             num_image;
    int             byte_length_image;
    int             byte_length_entire;
    //
    KLOCK_WATCHFACE_HEADER* p_header;
    kb_op_command_t *       p_cmd;
    char *                  p_string;
    //
    vlist_node_t*       node;
    int                 i, pos;

    // ---------- layout ----------
    //
    //          * header *
    //
    // ----------------------------
    //
    //           * cmd *
    //
    // ----------------------------
    //
    //          * string *
    //
    // ----------------------------
    //
    //          * image *
    //
    // ----------------------------

    // calculate length

    num_image           = context->image_list->size;
    byte_length_header  = sizeof(KLOCK_WATCHFACE_HEADER) + num_image * sizeof(KLOCK_IMAGE_META);
    num_cmd             = context->command_list->size;
    byte_length_cmd     = sizeof(kb_op_command_t) * num_cmd;
    byte_length_string  = context->string_buffer_ptr - context->string_buffer;
    byte_length_image   = 0;;

    for (node = context->image_list->head; node != NULL; node = node->next) {
        kb_image_t* image = (kb_image_t *)node->data;
        byte_length_image += image->size;
    }

    byte_length_entire  = byte_length_header
                        + byte_length_cmd
                        + byte_length_string
                        + byte_length_image;

    // allocate space and write header
    entire_raw = (unsigned char *)malloc(byte_length_entire);

    p_header                        = (KLOCK_WATCHFACE_HEADER *)entire_raw;
    p_header->header_magic          = HEADER_MAGIC_FLAG;
    p_header->variables_num         = context->num_var;
    p_header->cmd_block_start       = 0 + byte_length_header;
    p_header->cmd_num               = num_cmd;
    p_header->string_block_start    = p_header->cmd_block_start + byte_length_cmd;
    p_header->string_block_length   = byte_length_string;
    p_header->image_block_start     = p_header->string_block_start + byte_length_string;
    p_header->image_num             = context->image_list->size;

    // copy image metadata to header
    node = context->image_list->head;
    pos = p_header->image_block_start;

    for (i = 0; node != NULL; ++i, node = node->next) {
        KLOCK_IMAGE_META *  meta = p_header->image_meta + i;
        kb_image_t *        image = (kb_image_t *)node->data;

        meta->width = image->width;
        meta->height = image->height;
        meta->start = pos;

        pos += image->size;
    }

    // set pointers for writting
    p_cmd       = (kb_op_command_t *)(entire_raw + p_header->cmd_block_start);
    p_string    = (char *)(entire_raw + p_header->string_block_start);

    // dump command
    node = context->command_list->head;
    for (i = 0; node != NULL; ++i, node = node->next) {
        memcpy(p_cmd + i, node->data, sizeof(kb_op_command_t));
    }

    // dump string block
    memset(p_string, 0, byte_length_string);
    memcpy(p_string, context->string_buffer, byte_length_string);

    // dump images
    node = context->image_list->head;
    for (i = 0; node != NULL; ++i, node = node->next) {
        KLOCK_IMAGE_META *  meta = p_header->image_meta + i;
        kb_image_t *        image = (kb_image_t *)node->data;

        memcpy(meta->start + entire_raw, image->data, image->size);
    }

    *p_raw = entire_raw;
    *p_byte_length = byte_length_entire;

    return 1;
}

void DBG_print_header(KLOCK_WATCHFACE_HEADER* h) {
    unsigned int i;

    printf("header              = 0x%x\n", h->header_magic);
    printf("variable num        = %d\n", h->variables_num);
    printf("cmd block start     = %d\n", h->cmd_block_start);
    printf("cmd num             = %d\n", h->cmd_num);
    printf("string block start  = %d\n", h->string_block_start);
    printf("string block length = %d\n", h->string_block_length);
    printf("image block start   = %d\n", h->image_block_start);
    printf("image num           = %d\n", h->image_num);

    for (i = 0; i < h->image_num; ++i) {
        KLOCK_IMAGE_META *meta = h->image_meta + i;
        printf("image %d, w = %d, h = %d, start = %d\n", i, meta->width, meta->height, meta->start);
    }

}