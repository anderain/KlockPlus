#include <fxlib.h>
#include <stdio.h>
#include "preview.h"
#include "rtc.h"

void char2font(const char * fileName, FONTCHARACTER *font) {
    while (*font++ = *fileName++);
}

void font2char(const FONTCHARACTER *font, char * fileName) {
    while (*fileName++ = *font++);
}

#define FILE_NAME_MAX   25
#define FILE_NUM_MAX    20

int list_file(const char *path, char file_list[FILE_NUM_MAX][FILE_NAME_MAX]) {
    char full_path[100];
    char file_name[100];
    FONTCHARACTER font_path[100];
    FONTCHARACTER font_file_name[100];
    FILE_INFO info;
    int handle;
    int size = 0;

    sprintf(full_path, "%s%s", path, "*.kwf");

    char2font(full_path, font_path);

    if (0 != Bfile_FindFirst(font_path, &handle, font_file_name, &info)) {
        return 0;
    }

    do {
        font2char(font_file_name, file_name);
        strcpy(file_list[size++], file_name);
        if (size >= 99) break;
    } while (0 == Bfile_FindNext(handle, font_file_name, &info));

    Bfile_FindClose(handle);

    return size;

}

void launch_watchface(const char *filename) {
    kb_runtime_error_t  error_ret;
    int                 ret;
    kb_machine_t*       app;
    char                buf[200];

    app = load_app(filename);

    if (!app) {
        Bdisp_AllClr_VRAM();
        locate(1, 1);
        Print((unsigned char *)"Failed to load");
        locate(1, 2); Print((unsigned char *)filename);
        locate(1, 8);
        Print((unsigned char *)"Press EXIT to continue");
        while(1) {
            unsigned int key;
            GetKey(&key);
            if (key == KEY_CTRL_EXE || key == KEY_CTRL_EXIT) {
                return;
            }
        }
    }
    
    ret = start_preview(app, &error_ret);

    if (!ret) {
        char error_message[200];
        format_exec_error(&error_ret, error_message, sizeof(error_message));
        Bdisp_AllClr_VRAM();
        locate(1, 1); Print((unsigned char *)"Runtime Error!");
        locate(1, 2); Print((unsigned char *)filename);
        locate(1, 3); Print((unsigned char *)error_message);
        locate(1, 8); Print((unsigned char *)"Press EXIT to continue");
        while(1) {
            unsigned int key;
            GetKey(&key);
            if (key == KEY_CTRL_EXE || key == KEY_CTRL_EXIT) {
                break;
            }
        }
    }

    machine_destroy(app);
}

int select_file_ui(const char *path) {
    unsigned int key;
    int index = 0, i;
    
    char file_list[FILE_NUM_MAX][FILE_NAME_MAX];
    int file_list_length = 0;

    const int row = 6;
    int top = 0;
    int bottom = row;

    char strFolder[40];
    char strTitle[40];

    if (path) {
        file_list_length = list_file(path, file_list);
        if (file_list_length <= 0) {
            PopUpWin(1);
            locate(8, 4); Print((unsigned char *)"No file");
            GetKey(&key);
            return;
        }
    } else {
        return 0;
    }

    sprintf(strFolder, "Open from %s    ", path);
    sprintf(strTitle, "KlockPlus        [%2d]", file_list_length);

    top = 0;
    bottom = row - 1;
    if (bottom > file_list_length) bottom = file_list_length - 1;

    while (1) {
        Bdisp_AllClr_VRAM();
        PrintXY(0, 0, (unsigned char *)strTitle, 0);
        PrintXY(0, 64 - 8, (unsigned char *)strFolder, 0);

        top = index - row + 1;
        bottom = index + row - 1;

        if (top < 0) top = 0;
        if (bottom >= file_list_length) bottom = file_list_length - 1;
        if (bottom - top >= row) bottom = row + top - 1;

        for (i = top; i <= bottom; ++i) {
            PrintXY(0, 8 * (1 + i - top), (unsigned char *)file_list[i], 0);
        }

        Bdisp_AreaReverseVRAM(0 , 8 * (1 + index - top), 127, 8 * (2 + index - top) - 1);
        Bdisp_AreaReverseVRAM(0 , 0, 127, 7);
        Bdisp_AreaReverseVRAM(0 , 63 - 7, 127, 63);
        GetKey(&key);

        if (key == KEY_CTRL_UP) {
            index--;
        }
        else if (key == KEY_CTRL_DOWN) {
            index++;
        }
        else if (key == KEY_CTRL_EXIT) {
            return 0;
        }
        else if (key == KEY_CTRL_EXE) {
            char file_path[100];
            int ans = 0;

            sprintf(file_path, "%s%s", path, file_list[index]);

            PopUpWin(5);
            locate(3, 2); Print((unsigned char *)"Run watchface?");
            locate(4, 4); Print((unsigned char *)file_list[index]);
            locate(3, 6); Print((unsigned char *)"[F1]Yes   [F2]No");

            while (1) {
                GetKey(&key);
                if (key == KEY_CTRL_F1 || key == KEY_CTRL_EXE) {
                    ans = 1;
                    break;
                }
                else if (key == KEY_CTRL_F2 || key == KEY_CTRL_EXIT) {
                    break;
                }
            }
            if (ans) {
                launch_watchface(file_path);
            }
        }

        if (index < 0) index = file_list_length - 1;
        if (index >= file_list_length) index = 0;
    }
    return 0;
}

int set_time_ui () {
    int hh;
    int mm;
    unsigned int key;
    char buf[80];
    int tx1 = 49 - 2, ty1 = 20 - 2, tx2 = 49 + 30 + 1, ty2 = 20 + 8 + 1;

    unsigned char datetime[] = {
        0x20,   // 0 year-1
        0x23,   // 1 year-2
        0x06,   // 2 month
        0x06,   // 3 day
        0x00,   // 4 hour
        0x00,   // 5 min
        0x00,   // 6 sec 
        0x00    // 7 unused
    };

    get_time(&hh, &mm, NULL, NULL);

    while (1) {
        Bdisp_AllClr_VRAM();

        sprintf(buf, "%02d:%02d", hh, mm);
        locate(1, 1); Print((unsigned char *)"Set time");

        PrintXY(49, 20, (unsigned char *)buf, 1);
        Bdisp_DrawLineVRAM(tx1, ty1, tx1, ty2);
        Bdisp_DrawLineVRAM(tx1, ty2, tx2, ty2);
        Bdisp_DrawLineVRAM(tx2, ty2, tx2, ty1);
        Bdisp_DrawLineVRAM(tx2, ty1, tx1, ty1);


        locate(1, 6); Print((unsigned char *)"\xe6\x90 \xe6\x91       Edit hour");
        locate(1, 7); Print((unsigned char *)"\xe6\x92 \xe6\x93       Edit minute");
        locate(1, 8); Print((unsigned char *)"[EXE]     Confirm");

        Bdisp_AreaReverseVRAM(0 , 0, 127, 7);

        GetKey(&key);

        if (key == KEY_CTRL_EXIT) {
            return 0;
        }
        else if (key == KEY_CTRL_UP) {
            mm--;
            if (mm < 0) mm = 59;
        }
        else if (key == KEY_CTRL_DOWN) {
            mm++;
            if (mm >= 60) mm = 0;
        }
        else if (key == KEY_CTRL_LEFT) {
            hh--;
            if (hh < 0) hh = 23;
        }
        else if (key == KEY_CTRL_RIGHT) {
            hh++;
            if (hh >= 24) hh = 0;
        }
        else if (key == KEY_CTRL_EXE) {
            break;
        }
    }
    datetime[4] = BASE10_TO_BCD(hh);
    datetime[5] = BASE10_TO_BCD(mm);
    RTC_SetDateTime(datetime);

    PopUpWin(1);
    locate(5, 4); Print((unsigned char *)"Time updated!");
    GetKey(&key);

    return 1;
}

extern const unsigned char ICON_FLASH_MEM[];
extern const unsigned char ICON_SD_CARD[];
extern const unsigned char ICON_SETTING[];

int select_main_ui() {
    unsigned int key;
    int index = 0;

    while(1) {
        Bdisp_AllClr_VRAM();
        locate(7, 1);
        Print((unsigned char *)"Klock Plus");

        draw_image_raw(16, 16, 32, 32, index == 0, ICON_FLASH_MEM);
        draw_image_raw(48, 16, 32, 32, index == 1, ICON_SD_CARD);
        draw_image_raw(80, 16, 32, 32, index == 2, ICON_SETTING);
        if (index == 0) {
            locate(4, 8);
            Print((unsigned char *)"Open from flash");
        } else if (index == 1) {
            locate(3, 8);
            Print((unsigned char *)"Open from SD card");
        } else {
		    locate(8, 8);
            Print((unsigned char *)"Set time");
		}
        GetKey(&key);
        if (key == KEY_CTRL_LEFT) {
            index--; if (index < 0) index = 2;
        }
		else if (key == KEY_CTRL_RIGHT) {
			index++; if (index > 3) index = 0;	
		}
        else if (key == KEY_CTRL_EXE) {
            if (index == 0) {
				select_file_ui("\\\\fls0\\");
			}
			else if (index == 1) {
				select_file_ui("\\\\crd0\\");	
			}
            else if (index == 2) {
                set_time_ui();
            }
        }
    }
}

int AddIn_main(int isAppli, unsigned short OptionNum)
{
    unsigned int key;

    select_main_ui();

    while(1) GetKey(&key);

    return 1;
}


#pragma section _BR_Size
unsigned long BR_Size;
#pragma section


#pragma section _TOP

int InitializeSystem(int isAppli, unsigned short OptionNum)
{
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
}

#pragma section

