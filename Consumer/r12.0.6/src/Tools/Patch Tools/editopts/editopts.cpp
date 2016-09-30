// editopts.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    // Get the options file
    CNAVInfo navInfo;
    TCHAR szNavOptsFile[MAX_PATH] = {0};
    HNAVOPTS32 hNavOpts = NULL;
    TCHAR szExtensions[NAVOPTS32_MAX_STRING_LENGTH];

    _tcscpy(szNavOptsFile, navInfo.GetNAVOptPath());

    if(NULL == szNavOptsFile)
        return -1;

    // Find out if Script Blocking is installed by checking for
    // the existence of scriptUI.dll
    TCHAR szScriptUIDll[MAX_PATH] = {0};
    DWORD dwSBInstVal = 0;

    _tcscpy(szScriptUIDll, navInfo.GetNAVDir());
    _tcscat(szScriptUIDll, _T("\\ScriptUI.dll"));
    if( -1 != GetFileAttributes(szScriptUIDll) )
    {
        // Script Blocking is installed
        dwSBInstVal = 1;
    }

	if( NAVOPTS32_OK == NavOpts32_Allocate( &hNavOpts ) )
	{
        for( int i=0; i<3; i++)
        {
            // Open the current file
            NAVOPTS32_STATUS status = NAVOPTS32_OK;

            #ifdef _UNICODE
			    status = NavOpts32_LoadU( szNavOptsFile, hNavOpts, TRUE );
		    #else
			    status = NavOpts32_Load( szNavOptsFile, hNavOpts, TRUE );
		    #endif

		    if( status == NAVOPTS32_OK )
		    {
                // Add SCRIPTBLOCKING:Installed to the file
                NavOpts32_SetDwordValue( hNavOpts, SCRIPTBLOCKING_Installed, dwSBInstVal );
                
                // Add NAVPROXY:ShowProgressOut to the file with a value of 1
                NavOpts32_SetDwordValue( hNavOpts, NAVPROXY_ShowProgressOut, 1 );

                // Add SWF to the file extension list - read current values, add to SWF to them,
                // then write it back out
                status = NavOpts32_GetStringValue(hNavOpts, GENERAL_Ext, szExtensions, NAVOPTS32_MAX_STRING_LENGTH, NULL);

                if( NAVOPTS32_OK == status && NULL != szExtensions )
                {
                    // add SWF to szExtensions if it's not already there
                    if( NULL == _tcsstr(szExtensions,_T(" SWF ")) &&
                        NULL == _tcsstr(szExtensions,_T(" SWF")) &&
                        NULL == _tcsstr(szExtensions,_T("SWF ")) )
                    {
                        _tcscat(szExtensions, _T(" SWF"));
                        NavOpts32_SetStringValue(hNavOpts, GENERAL_Ext, szExtensions);
                    }
                }

                // Save the file
			    #ifdef _UNICODE
				    NavOpts32_SaveU( szNavOptsFile, hNavOpts );			
			    #else
				    NavOpts32_Save( szNavOptsFile, hNavOpts );			
			    #endif
            }

            if(i == 0)
            {
                // Change navopts file extension to navopts.def
                TCHAR* pszDot = _tcsrchr( szNavOptsFile, _TCHAR('.') );
                _tcscpy(CharNext(pszDot), _T("def"));
            }
            else if(i == 1)
            {
                // Change the navopts file extension to .bak
                TCHAR* pszDot = _tcsrchr( szNavOptsFile, _TCHAR('.') );
                _tcscpy(CharNext(pszDot), _T("bak"));
            }
        } // end for loop

        // Fire an options changed event so they get loaded
        CGlobalEvent event;
        if( event.Create( SYM_OPTIONS_CHANGED_EVENT ) )
		    ::PulseEvent( event );

        if( hNavOpts )
		    NavOpts32_Free( hNavOpts );
    }

	return 0;
}
