#ifndef _KONSTANT_H_
#define _KONSTANT_H_

typedef struct {
    int                 w;
    int                 h;
    int                 byte_size;
    unsigned            start_char;
    unsigned            end_char;
    const unsigned char* data;
} K_BITMAP_FONT_T;

extern const K_BITMAP_FONT_T TOM_THUMB[];
extern const K_BITMAP_FONT_T PORTFOLIO[];

#endif