#include "platform.h"
#include "navbtmap.h"

static WNDPROC  lpfnWndProcBITMAP = NULL;

// ---------------------------------------------------------------------------
//                      LOCAL FUNCTION DEFINITIONS
// ---------------------------------------------------------------------------

LRESULT SYM_EXPORT WINAPI NAVBITMAP_CtlWndProc(
    HWND    hwnd,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    );

// ---------------------------------------------------------------------------


/*@API:*************************************************************************
@Declaration:  
BOOL WINAPI NAVBITMAP_CtlRegister(HINSTANCE hInst)

@Description:
This function registers the Norton AntiVirus Express bitmap class.

@Parameters:
$hInst$ instance handle provided by Windows.

@Returns:
TRUE if the bitmap class was successfully registered.

@Compatibility: Win16 and Win32
*******************************************************************************/

BOOL WINAPI NAVBITMAP_CtlRegister(      // Register our own bitmap class
    HINSTANCE   hInst                   // [in] instance handle
    )
{
    auto    WNDCLASS    wc;

    GetClassInfo ( NULL, "BUTTON", &wc );

    lpfnWndProcBITMAP   = wc.lpfnWndProc;
    wc.lpfnWndProc      = NAVBITMAP_CtlWndProc;
    wc.lpszClassName    = (LPSTR) NAVBITMAP;
    wc.style           |= (CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW);
    wc.hInstance        = hInst;
    wc.hbrBackground    = (HBRUSH) (COLOR_BTNFACE + 1);

    return ( RegisterClass(&wc) );
}

/*@API:*************************************************************************
@Declaration:  
LRESULT SYM_EXPORT WINAPI NAVBITMAP_CtlWndProc(
    HWND    hwnd,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam )

@Description:
This function receives the Norton AntiVirus Express bitmap class messages.

@Parameters:
$hwnd$ the Norton AntiVirus Express bitmap class control window handle.

$message$ the Norton AntiVirus Express bitmap class control message to process.

$wParam$ depends on the message to process.

$lParam$ depends on the message to process.

@Returns:
Depends on the message being processed.

@Compatibility: Win16 and Win32
*******************************************************************************/

LRESULT SYM_EXPORT WINAPI NAVBITMAP_CtlWndProc(
    HWND    hwnd,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    switch (message)
        {
        case WM_ERASEBKGND:
            return DefWindowProc( hwnd, message, wParam, lParam );
            break;
        }
                                        /********************************
                                         * Call the default window proc *
                                         * of the static control.       *
                                         ********************************/
    return CallWindowProc(lpfnWndProcBITMAP, hwnd, message, wParam, lParam );
}


