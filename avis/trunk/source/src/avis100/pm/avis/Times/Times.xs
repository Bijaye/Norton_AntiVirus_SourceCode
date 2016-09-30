#ifdef __cplusplus
extern "C" {
#endif
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#ifdef __cplusplus
}
#endif

#include "ProcTime.h"

MODULE = AVIS::Times		PACKAGE = AVIS::Times		

PROTOTYPES: ENABLE

void
times()
PROTOTYPE:
PPCODE:
double utime, stime, cutime, cstime;
avis_win32_times(&utime, &stime, &cutime, &cstime);
EXTEND (SP, 4);
PUSHs(sv_2mortal(newSVnv(utime)));
PUSHs(sv_2mortal(newSVnv(stime)));
PUSHs(sv_2mortal(newSVnv(cutime)));
PUSHs(sv_2mortal(newSVnv(cstime)));
