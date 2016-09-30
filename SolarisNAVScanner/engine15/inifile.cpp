//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/inifile.cpv   1.3   06 Nov 1998 17:15:42   rpulint  $
//
// Description:
//      Contains INI file functions
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/inifile.cpv  $
// 
//    Rev 1.3   06 Nov 1998 17:15:42   rpulint
// Added Changes for OS/2
// 
//    Rev 1.2   23 Aug 1997 11:17:28   STRILLI
// Added checks for USE_HEUR so that we use heuristics on the 
// right platforms.
// 
//    Rev 1.1   18 Jul 1997 16:10:08   CNACHEN
// Stubbed stuff out for NTK platforms
// 
// 
// 
//    Rev 1.0   11 Jun 1997 17:34:20   CNACHEN
// Initial revision.
// 
//************************************************************************

// #include "platform.h"	// NEW_UNIX
#include "avtypes.h" 		// NEW_UNIX
#include "inifile.h"
#include "callfake.h"
#include "flimflam.h"

#if defined(USE_HEUR)

/*
int FileRead
(
    HFILE               hFile,
    LPVOID              lpvBuffer,
    int                 nBytes
)
{
    return(fread(lpvBuffer,1,nBytes,hFile));
}

int FileSeek
(
    HFILE               hFile,
    long                lOff,
    int                 nWhence
)
{
    return(fseek(hFile,lOff,nWhence));
}
*/

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
    LPSTR               lpszString
)
{
    int     nValue = 0;
    BOOL    bNeg = FALSE;

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
    HFILE               hFile,
    LPSTR               lpszLine,
    int                 nLineBufferSize
)
{
    int                 i = 0, j, k;
    BOOL                bEOF = FALSE;
    char                ch;

    while (i < nLineBufferSize)
    {
        if (FileRead(hFile,&lpszLine[i],1) != 1)
        {
            bEOF = TRUE;
            break;
        }

        if (lpszLine[i] == '\r' || lpszLine[i] == '\n')
        {
            int nRead;

            do
            {
                nRead = FileRead(hFile,&ch,1);

                if (nRead != 1)
                {
                    lpszLine[i] = 0;
                    bEOF = TRUE;
                    break;
                }
            }
            while (ch == '\r' || ch == '\n');

            if (nRead == 1)
                FileSeek(hFile,-1,SEEK_CUR);

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

    FileSeek(hFile,0,SEEK_SET);

    // construct our section string

    mystrcpy(szSectionString,"[");
    mystrcat(szSectionString,lpszSection);
    mystrcat(szSectionString,"]");

    // zero our output

    *lpszOut = 0;

    while (GetLine(hFile,szBuffer,512) != -1)
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

        while (GetLine(hFile,szBuffer,512) != -1)
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
    HFILE               hFile,
    LPSTR               lpszSection,
    LPSTR               lpszKey,
    int                 nDefault,
    LPINT               lpnOut
)
{
    int                 nLen;
    char                szOut[32];

    nLen = GetProfileString(hFile,
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

/*
main(int argc, char *argv[])
{
    FILE *stream = fopen(argv[1],"rb");
    char           szBuffer[129];
    int             nLineNum = 0;


    if (stream == NULL)
    {
        printf("error opening file %s\n",argv[1]);
        return(0);
    }

    while (1)
    {
        char szSection[100], szKey[100], szOut[100];
        int nLen, nOut;

        printf("Enter section:");
        gets(szSection);

        printf("Enter key:");
        gets(szKey);

        nLen = GetProfileInt(stream,
                             szSection,
                             szKey,
                             -100,
                             &nOut);

        printf("%d\n",nOut);
    }

    fclose(stream);
    return(0);
}
*/

#endif // #if defined(USE_HEUR)
