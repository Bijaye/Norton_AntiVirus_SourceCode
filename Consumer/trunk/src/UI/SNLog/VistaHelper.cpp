////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "shellapi.h"   // shellexecute

#include "ISSharedPathProvider.h"
#include "ccOSInfo.h"
 



 

bool ValidateUser ()

{
	ccLib::COSInfo m_OSInfo;	
    if (m_OSInfo.IsWinVista(true)) // If Vista, or later

    {

        // Vista

        //

        // Shell Execute the stub, if it works the user is an admin

        TCHAR szPath[_MAX_PATH] = {0};

        size_t sizePath = _MAX_PATH;

 

        if ( CISSharedPathProvider::GetPath(szPath, sizePath))

        {

            _tcscat ( szPath, _T("\\isUAC.exe"));

            HINSTANCE hInstance = ::ShellExecute(::GetDesktopWindow(), _T("open"), szPath, NULL, NULL, SW_SHOWNORMAL);

 

                    if ( hInstance <= (HINSTANCE) 32)

                    {

                CCTRCTXE2(_T("Failed to launch %s -- %d"), szPath, ::GetLastError());

                                return false;

                    }

 

            CCTRCTXI0(_T("validated"));

            return true;

        }

        else

        {

            CCTRCTXE0(_T("Failed to get path"));

            return false;

        }

 

    }

    else

    {

        // XP

        return m_OSInfo.IsAdministrator();

    }

}

