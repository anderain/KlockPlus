#include <fxlib.h>
#include <stdio.h> // Oh, I still need sprintf for some reason...
#include <string.h>
#include <stdlib.h>
#include "preview.h"
#include "rtc.h"

#define BCD_TO_BASE10(v)    (((v) >> 4) * 10 + ((v) & 0xf))

#define MYKEY_UP        2,9
#define MYKEY_DOWN      3,8
#define MYKEY_LEFT      3,9
#define MYKEY_RIGHT     2,8
#define MYKEY_ENTER     3,2
#define MYKEY_ESC       4,8

#define MYKEY_NUM_0     7,2
#define MYKEY_NUM_1     7,3
#define MYKEY_NUM_2     6,3
#define MYKEY_NUM_3     5,3
#define MYKEY_NUM_4     7,4
#define MYKEY_NUM_5     6,4
#define MYKEY_NUM_6     5,4
#define MYKEY_NUM_7     7,5
#define MYKEY_NUM_8     6,5
#define MYKEY_NUM_9     5,5


int is_keydown(int code1, int code2) {
    int kcode1, kcode2; short unused = 0;
    if (Bkey_GetKeyWait(&kcode1, &kcode2, KEYWAIT_HALTOFF_TIMEROFF, 0, 1, &unused)) {
        if (kcode1 == code1 && kcode2 == code2)
            return 1;
    }
    return 0;
}

int waitkey() {
    unsigned int key = 0;
    while (key != KEY_CTRL_EXE) {
        GetKey(&key);
    }
    return 0;
}

unsigned char *read_binary_file(const char *filename) {
    unsigned short      t_fliename[100];
    int                 i, fh, fsize;
    unsigned char *     raw;

    for (i = 0; filename[i]; ++i) {
        t_fliename[i] = filename[i];
    }
    t_fliename[i] = '\0';

    fh = Bfile_OpenFile(t_fliename, _OPENMODE_READ_SHARE);
    if (fh < 0) {
        return NULL;
    }

    fsize   = Bfile_GetFileSize(fh);
    raw     = (unsigned char *)malloc(fsize);

    Bfile_ReadFile(fh, raw, fsize, 0);
    Bfile_CloseFile(fh);

	return raw;
}

void plot_line (int x0, int y0, int x1, int y1, int dot) {
  int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
  int err = dx + dy, e2; 
 
  for (;;){ 
    Bdisp_SetPoint_VRAM(x0, y0, dot);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; } 
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

void draw_image_raw(int x, int y, int w, int h, int rev, const unsigned char* raw) {
    DISPGRAPH disp;
    WRITEMODIFY modify;
#ifdef IMB_WRITEMODIFY_REVERCE
    modify = rev ? IMB_WRITEMODIFY_REVERCE : IMB_WRITEMODIFY_NORMAL;
#else
    modify = rev ? IMB_WRITEMODIFY_REVERSE : IMB_WRITEMODIFY_NORMAL;
#endif
    disp.x = x;
    disp.y = y;
    disp.GraphData.width = w;
    disp.GraphData.height = h;
    disp.GraphData.pBitmap = raw;
    disp.WriteModify = modify;
    disp.WriteKind = IMB_WRITEKIND_OVER;
    Bdisp_WriteGraph_VRAM(&disp);
}

int provider_clr() {
    Bdisp_AllClr_VRAM();
    return 0;
}

int provider_set_pixel(int x, int y, int dot) {
    Bdisp_SetPoint_VRAM(x, y, dot);
    return 0;
}

int provider_plot_line(int x1, int y1, int x2, int y2, int dot) {
    plot_line(x1, y1, x2, y2, dot);
    return 0;
}

int provider_draw_image(int x, int y, int w, int h, int rev, const unsigned char *raw) {
    draw_image_raw(x, y, w, h, rev, raw);
    return 0;
}

kb_machine_t* load_app(const char *filename) {
    unsigned char *raw;
    kb_machine_t *app;
	KLOCK_WATCHFACE_HEADER *header;

    raw = read_binary_file(filename);

    if (!raw) {
        return NULL;
    }

    header = (KLOCK_WATCHFACE_HEADER *)raw;

    // validate magic header
    if (header->header_magic != HEADER_MAGIC_FLAG) {
        int                 i;
        kb_op_command_t*    cmd;

        // convert header
        header->header_magic        = endian_swap_dword(header->header_magic);
        header->variables_num       = endian_swap_dword(header->variables_num);
        header->cmd_block_start     = endian_swap_dword(header->cmd_block_start);
        header->cmd_num             = endian_swap_dword(header->cmd_num);
        header->string_block_start  = endian_swap_dword(header->string_block_start);
        header->string_block_length = endian_swap_dword(header->string_block_length);
        header->image_block_start   = endian_swap_dword(header->image_block_start);
        header->image_num           = endian_swap_dword(header->image_num);
        
        // convert image meta
        for (i = 0; i < header->image_num; ++i) {
            KLOCK_IMAGE_META *meta = &header->image_meta[i];

            meta->start     = endian_swap_dword(meta->start);
            meta->width     = endian_swap_dword(meta->width);
            meta->height    = endian_swap_dword(meta->height);
        }

        // convert commands
        cmd = (kb_op_command_t *)(raw + header->cmd_block_start);
        
        for (i = 0; i < header->cmd_num; ++i) {
            // char buf[100];
            cmd->op             = endian_swap_dword(cmd->op);
            cmd->param.index    = endian_swap_dword(cmd->param.index);

            // // debug print
            // sprintf(buf, "%s ", _KOCODE_NAME[cmd->op]);

            // if (cmd->op == KBO_PUSH_VAR
            //  || cmd->op == KBO_PUSH_STR
            //  || cmd->op == KBO_CALL_BUILT_IN
            //  || cmd->op == KBO_ASSIGN_VAR
            //  || cmd->op == KBO_GOTO
            //  || cmd->op == KBO_IFGOTO) {
            //     sprintf(buf + strlen(buf), "%d", cmd->param.index);
            // }
            // else if (cmd->op == KBO_PUSH_NUM) {
            //     char sz_num[KB_TOKEN_LENGTH_MAX];
            //     k_ftoa((float)cmd->param.number, sz_num, DEFAUL_FTOA_PRECISION);
            //     sprintf(buf + strlen(buf), "%s", sz_num);
            // }
            // else if (cmd->op >= KBO_OPR_NEG && cmd->op <= KBO_OPR_LTEQ || cmd->op == KBO_POP || cmd->op == KBO_STOP) {
            //     // nothing here
            // }

            // Bdisp_AllClr_DDVRAM();
            // locate(1, 1); Print(buf);
            // waitkey();

            cmd++;
        }

        // Debug print header
        // {
        //     char buf[100];

        //     Bdisp_AllClr_DDVRAM();

        //     sprintf(
        //         buf,
        //         "0x%x, %d, %d ,%d",
        //         header->header_magic,
        //         header->variables_num,
        //         header->cmd_block_start,
        //         header->cmd_num
        //     );
        //     locate(1, 1); Print(buf);
        //     sprintf(
        //         buf,
        //         "%d, %d, %d ,%d",
        //         header->string_block_start,
        //         header->string_block_length,
        //         header->image_block_start,
        //         header->image_num
        //     );
        //     locate(1, 2); Print(buf);
        //     sprintf(
        //         buf,
        //         "%d, %d, %d",
        //         header->image_meta[0].width,
        //         header->image_meta[0].height,
        //         header->image_meta[0].start
        //     );
        //     locate(1, 3); Print(buf);
        //     waitkey();
        // }
    }

    app = machine_create(raw);

    app->graph_clr          = provider_clr;
    app->graph_set_pixel    = provider_set_pixel;
    app->graph_plot_line    = provider_plot_line;
    app->graph_draw_image   = provider_draw_image;

    return app;
}

void get_time(int *hh, int *mm, int *ss, int *ms) {
    unsigned int bcd_h, bcd_m, bcd_s, bcd_ms;
    RTC_GetTime(&bcd_h, &bcd_m, &bcd_s, &bcd_ms);
    *hh = BCD_TO_BASE10(bcd_h);
    *mm = BCD_TO_BASE10(bcd_m);
    *ss = BCD_TO_BASE10(bcd_s);
    // *hh = BCD_TO_BASE10(bcd_ms);
}

int start_preview(kb_machine_t * machine, kb_runtime_error_t *error_ret) {
    int quit = 0;
    int ret = 1;
    int hh, mm, ss, ms;

    Bdisp_AllClr_VRAM();

    while (!quit) {
        Bdisp_AllClr_VRAM();
        
        // fetch time and assign to machine
        get_time(&hh, &mm, &ss, &ms);
        machine_var_assign_num(machine, 0, (KB_FLOAT)hh);
        machine_var_assign_num(machine, 1, (KB_FLOAT)mm);
        machine_var_assign_num(machine, 2, (KB_FLOAT)ss);
        machine_var_assign_num(machine, 3, (KB_FLOAT)ms);

        if (is_keydown(MYKEY_ENTER) || is_keydown(MYKEY_ESC)) {
            ret = 1;
            break;
        }

        // execute
        ret = machine_exec(machine, error_ret);
        if (!ret) {
            quit = 0;
            break;
        }

        Bdisp_PutDisp_DD();
    }

    return ret;
}
