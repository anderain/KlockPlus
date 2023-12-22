#include "../graphics/graph.h"
#include "../input.h"
#include "../ce-utils.h"
#include "scene.h"
#include "../../core/krt.h"

// #define DISPLAY_DEBUG_INFO	1
#define BLACK_DOT   0x000000
#define WHITE       0xffffff

TCHAR sceneAppFileToLoad[MAX_PATH];
static char fileToLoad[MAX_PATH];

#define KSCREEN_W   128
#define KSCREEN_H   64

char klockScreenBuffer[KSCREEN_H][KSCREEN_W];

#define KSCREEN_Clear()             (memset(klockScreenBuffer, 0, sizeof(klockScreenBuffer)))
#define KSCREEN_SetPixel(x, y, dot) (klockScreenBuffer[y][x] = !!(dot))

unsigned char* readBinaryFile(const TCHAR * tFileName) {
    HANDLE pFile;
    DWORD fileSize;
    unsigned char *buffer,*tmpBuf;
    DWORD dwBytesRead,dwBytesToRead;

    pFile = CreateFile(
        tFileName,
        GENERIC_READ,          
        FILE_SHARE_READ,
        NULL,               
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );

    if (pFile == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    fileSize = GetFileSize(pFile,NULL);

    buffer = (char *)malloc(fileSize);
    memset(buffer, 0, fileSize);
    dwBytesToRead = fileSize;
    dwBytesRead = 0;
    tmpBuf = buffer;

    do {
        ReadFile(pFile,tmpBuf,dwBytesToRead,&dwBytesRead,NULL);
        if (dwBytesRead == 0)
            break;
        dwBytesToRead -= dwBytesRead;
        tmpBuf += dwBytesRead;

    } while (dwBytesToRead > 0);

    CloseHandle(pFile);

    return buffer;
}

void Z_SetPixel(int x, int y, int dot) {
    if (x < 0 || x >= KSCREEN_W || y < 0 || y >= KSCREEN_H) {
        return;
    }
    KSCREEN_SetPixel(x, y, dot);
}

void Z_PlotLine (int x0, int y0, int x1, int y1, int dot) {
  int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
  int err = dx + dy, e2; 
 
  for (;;){ 
    // GP_SetPixel (x0, y0, dot ? BLACK_DOT : WHITE);
    KSCREEN_SetPixel(x0, y0, dot);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; } 
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

void Z_DrawImage (int dx, int dy, int w, int h, int rev, const unsigned char * raw) {
    const pitch = w / 8 + (w % 8 ? 1 : 0);
    int x, y;
    for (y = 0; y < h; ++y) {
        for (x = 0; x < w; ++x) {
            const K_BYTE eight_pixel = *(raw + y * pitch + x / 8);
            const int dot = (eight_pixel >> (7 - x % 8)) & 1;
            KSCREEN_SetPixel(dx + x, dy + y, dot ^ rev);
            // GP_SetPixel(dx + x, dy + y, dot ^ rev ? BLACK_DOT : WHITE);
        }
    }
}

int Provider_Clr() {
    KSCREEN_Clear();
    return 0;
}

int Provider_SetPixel(int x, int y, int dot) {
    // GP_SetPixel(x, y, dot ? BLACK_DOT : WHITE);
    Z_SetPixel(x, y, dot);
    return 0;
}

int Provider_PlotLine(int x1, int y1, int x2, int y2, int dot) {
    // GP_Line(x1, y1, x2, y2, dot ? BLACK_DOT : WHITE);
    Z_PlotLine(x1, y1, x2, y2, dot);
    return 0;
}

int Provider_DrawImage(int x, int y, int w, int h, int rev, const unsigned char *raw) {
    Z_DrawImage(x, y, w, h, rev, raw);
    return 0;
}

void Provider_GetTime(int *hh, int *mm, int *ss, int *ms) {
    SYSTEMTIME lt;
    GetLocalTime(&lt);
    *hh = lt.wHour;
    *mm = lt.wMinute;
    *ss = lt.wSecond;
    *ms = lt.wMilliseconds;
}

kb_machine_t* loadApp(const TCHAR *filename) {
    unsigned char *raw;
    kb_machine_t *app;
    KLOCK_WATCHFACE_HEADER *header;

    raw = readBinaryFile(filename);

    if (!raw) {
        return NULL;
    }

    header = (KLOCK_WATCHFACE_HEADER *)raw;
    // validate magic header
    if (header->header_magic != HEADER_MAGIC_FLAG) {
        int                 i;
        kb_op_command_t*    cmd;
        int                 image_num;
        int                 cmd_num;

        // convert header
        header->header_magic        = endian_swap_dword(header->header_magic);
        header->variables_num       = endian_swap_dword(header->variables_num);
        header->cmd_block_start     = endian_swap_dword(header->cmd_block_start);
        header->cmd_num             = endian_swap_dword(header->cmd_num);
        header->string_block_start  = endian_swap_dword(header->string_block_start);
        header->string_block_length = endian_swap_dword(header->string_block_length);
        header->image_block_start   = endian_swap_dword(header->image_block_start);
        header->image_num           = endian_swap_dword(header->image_num);
        
        cmd_num = header->cmd_num;
        image_num = header->image_num; 

        // convert image meta
        for (i = 0; i < image_num; ++i) {
            KLOCK_IMAGE_META *meta = &header->image_meta[i];

            meta->start     = endian_swap_dword(meta->start);
            meta->width     = endian_swap_dword(meta->width);
            meta->height    = endian_swap_dword(meta->height);
        }

        // convert commands
        cmd = (kb_op_command_t *)(raw + header->cmd_block_start);
        
        for (i = 0; i < cmd_num; ++i) {
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

    app->graph_clr          = Provider_Clr;
    app->graph_set_pixel    = Provider_SetPixel;
    app->graph_plot_line    = Provider_PlotLine;
    app->graph_draw_image   = Provider_DrawImage;

    return app;
}

kb_machine_t *      app = NULL;
kb_runtime_error_t  errorRet;
char                errorMessage[300];

static void initiailze() {
    w2c(sceneAppFileToLoad, fileToLoad);
    app = loadApp(sceneAppFileToLoad);
    if (!app) {
        strcpy(errorMessage, "Failed to load KWF file");
        return; 
    }
    KSCREEN_Clear();
}

static void dispose() {
    if (app) {
        machine_destroy(app);
    }
}

static void redraw(int delta) {
    int ret = 1;
    int hh, mm, ss, ms;
    
    GP_FrameBuffer_FillWhite();

    if (!app) {
        GP_DrawText(errorMessage, 0, 0, BLACK_DOT, 0, 0);
        GP_DrawText("Press ESC to exit KlockPlus", 0, 12, BLACK_DOT, 0, 0);
        GP_DrawText(fileToLoad, 4, 220, BLACK_DOT, 0, 0);
    } else {
        int l = 32 - 2;
        int t = 56 - 2;
        int w = 128 * 2 + 3;
        int h = 64 * 2 + 3;
        int x, y;

        Provider_GetTime(&hh, &mm, &ss, &ms);
        machine_var_assign_num(app, 0, (KB_FLOAT)hh);
        machine_var_assign_num(app, 1, (KB_FLOAT)mm);
        machine_var_assign_num(app, 2, (KB_FLOAT)ss);
        machine_var_assign_num(app, 3, (KB_FLOAT)ms);
        
        // execute
        ret = machine_exec(app, &errorRet);
        if (!ret) {
            format_exec_error(&errorRet, errorMessage, sizeof(errorMessage));
            machine_destroy(app);
            app = NULL;
            return;
        }

        GP_Line(l, t, l + w, t, BLACK_DOT);
        GP_Line(l + w, t, l + w, t + h, BLACK_DOT);
        GP_Line(l + w, t + h, l, t + h, BLACK_DOT);
        GP_Line(l, t + h, l, t, BLACK_DOT);

        GP_DrawText("KlockPlus-CE", 4, 4, BLACK_DOT, 0, 0);
        GP_DrawText("Press button (B) to go back", 4, 12, BLACK_DOT, 0, 0);
        GP_DrawText(fileToLoad, 4, 220, BLACK_DOT, 0, 0);

        // Put disp
        for (y = 0; y < KSCREEN_H; ++y) {
            for (x = 0; x < KSCREEN_W; ++x) {
                int rx = (x * 2) + 32;
                int ry = (y * 2) + 56;
                GP_IMAGE_COLOR c = klockScreenBuffer[y][x] ? BLACK_DOT : WHITE;
                GP_SetPixel(rx, ry, c);
                GP_SetPixel(rx + 1, ry, c);
                GP_SetPixel(rx, ry + 1, c);
                GP_SetPixel(rx + 1, ry + 1, c);
            }
        }

    }
#ifdef DISPLAY_DEBUG_INFO
	// Display debug info
	{
		char buf[100] = { 0 };

		strcpy(buf, "Delta=");
		my_itoa(delta, buf + strlen(buf), 10);
		GP_DrawText(buf, 4, 4, 0xffffff, 1, 0);
	

		strcpy(buf, "VIDEO MODE=");
		my_itoa(GP_GetVideoBitMode(), buf + strlen(buf), 10);

		strcpy(buf + strlen(buf), "BIT");

		GP_DrawText(buf, 4, 16, 0xffffff, 1, 0);

	}
#endif
}

static void update(int delta) {
    if (Input_IsKeyDown(INPUT_BUTTON_B)) {
        Scene_To(&Scene_Menu);
	}
}

SCENE Scene_App = {
	initiailze,
	dispose,
	redraw,
	update
};