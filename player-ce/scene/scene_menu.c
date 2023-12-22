#include "../graphics/graph.h"
#include "../input.h"
#include "../ce-utils.h"
#include "scene.h"
// #define DISPLAY_DEBUG_INFO   1

#define FILE_NAME_LENGTH    MAX_PATH
#define FILE_LIST_MAX       20

TCHAR   szExeName[MAX_PATH];
char    FileNames[FILE_LIST_MAX][FILE_NAME_LENGTH];
int     FileTotal = 0;
int     cursor = 0;

TCHAR * removeFileNameInPath(TCHAR *szName) {
    int pos = wcslen(szName);
    for (pos--; pos >= 0; --pos) {
        if (szName[pos] == '/' || szName[pos] == '\\') {
            szName[pos] = '\0';
            return szName;
        }
    }
    return szName;
}

static void initiailze() {
    WIN32_FIND_DATA     ffd;
    TCHAR               szDir[MAX_PATH];
    HANDLE              hFind = INVALID_HANDLE_VALUE;
    DWORD               dwError = 0;

#if (_WIN32_WCE >= 200) // WIN CE Ver 2.0+
    GetModuleFileName(NULL, szExeName, MAX_PATH);
    removeFileNameInPath(szExeName);
#else
    // GetModuleFileName not exist in CE1.x
    wcscpy(szExeName, _T(""));
#endif
    wsprintf(szDir, _T("%s*.kwf"), szExeName);

    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind)  {
        return;
    }

    FileTotal = 0;

    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // continue
        }
        else {
            w2c(ffd.cFileName, FileNames[FileTotal]);
            FileTotal++;
        }
        
        if (FileTotal >= FILE_LIST_MAX) {
            break;
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
}

static void dispose() {

}

static void redraw(int delta) {
    int top = 8, left = 12, i;

    GP_FrameBuffer_FillWhite();
    GP_DrawText("KlockPlus-CE Menu", left, top, 0, 0, 0);
    top += 8;
    GP_DrawText("----------------------------", left, top, 0, 0, 0);
    top += 8;
    GP_DrawText("Press button (A) to start", left, top, 0, 0, 0);
    top += 8;
    top += 8;
    for (i = 0; i < FileTotal; ++i) {
        if (cursor == i) {
            GP_DrawText(">", left, top, 0, 0, 0);
        }
        GP_DrawText(FileNames[i], left + 12, top, 0, 0, 0);
        top += 8;
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

extern TCHAR sceneAppFileToLoad[];

static void update(int delta) {
	if (Input_IsKeyDown(INPUT_BUTTON_DOWN)) {
		cursor++;
        if (cursor >= FileTotal) {
            cursor = 0;
        }
	}
	else if (Input_IsKeyDown(INPUT_BUTTON_UP)) {
		cursor--;
        if (cursor < 0) {
            cursor = FileTotal - 1;
        }
	}
    else if (Input_IsKeyDown(INPUT_BUTTON_A)) {
        TCHAR fileName[MAX_PATH];
        c2w(FileNames[cursor], fileName);
        wsprintf(sceneAppFileToLoad, _T("%s\\%s"), szExeName, fileName);
        Scene_To(&Scene_App);
	}
}

SCENE Scene_Menu = {
    initiailze,
    dispose,
    redraw,
    update
};