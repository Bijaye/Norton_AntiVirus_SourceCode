// Copyright 1997 Symantec
// -------------------------------------------------------------------------
// EXPORTS.CPP
// Contains exported entry points for this sample Standard About extension page
// These are the standard COM exports necessary for
// self register (DllRegisterServer), unregister (DllUnregisterServer),
// and object creation (DllGetClassObject).
//
// There is also a helper C export to invoke the standard about COM interface (DoStdAbout).
//
// $Header:   S:/NAVABOUT/VCS/exports.cpv   1.41   21 Oct 1998 15:14:16   rchinta  $
// -------------------------------------------------------------------------
// $Log:   S:/NAVABOUT/VCS/exports.cpv  $
// 
//    Rev 1.41   21 Oct 1998 15:14:16   rchinta
// For corporate mode, disabling registration
// 
//    Rev 1.40   13 Oct 1998 22:22:28   rchinta
// Removed the SystemInfo Extension for the OEM version.
// 
//    Rev 1.39   13 Oct 1998 12:54:24   JTaylor
// removed sdp hack.
// 
//    Rev 1.37   09 Oct 1998 16:51:18   JTaylor
// Removed temp SDP hack
// 
//    Rev 1.35   21 Sep 1998 18:07:00   rchinta
// Changed the PublicVersion to 5.01 and PublicRevision to 5.01.00 to match
// the version.dat
// 
//    Rev 1.34   04 Sep 1998 06:38:10   rchinta
// Added generic OEM title
// 
//    Rev 1.33   28 Jul 1998 19:06:00   mdunn
// Added a few new folks to the easter egg name list.
// 
//    Rev 1.32   23 Jul 1998 14:37:08   JBRENNA
// Correct spelling on Tim's name and add the nicknames back.
// 
//    Rev 1.31   21 Jul 1998 17:42:28   jtaylor
// Updated user registration information to come from HKLM in corporate mode and HKCU in retail mode.
//
//    Rev 1.30   20 Jul 1998 14:51:20   mdunn
// Added Sheelagh's name.
//
//    Rev 1.29   17 Jul 1998 23:03:40   mdunn
// :)
//
//    Rev 1.28   16 Jul 1998 09:09:04   JBRENNA
// Add Kurt Weber to SARC credits.
//
//    Rev 1.27   15 Jul 1998 20:08:12   JBRENNA
// Change the credits hot-key to CTRL+SHIFT+ALT+F+B.
//
//    Rev 1.26   15 Jul 1998 16:09:34   JBRENNA
// Add 3 QA names as per Paul's request.
//
//    Rev 1.25   15 Jul 1998 12:12:14   JBRENNA
// Updated to include SARC QA in the SARC list.
//
//    Rev 1.24   15 Jul 1998 08:48:30   JBRENNA
// Update the SARC dev credits.
//
//    Rev 1.23   14 Jul 1998 15:16:28   JBRENNA
// Update the credits for Atomic.
//
//    Rev 1.22   24 Jun 1998 12:50:30   mdunn
// Added #include's of slhelper.h and sloldmap.h to fix breakages caused by
// type names that changed from release 005 to 007.
//
//    Rev 1.21   19 May 1998 15:20:42   DBuches
// Added sysinfo tab to about box.
//
//    Rev 1.20   30 Apr 1998 21:31:48   mdunn
// Now reading the public version and revision from VERSION.DAT.
//
//    Rev 1.19   28 Apr 1998 19:27:46   mdunn
// Added support for reading the version number from VERSION.DAT in the NAV
// install dir, and added Win 98 detection to the code that selects the
// string to display as the OS description.
//
//    Rev 1.18   17 Apr 1998 16:15:18   RStanev
// SmellyCat changes.
//
//    Rev 1.17   06 Mar 1998 16:39:26   JTaylor
// Changed the virus definitions date to show the definitions currently used by NAVW.  They used to show the newest defs on the system.
//
//    Rev 1.16   17 Feb 1998 10:57:30   mdunn
// Updated the version number to 5.00, and removed the accent from Rene
// Visser's name since that character won't display properly on Asian-
// language Windows.
//
//    Rev 1.15   17 Dec 1997 20:30:56   jtaylor
// Added Mark S. to the credits.
//
//    Rev 1.14   08 Dec 1997 16:43:04   jtaylor
// Changed the product name in the version information to NAV for scanner.(switched on Personality DLL)
//
//    Rev 1.13   08 Dec 1997 15:54:58   jtaylor
// Made changes required to upgrade to vc 5.0 and coresdk.005
//
//    Rev 1.12   07 Dec 1997 15:52:02   jtaylor
// Added an extra flag to DoStdAbout that prevents Registration for scanner
// only products.
//
//    Rev 1.11   04 Sep 1997 17:29:24   KSACKIN
// Added "Sparky".
//
//    Rev 1.10   02 Sep 1997 11:47:36   KSACKIN
// Fixed a name change for the credits.
//
//    Rev 1.9   28 Aug 1997 11:31:28   KSACKIN
// Updated the credits text to reflect a couple of name changes.
//
//    Rev 1.8   23 Aug 1997 14:18:18   KSACKIN
// Fixed a check for our U F and O atoms being NULL before we actually delete
// them.
//
//    Rev 1.7   23 Aug 1997 14:14:56   KSACKIN
// Added the hot key support back in, but now we need to press Ctrl-Alt-Shift
// before our easter egg, or our credits.  Ctrl-V still does our version
// information.
//
//    Rev 1.6   23 Aug 1997 13:53:30   KSACKIN
// Removed the HotKey support from the about box.  We now show version
// information all of the time, and now only on Ctrl-V.  We also no longer call
// to our credits or gang screen.  This needs to be fixed soon.
//
//    Rev 1.5   22 Aug 1997 19:30:08   KSACKIN
// Added the Nav Credits as a NULL terminated string list.  Now, when we go to
// display the credits, we format a string based on this credit list.
//
//    Rev 1.4   21 Aug 1997 01:52:00   KSACKIN
// Added OS and version to the about box.
//
//    Rev 1.3   20 Aug 1997 12:56:52   KSACKIN
// Fixed a problem with loading the NT Workstation string if we detected that we
// are running NT Server.  We now load the correct string.
//
//    Rev 1.2   20 Aug 1997 11:14:04   KSACKIN
// Now, if you request version information, we stop our scrolling credits.
//
//    Rev 1.1   20 Aug 1997 11:02:14   KSACKIN
// Added NAV version information and build and part number to the about box.
//
//    Rev 1.0   19 Aug 1997 14:09:46   KSACKIN
// Initial revision.
//
//    Rev 1.10.1.3   19 Aug 1997 12:31:30   KSACKIN
// Added HotKey support for easter egg and version information.
//
//    Rev 1.10.1.2   14 Aug 1997 23:06:58   KSACKIN
// Added a call to SetImage() before invoking the about box.  We now load our
// custom about bitmap first, and then hand that off to the StdAbout box.
//
//    Rev 1.10.1.1   14 Aug 1997 11:47:06   KSACKIN
// Added a parameter to VLRegisterVirusDB.  This parameter dictates the location
// of the NavEx15.inf file.  Since we are only using the data base to retrieve
// the number of virus definitions, we are passing this as NULL so that we don't
// have to retrieve this file's location as well.
//
//    Rev 1.10.1.0   12 Aug 1997 22:44:52   DALVIDR
// Branch base for version QAKG
//
//    Rev 1.10   08 Aug 1997 15:03:24   KSACKIN
// Changed displayed version number back to 4.0.  We no longer have a need to
// have different version numbers internally and externally.
//
//    Rev 1.9   31 Jul 1997 16:05:24   KSACKIN
// Fixed our Internal verison information string to be 5.0, and our external
// version is 4.0.
//
//    Rev 1.8   30 Jul 1997 20:58:24   KSACKIN
// Commented out the code for the registering of the AboutBox COM object.  WE
// no longer need this, as we are just using the standard about box interface
// and adding some custom text.
//
//    Rev 1.7   22 Jul 1997 17:36:52   DBUCHES
// Added support for retrieving SKU from PartNum.Dat file.
//
//    Rev 1.6   08 Jul 1997 10:00:20   TIVANOV
// change the version to 4.0
//
//    Rev 1.5   02 Jul 1997 15:49:38   TIVANOV
// changed the reg version number
//
//    Rev 1.4   01 Jul 1997 11:18:54   TIVANOV
// change the version to 4.0
//
//    Rev 1.3   01 Jul 1997 10:19:30   TIVANOV
// change the product verstion to 4.0
//
//    Rev 1.2   09 Jun 1997 20:09:58   TIVANOV
// move the reg key string to a define and make the dllunregserver use it
//
//    Rev 1.1   09 Jun 1997 19:53:02   TIVANOV
// change the reg key string and make the prop page to apear on the local produc
// t NAV about box only
//
//    Rev 1.0   06 Jun 1997 20:01:14   BMCCORK
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "symlean.h"

// Initialize GUIDS
#pragma data_seg( ".text" )
#define INITGUID
#include <initguid.h>
#include <cguid.h>
#include "regint.h"
#include "aboutint.h"
#include "slhelper.h"
#include "sloldmap.h"
#include "navabout.h"				    // extension object
#include "regguid.h"					// class ID's for on-line reg.
//#undef INITGUID
#pragma data_seg()

#undef  SYM_DEBUG
#include "defutils.h"                   // for NavVDefs* functions.
#include "avapi.h"
#include "xapi.h"
#define SYM_DEBUG

#include "resource.h"
#include "navdoabt.h"                   // Prototype for DoStdAbout() global C helper function

#include "n32behav.h"
#include "navver.h"

#include "DefAnnuity.h"

///////////////////////////////////////
// Global Variables

LONG g_cLock = 0;						// Used by LockContainer

#define REGSTR_ABOUT_NAV _T("Software\\Symantec\\Norton AntiVirus\\5.02\\English\\")
const LPCTSTR szCompany  = _T("Symantec");
const LPCTSTR szProduct  = _T("Norton AntiVirus");
const LPCTSTR szProductOEM  = _T("Norton AntiVirus (OEM)");
const LPCTSTR szLanguage = _T("English");

                                        // szDefaultPublicRevision and
                                        // szDefaultPublicVersion are only
                                        // used as a defaults for
                                        // GetPrivateProfileString.  When
                                        // doing an inline, you *MUST* update
                                        // these to match VERSION.DAT!!

const LPCTSTR szDefaultPublicRevision  = _T("5.02.00");
const LPCTSTR szDefaultPublicVersion   = _T("5.02");



LPTSTR g_szzNavCredits [] = {
	_T("Product Management"),
	_T("   Betsy Baker"),
	_T("   Steve Cullen"),
	_T("   Lily De Los Rios"),
	_T("   Marian Merritt"),
	_T("   John Moldenhauer"),
	_T("   Robert Pettit"),
	_T("   Sharon Ruckman"),
	_T("   Jack Tang"),
    _T(""),
	_T("Program Management"),
	_T("   Brian Foster"),
	_T("   Francia Saplala"),
	_T("   Gary Westerland"),
    _T(""),
	_T("Software Development"),
	_T("   David \"Davros\" Allee"),
	_T("   Jim Brennan"),
	_T("   David \"Notorious\" Buches"),
	_T("   Tim Cashin"),
	_T("   Michael \"DDS\" Dunn"),
	_T("   Scott Edwards"),
	_T("   Barry Gerhardt"),
	_T("   David Hertel"),
	_T("   Michael Keating"),
	_T("   John Millard"),
	_T("   David Shannon"),
	_T("   Radoslav Stanev"),
	_T("   Jacob Taylor"),
    _T(""),
	_T("Quality Assurance"),
	_T("   Kerry Boyte"),
	_T("   Paul \"FB\" Davis"),
	_T("   Rob Ficcaglia"),
	_T("   Debora Ka"),
	_T("   Olga Komsitsky"),
	_T("   Bob Kolosky"),
	_T("   Marc Marcuse"),
	_T("   Melissa Mendonca"),
	_T("   Chris Nevarez"),
	_T("   Greg \"Atomic\" Patterson"),
	_T("   Garret Polk "),
	_T("   Ellis Rahhal"),
	_T("   Daniel Uyemura"),
	_T("   Jim Waggoner"),
	_T("   Carlin Wong"),
    _T(""),
	_T("SWAT Team"),
	_T("   Jim Belden"),
	_T("   Stanley Ballenger"),
	_T("   Rekha Chintalacharuvu"),
	_T("   Chuck Eaton"),
	_T("   Nigel Gallagher"),
	_T("   Vance Gloster"),
	_T("   Igor Goldshteyn"),
	_T("   Rion Millen"),
	_T("   Rajesh Pulinthanatu"),
	_T("   Dan Sackinger"),
	_T("   Mark Santhasouk"),
	_T("   Francia Saplala"),
	_T("   Scott Smith"),
	_T("   P. Venkatraman"),
	_T("   Mark Zaremba"),
    _T(""),
	_T("Shared Tech"),
	_T("   Cameron Bigger"),
	_T("   Don Carver"),
	_T("   Leo Cohen"),
	_T("   Alireza Faroush"),
	_T("   Ben Hallert"),
	_T("   Bruce Hellstrom"),
	_T("   Roger Holman"),
	_T("   Jim Lamb"),
	_T("   Jim Lucan"),
	_T("   Bryan Martin"),
	_T("   Patrick Martin"),
	_T("   Bruce McCorkendale"),
	_T("   Eoghan O'Donnell"),
	_T("   Heath Perryman"),
	_T("   Sam Porterfield"),
	_T("   Mark Spiegel"),
	_T("   Peter Van Nuys"),
	_T("   Oleg Volochtchouk"),
    _T(""),
	_T("Documentation and Online Help"),
	_T("   Elizabeth Anders"),
	_T("   Alred Ghadimi"),
	_T("   Karen Goldsmith"),
	_T("   Robert Hoffman"),
	_T("   Romey Keys"),
	_T("   Robert Squires"),
    _T(""),
	_T("Technical Support"),
	_T("   Beth Bryant"),
	_T("   Earl Campbell"),
	_T("   Brian Cochrane"),
	_T("   Erich Dobroth"),
	_T("   Christine Frazer"),
	_T("   Rochelle Hamlett"),
	_T("   Pamela Heine"),
	_T("   Ty Hodson"),
	_T("   Todd Kieser"),
	_T("   Carrie Kelso"),
	_T("   Danny Krautschek"),
	_T("   Tyrone McDermott"),
	_T("   Deborah Light"),
	_T("   David Lucas"),
	_T("   Russell Lusignan"),
	_T("   Dan Pederson"),
	_T("   LaVonne Perry"),
	_T("   Leon Plueard"),
	_T("   Carol Simmonds"),
	_T("   Matthew Smith"),
	_T("   Olga Stamatiou"),
	_T("   Chris Steele"),
	_T("   Joe Trozelle"),
	_T("   Joe Waisman"),
	_T("   Robert Walling"),
	_T("   Jason Walsh"),
	_T("   Gwen White"),
	_T("   Ken Zug"),
    _T(""),
	_T("Engineering Services Team"),
	_T("   Stephen Blackmoore"),
	_T("   Emma Fielding"),
	_T("   Sara Flood"),
	_T("   Renal Fuller"),
	_T("   Pat Hoed"),
	_T("   Will Jobe"),
	_T("   Helen Kim"),
	_T("   Sebastian Lai"),
	_T("   Dave Lewis"),
	_T("   Scott Mynatt"),
	_T("   Reiko Nozawa"),
	_T("   Sheelagh O'Connor "),
	_T("   Ed Thompson"),
	_T("   Dwight Wilson"),
    _T(""),
	_T("Symantec AntiVirus Research Center (SARC)"),
	_T("   David Banes"),
	_T("   Diop Bankole"),
	_T("   Frank Barajas"),
	_T("   Sherralee Buzzell"),
	_T("   Matt Candelaria"),
	_T("   Darren Chi"),
	_T("   Eric Chien"),
	_T("   Steven Chu"),
	_T("   Philip DeBats"),
	_T("   Raul Elnitiarta"),
	_T("   Chris Formulak"),
	_T("   Yuji Hoshizawa"),
	_T("   Henry Jalandoni"),
	_T("   Tigran Khanpapyan"),
	_T("   Darren Kessner"),
	_T("   Maryl Magee"),
	_T("   Carey Nachenberg"),
	_T("   Cary Ng"),
	_T("   Abid Oonwala"),
	_T("   Peter Pak"),
	_T("   Charles Renert"),
	_T("   Ian Roessle"),
	_T("   Steve Trilling"),
	_T("   Rene Visser"),
	_T("   Kurt Weber"),
	_T("   John Wilber"),
	_T("   Motoaki Yamamura"),
    NULL
};



HINSTANCE ghInst;

#if defined (WIN32)
    #define IS_WIN32 TRUE
#else
    #define IS_WIN32 FALSE
#endif

#define IS_NT      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)


/////////////////////////////////////////////////////////////////////////////
// Local functions
BOOL PASCAL GetPartNumber ( LPTSTR szPartNum );
BOOL PASCAL GetDatFileName ( LPTSTR szFileName );
LPTSTR PASCAL FindFileName (LPCTSTR lpName, BOOL bSkipSlash);
DWORD GetDefinitionDate(LPTSTR lpDateStr, int size);
UINT GetNumVirusEntries( void );
WORD DIBNumColors (LPTSTR lpbi);
WORD PaletteSize (LPTSTR lpbi);
HPALETTE CreateDIBPalette (HANDLE hDIB);
void LOCAL PASCAL CenterMain(HWND hWnd);
void FormatNoticeText( CString *lpstrNoticeText, BOOL bShowNavBuild );

BOOL SYM_EXPORT CALLBACK NavEggProc (   // Display Team Photo
    HWND        hDlg,                   // [in] Dialog Box handle
    UINT        uMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] Message wParam
    LPARAM      lParam                  // [in] Message lParam
    );

////////////////////////////////////////////////////////////////
// CNavAboutHotKeyCallback

class CNavAboutHotKeyCallback : public IHotKeyCallback
{
	UINT m_cRef;						// object reference count

public:
    CNavAboutHotKeyCallback();
    virtual ~CNavAboutHotKeyCallback();

	// IUnknown -------------------------------------------------------------
	STDMETHODIMP QueryInterface( REFIID riid, LPVOID FAR* ppvObj );
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IHotKeyCallback ------------------------------------------------------
	STDMETHODIMP Register( HWND hWndTarget );
	STDMETHODIMP Unregister( HWND hWndTarget );
	STDMETHODIMP OnHotKey( UINT nID );
	STDMETHODIMP GetNoticeText( LPTSTR pszNotice, LPINT pnNoticeLen );

protected:
    ATOM m_Fatom;
    ATOM m_Batom;
    ATOM m_Aatom;
    ATOM m_Vatom;
    ATOM m_Versionatom;
    BOOL m_bBusy;
    BOOL m_bShowNavBuild;
    BOOL m_bDisplayCredits;
};

////////////////////////////////////////////////////////////////////////////
// CNavAboutHotKeyCallback

CNavAboutHotKeyCallback::CNavAboutHotKeyCallback() : m_cRef( 0 )
{
    m_Fatom = GlobalAddAtom( _T( "EggHotkeyF" ));
    m_Batom = GlobalAddAtom( _T( "EggHotkeyB" ));
    m_Aatom = GlobalAddAtom( _T( "EggHotkeyA" ));
    m_Vatom = GlobalAddAtom( _T( "EggHotkeyV" ));
    m_Versionatom = GlobalAddAtom( _T( "Version Hotkey Atom" ));

    m_bBusy           = FALSE;
    m_bShowNavBuild   = FALSE;
    m_bDisplayCredits = FALSE;
}

CNavAboutHotKeyCallback::~CNavAboutHotKeyCallback()
{
    if ( m_Fatom != 0 )
        GlobalDeleteAtom( m_Fatom );
    if ( m_Batom != 0 )
        GlobalDeleteAtom( m_Batom );
    if ( m_Aatom != 0 )
        GlobalDeleteAtom( m_Aatom );
    if ( m_Vatom != 0 )
        GlobalDeleteAtom( m_Vatom );
    if ( m_Versionatom != 0 )
        GlobalDeleteAtom( m_Versionatom );
}

////////////////////////////////////////////////////////////////
// IUnknown

///////////////////////////////////////
// QueryInterface

STDMETHODIMP CNavAboutHotKeyCallback::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	*ppvObj = NULL;

	if ( riid == IID_IUnknown )
		*ppvObj = (IUnknown*)( (IHotKeyCallback*)this );

	else if ( riid == IID_IHotKeyCallback )
		*ppvObj = (IUnknown*)( (IHotKeyCallback*)this );

	// add tests for additional interfaces here...

	if ( *ppvObj )
	{
		( (LPUNKNOWN)*ppvObj )->AddRef();
		return NOERROR;
	}

	return E_NOINTERFACE;
}

///////////////////////////////////////
// AddRef

STDMETHODIMP_(ULONG) CNavAboutHotKeyCallback::AddRef()
{
	return ( ++m_cRef );
}

///////////////////////////////////////
// Release

STDMETHODIMP_(ULONG) CNavAboutHotKeyCallback::Release()
{
	UINT uRef = --m_cRef;

	if ( uRef == 0 )
		delete this;

	return uRef;
}

////////////////////////////////////////////////////////////////
// IHotKeyCallback

///////////////////////////////////////
// Register

STDMETHODIMP CNavAboutHotKeyCallback::Register( HWND hWndTarget )
{
    if ( RegisterHotKey( hWndTarget, m_Fatom,       MOD_CONTROL | MOD_ALT | MOD_SHIFT,        0x0046 ) && // F
         RegisterHotKey( hWndTarget, m_Batom,       MOD_CONTROL | MOD_ALT | MOD_SHIFT,        0x004F ) && // B
         RegisterHotKey( hWndTarget, m_Aatom,       MOD_CONTROL | MOD_ALT | MOD_SHIFT,        0x0041 ) && // A
         RegisterHotKey( hWndTarget, m_Vatom,       MOD_CONTROL | MOD_ALT | MOD_SHIFT,        0x0056 ) && // V
         RegisterHotKey( hWndTarget, m_Versionatom, MOD_CONTROL, 0x0056 ) )  // Ctrl-V
        {
        return S_OK;
        }
    else
        {
        return E_FAIL;
        }
}

///////////////////////////////////////
// Unregister

STDMETHODIMP CNavAboutHotKeyCallback::Unregister( HWND hWndTarget )
{
    if ( UnregisterHotKey( hWndTarget, m_Fatom       ) &&
         UnregisterHotKey( hWndTarget, m_Batom       ) &&
         UnregisterHotKey( hWndTarget, m_Aatom       ) &&
         UnregisterHotKey( hWndTarget, m_Vatom       ) &&
         UnregisterHotKey( hWndTarget, m_Versionatom ) )
        {
        return S_OK;
        }
    else
        {
        return E_FAIL;
        }
}

///////////////////////////////////////
// OnHotKey

STDMETHODIMP CNavAboutHotKeyCallback::OnHotKey( UINT nID )
{
HRESULT             hReturnVal = S_OK;

    if ( ( nID != m_Fatom       ) &&
         ( nID != m_Batom       ) &&
         ( nID != m_Aatom       ) &&
         ( nID != m_Vatom       ) &&
         ( nID != m_Versionatom ) )
        {
		return E_NOTIMPL;
        }

                                        // Check to see if this is our Ctrl-V
                                        // code.
    if ( nID == m_Versionatom )
        {
                                        // Update our text with NAV build
                                        // information.
        m_bShowNavBuild=TRUE;

                                        // Turn off credit scrolling if we now
                                        // want version information.
        m_bDisplayCredits=FALSE;
        hReturnVal = ABOUT_S_UPDATENOTICE;
        }
                                        // If this is our FB code...
    else if ( ( nID == m_Fatom ) ||
              ( nID == m_Batom ) )
        {
                                        // Check for FB.
        if ( (((DWORD)GetAsyncKeyState( 0x0046 )) & 0x8000 ) &&  // F
             (((DWORD)GetAsyncKeyState( 0x0042 )) & 0x8000 ) )   // B
             {
             m_bDisplayCredits=TRUE;
             hReturnVal = ABOUT_S_UPDATENOTICE;
             }
        }
                                        // If this is our AV code.
    else if ( ( nID == m_Aatom ) ||
              ( nID == m_Vatom ) )
        {
        if ( !m_bBusy )
            {
            if ( (((DWORD)GetAsyncKeyState( 0x0041 )) & 0x8000 ) &&  // A
                 (((DWORD)GetAsyncKeyState( 0x0056 )) & 0x8000 ) )   // V
                {
                m_bBusy=TRUE;
                DialogBox ( ghInst, MAKEINTRESOURCE(IDD_NAVEGG), NULL, (DLGPROC)NavEggProc );
                m_bBusy=FALSE;
                }
            }
        }

  return hReturnVal;
}

///////////////////////////////////////
// GetNoticeText

STDMETHODIMP CNavAboutHotKeyCallback::GetNoticeText( LPTSTR pszNotice, LPINT pnNoticeLen )
{
    CString strNotice;

                                        // If we are displaying our credits...
    if ( TRUE == m_bDisplayCredits )
        {
        int nPosition=0;
                                        // Gather our credits.
        while ( g_szzNavCredits[nPosition] )
            {
            strNotice += g_szzNavCredits[nPosition];
            strNotice += _T("\n");
            nPosition++;
            }
        }
    else
        {
        FormatNoticeText( &strNotice, m_bShowNavBuild );
        }

	if (( pnNoticeLen != NULL ) && !IsBadWritePtr( pnNoticeLen, sizeof( INT )))
		*pnNoticeLen = strNotice.GetLength();

	if (( pszNotice != NULL ) && !IsBadWritePtr( pszNotice, strNotice.GetLength() ))
		_tcscpy( pszNotice, (LPCTSTR)strNotice );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// Exports

///////////////////////////////////////
// DllMain

STDAPI_(BOOL) APIENTRY DllMain(
	HINSTANCE hDll,
	DWORD dwReason,
	LPVOID /* lpReserved */ )
{
#ifdef _DEBUG
	TCHAR szMod[_MAX_PATH];
	GetModuleFileName( hDll, szMod, _MAX_PATH );
#endif

	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH:
		{
			TRACE2( _T( "DLL_PROCESS_ATTACH: %s, (Process %08lX)\n" ),
				szMod, GetCurrentProcessId() );

			// save instance handle
            ghInst=hDll;
            SetInstanceHandle( hDll );
			break;
		}

		case DLL_PROCESS_DETACH:
			TRACE2( _T( "DLL_PROCESS_DETACH: %s, (Process %08lX)\n" ),
				szMod, GetCurrentProcessId() );
			break;

		case DLL_THREAD_ATTACH:
			TRACE2( _T( "DLL_THREAD_ATTACH: %s, (Thread %08lX)\n" ),
				szMod, GetCurrentThreadId() );
			break;

		case DLL_THREAD_DETACH:
			TRACE2( _T( "DLL_THREAD_DETACH: %s, (Thread %08lX)\n" ),
				szMod, GetCurrentThreadId() );
			break;

		default:
			break;
	}

	return TRUE;
}

///////////////////////////////////////
// DllGetClassObject

STDAPI DllGetClassObject( REFCLSID rclsid, REFIID riid,
	LPVOID FAR* ppvObj )
{
	IClassFactory* pFactory = NULL;

//	ABOUT BOX EXTENSION -----------------------------------------------------

	if ( IsEqualCLSID( rclsid, CLSID_CNavAboutExt ))
		pFactory = new CAboutExtFactory();

//	-------------------------------------------------------------------------

	// add tests for additional objects here...

	if ( pFactory )
	{
		if ( SYM_FAILED( pFactory->QueryInterface( riid, ppvObj )))
		{
			delete pFactory;
			*ppvObj = NULL;
			return E_INVALIDARG;
		}
	}
	else
		return CLASS_E_CLASSNOTAVAILABLE;

	return NOERROR;
}

///////////////////////////////////////
// DllCanUnloadNow

STDAPI DllCanUnloadNow()
{
	TRACE1( "DllCanUnloadNow: %d open refs\n", g_cRefThis );
	BOOL bCanUnload = ( g_cRefThis == 0 ) && ( g_cLock == 0 );
	return ( bCanUnload ? S_OK : S_FALSE );
}

///////////////////////////////////////
// DllRegisterServer

STDAPI DllRegisterServer()
{
/*
//	ABOUT BOX EXTENSION -----------------------------------------------------
	OLECHAR szClassID[_MAX_PATH] = { L"" };
    CString strExtName( IDS_COMPONENT_NAME );
    ASSERT( !strExtName.IsEmpty() );

	RegisterComObject( strExtName, CLSID_CNavAboutExt );

    if ( StringFromGUID2( CLSID_CNavAboutExt, szClassID, _MAX_PATH ) > 0 )
	{
		CString strClassID( szClassID );

        //register with the local product key
        CString strSubKey(REGSTR_ABOUT_NAV);
        strSubKey += REGSTR_KEY_ABOUT;
		strSubKey.Format( _T( "\\%s" ), (LPCTSTR) strExtName );

		CRegistryKey regKey( HKEY_LOCAL_MACHINE, strSubKey );
		regKey.SetValue( _T( "" ), (LPCTSTR)strClassID );

	    CString strName( IDS_COMPONENT_NAME );
        ASSERT( !strName.IsEmpty() );
	    return RegisterComObject( strName, CLSID_CNavAboutExt );
	}
  return E_UNEXPECTED;
  */

//	-------------------------------------------------------------------------
	return S_OK;
}

///////////////////////////////////////
// DllUnregisterServer

STDAPI DllUnregisterServer()
{
//	ABOUT BOX EXTENSION -----------------------------------------------------
    /*
    CString strExtName( IDS_COMPONENT_NAME );
    ASSERT( !strExtName.IsEmpty() );
    CString strSubKey(REGSTR_ABOUT_NAV);
    strSubKey += REGSTR_KEY_ABOUT;
    ASSERT( !strSubKey.IsEmpty() );

	strSubKey.Format( _T( "\\%s" ), (LPCTSTR) strExtName );

	CRegistryKey::DeleteKey( HKEY_LOCAL_MACHINE, strSubKey );

	UnregisterComObject( CLSID_CNavAboutExt );
//	-------------------------------------------------------------------------
    */
	return S_OK;
}

///////////////////////////////////////
/*	DoStdAbout

    C helper entry point to invoke the standard about box.

 NOTE:
    You should make this function return an error code and you should pass in
    the language and SKU/PartNumber or put them in the string table and modify
    this routine to get them.  This is just a crude example that shows how
    to invoke the standard about COM interface.

*/


/////////////////////////////////////////////////////////////////////
// void FormatNoticeText( CString *lpstrNoticeText, BOOL bShowNavBuild )
//
// Purpose:
//   This function will format the notice text for our about box based
//   on the settings passed in.
//
// Parameters:
//    CString *lpstrNoticeText - [out] Formatted notice text.
//    BOOL     bShowNavBuild   - [in]  Show NAV version information?
//
// Returns:
//    Notice text in strNoticeText.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 08-19-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void FormatNoticeText(
    CString *lpstrNoticeText,
    BOOL     bShowNavBuild
    )
{
CString strFormatString;
CString strTempString;
CString strDefinitionDate;
BOOL bLoadSuccess;

/////////////////////////////////////////////////////////////////////
// Format Norton AntiVirus version string.

                                        // "Norton AntiVirus for "
    if( NAV_IS_OEM() )
    {
        bLoadSuccess = strFormatString.LoadString( IDS_PRODUCT_TITLE_OEM );
    }
    else
    {
        bLoadSuccess = strFormatString.LoadString( IDS_PRODUCT_TITLE );
    }

    if ( bLoadSuccess )
        {
        if (SystemGetWindowsType() == SYM_SYSTEM_WIN_95)
            {
                                        // Load Windows 95 string.
            strTempString.LoadString( IDS_PRODUCT_WIN95 );
            strFormatString += strTempString;
            }
                                        // Display NAV product name and type
                                        // (Server vs. Workstation).
        else if ( NAV_IS_SERVER() )
            {
                                        // Load NT Workstation string.
            strTempString.LoadString( IDS_PRODUCT_NTS );
            strFormatString += strTempString;
            }
        else
            {
                                        // Load NT Server String.
            strTempString.LoadString( IDS_PRODUCT_NTWS );
            strFormatString += strTempString;
            }

        *lpstrNoticeText += strFormatString;
        strFormatString.Empty();
        strTempString.Empty();
        }

/////////////////////////////////////////////////////////////////////
// Optionally, show NAV build and part number

                                        // If we are to show build number,
                                        // tack it on the end.
    if ( bShowNavBuild )
        {
                                        // Display the NAV build number
        strTempString.Empty();
        if ( strTempString.LoadString( IDS_BUILD_STR ) )
            {
            *lpstrNoticeText += _T("  Build ");
            *lpstrNoticeText += NAV_BUILD_STRING;
            *lpstrNoticeText += _T("\n");
            }

                                        // Diaply the part number.
        strTempString.Empty();
        if ( strTempString.LoadString( IDS_PART_NUMBER ) )
            {
            *lpstrNoticeText += strTempString;

            GetPartNumber ( strTempString.GetBuffer(262) );
            strTempString.ReleaseBuffer();

            *lpstrNoticeText += strTempString;
            *lpstrNoticeText += _T("\n");
            }

                                        // Display the def annuity status.
        auto DWORD dwVersion, dwInstalledOn, dwPaidOn;
        auto long lExpiresIn;
        auto TCHAR szTemp[200];

        if ( DefAnnuityQueryStatus ( &dwVersion, &dwInstalledOn, &dwPaidOn, &lExpiresIn ) == DefAnnuitySuccess )
            {

            _stprintf ( szTemp,
                        _T("DefAnnuity version %d.%02d.%02d.%02d was installed on %d; "),
                        dwVersion >> 24,
                        ( dwVersion >> 16 ) & 0xFF,
                        ( dwVersion >> 8 ) & 0xFF,
                        dwVersion & 0xFF,
                        dwInstalledOn );
            *lpstrNoticeText += szTemp;
            if ( dwPaidOn == 0xFFFFFFFFL )
                {
                *lpstrNoticeText += _T("subscription checks are disabled.\n");
                }
            else
                {
                _itot ( dwPaidOn, szTemp, 10 );
                if ( dwPaidOn == 0xFFFFFFFE )
                    {
                    *lpstrNoticeText += _T("free subscription");
                    }
                else
                    {
                    *lpstrNoticeText += _T("paid subscription (last paid on ");
                    *lpstrNoticeText += szTemp;
                    *lpstrNoticeText += _T(")");
                    }
                if ( lExpiresIn > 0 )
                    {
                    *lpstrNoticeText += _T(" expires in ");
                    _itot ( lExpiresIn, szTemp, 10 );
                    *lpstrNoticeText += szTemp;
                    *lpstrNoticeText += ( lExpiresIn == 1 ) ? _T(" day.\n") : _T(" days.\n");
                    }
                else
                    {
                    *lpstrNoticeText += _T(" has expired.\n");
                    }
                }
            }
        else
            {
                *lpstrNoticeText += _T("DefAnnuity error!\n");
            }
        }
    else
        {
                                        // Add blank space if no build number.
        *lpstrNoticeText += _T("\n\n");
        }

/////////////////////////////////////////////////////////////////////
// Format virus definition date and time string

                                        // Load our format specifier for our
                                        // definition date and number of virus
                                        // definitions.
    if ( strFormatString.LoadString( IDS_VIRUS_DEF_NOTICE_FORMAT_STR ) )
        {
                                        // Format virus definition string.
        GetDefinitionDate(strDefinitionDate.GetBuffer( 40 ), 40);
        strDefinitionDate.ReleaseBuffer();
        strTempString.Empty();
        strTempString.Format( strFormatString, (LPCTSTR)strDefinitionDate, GetNumVirusEntries() );

                                        // Tack formatted string onto current
                                        // string.
        *lpstrNoticeText += _T("\n");
        *lpstrNoticeText += strTempString;
        *lpstrNoticeText += _T("\n");
        }

/////////////////////////////////////////////////////////////////////
// Display Windows version.

    strTempString.Empty();
    strFormatString.Empty();
    *lpstrNoticeText += _T("\n");
    int nOSID=0;


    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof( osvi );
    GetVersionEx( &osvi );

    if ( (osvi.dwPlatformId == VER_PLATFORM_WIN32s) )
        {
        nOSID = IDS_WINDOWS_VERSION_WIN32S;
        }
    else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
        {
        if ( osvi.dwMinorVersion >= 10 )
            nOSID = IDS_WINDOWS_VERSION_WIN98;
        else
            nOSID = IDS_WINDOWS_VERSION_WIN95;
        }
    else if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
        {
            nOSID = IDS_WINDOWS_VERSION_NT;
        }

                                        // If we were able to determine the OS
                                        // ID...
    if ( 0 != nOSID )
        {
                                        // Load this OS's ID string from our
                                        // string table.
        if ( strFormatString.LoadString( nOSID ) )
            {
            strTempString.Format( strFormatString, osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber & 0xFFFF );

                                        // Tack on our windows version name.
            *lpstrNoticeText += strTempString;
            }

                                        // Only if we are displaying the NAV
                                        // version information, display the
                                        // service pack string for Windows.
        if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
            {
            if ( bShowNavBuild )
                {
                *lpstrNoticeText += _T("\n");
                *lpstrNoticeText += osvi.szCSDVersion;
                }
            }
        }
}

// ==== GetCorporateMode ==================================================
//
//  This function fetches the corporate value from the registry.  If the
//  value does not exist, a value is returned to indicate this.
//
//  Input:
//
//  Output:
//      TRUE if corporate features is enabled, false otherwise
//
// ========================================================================
//  Function created: 4/98, SEDWARD
//  5/14/98 - JTAYLOR - Fixed KEY_ALL_ACCESS bug.
// ========================================================================

BOOL IsCorporateMode()
{
    auto    BOOL            bResult = FALSE;
    auto    DWORD           dwRegValueType = 0;
    auto    DWORD           dwDataBuf;
    auto    DWORD           dwSize;
    auto    HKEY            hKey = NULL;
    auto    long            lResult;


    // open the registry key
    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,                      // handle of open key
                           "SOFTWARE\\Symantec\\Norton AntiVirus",  // address of name of subkey to open
                           0,                                       // reserved
                           (KEY_READ | KEY_WRITE),                  // security access mask
                           &hKey);                                  // address of handle of open key
    if (ERROR_SUCCESS != lResult)
        {
        return FALSE;
        }

    // get the value of "corporate features" (if it exists)
    dwSize = sizeof(DWORD);
    lResult = RegQueryValueEx(hKey              // handle of key to query
                    , "corporatefeatures"       // address of name of value to query
                    , 0                         // reserved (must be zero)
                    , &dwRegValueType           // address of buffer for value type
                    , (LPBYTE)&dwDataBuf        // address of data buffer
                    , &dwSize);                 // address of data buffer size

    if ((ERROR_SUCCESS != lResult)  ||  (REG_DWORD != dwRegValueType))
        {
        bResult = FALSE;
        }
    else
        {
        bResult = TRUE;
        }

    if (NULL != hKey)
        {
        RegCloseKey(hKey);
        }

    return bResult;
}


void WINAPI DoStdAbout( HWND hwndParent )
{
    TCHAR  szPublicVersion[] = _T("xx.xx.xx");
    TCHAR  szDisplayVersion[] = _T("xx.xx.xx");
    TCHAR  szVersionINI [MAX_PATH];
    TCHAR* ptr;
    DWORD  dwFlags;

	// storage for specified parmaters
    if( NAV_IS_OEM() )
        dwFlags = PTF_MACHINE;
    else
        dwFlags = PTF_MACHINE | PTF_ENABLE_SYSINFO;

    // If CorporateFeatures are active, then display machine registration info.
    // Otherwise display user registration info.
    if( TRUE == IsCorporateMode() )
    {
        dwFlags |= PTF_MACHINE;
        dwFlags |= PTF_DISABLE_REGISTRATION;
    }
    else
        dwFlags |= PTF_USER;

    TCHAR szPartNum[_MAX_PATH];

    // attempt to initialize COM
	if ( SYM_FAILED( CoInitialize( NULL )))
		return;

	HRESULT hResult;
    IProductTransaction* pTransaction;

	// attempt to create user registration object
    hResult = CoCreateInstance( CLSID_CSymAboutBox, NULL,
            CLSCTX_INPROC_SERVER, IID_IProductTransaction, (LPVOID*)&pTransaction );

	if ( SYM_SUCCEEDED( hResult ))
	{
		// ensure release of interface regardless of code-path
        CEnsureRelease er( pTransaction );

        // Get part number from PARTNUM.DAT
        GetPartNumber( szPartNum );

        // Read in the public revision number.
        GetModuleFileName ( NULL, szVersionINI, MAX_PATH );

        ptr = FindFileName ( szVersionINI, FALSE );

        if ( NULL != ptr )
            {
            lstrcpy ( ptr, _T("\\version.dat") );
            }
        else
            {
            szVersionINI[0] = '\0';     // This will make GPPS() below fail
                                        // and use the default version strings.
            }

        GetPrivateProfileString ( _T("Versions"), _T("PublicVersion"),
                                  szDefaultPublicVersion, szPublicVersion,
                                  1 + lstrlen ( szPublicVersion ),
                                  szVersionINI );

        GetPrivateProfileString ( _T("Versions"), _T("PublicRevision"),
                                  szDefaultPublicRevision, szDisplayVersion,
                                  1 + lstrlen ( szDisplayVersion ),
                                  szVersionINI );


		// attempt to initialize trial object from arguments
        if( NAV_IS_OEM() )
        {
            hResult = pTransaction->Initialize( szCompany, szProductOEM,
                                            szPublicVersion,
                                			szLanguage, dwFlags, szPartNum );
        }
        else
        {
            hResult = pTransaction->Initialize( szCompany, szProduct,
                                            szPublicVersion,
                                			szLanguage, dwFlags, szPartNum );
        }

        ISymAboutBox* pAboutBox = NULL;

        hResult = pTransaction->QueryInterface( IID_ISymAboutBox,
            (LPVOID*)&pAboutBox );

        if ( SUCCEEDED( hResult ))
            {
            CString strNotices;

            CNavAboutHotKeyCallback* pNavAboutHotKeyHandler = new CNavAboutHotKeyCallback();

            FormatNoticeText( &strNotices, FALSE );
            pAboutBox->SetTextProperty( SAP_NOTICES, strNotices );
            pAboutBox->SetTextProperty( SAP_DISPLAYVERSION, szDisplayVersion );

            HBITMAP hBitmap = LoadBitmap( GetResourceHandle(),
                MAKEINTRESOURCE( IDB_ABOUT ));
            HICON hNAVIcon = LoadIcon( GetResourceHandle(),
                MAKEINTRESOURCE( IDI_NAVW ));

            if ( hBitmap != NULL )
                pAboutBox->SetImage( IMAGE_BITMAP, hBitmap );
            if ( hNAVIcon != NULL )
                pAboutBox->SetImage( IMAGE_ICON, hNAVIcon );

            pAboutBox->SetHotKeyCallback(
                (IHotKeyCallback*)pNavAboutHotKeyHandler );
            }


		if ( SYM_SUCCEEDED( hResult ))
		{
            // invoke registration wizard
            hResult = pTransaction->Invoke( NULL, hwndParent, FALSE );

		#ifdef _DEBUG
			if ( hResult == S_OK )
				TRACE( _T( "Transaction succeeded\n" ));
			else if ( hResult == S_FALSE )
				TRACE( _T( "Transaction cancelled\n" ));
			else
				TRACE( _T( "Transaction failed due to error\n" ));
		#endif // _DEBUG
		}
	}

	// uninitialize COM
	CoUninitialize();
}

///////////////////////////////////////////////////////////////////////
// Local Functions

UINT GetNumVirusEntries( void )
{
    HVCONTEXT hVirusContext            = VLScanInit (1, &gstNAVCallBacks, 0);
    UINT      uNumViruses              = 0;
    TCHAR     szVirScan1Dat[_MAX_PATH] = "";
    TCHAR     szVirScanInf[_MAX_PATH]  = "";
    TCHAR     szDefDir[_MAX_PATH];
    CDefUtils DefUtils;

    if (!hVirusContext)
        return uNumViruses;

    if (IS_NT)
        DefUtils.InitWindowsApp(_T("NAVNT_50_NAVW"));
    else
        DefUtils.InitWindowsApp(_T("NAV95_50_NAVW"));

    // Setup a temporary usage.
    if (DefUtils.GetCurrentDefs (szDefDir, sizeof (szDefDir)))
    {
        // Tell our AVAPI1 callback library the definition directory.
        NAVSetDefsPath (szDefDir);

        // Construct full path strings to VirScan1.DAT and VirScan.INF files.
        _tcscpy (szVirScan1Dat, szDefDir);
        NameAppendFile (szVirScan1Dat, _T("VirScan1.DAT"));

        _tcscpy (szVirScanInf, szDefDir);
        NameAppendFile (szVirScanInf, _T("VirScan.INF"));

        // Fetch the number of viruses.
        if (VS_OK == VLRegisterVirusDB (hVirusContext, szVirScan1Dat, szVirScanInf, NULL))
        {
            uNumViruses = VLNumClaimedVirusEntries (hVirusContext);
        }
        VLScanClose (hVirusContext);
    }

    return uNumViruses;
}

DWORD GetDefinitionDate(LPTSTR lpDateStr, int size)
{
    CDefUtils DefUtils;
    SYSTEMTIME date;
    DWORD dwVersion;

    if (IS_NT)
        DefUtils.InitWindowsApp(_T("NAVNT_50_NAVW"));
    else
        DefUtils.InitWindowsApp(_T("NAV95_50_NAVW"));

    memset(&date, 0, sizeof(SYSTEMTIME));
    if (DefUtils.GetCurrentDefsDate (&date.wYear, &date.wMonth, &date.wDay, &dwVersion))
    {
        GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &date, NULL, lpDateStr, size);
        return TRUE;
    }
    return dwVersion;
}


////////////////////////////////////////////////////////////////////////
//
// GetPartNum() - This routine retrieves out SKU number from PARTNUM.DAT
//
// Returns: TRUE if successful
//          FALSE otherwise
//
////////////////////////////////////////////////////////////////////////
// 7/22/97 DBUCHES - Function created
////////////////////////////////////////////////////////////////////////

BOOL PASCAL GetPartNumber( LPTSTR szPartNum )
{
    TCHAR szSection[] = _T("PartNumber");
    TCHAR szKey[] = _T("SKU");
    TCHAR szFileName[_MAX_PATH];

    // Get name of data file
    if( FALSE == GetDatFileName(szFileName) )
        return FALSE;

    // Get SKU from data file
    GetPrivateProfileString( szSection,
                             szKey,
                             _T("0"),
                             szPartNum,
                             _MAX_PATH,
                             szFileName );
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// GetDatFileName() - This routine will return the full path to
//                    PartNum.Dat.  This routine assumes that this file
//                    resides in the same directory in which the module
//                    that launched us lives.
//
// Returns: TRUE if successful
//          FALSE otherwise
//
////////////////////////////////////////////////////////////////////////
// 7/22/97 DBUCHES - Function created
////////////////////////////////////////////////////////////////////////

BOOL PASCAL GetDatFileName( LPTSTR szFileName )
{
    TCHAR szModuleFileName[_MAX_PATH];
    TCHAR szPartNumDatFileName[] = _T("\\PARTNUM.DAT");

    // Get name of module that launched us
    if( FALSE == GetModuleFileName( NULL, szModuleFileName, _MAX_PATH ))
        return FALSE;

    // Strip off file name
    LPTSTR p;
    p = FindFileName( szModuleFileName, FALSE );
    if ( p == NULL )
        return FALSE;
    *p = _T('\0');

    // Append data file name
    lstrcat(szModuleFileName, szPartNumDatFileName);
    lstrcpy(szFileName, szModuleFileName);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////
//
//   FindFileName()
//
//   Helper function, used by NameStripFile() and NameMakeUnique().
//
//   If the bSkipSlash parameter is TRUE, then no returned values will
//   point at a backslash.
//   The following describes the behavior when bSkipSlash is FALSE:
//   If lpName does not contain '\' or ':', return lpName.
//   If lpName contains '\NAME', return pointer to 'NAME'.
//   If lpName contains 'A:\NAME', return pointer to 'NAME'.
//   If lpName contains 'A:\DIR\NAME', return pointer to '\NAME'
//   If lpName contains '\\SERVER', return pointer to '\0' (no filename)
//   If lpName contains '\\SERVER\VOLUME', return pointer to '\0' (no filename)
//   If lpName contains '\\SERVER\VOLUME\NAME', return pointer to '\NAME'
//
//   When the bSkipSlash parameter is FALSE, this function will return a
//   pointer to a '\' only when lpName contains a path beyond the root
//   (deeper than 1), otherwise it will not.
//
// See Also:
//   NameStripFile(), NameMakeUnique()
//
////////////////////////////////////////////////////////////////////////
// 12/15/94 BEM Comment header created.
// 7/22/97 DBUCHES - Stolen from Core
////////////////////////////////////////////////////////////////////////

LPTSTR PASCAL FindFileName (LPCTSTR lpName, BOOL bSkipSlash)
{
    LPCTSTR     p;
    LPCTSTR     lpszFound;
    LPCTSTR     pNullByte;
    BOOL        bIsUNC = FALSE;
    int         nSlashCount = 0;        // Count of slashes beyond '\\' in UNC



    p = lpszFound = lpName;
                                        // Find the last '\' or ':' in lpName
                                        // Detect UNC while we go through
    while (*p)
        {
        if (*p == ':')
            lpszFound = p;
        else if (*p == '\\')
            {
            if (bIsUNC)
               nSlashCount++;
                                        // If the first and second characters
                                        // are '\', then its a UNC.
            if (lpszFound == lpName &&
                *lpszFound == '\\' && (p == lpszFound+1))
                bIsUNC = TRUE;
            lpszFound = p;
            }
        p = AnsiNext(p);
        }

    pNullByte = p;
    p = lpszFound;                      // Points to the last occurance of
                                        // '\' or ':' in lpName, or
                                        // points to the first char of
                                        // lpName if none was found.
                                        //

    if (*p == ':')                      // Is this a drive letter?
        p++;                            // Yes, then keep it.
                                        // This is DBCS safe because we
                                        // know we're not on a lead byte.

    if (p == lpName && *p == '\\')      // Is this file in root dir?
        p++;                            // Yes, keep the '\'
                                        // This is DBCS safe because we
                                        // know we're not on a lead byte.
    else
        {
        if (bIsUNC)                     // If it's a UNC, make sure that
                                        // there was a file name, which
                                        // must come after \\SERVER\VOLUME
                                        // so the number of slashes past
                                        // the initial '\\' must be 2 or more
                                        // otherwise there wasn't a name.
            {
            if (nSlashCount < 2)
                p = pNullByte;          // No file name, so return ptr to '\0'
            }
        else if (p > lpName && *p == '\\' && *AnsiPrev(lpName, p) == ':')
            p++;                        // 'A:\', keep the '\'
                                        // This is DBCS safe because we
                                        // know we're not on a lead byte.
        }

    if (bSkipSlash && *p == '\\')
        p++;                            // This is DBCS safe because we
                                        // know we're not on a lead byte.

    return ((LPTSTR)p);
}

//---------------------------------------------------------------------
//
// Function:   DIBNumColors
//
// Purpose:    Given a pointer to a DIB, returns a number of colors in
//             the DIB's color table.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

WORD DIBNumColors (LPTSTR lpbi)
{
   WORD wBitCount;


      // If this is a Windows style DIB, the number of colors in the
      //  color table can be less than the number of bits per pixel
      //  allows for (i.e. lpbi->biClrUsed can be set to some value).
      //  If this is the case, return the appropriate value.

   DWORD dwClrUsed;

   dwClrUsed = ((LPBITMAPINFOHEADER) lpbi)->biClrUsed;

   if (dwClrUsed)
      return (WORD) dwClrUsed;


      // Calculate the number of colors in the color table based on
      //  the number of bits per pixel for the DIB.

   wBitCount = ((LPBITMAPINFOHEADER) lpbi)->biBitCount;

   switch (wBitCount)
      {
      case 1:
         return 2;

      case 4:
         return 16;

      case 8:
         return 256;

      default:
         return 0;
      }
}


//---------------------------------------------------------------------
//
// Function:   PaletteSize
//
// Purpose:    Given a pointer to a DIB, returns number of bytes
//             in the DIB's color table.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

WORD PaletteSize (LPTSTR lpbi)
{
   return (DIBNumColors (lpbi) * sizeof (RGBQUAD));
}


//---------------------------------------------------------------------
//
// Function:   CreateDIBPalette
//
// Purpose:    Given a handle to a DIB, constructs a logical palette,
//             and returns a handle to this palette.
//
//             Stolen almost verbatim from ShowDIB.
//
// Parms:      hDIB == HANDLE to global memory with a DIB header
//                     (either BITMAPINFOHEADER or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

HPALETTE CreateDIBPalette (HANDLE hDIB)
{
   LPLOGPALETTE     lpPal;
   HANDLE           hLogPal;
   HPALETTE         hPal = NULL;
   int              i, wNumColors;
   LPTSTR           lpbi;
   LPBITMAPINFO     lpbmi;
   LPBITMAPCOREINFO lpbmc;

   if (!hDIB)
      return NULL;

   lpbi         = (LPTSTR) LockResource (hDIB);
   lpbmi        = (LPBITMAPINFO) lpbi;
   lpbmc        = (LPBITMAPCOREINFO) lpbi;
   wNumColors   = DIBNumColors (lpbi);

   if (wNumColors)
      {
      hLogPal = GlobalAlloc (GHND, sizeof (LOGPALETTE) +
                             sizeof (PALETTEENTRY) * wNumColors);

      if (!hLogPal)
         return NULL;

      lpPal = (LPLOGPALETTE) GlobalLock (hLogPal);

      lpPal->palVersion    = 0x300;
      lpPal->palNumEntries = wNumColors;

      for (i = 0;  i < wNumColors;  i++)
         {
         lpPal->palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
         lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
         lpPal->palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
         lpPal->palPalEntry[i].peFlags = 0;
         }

      hPal = CreatePalette (lpPal);
      GlobalFree   (hLogPal);
      }
   return hPal;
}   // CreateDIBPalette

void LOCAL PASCAL CenterMain(HWND hWnd)
{
    auto        int             xPopup;
    auto        int             yPopup;
    auto        int             cxPopup;
    auto        int             cyPopup;
    auto        int             cxScreen;
    auto        int             cyScreen;
    auto        RECT            rcWindow;


                                        /* retrieve main display dimensions */
    cxScreen = GetSystemMetrics(SM_CXSCREEN);
    cyScreen = GetSystemMetrics(SM_CYSCREEN);

                                        /* retrieve popup rectangle */
    GetWindowRect(hWnd, (LPRECT) & rcWindow);

                                        /* calculate popup size */
    cxPopup = rcWindow.right - rcWindow.left;
    cyPopup = rcWindow.bottom - rcWindow.top;

                                        /* center within entire screen */
    xPopup = (cxScreen - cxPopup) / 2;
    yPopup = (cyScreen - cyPopup) / 2;

                                        /* move window to new location and display */
    MoveWindow(hWnd, (xPopup > 0) ? xPopup : 0, (yPopup > 0) ? yPopup : 0, cxPopup, cyPopup, TRUE);
}


//***************************************************************************
// NavEggProc()
//
// Description:
//      Displays The Norton AntiVirus for Windows '95 Team Photo!!!  This is
//      a simple dialog box proc that handles things for us.  Source stolen
//      directly from NU Team and pasted in to save time.
//
// Parameters:
//      hWnd,                           [in] Dialog Box handle
//      uMessage,                       [in] Message to process
//      wParam,                         [in] Message wParam
//      lParam                          [in] Message lParam
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 07/04/1995 MARKL Function Created.
//***************************************************************************

void FunWithStrings ( HWND, LPCTSTR, WPARAM, LPARAM );

BOOL SYM_EXPORT CALLBACK NavEggProc (   // Display Team Photo
    HWND        hDlg,                   // [in] Dialog Box handle
    UINT        uMessage,               // [in] Message to process
    WPARAM      wParam,                 // [in] Message wParam
    LPARAM      lParam                  // [in] Message lParam
    )
{
    static      TCHAR           szGreet[100];
    static      HBITMAP         hBmp = NULL;
    auto        HPALETTE        hPal = NULL;
    auto        HGLOBAL         hDIB;
    auto        HDC             hDC;
    auto        HDC             hMemDC;
    auto        HRSRC           hRes;
    auto        HPALETTE        hOldPal;
    auto        HBITMAP         hOldBmp;
    auto        PAINTSTRUCT     ps;
    auto        LPBITMAPINFOHEADER lpbmInfoHdr;
    auto        LPTSTR          lpBits;
    static      int             nWhich;
    auto        BOOL            rtn = TRUE;
    auto	RECT		rect;

    switch (uMessage)
    {
    case WM_MOUSEMOVE:
        FunWithStrings ( hDlg, szGreet, wParam, lParam );
    break;

    case WM_INITDIALOG:
//        NG_SetFontsInDialog (hDlg, 0);
        if (hBmp)
            {
            DeleteObject( hBmp );
            hBmp = NULL;
            }
        hRes = FindResource( ghInst, MAKEINTRESOURCE(IDB_NAVTEAM), RT_BITMAP );
        if (!hRes)
            {
            EndDialog( hDlg, IDOK );
            break;
            }
        hDIB = LoadResource( ghInst, hRes );
        if (!hDIB)
            {
            EndDialog( hDlg, IDOK );
            break;
            }

        lpbmInfoHdr = (LPBITMAPINFOHEADER) LockResource (hDIB);
        if (!lpbmInfoHdr)
            {
            EndDialog( hDlg, IDOK );
            break;
            }

        lpBits = (LPTSTR)lpbmInfoHdr + *(LPDWORD)lpbmInfoHdr +
                            PaletteSize((LPTSTR)lpbmInfoHdr);

        if (!lParam)
            {
            nWhich = -1;
            GetWindowText( hDlg, szGreet, sizeof(szGreet) );
            SetWindowPos( hDlg, NULL, 0, 0,
                (int)lpbmInfoHdr->biWidth + 2 * GetSystemMetrics( SM_CXBORDER ),
                (int)lpbmInfoHdr->biHeight + GetSystemMetrics( SM_CYCAPTION ) +
                              GetSystemMetrics( SM_CYBORDER ),
                SWP_NOMOVE | SWP_NOZORDER );
            CenterMain( hDlg );
            }
        hPal = CreateDIBPalette( hDIB );
        hDC = GetDC( hDlg );
        if (!hDC)
            {
            EndDialog( hDlg, IDOK );
            break;
            }
        hBmp = CreateCompatibleBitmap( hDC,
                (WORD)lpbmInfoHdr->biWidth, (WORD)lpbmInfoHdr->biHeight );
        if (hPal)
            {
            hOldPal = SelectPalette( hDC, hPal, FALSE);
            RealizePalette( hDC );
            }

        SetDIBits( hDC, hBmp, 0, (WORD)lpbmInfoHdr->biHeight,
                    lpBits, (LPBITMAPINFO)lpbmInfoHdr, DIB_RGB_COLORS );

        if (hPal)
            {
            SelectPalette( hDC, hOldPal, FALSE );
            DeleteObject( hPal );
            }
        ReleaseDC( hDlg, hDC );
        FreeResource( hDIB );
        break;  // WM_INITDIALOG

    case WM_PAINT:
        BeginPaint( hDlg, &ps );
        GetClientRect( hDlg, &rect );
        hMemDC = CreateCompatibleDC( ps.hdc );
        if (hMemDC)
            {
            hOldBmp = (HBITMAP)SelectObject( hMemDC, hBmp );
            BitBlt( ps.hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY );
            SelectObject( hMemDC, hOldBmp );
            DeleteDC( hMemDC );
            }
        EndPaint( hDlg, &ps );
        break;

    case WM_SIZE:
        InvalidateRect( hDlg, NULL, FALSE );
        break;

    case WM_KILLFOCUS:
        if ((HWND)wParam == hDlg)
            break;
        // fall thru

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
        wParam = IDOK;
        // fall thru

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
            EndDialog( hDlg, wParam );
            break;

        default:
            break;
        }
        break;  // WM_COMMAND

    case WM_NCDESTROY:
        if (hBmp)
            {
            DeleteObject( hBmp );
            hBmp = NULL;
            }
        break;

    default:
        rtn = FALSE;
        break;
    }   // switch uMsg
    return rtn;
} // NavEggProc()

struct DDS
    {
    LPCTSTR szText;
    LPCTSTR szSecretText;
    RECT    rBox;
    };

void FunWithStrings ( HWND hwnd, LPCTSTR szGreet, WPARAM wParam, LPARAM lParam )
{
static BOOL  bSecret;
static POINT pt;
static RECT  rect;
static TCHAR szWelcome[] = _T("Welcome to the SLAC Lab!");
static DDS   whee[] =
    {
    {"The Roomarang(tm)", "Fun for the whole family!", {560,332,607,357} },
    {"The almighty belly", "Ctrl+Alt+Shift+FB", {305,241,360,291} },
    {"What, no skates?", "", {80,324,125,352} },
    {"Sponsored by Baskin Robbins", "We need more sprinkles, Brian!", {115,279,146,314} },
    {"Sponsored by Baskin Robbins", "", {399,104,431,124} },
    {"The Peter Norton Pose(tm)", "", {227,122,284,145} },
    {"Project Atlanta!", "", {46,95,86,112} },
    {"Brian Foster", "You will respect my authoritaaaa!", {453,134,492,185} },
    {"Reiko Nozawa", "", {372,124,406,170} },
    {"Paul \"FB\" Davis", "FB in LA", {316,155,353,199} },
    {"Marian Merritt", "", {285,112,319,155} },
    {"David Allee", "You're in trubble!", {221,146,257,191} },
    {"Michael Keating", "", {163,141,192,182} },
    {"Radoslav Stanev", "", {121,190,163,239} },
    {"Garret Polk", "", {76,131,109,175} },
    {"Ed Thompson", "", {535,23,577,69} },
    {"Emma Fielding", "", {498,45,528,85} },
    {"Scott Mynatt", "", {445,21,476,60} },
    {"Sara Flood", "", {396,44,423,75} },
    {"Michael Dunn", "He's a pinball wizard", {373,14,400,45} },
    {"Sebastian Somchai Lai", "", {352,45,380,81} },
    {"Scott Edwards", "They call him \"professor\" now :)", {318,52,348,82} },
    {"Bob Kolosky", "", {314,19,338,50} },
    {"Chris Nevarez", "", {279,37,310,75} },
    {"Marc Marcuse", "", {249,60,274,94} },
    {"Jim Waggoner", "", {246,13,270,46} },
    {"Jacob Taylor", "", {216,38,244,73} },
    {"Tim Cashin", "Look out belooooowww!", {188,24,212,58} },
    {"Sheelagh O'Connor", "", {159,39,187,81} },
    {"Dan Sackinger", "Symantec's own pool shark", {126,26,155,62} },
    {"Jim Brennan", "", {83,24,107,58} },
    {"Rob Ficcaglia", "", {51,42,83,83} },
    {"Olga Komsitsky", "I hear her shoes approaching...", {9,35,44,71} },
    {"David Shannon", "I'm having more fun!", {128,122,159,160} },
    {"Dave Buches", "\"boo-SHAY\"", {591,16,619,61} },
    {"Carlin Wong", "", {568,57,597,102} },
    {"Russel Latimer", "", {523,90,550,134} },
    {"Debbie Ka", "", {457,61,482,96} },
    {"Dan Uyemura", "", {434,95,460,132} },
    {"", "", {0,0,0,0} }
    };

    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);
    bSecret = ( MK_CONTROL == ( wParam & ( MK_CONTROL | MK_SHIFT ) ) );

    for ( DDS* p = &whee[0]; p->szText[0]; p++ )
        {
        if ( PtInRect ( &p->rBox, pt ) )
            {
            SetWindowText ( hwnd, (bSecret && p->szSecretText[0]) ?
                                       p->szSecretText : p->szText );
            return;
            }
        }

    SetWindowText ( hwnd, bSecret ? szWelcome : szGreet );
}

// END OF LINE ////////////////////////////////////////////////////////////////

