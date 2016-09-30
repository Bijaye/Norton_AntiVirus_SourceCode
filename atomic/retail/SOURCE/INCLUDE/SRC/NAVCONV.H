#ifndef _CONVERT_NAV_OPTIONS_
#define _CONVERT_NAV_OPTIONS_

STATUS WINAPI ConvertNavOptions (       // Initialize NAV options structure
    DWORD dwVersion,                    // [in] depending on version number
    LPNAVOPTIONS lpNavOptions,          // [in] actual version structure
    LPSTR lpBuffer );                   // [in] buffer as read from disk

#endif // _CONVERT_NAV_OPTIONS_

