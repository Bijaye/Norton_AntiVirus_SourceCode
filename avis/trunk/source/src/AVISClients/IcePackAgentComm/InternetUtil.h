#ifndef __INTERNETUTIL__H__
#define __INTERNETUTIL__H__

#include <wininet.h>
#include <string>
#include "MyTraceClient.h"

#define AVISTX_RC_BASE				1000
#define AVISTX_SUCCESS				AVISTX_RC_BASE + 1
#define AVISTX_NETWORK_FAILURE		AVISTX_RC_BASE + 2
#define AVISTX_NO_HTTP_CODE			AVISTX_RC_BASE + 3
#define AVISTX_CRITICAL_ERROR		AVISTX_RC_BASE + 4
#define AVISTX_GATEWAY_ERROR		AVISTX_RC_BASE + 5
#define AVISTX_TERMINATED_BY_CALLER	AVISTX_RC_BASE + 6
#define AVISTX_FILE_READ_ERROR		AVISTX_RC_BASE + 7
#define AVISTX_YOUVE_BEEN_REDIRECTED  AVISTX_RC_BASE + 8

class InternetUtil
{
public:
	static bool		ParseURL(	LPCTSTR				pstrURL, 
								std::string&		server, 
								INTERNET_PORT&		nPort );

	static DWORD	ProcessInternetError( 
								MyTraceClient&		_tc, 
								InternetException*	pExcept = NULL );
};



#endif
