#include <fxlib.h>
#include "preview.h"

int AddIn_main(int isAppli, unsigned short OptionNum)
{
    unsigned int        key;
    kb_runtime_error_t  error_ret;
    int                 ret;
    kb_machine_t*       app;
    char                buf[200];

    app = load_app("\\\\fls0\\SAMPLE2.kwf");

    if (!app) {
        locate(1, 1);
        Print("failed to load");
        while(1) GetKey(&key);
    }
    
    ret = start_preview(app, &error_ret);
    if (!ret) {
        char error_message[200];
        format_exec_error(&error_ret, error_message, sizeof(error_message));
        sprintf(buf, "Runtime Error: %s\n%s\n", "file", error_message);
        locate(1, 1);
        Print(buf);
    }

    machine_destroy(app);

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

