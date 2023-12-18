#include <stdlib.h>
#include <string.h>
#include "kommon.h"

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
