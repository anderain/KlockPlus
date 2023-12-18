#ifndef _PREVIEW_H_
#define _PREVIEW_H_

#include "../core/krt.h"
#include "../core/utils.h"

int start_preview(
    kb_machine_t * machine,
    kb_runtime_error_t *error_ret
);

kb_machine_t* load_app(const char *filename);


void draw_image_raw(int x, int y, int w, int h, int rev, const unsigned char* raw);

#endif