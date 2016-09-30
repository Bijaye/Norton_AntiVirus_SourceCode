//-----------------------------------------------------------------------------
// This file contains the function MKDIR_INOC_DB for the
// VxD platfrom.  For all other platforms, see dbcreat.asm.
//-----------------------------------------------------------------------------
// $Header:   S:/CERTLIBN/VCS/mkinocd.cpv   1.4   12 May 1997 12:34:52   MKEATIN  $
//-----------------------------------------------------------------------------
// $Log:   S:/CERTLIBN/VCS/mkinocd.cpv  $
// 
//    Rev 1.4   12 May 1997 12:34:52   MKEATIN
// Don't include avapi.h
// 
//    Rev 1.3   10 May 1996 20:45:26   YVANG
// DBCS enable.
// 
//    Rev 1.2   14 Jun 1995 09:17:54   RSTANEV
// Moved code/data to pageable segment.
//
//    Rev 1.1   08 Jun 1995 21:37:28   SZIADEH
// replace the call to FIL4_CREATE_DIR with DirMake()
// repalce the content of the loop.
//
//
//
//    Rev 1.0   06 Jun 1995 17:09:06   SZIADEH
// Initial revision.
//
//-----------------------------------------------------------------------------
#include "platform.h"
#include "ctsn.h"
#include "xapi.h"
#include "file.h"
#include "virscan.h"

#if defined(SYM_VXD)

#pragma VxD_PAGEABLE_CODE_SEG
#pragma VxD_PAGEABLE_DATA_SEG

//-----------------------------------------------------------------------------
// lpszFileName:  the path\filename using the current character set
//
//-----------------------------------------------------------------------------
BOOL WINAPI MKDIR_INOC_DB  (LPSTR  lpszFileName )
{

    auto CHAR   szOEMFileName [ SYM_MAX_PATH];
    auto LPSTR  lpszOEMFileName=szOEMFileName;
                                               // CCS: Current Character Set
    auto CHAR   szCCSFileName[2*SYM_MAX_PATH];
    auto LPSTR  lpszCCSFileName=szCCSFileName;
    auto DWORD  dwCharSet;

    dwCharSet = ((tcb_s*)Get_Cur_Thread_Handle())->TCB_Flags & THFLAG_CHARSET_MASK;

    VxDGenericStringConvert(lpszFileName, dwCharSet, lpszOEMFileName, THFLAG_OEM);

    if (IsUNCName( lpszOEMFileName ) )
        {
        lpszOEMFileName = UNC_SkipServerVolumeName ( lpszOEMFileName );
                                           // skip first '\' in "\path\filename"
        lpszOEMFileName +=1;
        }
    else
        {
                                           // skip over "Drive:\"
        lpszOEMFileName +=3;
        }

    while ( *lpszOEMFileName != NULL )
        {
                                      // search for the next backslash
        for (;*lpszOEMFileName != '\\' &&  
              *lpszOEMFileName != NULL;    
                  lpszOEMFileName = AnsiNext(lpszOEMFileName));
                                      // YVANG: search started at valid
                                      //   DBCS boundary, so AnsiNext()
                                      //   will handle pointer increment
                                      //   properly 

        if (*lpszOEMFileName != NULL )
            {
                                      // backsalsh is found replace it with
                                      // EOS
            *lpszOEMFileName='\0';
                                      // convert string back to current charset
                                      // note that szOEMFileName is used since
                                      // it points to the start of the string

            VxDGenericStringConvert((LPSTR)szOEMFileName, THFLAG_OEM,
                                           lpszCCSFileName, dwCharSet);

            //
            // DO NOT check for error when calling DirMake()
            // it could be the case that we're trying to create a subdir
            // in a dir that already exists and fail the first time around.
            // For example:
            // "C:\NAV\DIR" where "C:\NAV" already exists. The call to
            // create ("C:\NAV") will fail; however, the call to
            // create ("C:\NAV\DIR") succeedes.
            //

            DirMake ( lpszCCSFileName );

                                      // put backslash back and skip over it
            *lpszOEMFileName='\\';
            lpszOEMFileName++;
            }
        }

    return ( 1 );

}

#endif
