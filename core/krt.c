#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "konstant.h"
#include "krt.h"
#include "utils.h"

#define KB_RT_STRINGIFY_BUF_SIZE 200

char stringify_buf[KB_RT_STRINGIFY_BUF_SIZE];

static const char * DBG_RT_VALUE_NAME[] = {
    "nil", "number", "string"
};

kb_rt_value* rtvalue_create_number(const KB_FLOAT num) {
    kb_rt_value* v = (kb_rt_value *)malloc(sizeof(kb_rt_value));
    v->type = RVT_NUMBER;
    v->data.num = num;
    return v;
}

kb_rt_value* rtvalue_create_string(const char* sz) {
    kb_rt_value* v = (kb_rt_value *)malloc(sizeof(kb_rt_value));
    v->type = RVT_STRING;
    v->data.sz = STR_DUMP(sz);
    return v;
}

kb_rt_value* rtvalue_create_from_concat(const char* sz_left, const char* sz_right) {
    char *sz_new, *p;
    int new_length = strlen(sz_left) + strlen(sz_right) + 1;

    kb_rt_value* v = (kb_rt_value *)malloc(sizeof(kb_rt_value));
    v->type = RVT_STRING;


    p = sz_new = (char *)malloc(new_length);
    p += STR_COPY(p, new_length, sz_left);
    p += STR_COPY(p, new_length, sz_right);

    v->data.sz = sz_new;
    return v;
}

kb_rt_value* rtvalue_duplicate(const kb_rt_value* v) {
    if (v->type == RVT_NUMBER) {
        return rtvalue_create_number(v->data.num);
    }
    else if (v->type == RVT_STRING) {
        return rtvalue_create_string(v->data.sz);
    }
    return NULL;
}

char* rtvalue_stringify(const kb_rt_value* v) {
    if (v->type == RVT_NUMBER) {
        char _buf[KB_RT_STRINGIFY_BUF_SIZE];
        k_ftoa(v->data.num, _buf, DEFAUL_FTOA_PRECISION);
        return STR_DUMP(_buf);
    }
    else if (v->type == RVT_STRING) {
        return STR_DUMP(v->data.sz);
    }
    return STR_DUMP("<unknown_value>");
}


void rtvalue_destroy(kb_rt_value * val) {
    if (val->type == RVT_STRING) {
        free(val->data.sz);
    }
    free(val);
}

void machine_command_reset(kb_machine_t* machine) {
    machine->cmd_ptr = (kb_op_command_t *)(machine->raw + machine->header->cmd_block_start);
}

kb_machine_t* machine_create(unsigned char * raw) {
    kb_machine_t* machine;
    int var_num, i;

    machine = (kb_machine_t *)malloc(sizeof(kb_machine_t));

    machine->raw        = raw;
    machine->header     = (KLOCK_WATCHFACE_HEADER *)raw;
    machine->stack      = vl_new_list();

    // initialize var with number 0
    var_num = machine->header->variables_num;
    machine->variables  = (kb_rt_value **)malloc(sizeof(kb_rt_value *) * var_num);
    for (i = 0; i < var_num; ++i) {
       machine->variables[i] = rtvalue_create_number(0);
    }    

    return machine;
}

void machine_destroy(kb_machine_t* machine) {
    int i;
    int var_num = machine->header->variables_num;
    for (i = 0; i < var_num; ++i) {
        rtvalue_destroy(machine->variables[i]);
    }
    free(machine->variables);
    vl_destroy(machine->stack, rtvalue_destroy);
    free(machine);
}

int machine_var_assign_num(kb_machine_t* machine, int var_index, KB_FLOAT num) {
    kb_rt_value* var_value;
    int var_num = machine->header->variables_num;

    if (var_index < 0 || var_index >= var_num) {
        return 0;
    }

    var_value = machine->variables[var_index];
    if (var_value->type == RVT_NUMBER) {
        var_value->data.num = num;
    }
    else {
        rtvalue_destroy(var_value);
        machine->variables[var_index] = rtvalue_create_number(num);
    }

    return 1;
}

extern const char *_KOCODE_NAME[];

#define format_exec_error_append(new_part) (p += STR_COPY(p, sizeof(buf) - (p - buf), new_part))

void format_exec_error(const kb_runtime_error_t *error_ret, char *message, int message_length) {
    char buf[KB_ERROR_MESSAGE_MAX];
    char *p = buf;

    if (error_ret->type == KBRE_OPERAND_TYPE_MISMATCH) {
        format_exec_error_append("Type mismatch error occurred. The operand was a '");
        format_exec_error_append(error_ret->message);
        format_exec_error_append("' when performing ");
        format_exec_error_append(_KOCODE_NAME[error_ret->cmd_op]);
    }
    else if (error_ret->type == KBRE_STACK_OVERFLOW) {
        format_exec_error_append("Stack overflow occurred. PTR = ");
        format_exec_error_append(error_ret->message);
    }
    else if (error_ret->type == KBRE_UNKNOWN_BUILT_IN_FUNC) {
        format_exec_error_append("Unknown built function: ");
        format_exec_error_append(error_ret->message);
    }
    else if (error_ret->type == KBRE_UNKNOWN_CMD) {
        format_exec_error_append("Unknown opcode: ");
        format_exec_error_append(error_ret->message);
    }
    else if (error_ret->type == KBRE_INVALID_IMAGE_INDEX) {
        format_exec_error_append("Invalid image index: ");
        format_exec_error_append(error_ret->message);
    }
    else {
        format_exec_error_append("Other problems occurred during runtime. ");
        if (!STR_EQUAL(error_ret->message, "")) {
             format_exec_error_append(error_ret->message);
        }
    }

    STR_COPY(message, message_length, buf);
}

void DBG_print_context_command(const kb_op_command_t *cmd);

#define machine_cmd_start_pointer(machine) ((kb_op_command_t *)((machine)->raw + (machine)->header->cmd_block_start))

#define exec_return_error_with_message(error_code, msg) NULL;       \
    error_ret->type = error_code;                                   \
    error_ret->cmd_op = machine->cmd_ptr->op;                       \
    STR_COPY(error_ret->message, sizeof(error_ret->message), msg);  \
    return 0

#define exec_return_error_with_int(error_code, intval) NULL;        \
    error_ret->type = error_code;                                   \
    error_ret->cmd_op = machine->cmd_ptr->op;                       \
    k_itoa(intval, error_ret->message, 10);                         \
    return 0

#define exec_pop_and_check_type(opr_index, val_type)  NULL;         \
    operand[opr_index] = (kb_rt_value *)vl_pop_back(machine->stack);\
    if (operand[opr_index] == NULL) {                               \
        exec_return_error_with_int(                                 \
            KBRE_STACK_OVERFLOW,                                    \
            machine->cmd_ptr - machine_cmd_start_pointer(machine)   \
        );                                                          \
    }                                                               \
    if (operand[opr_index]->type != val_type) {                     \
        exec_return_error_with_message(                             \
            KBRE_OPERAND_TYPE_MISMATCH,                             \
            DBG_RT_VALUE_NAME[operand[opr_index]->type]             \
        );                                                          \
    } NULL

int machine_exec_call_built_in (int func_index, kb_machine_t* machine, kb_runtime_error_t *error_ret, kb_rt_value ** operand);

int machine_exec(kb_machine_t* machine, kb_runtime_error_t *error_ret) {
    kb_op_command_t *   cmd_block_ptr = machine_cmd_start_pointer(machine);
    int                 cmd_num = machine->header->cmd_num;
    kb_rt_value *       operand[10];
    KB_FLOAT            num_result;

    error_ret->type = KBRE_NO_ERROR;
    error_ret->message[0] = '\0';

    machine_command_reset(machine);

    while (machine->cmd_ptr - cmd_block_ptr < cmd_num) {
        kb_op_command_t * cmd = machine->cmd_ptr;


        // printf("stack: %d\n", machine->stack->size);
        // getchar();
        // DBG_print_context_command(cmd);

        switch(cmd->op) {
            case KBO_NUL: {
	            break;
            }
            case KBO_PUSH_VAR: {
                kb_rt_value *var_value = machine->variables[cmd->param.index];
                vl_push_back(machine->stack, rtvalue_duplicate(var_value));
	            break;
            }
            case KBO_PUSH_NUM: {
                num_result = cmd->param.number;
                vl_push_back(machine->stack, rtvalue_create_number(num_result));
	            break;
            }
            case KBO_PUSH_STR: {
                char *sz = machine->raw + machine->header->string_block_start + cmd->param.index;
                vl_push_back(machine->stack, rtvalue_create_string(sz));
	            break;
            }
            case KBO_POP: {
                kb_rt_value *popped = (kb_rt_value *)vl_pop_back(machine->stack);
                rtvalue_destroy(popped);
	            break;
            }
            case KBO_OPR_NEG: {
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = -operand[0]->data.num;

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
	            break;
            }
            case KBO_OPR_CONCAT: {
                char *concat_left, *concat_right;

                operand[1] = (kb_rt_value *)vl_pop_back(machine->stack);
                operand[0] = (kb_rt_value *)vl_pop_back(machine->stack);

                concat_left = rtvalue_stringify(operand[0]);
                concat_right = rtvalue_stringify(operand[1]);

                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);

                vl_push_back(machine->stack, rtvalue_create_from_concat(concat_left, concat_right));

                free(concat_left);
                free(concat_right);
	            break;
            }
            case KBO_OPR_ADD: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = operand[0]->data.num + operand[1]->data.num;

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
	            break;
            }
            case KBO_OPR_SUB: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = operand[0]->data.num - operand[1]->data.num;

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
	            break;
            }
            case KBO_OPR_MUL: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = operand[0]->data.num * operand[1]->data.num;

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
	            break;
            }
            case KBO_OPR_DIV: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = operand[0]->data.num / operand[1]->data.num;
                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
	            break;
            }
            case KBO_OPR_INTDIV: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = (KB_FLOAT)((int)operand[0]->data.num / (int)operand[1]->data.num);

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
	            break;
            }
            case KBO_OPR_MOD: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = (KB_FLOAT)((int)operand[0]->data.num % (int)operand[1]->data.num);

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
	            break;
            }
            case KBO_OPR_NOT: {
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = (KB_FLOAT)(!(int)operand[0]->data.num);

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                break;
            }
            case KBO_OPR_AND: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = (KB_FLOAT)((int)operand[0]->data.num && (int)operand[1]->data.num);

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
	            break;
            }
            case KBO_OPR_OR: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = (KB_FLOAT)((int)operand[0]->data.num || (int)operand[1]->data.num);

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
                break;
            }
            case KBO_OPR_EQUAL: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = (KB_FLOAT)(operand[0]->data.num == operand[1]->data.num);

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
	            break;
            }
            case KBO_OPR_NEQ: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = (KB_FLOAT)(operand[0]->data.num != operand[1]->data.num);

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
                break;
            }
            case KBO_OPR_GT: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = (KB_FLOAT)(operand[0]->data.num > operand[1]->data.num);

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
	            break;
            }
            case KBO_OPR_LT: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = (KB_FLOAT)(operand[0]->data.num < operand[1]->data.num);

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
	            break;
            }
            case KBO_OPR_GTEQ: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = (KB_FLOAT)(operand[0]->data.num >= operand[1]->data.num);

                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
	            break;
            }
            case KBO_OPR_LTEQ: {
                exec_pop_and_check_type(1, RVT_NUMBER);
                exec_pop_and_check_type(0, RVT_NUMBER);

                num_result = (KB_FLOAT)(operand[0]->data.num <= operand[1]->data.num);
                
                vl_push_back(machine->stack, rtvalue_create_number(num_result));
                rtvalue_destroy(operand[0]);
                rtvalue_destroy(operand[1]);
	            break;
            }
            case KBO_CALL_BUILT_IN: {
                int call_success = machine_exec_call_built_in(cmd->param.index, machine, error_ret, operand);
                if (!call_success) {
                    return 0;
                }
	            break;
            }
            case KBO_ASSIGN_VAR: {
                kb_rt_value *var_value = machine->variables[cmd->param.index];
                kb_rt_value *popped = (kb_rt_value *)vl_pop_back(machine->stack);
                rtvalue_destroy(var_value);
                machine->variables[cmd->param.index] = popped;
	            break;
            }
            case KBO_GOTO: {
                machine->cmd_ptr = cmd_block_ptr + cmd->param.index;
	            continue;
            }
            case KBO_IFGOTO: {
                int is_true = 0;
                kb_rt_value *popped = (kb_rt_value *)vl_pop_back(machine->stack);
                if (popped->type == RVT_NUMBER) {
                    is_true = (int)popped->data.num;
                }
                else if (popped->type == RVT_STRING) {
                    is_true = 1;
                }
                rtvalue_destroy(popped);
                if (is_true) {
                    machine->cmd_ptr = cmd_block_ptr + cmd->param.index;
                    continue;
                }
                break;
            }
            case KBO_STOP: {
	            goto stop_exec;
            }
            default: {
                exec_return_error_with_int(KBRE_UNKNOWN_CMD, cmd->op);
                break;
            }
        }
        ++machine->cmd_ptr;
    }

stop_exec:

    return 1;
}

void machine_font_print(kb_machine_t* machine, int x, int y, int rev, const char *sz, const K_BITMAP_FONT_T *font) {
    unsigned char c;
    const unsigned char *image_raw;

    for (; *sz; sz++, x += font->w) {
        c = *(unsigned char *)sz;
        if (c < font->start_char || c > font->end_char) {
            continue;
        }
        image_raw = font->data + font->byte_size * (c - font->start_char);
        machine->graph_draw_image(x, y, font->w, font->h, rev, image_raw);
    }
}

#define machine_exec_call_built_in_single_arg(math_func) NULL;      \
    exec_pop_and_check_type(0, RVT_NUMBER);                         \
    num_result = (KB_FLOAT)math_func(operand[0]->data.num);         \
    vl_push_back(machine->stack, rtvalue_create_number(num_result));\
    rtvalue_destroy(operand[0])

int machine_exec_call_built_in (int func_index, kb_machine_t* machine, kb_runtime_error_t *error_ret, kb_rt_value ** operand) {
    KB_FLOAT num_result;
    switch (func_index) {
        //----------------------------------------
        //  batch one
        //----------------------------------------
        // sin
        case 0: {
            machine_exec_call_built_in_single_arg(sin);
            return 1;
        }
        // cos
        case 1: {
            machine_exec_call_built_in_single_arg(cos);
            return 1;
        }
        // tan
        case 2: {
            machine_exec_call_built_in_single_arg(tan);
            return 1;
        }
        // sqrt
        case 3: {
            machine_exec_call_built_in_single_arg(sqrt);
            return 1;
        }
        // exp
        case 4: {
            machine_exec_call_built_in_single_arg(exp);
            return 1;
        }
        // abs
        case 5: {
            machine_exec_call_built_in_single_arg(fabs);
            return 1;
        }
        // log
        case 6: {
            machine_exec_call_built_in_single_arg(log);
            return 1;
        }
        // rand
        case 7: {
            int rand_int = rand() % 1000;
            vl_push_back(machine->stack, rtvalue_create_number(1.0f * rand_int / 1000));
            return 1;
        }
        // clr
        case 8: {
            if (machine->graph_clr) {
                machine->graph_clr();
            }
            vl_push_back(machine->stack, rtvalue_create_number(0));
            return 1;
        }
        // zeropad
        case 9: {
            int digits;
            int max = sizeof(stringify_buf);
            char *buf = stringify_buf;
            int old_len;

            exec_pop_and_check_type(1, RVT_NUMBER);
            exec_pop_and_check_type(0, RVT_NUMBER);

            digits = (int)operand[1]->data.num;
            k_ftoa(operand[0]->data.num, buf, max);
            old_len = strlen(buf);

            if (digits < max - 1 && old_len < digits) {
                int i;
                int needed = digits - old_len;
                for (i = 0; i < old_len; ++i) {
                    buf[i + needed] = buf[i];
                }
                buf[i + needed] = '\0';
                for (i = 0; i < needed; ++i) {
                    buf[i] = '0';
                }
            }

            vl_push_back(machine->stack, rtvalue_create_string(buf));
            rtvalue_destroy(operand[0]);
            rtvalue_destroy(operand[1]);
            
            return 1;
        }
        // printmini
        case 10: {
            exec_pop_and_check_type(3, RVT_NUMBER);
            exec_pop_and_check_type(2, RVT_STRING);
            exec_pop_and_check_type(1, RVT_NUMBER);
            exec_pop_and_check_type(0, RVT_NUMBER);
            if (machine->graph_draw_image) {
                machine_font_print(
                    machine,
                    (int)operand[0]->data.num,
                    (int)operand[1]->data.num,
                    (int)operand[3]->data.num,
                    operand[2]->data.sz,
                    TOM_THUMB
                );
            }
            vl_push_back(machine->stack, rtvalue_create_number(0));
            rtvalue_destroy(operand[0]);
            rtvalue_destroy(operand[1]);
            rtvalue_destroy(operand[2]);
            rtvalue_destroy(operand[3]);
            return 1;
        }
        // print
        case 11: {
            exec_pop_and_check_type(3, RVT_NUMBER);
            exec_pop_and_check_type(2, RVT_STRING);
            exec_pop_and_check_type(1, RVT_NUMBER);
            exec_pop_and_check_type(0, RVT_NUMBER);
            if (machine->graph_draw_image) {
                machine_font_print(
                    machine,
                    (int)operand[0]->data.num,
                    (int)operand[1]->data.num,
                    (int)operand[3]->data.num,
                    operand[2]->data.sz,
                    PORTFOLIO
                );
            }
            vl_push_back(machine->stack, rtvalue_create_number(0));
            rtvalue_destroy(operand[0]);
            rtvalue_destroy(operand[1]);
            rtvalue_destroy(operand[2]);
            rtvalue_destroy(operand[3]);
            return 1;
        }
        // print
        case 12: {
            exec_pop_and_check_type(3, RVT_NUMBER);
            exec_pop_and_check_type(2, RVT_STRING);
            exec_pop_and_check_type(1, RVT_NUMBER);
            exec_pop_and_check_type(0, RVT_NUMBER);
            if (machine->graph_draw_image) {
                int x, y, w, h, offset;
                int image_index = (int)operand[3]->data.num;
                int image_num = machine->header->image_num;
                char *sz;
                KLOCK_IMAGE_META *meta;

                if (image_index < 0 || image_index >= image_num) {
                    exec_return_error_with_int(KBRE_INVALID_IMAGE_INDEX, image_index);
                }

                meta = &machine->header->image_meta[image_index];

                x = (int)operand[0]->data.num;
                y = (int)operand[1]->data.num;
                w = meta->width;
                h = meta->height / 11;
                offset = KB_IMG_GET_BYTE_SIZE(meta->width, meta->height) / 11;

                if (h > 0) {
                    sz = operand[2]->data.sz;
                    for (; *sz; ++sz, x += meta->width) {
                        int cursor_char_index;

                        if (*sz == ':') {
                            cursor_char_index = 0;
                        }
                        else if (*sz >= '0' && *sz <= '9') {
                            cursor_char_index = 1 + *sz - '0';
                        }
                        else {
                            continue;
                        }

                        machine->graph_draw_image(
                            x, y, w, h, 0,
                            machine->raw + meta->start + offset * cursor_char_index
                        );
                    }
                }
            }
            vl_push_back(machine->stack, rtvalue_create_number(0));
            rtvalue_destroy(operand[0]);
            rtvalue_destroy(operand[1]);
            rtvalue_destroy(operand[2]);
            rtvalue_destroy(operand[3]);
            return 1;
        }
        // line
        case 13: {
            exec_pop_and_check_type(4, RVT_NUMBER);
            exec_pop_and_check_type(3, RVT_NUMBER);
            exec_pop_and_check_type(2, RVT_NUMBER);
            exec_pop_and_check_type(1, RVT_NUMBER);
            exec_pop_and_check_type(0, RVT_NUMBER);
            if (machine->graph_plot_line) {
                machine->graph_plot_line(
                    (int)operand[0]->data.num,
                    (int)operand[1]->data.num,
                    (int)operand[2]->data.num,
                    (int)operand[3]->data.num,
                    (int)operand[4]->data.num
                );
            }
            vl_push_back(machine->stack, rtvalue_create_number(0));
            rtvalue_destroy(operand[0]);
            rtvalue_destroy(operand[1]);
            rtvalue_destroy(operand[2]);
            rtvalue_destroy(operand[3]);
            rtvalue_destroy(operand[4]);
            return 1;
        }
        // pixel
        case 14: {
            exec_pop_and_check_type(2, RVT_NUMBER);
            exec_pop_and_check_type(1, RVT_NUMBER);
            exec_pop_and_check_type(0, RVT_NUMBER);
            if (machine->graph_plot_line) {
                machine->graph_set_pixel(
                    (int)operand[0]->data.num,
                    (int)operand[1]->data.num,
                    (int)operand[2]->data.num
                );
            }
            vl_push_back(machine->stack, rtvalue_create_number(0));
            rtvalue_destroy(operand[0]);
            rtvalue_destroy(operand[1]);
            rtvalue_destroy(operand[2]);
            return 1;
        }
        // blt
        case 15: {
            exec_pop_and_check_type(2, RVT_NUMBER);
            exec_pop_and_check_type(1, RVT_NUMBER);
            exec_pop_and_check_type(0, RVT_NUMBER);
            if (machine->graph_draw_image) {

                int image_index = (int)operand[2]->data.num;
                int image_num = machine->header->image_num;
                KLOCK_IMAGE_META *meta;

                if (image_index < 0 || image_index >= image_num) {
                    exec_return_error_with_int(KBRE_INVALID_IMAGE_INDEX, image_index);
                }

                meta = &machine->header->image_meta[image_index];

                machine->graph_draw_image(
                    (int)operand[0]->data.num, // x
                    (int)operand[1]->data.num, // y
                    (int)meta->width,
                    (int)meta->height,
                    0,
                    machine->raw + meta->start
                );
            }
            rtvalue_destroy(operand[0]);
            rtvalue_destroy(operand[1]);
            rtvalue_destroy(operand[2]);
            vl_push_back(machine->stack, rtvalue_create_number(0));
            return 1;
        }
        // p
        case 16: {
            kb_rt_value *popped = (kb_rt_value *)vl_pop_back(machine->stack);
            char *sz = rtvalue_stringify(popped);
            rtvalue_destroy(popped);
            printf("%s\n", sz);
            free(sz);
            vl_push_back(machine->stack, rtvalue_create_number(0));
            return 1;
        }
    }
    exec_return_error_with_int(KBRE_UNKNOWN_BUILT_IN_FUNC, func_index);
}