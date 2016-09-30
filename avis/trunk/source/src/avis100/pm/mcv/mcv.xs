#ifdef __cplusplus
  extern "C" {
#endif

#include "extern.h"
#include "perl.h"
#include "xsub.h"

#ifdef __cplusplus
  }
#endif

#include "DocProp.h"

MODULE = Mcv  PACKAGE = Mcv
PROTOTYPES: DISABLE

int
TW()
CODE:
     RETVAL = TW;
OUTPUT:
RETVAL

int
JP()
CODE:
     RETVAL = JP;
OUTPUT:
RETVAL

int
KR()
CODE:
     RETVAL = KR;
OUTPUT:
RETVAL

int
CH()
CODE:
     RETVAL = CH;
OUTPUT:
RETVAL

int
IsWfw(fname)
char *fname
CODE:
     RETVAL = IsWfw(fname);
OUTPUT:
RETVAL

int
IsVBA5(fname)
char *fname
CODE:
     RETVAL = IsVBA5(fname);
OUTPUT:
RETVAL

int
IsXl(fname)
char *fname
CODE:
     RETVAL = IsXl(fname);
OUTPUT:
RETVAL

int
GetWfwCs(fname)
char *fname
CODE:
     RETVAL = GetWfwCs(fname);
OUTPUT:
RETVAL

int
GetWordDocVersion(fname)
char *fname
CODE:
     RETVAL = GetWordDocVersion(fname);
OUTPUT:
RETVAL

int
GetXlDocVersion(fname)
char *fname
CODE:
     RETVAL = GetXlDocVersion(fname);
OUTPUT:
RETVAL

int
IsWordDoc(fname)
char *fname
CODE:
     RETVAL = IsWordDoc(fname);
OUTPUT:
RETVAL

int
IsXlDoc(fname)
char *fname
CODE:
     RETVAL = IsXlDoc(fname);
OUTPUT:
RETVAL

int
WfwGetTemplateByte(fname)
char *fname
CODE:
     RETVAL = WfwGetTemplateByte(fname);
OUTPUT:
RETVAL

int
WfwSetTemplateByte(fname, value)
char *fname
int value
CODE:
     RETVAL = WfwSetTemplateByte(fname, value);
OUTPUT:
RETVAL

int
GetWfwDBCSCountryCode(fname)
char *fname
CODE:
     RETVAL = GetWfwDBCSCountryCode(fname);
OUTPUT:
RETVAL

