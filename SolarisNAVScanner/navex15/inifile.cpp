//************************************************************************
//
// $Header:   S:/NAVEX/VCS/inifile.cpv   1.5   10 Nov 1998 13:09:18   rpulint  $
//
// Description:
//      Contains INI file functions
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/inifile.cpv  $
// 
//    Rev 1.5   10 Nov 1998 13:09:18   rpulint
// re-checkin the changes made for OS/2 platform
// 
//    Rev 1.3   06 Nov 1998 17:35:38   rpulint
// Added changes for OS/2 platform
// 
//    Rev 1.2   03 Nov 1997 18:08:56   CNACHEN
// Updated to use LPCALLBACKREV1 (Defined to REV2 for NAVEX15 automatically)
// 
// 
//    Rev 1.1   08 Aug 1997 12:59:40   DCHI
// Added #defines of SEEK_CUR, etc.
// 
//    Rev 1.0   07 Aug 1997 18:23:22   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "callback.h"
#include "inifile.h"

#if defined(SYM_NLM) || defined(SYM_NTK)

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

#endif

void mystrcpy
(
    LPSTR           lpszDest,
    LPSTR           lpszSource
)
{
    while (*lpszSource)
        *lpszDest++ = *lpszSource++;

    *lpszDest = 0;
}

void mystrncpy
(
    LPSTR           lpszDest,
    LPSTR           lpszSource,
    int             nLen
)
{
    int             i = 0;

    while (*lpszSource && i < nLen)
    {
        *lpszDest++ = *lpszSource++;
        i++;
    }

    *lpszDest = 0;
}

void mystrcat
(
    LPSTR           lpszDest,
    LPSTR           lpszSource
)
{
	while (*lpszDest++);
	lpszDest--;
    mystrcpy(lpszDest,lpszSource);
}

int mytoupper
(
    int     ch
)
{
    if (ch >= 'a' && ch <= 'z')
        ch -= 32;

    return(ch);
}

void mystrupr
(
    LPSTR           lpszString
)
{
    while (*lpszString)
    {
        *lpszString = mytoupper(*lpszString);
        lpszString++;
    }
}

int mystricmp
(
    LPSTR           lpszS1,
    LPSTR           lpszS2
)
{
    int             nCmp;

    while (*lpszS1 && *lpszS2)
    {

        nCmp = mytoupper(*lpszS1) - mytoupper(*lpszS2);
        if (nCmp != 0)
			return(nCmp);

		lpszS1 ++;
		lpszS2 ++;
    }

    return(*lpszS1 - *lpszS2);
}

int mymemicmp
(
    LPSTR           lpszS1,
    LPSTR           lpszS2,
    int             nLen
)
{
    int             nCmp, i = 0;

    while (*lpszS1 && *lpszS2 && i < nLen)
    {

        nCmp = mytoupper(*lpszS1) - mytoupper(*lpszS2);
        if (nCmp != 0)
			return(nCmp);
		lpszS1++;
		lpszS2++;
		i++;
    }

    if (i == nLen)
        return(0);

    return(*lpszS1 - *lpszS2);
}

int mystrlen
(
    LPSTR           lpszString
)
{
	int             nLen = 0;

	while (*lpszString)
	{
		lpszString ++;
		nLen++;
	}

	return(nLen);
}

LPSTR mystrchr
(
    LPSTR               lpszSource,
    char                cFindMe
)
{
    while (*lpszSource)
    {
        if (*lpszSource == cFindMe)
            return(lpszSource);

        lpszSource++;
    }

    return(NULL);
}

int myatoi
(
	LPSTR				lpszString
)
{
	int 	nValue = 0;
	BOOL	bNeg = FALSE;

	while (*lpszString == ' ' || *lpszString == '\t')
	{
		lpszString++;
	}


	if (*lpszString == '-')
	{
		lpszString++;
		bNeg = TRUE;
	}

	while (*lpszString)
	{
		if (*lpszString < '0' || *lpszString > '9')
			break;

		nValue *= 10;
		nValue += *lpszString - '0';

		lpszString++;
	}

	if (bNeg == TRUE)
		nValue = -nValue;

	return(nValue);
}


int GetLine
(
    LPCALLBACKREV1      lpstCallBacks,          // used to be rev2
    HFILE               hFile,
	LPSTR               lpszLine,
	int                 nLineBufferSize
)
{
	int                 i = 0, j, k;
	BOOL				bEOF = FALSE;
	char                ch;

	while (i < nLineBufferSize)
	{
        if (lpstCallBacks->FileRead(hFile,&lpszLine[i],1) != 1)
		{
			bEOF = TRUE;
			break;
		}

		if (lpszLine[i] == '\r' || lpszLine[i] == '\n')
		{
			int nRead;

			do
			{
                nRead = lpstCallBacks->FileRead(hFile,&ch,1);

				if (nRead != 1)
				{
					lpszLine[i] = 0;
					bEOF = TRUE;
					break;
				}
			}
			while (ch == '\r' || ch == '\n');

			if (nRead == 1)
                lpstCallBacks->FileSeek(hFile,-1,SEEK_CUR);

			break;
		}

		i++;
	}

	lpszLine[i] = 0;

	if (i > 0)
	{
		k = i;

		for (j=0;j<i;j++)
		{
			if (lpszLine[j] == ';')
			{
				lpszLine[j] = 0;
				k = j;
				break;
			}
		}

		for (j=k-1;j>=0;j--)
		{
			if (lpszLine[j] == ' ' || lpszLine[j] == '\t')
				lpszLine[j] = 0;
			else
				break;
		}

	}

    if (bEOF == TRUE && i == 0)
		return(-1);

	return(i);
}


int GetProfileString
(
    LPCALLBACKREV1      lpstCallBacks,          // used to be rev2
    HFILE               hFile,
    LPSTR               lpszSection,
    LPSTR               lpszKey,
    LPSTR               lpszDefault,
    LPSTR               lpszOut,
    int                 nOutBufferSize
)
{
#ifdef __WATCOMC__
	char				szBuffer[513];
	char				szSectionString[128];
#else		
    char                szBuffer[513], szSectionString[128];
#endif
	
    BOOL                bFoundSection = FALSE;

    // go to the TOF

    lpstCallBacks->FileSeek(hFile,0,SEEK_SET);

    // construct our section string

    mystrcpy(szSectionString,"[");
    mystrcat(szSectionString,lpszSection);
    mystrcat(szSectionString,"]");

    // zero our output

    *lpszOut = 0;

    while (GetLine(lpstCallBacks,hFile,szBuffer,512) != -1)
    {
        if (mystricmp(szBuffer,szSectionString) == 0)
        {
            bFoundSection = TRUE;
            break;
		}
	}

	if (bFoundSection == TRUE)
	{
		int         nLen = mystrlen(lpszKey);

        while (GetLine(lpstCallBacks,hFile,szBuffer,512) != -1)
		{
			if (szBuffer[0] == '[')
				break;

			if (mymemicmp(szBuffer,lpszKey,nLen) == 0)
            {
                LPSTR           lpszPtr;

				lpszPtr = mystrchr(szBuffer,'=');
                if (lpszPtr != NULL)
                {
                    mystrncpy(lpszOut,lpszPtr+1,nOutBufferSize-1);
					return(mystrlen(lpszOut));
                }

                break;
            }
        }
    }

    if (nOutBufferSize != 0)
        mystrncpy(lpszOut,lpszDefault,nOutBufferSize-1);

	return(mystrlen(lpszOut));
}



int GetProfileInt
(
    LPCALLBACKREV1      lpstCallBacks,          // used to be rev2
    HFILE               hFile,
    LPSTR               lpszSection,
    LPSTR               lpszKey,
	int                 nDefault,
	LPINT               lpnOut
)
{
	int 				nLen;
	char				szOut[32];

    nLen = GetProfileString(lpstCallBacks,
                            hFile,
							lpszSection,
							lpszKey,
							"",
							szOut,
							32);

	if (nLen == 0)
		*lpnOut = nDefault;
	else
		*lpnOut = myatoi(szOut);

	return(*lpnOut);
}

