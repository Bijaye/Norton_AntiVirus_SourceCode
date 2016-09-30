// This is a temporary file to resolve the unresolved variables for
// the Quake library under OS/2


// These are defined before platform to avoid a conflict with
// a global extern declaration for them.
#define _F000H _XF000H
#define _0040H _X0040H

#include <platform.h>
#include <shellapi.h>                   // for HKEY only
#include <string.h>                     // for lstrcpy
#include <stdlib.h>                     // for _ltoa and _ultoa

#define MESSAGEBOX(s) MessageBox ( NULL, (LPCSTR)"Function temporarily stubbed for Mirrors port", (LPCSTR)(s), MB_OK )

#undef _F000H
#undef _0040H
// Protected mode violations will occur if one of these is
// accessed. 0xF000 is added t0 0x0040 to help make certain
// we do not accidently access a valid selector.
unsigned int _near _F000H = 0xF000;
unsigned int _near _0040H = 0xF040;

// Use of these must be checked and the code that is DOS
// version specific modified for use with OS/2 as necessary.

int _dosminor = 0;
int _dosmajor = 20;

// Undoc win 3.0, Doc win 3.1 function stubout -- resolve where called
// void WINAPI SetSelectorBase ( WORD sel, DWORD base )
UINT WINAPI SetSelectorBase ( UINT sel, DWORD base )
{
    MESSAGEBOX ("SetSelectorBase");

    return ( 0 );
}


// Undoc win 3.0, Doc win 3.1 function stubout -- resolve where called
UINT WINAPI SetSelectorLimit ( UINT sel, DWORD base )
{
    MESSAGEBOX ("SetSelectorLimit");

    return ( 0 );
}


// Undoc win 3.0, Doc win 3.1 function stubout -- resolve where called
DWORD WINAPI GetSelectorBase(UINT uInt)
{
    uInt;

    MESSAGEBOX ("GetSelectorBase");
    return ( 0L );
}

#if 0
// Doc win 3.1 function stubout -- resolve where called
LONG WINAPI RegQueryValue ( HKEY hkey, LPCSTR lpszSubKey, LPSTR lpszValue, LONG FAR *lpcb )
{
    MESSAGEBOX ("RegQueryValue");

    lstrcpy ( lpszValue, (LPSTR) "A fake value - seek fakeit.c" );
    *lpcb = 28;

    return ( (LONG) !ERROR_SUCCESS );
}


LONG WINAPI RegOpenKey ( HKEY hkey, LPCSTR lpszSubKey, HKEY FAR * lphkResult )
{
    MESSAGEBOX ("RegOpenKey");

    return ( (LONG) !ERROR_SUCCESS );
}


LONG WINAPI RegCloseKey ( HKEY hkey )
{
    MESSAGEBOX ("RegCloseKey");

    return ( (LONG) !ERROR_SUCCESS );
}
#endif

#if 0
LONG PASCAL DoEnvironmentSubst ( void )
{
    MESSAGEBOX ("DoEnvironmentSubst");

    return ( (LONG) !ERROR_SUCCESS );
}
#endif

char * _ultoa ( unsigned long l, char *psz, int i)
{
    return ( ltoa ( l, psz, i ));
}


