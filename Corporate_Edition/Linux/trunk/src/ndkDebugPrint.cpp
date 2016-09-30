
/***************************************************************************************/
#include "REGISTRY.H"
#include "ndkDebugPrint.h"
#include <syslog.h>

void SysLogLine( char* line )
{
    // Dump each line separately
    char* str = line;
    while ( str )
    {
	char* ptr = strchr( str, '\n' );
	if ( ptr )
        {
	    if ( ptr > str && ptr[-1] == '\r' )
	    {
		ptr[-1] = 0;
	    }
	    *ptr++ = 0;
	}

	if ( str == line || str[0] )
	{
	    syslog( LOG_DEBUG, "%s", str );
	}
        str = ptr;
    }
}
/***************************************************************************************/
char *_VPstrncpy(char *d,const char *s,int n) {
	char *org=d;
	n--;
	while(n&&*s) {
		*d++=*s++;
		n--;
		}
	*d=0;
	return org;
}		//MLR Fixed
/***************************************************************************************/
BOOL StrEqual(char *s1,char *s2) {// returns TRUE if the strings are the same

	size_t i;

	if (strlen(s1) != strlen(s2)) {
		return FALSE;
	}

	for (i = 0 ; i < strlen(s1) && i < strlen(s2) ; i++) {
		if (s1[i] != s2[i]) {
			return FALSE;
		}
	}

	return TRUE;
}
/***************************************************************************************/
