//***************************************************************************
// HyperLink.cpp : implementation file
//***************************************************************************
// CBROWN 07/10/1999
//***************************************************************************
#include "stdafx.h"
#include "HLink.h"
#include "HyperLink.h"

#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//***************************************************************************
// Local functions...
//***************************************************************************
namespace {
LONG GetRegKey( HKEY key, LPCTSTR subkey, LPTSTR retdata )
{
    HKEY hkey(0);

    LONG retval = RegOpenKeyEx( key, 
		                        subkey, 
								0, 
								KEY_QUERY_VALUE, 
								&hkey );

    if( retval == ERROR_SUCCESS) 
	{
		TCHAR data[MAX_PATH];
		long datasize = (sizeof(data)/sizeof(data[0]));

		RegQueryValue(hkey, NULL, data, &datasize);

		lstrcpy(retdata,data);

		RegCloseKey(hkey);
    }

    return retval;
}}

//***************************************************************************
// CHyperLink
//***************************************************************************
CHyperLink::CHyperLink() :
m_hHandCursor(NULL),
m_crHyperLinkColor( RGB(0,0,255) )
{

}

//***************************************************************************
//***************************************************************************
CHyperLink::~CHyperLink()
{
	m_Font.DeleteObject();

	//
	// Cleanup the cursor...
	//
	if( m_hHandCursor )
	{
		DestroyCursor( m_hHandCursor );
	}
}


//***************************************************************************
//***************************************************************************
BEGIN_MESSAGE_MAP(CHyperLink, CStatic)
	//{{AFX_MSG_MAP(CHyperLink)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SETCURSOR()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//***************************************************************************
// CHyperLink message handlers
//***************************************************************************
void CHyperLink::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class

	//
	// Set underline on font... 
	//
	CFont  *pTheFont = GetFont();

	if( NULL != pTheFont )
	{
		LOGFONT lf = {0};

		pTheFont->GetLogFont(&lf);

		lf.lfUnderline = TRUE;


		if( m_Font.CreateFontIndirect(&lf) )
		{
			CStatic::SetFont(&m_Font);    
		}
	}

	//
	// Load the hand cursor...
	//
	m_hHandCursor = AfxGetApp()->LoadCursor( IDC_HAND_CURSOR );


	//
	// We failed to load our hand cursor resource...  Oh, so bad!
	//
	ASSERT( m_hHandCursor );


	CStatic::PreSubclassWindow();
}


//***************************************************************************
//***************************************************************************
HBRUSH CHyperLink::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	//
	// Set the text color and transparent mode...
	//
	pDC->SetTextColor( m_crHyperLinkColor );
	pDC->SetBkMode( TRANSPARENT );
	
	//
	// Return stock null brush...
	//
	return static_cast<HBRUSH>( GetStockObject(NULL_BRUSH) );
}



//***************************************************************************
//***************************************************************************
BOOL CHyperLink::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	BOOL bResult(FALSE);

	if( m_hHandCursor )
	{
		::SetCursor( m_hHandCursor );

		bResult = TRUE;
	}
	else
	{
	
		bResult = CStatic::OnSetCursor(pWnd, nHitTest, message);
	}

	return bResult;
}

//***************************************************************************
// "Normally, a static control does not get mouse events unless it has
// SS_NOTIFY. This achieves the same effect as SS_NOTIFY, but it's fewer
// lines of code and more reliable than turning on SS_NOTIFY in OnCtlColor
// because Windows doesn't send WM_CTLCOLOR to bitmap static controls."
// (Paul DiLascia)
//***************************************************************************
UINT CHyperLink::OnNcHitTest(CPoint point) 
{
	return HTCLIENT;		
}

//***************************************************************************
//***************************************************************************
void CHyperLink::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//
	// Get the window text for the URL...
	//
	CString strWindowText;
	GetWindowText( strWindowText );

	//
	// Execute the URL....
	//
	GotoURL( strWindowText, SW_SHOW );

	CStatic::OnLButtonDown(nFlags, point);
}


//***************************************************************************
// "GotoURL" function by Stuart Patterson
// As seen in the August, 1997 Windows Developer's Journal.
//***************************************************************************
HINSTANCE CHyperLink::GotoURL( LPCTSTR url, int showcmd )
{
    TCHAR key[MAX_PATH + MAX_PATH];	

	//
    // First try ShellExecute()
	//
    HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL,NULL, showcmd);

	//
    // If it failed, get the .htm regkey and lookup the program
	//
    if( (UINT)result <= HINSTANCE_ERROR ) 
	{		
        if( ERROR_SUCCESS == GetRegKey( HKEY_CLASSES_ROOT, _T(".htm"), key ))  
		{
            _tcscat( key, _T("\\shell\\open\\command") );

            if( ERROR_SUCCESS == GetRegKey(HKEY_CLASSES_ROOT,key,key) ) 
			{
                TCHAR *pos;

                pos = _tcsstr( key, _T("\"%1\"") );

                if (pos == NULL) 
				{										// No quotes found
                    pos = strstr(key, _T("%1"));       // Check for %1, without quotes
                    
					if (pos == NULL)                   // No parameter at all...
					{
                        pos = key+lstrlen(key)-1;
					}
                    else
					{
                        *pos = '\0';                   // Remove the parameter
					}
                }
                else
				{
                    *pos = '\0';                       // Remove the parameter
				}

                _tcscat(pos, _T(" "));
                _tcscat(pos, url);

                result = reinterpret_cast<HINSTANCE>( WinExec(key,showcmd) );
            }
        }
	}
	  
    return result;
}


