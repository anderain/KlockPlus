#include "fxlib.h"


int AddIn_main(int isAppli, unsigned short OptionNum)
{
    unsigned int key;

    Bdisp_AllClr_DDVRAM();

    locate(1,4);
    Print((unsigned char*)"This application is");
    locate(1,5);
    Print((unsigned char*)" sample Add-In.");

    while(1){
        GetKey(&key);
    }

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

