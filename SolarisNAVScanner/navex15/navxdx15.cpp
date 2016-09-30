#include "platform.h"
#include <stdio.h>
#include "avtypes.h"
#include "n30type.h"
#include "callbk15.h"
#include "navex15.h"

typedef EXTSTATUS (WINAPI *LPFUNC_QUERYINTERFACE)(DWORD,LPLPVOID);

void main(int argc, char **argv)
{
    DWORD dwAddress;
    LPFUNC_QUERYINTERFACE *fpfpQueryInterface;

    sscanf(argv[1],"%08lX",&dwAddress);
    fpfpQueryInterface  = (LPFUNC_QUERYINTERFACE *)dwAddress;
    *fpfpQueryInterface = EXTQueryInterface;
}
    


