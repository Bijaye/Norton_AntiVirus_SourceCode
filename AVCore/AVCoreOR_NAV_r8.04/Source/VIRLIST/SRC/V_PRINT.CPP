// Copyright 1995 Symantec Corporation
//***************************************************************************
// 
// $Header:   S:/VIRLIST/VCS/v_print.cpv   1.5   31 Dec 1997 12:33:10   MDUNN  $ 
// 
// Description:                                                         
// 
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:/VIRLIST/VCS/v_print.cpv  $ 
// 
//    Rev 1.5   31 Dec 1997 12:33:10   MDUNN
// Port from Roswell (G branch): Fixed alloc/free code in CommentsLine() to
// fix the crash seen on NT when printing certain virus information.
// 
//    Rev 1.4   01 Mar 1996 12:01:38   JWORDEN
// DBCS enable
// 
//    Rev 1.2.1.0   12 Jul 1995 18:59:18   JREARDON
// Branch base for version QAK8
// 
//    Rev 1.2   05 Jul 1995 19:21:12   Sheathco
// Bug fix #38922 Corrects print out of 6 charactericts, and Common/Rare flag
// 
// 
//    Rev 1.1   23 Jun 1995 15:01:08   KEITH
// Tape on a header
//***************************************************************************

#include "platform.h"
#include "hstring.h"
#include "virusdbf.h"

#include "vlist.h"

#define CRLF "\r\n"
#define COMMENT_WIDTH   70
#define SIZE_WORK       256

extern   HINSTANCE   hInstance;

extern  int gwForPrintHack;

BOOL LOCAL AdjustMemoryIfRequired (     // Adjust Memory If Required
    HGLOBAL FAR * lphMem,               // pointer to Handle to memory
    HPSTR FAR *   hppMem,               // pointer to pointer to memory
    LPDWORD       lpdwMemSize,          // present size of memory
    LPDWORD       lpdwBytesLeft,        // bytes left in it
    UINT          uWeNeedThisMuch );    // how much more do we need

VOID LOCAL VirusNameLine (
    HWND    hDlg,
    HPSTR   hpStr,
    LPDWORD lpdwOffset );

VOID LOCAL VirusAliasLine (
    HWND    hDlg,
    HPSTR   hpStr,
    LPDWORD lpdwOffset );

VOID LOCAL LikelihoodLine (
    HWND    hDlg,
    HPSTR   hpStr,
    LPDWORD lpdwOffset );

VOID LOCAL VirusLengthLine (
    HWND    hDlg,
    HPSTR   hpStr,
    LPDWORD lpdwOffset );

VOID LOCAL TypeOfVirusLine (
    HWND    hDlg,
    HPSTR   hpStr,
    LPDWORD lpdwOffset );

VOID LOCAL FileInfectorLine (
    HWND    hDlg,
    HPSTR   hpStr,
    LPDWORD lpdwOffset );

VOID LOCAL GetMemoryResidentLine (
    HWND    hDlg,
    HPSTR   lpMem,
    LPDWORD lpdwOffset,
    BOOL    bMacType );

VOID LOCAL SizeStealthEncryptingLine (
    HWND    hDlg,
    HPSTR   hpStr,
    LPDWORD lpdwOffset );

VOID LOCAL FullStealthPolymorphicLine (
    HWND    hDlg,
    HPSTR   hpStr,
    LPDWORD lpdwOffset );

VOID LOCAL CommentsLine (
    HWND    hDlg,
    HPSTR   hpStr,
    LPDWORD lpdwOffset );

DWORD LOCAL BuildPrintHeader (          // Build virus information header
    HWND    hDlg,                       // [in] from this dialog box
    LPTSTR  lpMem );                    // [in] into this memory location

VOID LOCAL BuildRestOfVirusInfo (
    HWND    hDlg,                       // [in] from this dialog box
    HGLOBAL hMem,                       // [in] to this buffer
    DWORD   dwOffset,                   // [in] at this offset
    BOOL    bMacType );                 // [in] mac type of info

UINT LOCAL SetTab (                     // Fill with spaces upto the specified
    UINT    uCharInLine,                // position.
    HPSTR   hpMem,                      // [in] pointer to mem that should be filled
    UINT    uTab );                     // [in] to this tab position

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

HGLOBAL WINAPI BuildVirusInfo (         // Build virus information
    HWND    hDlg,                       // [in] dialog box handle that has info
    BOOL    bMacType )                  // [in] include mac type of virus info
{
    const   UINT        SIZE_PRINT_BUFFER = 10240;
    auto    HGLOBAL     hMem;
    auto    HPSTR       hpMem;
    auto    DWORD       dwOffset;

    hMem = MemAlloc( GHND, SIZE_PRINT_BUFFER );

    if ( hMem )
        {
        hpMem = (HPSTR)MemLock ( hMem );
        dwOffset = BuildPrintHeader( hDlg, hpMem );
        MemRelease ( hMem );

        BuildRestOfVirusInfo(           // Build virus information
                        hDlg,           // from this dialog box
                        hMem,           // memory where to build
                        dwOffset,       // starting at offset
                        bMacType );     // mac type info flag.

        MemRelease ( hMem );
        }

    return ( hMem );
}


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

DWORD LOCAL BuildPrintHeader (          // Build virus information header
    HWND    hDlg,                       // [in] from this dialog box
    LPTSTR  lpMem )                     // [in] into this memory location
{
    auto    DWORD   i;
    auto    UINT    uCopy;

   
    // ************************************ //
    // ***      Get Window Header       *** //
    // ************************************ //

    i = LoadString (hInstance, IDS_STARS, lpMem, SIZE_WORK);
    STRCPY (lpMem + i, CRLF);
    i += STRLEN (CRLF);

    uCopy = (COMMENT_WIDTH - GetWindowTextLength(hDlg)) / 2;
    i += SetTab(0, lpMem + i, uCopy);

    i += GetWindowText (hDlg, lpMem + i, SIZE_WORK);
    STRCPY (lpMem + i, CRLF);
    i += STRLEN (CRLF);
    i += LoadString (hInstance, IDS_STARS, lpMem + i, SIZE_WORK);

    STRCPY (lpMem + i, CRLF);
    i += STRLEN (CRLF);
    STRCPY (lpMem + i, CRLF);
    i += STRLEN (CRLF);
    STRCPY (lpMem + i, CRLF);
    i += STRLEN (CRLF);

    return ( i );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL BuildRestOfVirusInfo (
    HWND    hDlg,                       // [in] from this dialog box
    HGLOBAL hMem,                       // [in] to this buffer
    DWORD   dwOffset,                   // [in] at this offset
    BOOL    bMacType )                  // [in] mac type of info
{
    auto    DWORD       dwMemSize;
    auto    DWORD       dwBytesLeft;
    auto    HPSTR       hpMem;

    SYM_ASSERT ( hMem );
    dwMemSize = MemSize ( hMem );
    SYM_ASSERT ( dwMemSize );
    SYM_ASSERT ( dwMemSize > dwOffset );

    dwBytesLeft = dwMemSize - dwOffset;

    hpMem = (HPSTR)MemLock ( hMem );

    if ( hpMem )
        {
        int     nStep = 1;

        while ( nStep )
            {
            BOOL bStatus = AdjustMemoryIfRequired (
                            &hMem,          // Handle to memory
                            &hpMem,         // pointer to memory
                            &dwMemSize,     // present size of memory
                            &dwBytesLeft,   // bytes left in it
                            100             // how much more do we need
                            );

            if ( bStatus == TRUE )
                {
                switch ( nStep )
                    {
                    case 1:
                        {
                                        // -----------------------------------
                                        // LINE 1:  Virus Name
                                        // -----------------------------------
                        VirusNameLine (
                                      hDlg,
                                      hpMem,
                                      &dwOffset
                                      );
                        nStep ++;
                        break;
                        }

                    case 2:
                        {
                                        // -----------------------------------
                                        // LINE 2:  Virus Alias
                                        // -----------------------------------

                        VirusAliasLine (
                                       hDlg,
                                       hpMem,
                                       &dwOffset
                                       );
                        nStep ++;
                        break;
                        }

                    case 3:
                        {
                                        // -----------------------------------
                                        // LINE 3:  Type of files it infects
                                        // -----------------------------------
                        FileInfectorLine (
                                         hDlg,
                                         hpMem,
                                         &dwOffset
                                         );
                        nStep ++;
                        break;
                        }

                    case 4:
                        {
                                        // -----------------------------------
                                        // LINE 4:  Likelihood
                                        // -----------------------------------

                        LikelihoodLine (
                                       hDlg,
                                       hpMem,
                                       &dwOffset
                                       );
                        nStep ++;
                        break;
                        }

                    case 5:
                        {
                                        // -----------------------------------
                                        // LINE 5:  Virus Length
                                        // -----------------------------------
                        VirusLengthLine (
                                       hDlg,
                                       hpMem,
                                       &dwOffset
                                       );
                        nStep ++;
                        break;
                        }

                    case 6:
                        {
                                        // -----------------------------------
                                        // LINE 6:  Type of virus text
                                        // -----------------------------------
                        TypeOfVirusLine (
                                       hDlg,
                                       hpMem,
                                       &dwOffset
                                       );
                        nStep ++;
                        break;
                        }

                    case 7:
                        {
                                        // -----------------------------------
                                        // LINE 7:  Memory resident and 
                                        //          trigger line
                                        // -----------------------------------
                        GetMemoryResidentLine (
                                              hDlg,
                                              hpMem,
                                              &dwOffset,
                                              bMacType
                                              );
                        nStep ++;
                        break;
                        }

                    case 8:
                        {
                                        // -----------------------------------
                                        // LINE 8:  Size stealth and encrypting
                                        // -----------------------------------
                        SizeStealthEncryptingLine (
                                                  hDlg,
                                                  hpMem,
                                                  &dwOffset
                                                  );
                        nStep ++;
                        break;
                        }

                    case 9:
                        {
                                        // -----------------------------------
                                        // LINE 9:  Full stealth and 
                                        //          polymorphic line
                                        // -----------------------------------
                        FullStealthPolymorphicLine(
                                                  hDlg,
                                                  hpMem,
                                                  &dwOffset
                                                  );
                        nStep ++;
                        break;
                        }

                    case 10:
                        {
                                        // -----------------------------------
                                        // LINE 10:  Comments line
                                        // -----------------------------------
                        CommentsLine (
                                     hDlg,
                                     hpMem,
                                     &dwOffset
                                     );
                        nStep = 0;
                        break;
                        }
                    }
                }
            else
                break;
            }
        }
}


// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

UINT LOCAL SetTab (                     // Fill with spaces upto the specified
    UINT    uCharInLine,                // position.
    HPSTR   hpMem,                      // [in] pointer to mem that should be filled
    UINT    uTab )                      // [in] to this tab position
{
   UINT uChars = 0;

   uTab --;
   if (uCharInLine < uTab)
      {
      for (uChars = 0; uChars < (uTab - uCharInLine); uChars++)
         *hpMem++ = ' ';
      }

   return (uChars);
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

HGLOBAL SYM_EXPORT WINAPI BuildVirusListInfo (// Print the virus list
    HWND    hDlg,                       // [in] main dlg box handle
    HWND    hListBox )                  // [in] listbox handle
{
    auto    DWORD       dwSizePrintBuffer = 10240;
    auto    HGLOBAL     hMem;
	 auto    LPVIRUSDBF    lpVir;

    lpVir = (LPVIRUSDBF)GetWindowLong (hDlg, DWL_USER );
    SYM_ASSERT ( lpVir );
    hMem = MemAlloc( GHND, dwSizePrintBuffer );

    if ( hMem )
        {
        HPSTR   hpMem    = (HPSTR)MemLock (hMem);
        DWORD   dwOffset = BuildPrintHeader( hDlg, hpMem );



                                        // -----------------------------------
                                        // LINE 1:  Type of Viruses
                                        // -----------------------------------

        UINT uCharInLine = 0;
        UINT uLength = LoadString (
                                  hInstance,
                                  IDS_DISPLAY,
                                  &hpMem[ dwOffset ],
                                  SIZE_WORK
                                  );

        dwOffset    += uLength;
        uCharInLine += uLength;

        dwOffset += SetTab (
                           uCharInLine,
                           &hpMem[ dwOffset ],
                           12
                           );

        dwOffset += ComboBox_GetText ( GetDlgItem(hDlg, IDI_VL_COMBOX),
                                       &hpMem[ dwOffset ], SIZE_WORK );

        HSTRCPY ( &hpMem [ dwOffset ], CRLF );
        dwOffset += STRLEN (CRLF);
        HSTRCPY ( &hpMem [ dwOffset ], CRLF );
        dwOffset += STRLEN (CRLF);
                                        // -----------------------------------
                                        // LINE 2:  Virus Name Infects strings
                                        // -----------------------------------

        uCharInLine = 0;
        uLength = LoadString ( hInstance, IDS_VIRUS_NAME,
                               &hpMem[ dwOffset] , SIZE_WORK );

        dwOffset    += uLength;
        uCharInLine += uLength;

        dwOffset += SetTab (
                           uCharInLine,
                           &hpMem[ dwOffset ],
                           40
                           );

        dwOffset += GetWindowText ( GetDlgItem(hDlg, IDI_VL_INFTXT ),
                                    &hpMem[ dwOffset ], SIZE_WORK );

        HSTRCPY ( &hpMem[ dwOffset ], CRLF );
        dwOffset += STRLEN (CRLF);
        HSTRCPY ( &hpMem[ dwOffset ], CRLF );
        dwOffset += STRLEN (CRLF);
                                        // -----------------------------------
                                        // Here are the listbox items.
                                        // -----------------------------------

        UINT nNumItems = VListBox_GetCount ( hListBox );

        if ( nNumItems != LB_ERR && nNumItems )
            {
            TCHAR   szWork [ SYM_MAX_PATH + 1 ];

            for (UINT j = 0; j < nNumItems; j++)
                {
                DWORD   dwBytesLeft = dwSizePrintBuffer - dwOffset;

                AdjustMemoryIfRequired (
                                       &hMem,
                                       &hpMem,
                                       &dwSizePrintBuffer,
                                       &dwBytesLeft,
                                       1024
                                       );

                uCharInLine = 0;
                GetListBoxString(lpVir, j, szWork);
			
                for ( UINT k=0; szWork[k] != '\t'; k++, uCharInLine++ )
                    hpMem[ dwOffset++ ] = szWork[k];

                dwOffset += SetTab (
                                   uCharInLine,
                                   &hpMem [ dwOffset ],
                                   40
                                   );
                k++;

                while ( szWork[k] )
                    hpMem[ dwOffset++ ] = szWork[ k++ ];

                HSTRCPY ( &hpMem[ dwOffset ], CRLF);
                dwOffset += STRLEN (CRLF);
                }
            }

        HSTRCPY ( &hpMem[ dwOffset ], CRLF );
        dwOffset += STRLEN (CRLF);
        HSTRCPY ( &hpMem[ dwOffset ], CRLF );
        dwOffset += STRLEN (CRLF);

        MemRelease (hMem);
        }

   return ( hMem );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL GetMemoryResidentLine (
    HWND    hDlg,
    HPSTR   hpMem,
    LPDWORD lpdwOffset,
    BOOL    bMacType )
{

                                        // ----------------------------------
                                        // Show this line only if it pertains
                                        // to a MAC virus
                                        // ----------------------------------
    
    if ( bMacType == TRUE )
        {
        UINT uLength = GetWindowText (
                                     GetDlgItem(hDlg, IDI_VI_MACVIRTXT),
                                     &hpMem[ *lpdwOffset ],
                                     SIZE_WORK
                                     );
        UINT uCharInLine = uLength;
        *lpdwOffset += uLength;

        uLength = SetTab (
                         uCharInLine,
                         &hpMem[ *lpdwOffset ],
                         20
                         );

        *lpdwOffset += uLength;
        uCharInLine += uLength;

        if ( gwForPrintHack & bitINFO_INTHEWILD)
            {
            *lpdwOffset += LoadString (
                                      hInstance,
                                      IDS_YES,
                                      &hpMem[ *lpdwOffset ],
                                      SIZE_WORK
                                      );
            }
        else
            {
            *lpdwOffset += LoadString (
                                      hInstance,
                                      IDS_NO,
                                      &hpMem[ *lpdwOffset ],
                                      SIZE_WORK
                                      );
            }
        }
    else
        {
        UINT uCharInLine = 0;
        UINT uLength = GetWindowText (
                                     GetDlgItem (hDlg, IDI_VI_MEMRESTXT),
                                     &hpMem[ *lpdwOffset ],
                                     SIZE_WORK
                                     );
        *lpdwOffset += uLength;
        uCharInLine += uLength;

        uLength = SetTab( uCharInLine, &hpMem[ *lpdwOffset ], 20 );

        *lpdwOffset += uLength;
        uCharInLine += uLength;

        if ( gwForPrintHack & bitINFO_RESIDENT)
            {
            uLength = LoadString (
                                 hInstance,
                                 IDS_YES,
                                 &hpMem[ *lpdwOffset ],
                                 SIZE_WORK
                                 );
            }
        else
            {
            uLength = LoadString (
                                 hInstance,
                                 IDS_NO,
                                 &hpMem[ *lpdwOffset ],
                                 SIZE_WORK
                                 );
            }

        *lpdwOffset += uLength;
        uCharInLine += uLength;

        uLength = SetTab (
                         uCharInLine,
                         &hpMem[ *lpdwOffset ],
                         27
                         );

        *lpdwOffset += uLength;
        uCharInLine += uLength;

        // ************************************************************ //

        uLength = GetWindowText ( GetDlgItem(hDlg, IDI_VI_OVERWRTXT),
                                  &hpMem[ *lpdwOffset ], SIZE_WORK );

        *lpdwOffset += uLength;
        uCharInLine += uLength;

        uLength = SetTab (
                         uCharInLine,
                         &hpMem[ *lpdwOffset ],
                         43
                         );
    
        *lpdwOffset += uLength;
        uCharInLine += uLength;

          if(gwForPrintHack & bitINFO_TRIGGERED)
            {
            uLength = LoadString (
                                 hInstance,
                                 IDS_YES,
                                 &hpMem[ *lpdwOffset ],
                                 SIZE_WORK
                                 );
            }
        else
            {
            uLength = LoadString (
                                 hInstance,
                                 IDS_NO,
                                 &hpMem[ *lpdwOffset ],
                                 SIZE_WORK
                                 );
            }

        *lpdwOffset += uLength;
        uCharInLine += uLength;

        uLength = SetTab (
                         uCharInLine,
                         &hpMem[ *lpdwOffset ],
                         51
                         );

        *lpdwOffset += uLength;
        uCharInLine += uLength;

        STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
        *lpdwOffset += STRLEN (CRLF);
        }
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL SizeStealthEncryptingLine (
    HWND    hDlg,
    HPSTR   hpMem,
    LPDWORD lpdwOffset )
{
    UINT uCharInLine = 0;
    UINT uLength = GetWindowText (
                                 GetDlgItem ( hDlg, IDI_VI_STEALTHTXT ),
                                 &hpMem[ *lpdwOffset ],
                                 SIZE_WORK
                                 );

    *lpdwOffset += uLength;
    uCharInLine += uLength;

    uLength = SetTab(
                    uCharInLine,
                    &hpMem[ *lpdwOffset],
                    20
                    );

    *lpdwOffset += uLength;
    uCharInLine += uLength;

       if(gwForPrintHack & bitINFO_SIZESTLTH)
        {
        uLength = LoadString (
                             hInstance,
                             IDS_YES,
                             &hpMem[ *lpdwOffset ],
                             SIZE_WORK
                             );
        }
    else 
        {
        uLength = LoadString (
                             hInstance,
                             IDS_NO,
                             &hpMem[ *lpdwOffset ],
                             SIZE_WORK
                             );
        }

    *lpdwOffset += uLength;
    uCharInLine += uLength;

    uLength = SetTab(
                    uCharInLine,
                    &hpMem[ *lpdwOffset ],
                    27
                    );

    *lpdwOffset += uLength;
    uCharInLine += uLength;

    // ************************************************************ //
    
    uLength = GetWindowText ( GetDlgItem( hDlg, IDI_VI_COMPTXT ),
                              &hpMem[ *lpdwOffset ], SIZE_WORK );
    *lpdwOffset += uLength;
    uCharInLine += uLength;

    uLength = SetTab(
                    uCharInLine,
                    &hpMem[ *lpdwOffset ],
                    43
                    );

    *lpdwOffset += uLength;
    uCharInLine += uLength;

      if( gwForPrintHack & bitINFO_ENCRYPT)
        {
        uLength = LoadString (
                             hInstance,
                             IDS_YES,
                             &hpMem[ *lpdwOffset ],
                             SIZE_WORK
                             );
        }
    else 
        {
        uLength = LoadString (
                             hInstance,
                             IDS_NO,
                             &hpMem[ *lpdwOffset ],
                             SIZE_WORK
                             );
        }

    *lpdwOffset += uLength;
    uCharInLine += uLength;

    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);
}


// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL FullStealthPolymorphicLine (
    HWND    hDlg,
    HPSTR   hpMem,
    LPDWORD lpdwOffset )
{
    UINT uCharInLine = 0;
    UINT uLength = GetWindowText ( GetDlgItem(hDlg, IDI_VI_SLFENCRTXT),
                                   &hpMem[ *lpdwOffset ], SIZE_WORK );
    *lpdwOffset += uLength;
    uCharInLine += uLength;

    uLength = SetTab(
                    uCharInLine,
                    &hpMem[ *lpdwOffset ],
                    20
                    );

    *lpdwOffset += uLength;
    uCharInLine += uLength;

        if(gwForPrintHack & bitINFO_FULLSTLTH)
        {
        uLength = LoadString (
                             hInstance,
                             IDS_YES,
                             &hpMem[ *lpdwOffset ],
                             SIZE_WORK
                             );
        }
    else 
        {
        uLength = LoadString (
                             hInstance,
                             IDS_NO,
                             &hpMem[ *lpdwOffset ],
                             SIZE_WORK
                             );
        }


    *lpdwOffset += uLength;
    uCharInLine += uLength;

    uLength = SetTab(
                    uCharInLine,
                    &hpMem[ *lpdwOffset ],
                    27
                    );

    *lpdwOffset += uLength;
    uCharInLine += uLength;

    // ************************************************************ //

    uLength = GetWindowText ( GetDlgItem(hDlg, IDI_VI_PARASTXT),
                              &hpMem[ *lpdwOffset ], SIZE_WORK );

    *lpdwOffset += uLength;
    uCharInLine += uLength;

    *lpdwOffset += SetTab(
                         uCharInLine,
                         &hpMem[ *lpdwOffset],
                         43
                         );

        if( gwForPrintHack & bitINFO_POLYMORPH)
        {
        *lpdwOffset += LoadString (
                             hInstance,
                             IDS_YES,
                             &hpMem[ *lpdwOffset ],
                             SIZE_WORK
                             );
        }
    else 
        {
        *lpdwOffset += LoadString (
                             hInstance,
                             IDS_NO,
                             &hpMem[ *lpdwOffset ],
                             SIZE_WORK
                             );
        }

    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL CommentsLine (
    HWND    hDlg,
    HPSTR   hpMem,
    LPDWORD lpdwOffset )
{
    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);

    *lpdwOffset += GetWindowText ( GetDlgItem(hDlg, IDI_VI_COMMGRP),
                                   &hpMem[ *lpdwOffset ], SIZE_WORK );

    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);

                                        // ----------------------------------
                                        // We chop the comment up into bite 
                                        // sizes if required
                                        // ----------------------------------

    int nTextLength = GetWindowTextLength( GetDlgItem(hDlg, IDI_VI_COMMENT) );

    if ( nTextLength > ( COMMENT_WIDTH-1 ) )
        {
        nTextLength--;

        LPTSTR lpBuffer = (LPTSTR)MemAllocPtr(GHND, nTextLength + 10);
        LPTSTR lpComment = lpBuffer;

        if ( lpComment )
            {
            int     nCopy;
            char    szWork [ SYM_MAX_PATH ];

            GetWindowText ( GetDlgItem(hDlg, IDI_VI_COMMENT), lpComment,
                            nTextLength + 10);

            while ( nTextLength )
                {
                MEMSET ( szWork, EOS, sizeof(szWork) );

                if ( (int)(nTextLength - COMMENT_WIDTH) > 0 )
                    nCopy = COMMENT_WIDTH;
                else
                    nCopy = nTextLength;

                MEMCPY ( szWork, lpComment, nCopy );

                if ( nCopy != nTextLength )
                    {
                    for ( int j = STRLEN(szWork); szWork[j] != ' ' && j; j-- )
                        ;

                    nCopy -= ( STRLEN( szWork ) - j );
                    }

                if ( *szWork == ' ' )
                    {
                    MEMCPY ( &hpMem[ *lpdwOffset], &szWork[1], (nCopy-1) );
                    *lpdwOffset += (nCopy-1);
                    }
                else
                    {
                    MEMCPY ( &hpMem[ *lpdwOffset ], szWork, nCopy);
                    *lpdwOffset += nCopy;
                    }

                nTextLength -= nCopy;
                lpComment   += nCopy;
             
                STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
                *lpdwOffset += STRLEN (CRLF);
                }

            MemFreePtr( lpBuffer );
            }
        }
    else
        {
        *lpdwOffset += GetWindowText ( GetDlgItem(hDlg, IDI_VI_COMMENT),
                                       &hpMem[ *lpdwOffset ], SIZE_WORK );
        }

    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);

    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);
}


// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL VirusNameLine (
    HWND    hDlg,
    HPSTR   hpMem,
    LPDWORD lpdwOffset )
{
    UINT uCharInLine = 0;
    UINT uLength = GetWindowText (
                        GetDlgItem(hDlg, IDI_VI_VNAMETXT),
                        &hpMem[ *lpdwOffset ],
                        SIZE_WORK );
    
    *lpdwOffset += uLength;
    uCharInLine += uLength;

    *lpdwOffset += SetTab(
                         uCharInLine,
                         &hpMem[ *lpdwOffset ],
                         15
                         );

    *lpdwOffset += GetWindowText (
                        GetDlgItem(hDlg, IDI_VI_VNAME),
                        &hpMem[ *lpdwOffset ],
                        SIZE_WORK
                        );

    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL VirusAliasLine (
    HWND    hDlg,
    HPSTR   hpMem,
    LPDWORD lpdwOffset )
{
    UINT uCharInLine = 0;
    UINT uLength = GetWindowText (
                        GetDlgItem(hDlg, IDI_VI_ALIASTXT),
                        &hpMem[ *lpdwOffset ],
                        SIZE_WORK
                        );
    
    *lpdwOffset += uLength;
    uCharInLine += uLength;

    *lpdwOffset += SetTab(
                         uCharInLine,
                         &hpMem[ *lpdwOffset ],
                         15
                         );

    *lpdwOffset += GetWindowText (
                        GetDlgItem(hDlg, IDI_VI_ALIAS),
                        &hpMem[ *lpdwOffset ],
                        SIZE_WORK
                        );

    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL FileInfectorLine (
    HWND    hDlg,
    HPSTR   hpMem,
    LPDWORD lpdwOffset )
{
    UINT uCharInLine = 0;
    UINT uLength = GetWindowText (
                        GetDlgItem(hDlg, IDI_VI_INFECTSTXT),
                        &hpMem[ *lpdwOffset ],
                        SIZE_WORK );

    *lpdwOffset += uLength;
    uCharInLine += uLength;

    *lpdwOffset += SetTab( uCharInLine, &hpMem[ *lpdwOffset ], 15 );

    *lpdwOffset += GetWindowText (
                        GetDlgItem(hDlg, IDI_VI_INFECTS),
                        &hpMem[ *lpdwOffset ],
                        SIZE_WORK );

    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL LikelihoodLine (
    HWND    hDlg,
    HPSTR   hpMem,
    LPDWORD lpdwOffset )
{
    UINT uCharInLine = 0;
    UINT uLength = GetWindowText (
                                 GetDlgItem(hDlg, IDI_VI_LIKETXT),
                                 &hpMem[ *lpdwOffset ],
                                 SIZE_WORK
                                 );
    *lpdwOffset += uLength;
    uCharInLine += uLength;

    *lpdwOffset += SetTab(
                         uCharInLine,
                         &hpMem[ *lpdwOffset ],
                         15
                         );

    *lpdwOffset += GetWindowText (
                        GetDlgItem(hDlg, IDI_VI_LIKE),
                        &hpMem[ *lpdwOffset ],
                        SIZE_WORK
                        );

    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);
}

VOID LOCAL VirusLengthLine (
    HWND    hDlg,
    HPSTR   hpMem,
    LPDWORD lpdwOffset )
{
    UINT uCharInLine = 0;
    UINT uLength = GetWindowText (
                                 GetDlgItem(hDlg, IDI_VI_LENGTHTXT),
                                 &hpMem[ *lpdwOffset ],
                                 SIZE_WORK
                                 );

    *lpdwOffset += uLength;
    uCharInLine += uLength;

    *lpdwOffset += SetTab(
                         uCharInLine,
                         &hpMem[ *lpdwOffset ],
                         15
                         );
    
    *lpdwOffset += GetWindowText (
                        GetDlgItem(hDlg, IDI_VI_LENGTH),
                        &hpMem[ *lpdwOffset ],
                        SIZE_WORK
                        );

    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);
}

VOID LOCAL TypeOfVirusLine (
    HWND    hDlg,
    HPSTR   hpMem,
    LPDWORD lpdwOffset )
{
    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);

    *lpdwOffset += GetWindowText (
                                 GetDlgItem(hDlg, IDI_VI_TYPEGRP),
                                 &hpMem[ *lpdwOffset ],
                                 SIZE_WORK
                                 );

    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);
    STRCPY ( &hpMem[ *lpdwOffset ], CRLF );
    *lpdwOffset += STRLEN (CRLF);
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

BOOL LOCAL AdjustMemoryIfRequired (     // Adjust Memory If Required
    HGLOBAL FAR * lphMem,               // pointer to Handle to memory
    HPSTR FAR *   hppMem,               // pointer to pointer to memory
    LPDWORD       lpdwMemSize,          // present size of memory
    LPDWORD       lpdwBytesLeft,        // bytes left in it
    UINT          uWeNeedThisMuch )     // how much more do we need
{
    const   UINT    uIncrementSize = 10240;
    auto    BOOL    bReturn = TRUE;

    if ( *lpdwBytesLeft <= uWeNeedThisMuch )
        {
        MemRelease ( *lphMem );
        *(hppMem) = NULL;

        DWORD   dwMemSize = *lpdwMemSize + uIncrementSize;

        HGLOBAL hNewMem = MemReAlloc (*lphMem, dwMemSize, GHND );

        if ( hNewMem )
            {
            *lphMem = hNewMem;
            *lpdwMemSize = dwMemSize;
            *lpdwBytesLeft += uIncrementSize;
            *hppMem = (HPSTR)MemLock ( hNewMem );
            }
        else
            {
            bReturn = FALSE;
            }
        }

    return ( bReturn );
}
