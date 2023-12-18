#ifndef _PREVIEW_H_
#define _PREVIEW_H_

#include "../core/krt.h"
#include "../core/utils.h"

#define BCD_TO_BASE10(v)    (((v) >> 4) * 10 + ((v) & 0xf))
#define BASE10_TO_BCD(v)    (((v) / 10) * 16 + (v) % 10)

int start_preview(
    kb_machine_t * machine,
    kb_runtime_error_t *error_ret
);

kb_machine_t* load_app(const char *filename);

void draw_image_raw(int x, int y, int w, int h, int rev, const unsigned char* raw);
void get_time(int *hh, int *mm, int *ss, int *ms);

#endif