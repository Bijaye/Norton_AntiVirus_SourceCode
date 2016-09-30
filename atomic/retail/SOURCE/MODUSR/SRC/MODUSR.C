// Copyright 1997 Symantec Corporation, Peter Norton Product Group
//**************************************************************************/
//*                                                                        */
//* $Header:   S:/MODUSR/VCS/modusr.c_v   1.0   25 Jun 1997 16:01:42   DSACKIN  $                                                           */
//*                                                                        */
//* MODUSR.CPP                                                             */
//*                                                                        */
//*     To encrypt username, password and Domain name in given file.       */
//*                                                                        */
//**************************************************************************/
//*
//* $Log:   S:/MODUSR/VCS/modusr.c_v  $
// 
//    Rev 1.0   25 Jun 1997 16:01:42   DSACKIN
// Initial revision.
// 
//    Rev 1.1   24 Apr 1997 10:48:34   pvenkat
// #ifdef'ed Decrypt.  Now this utility cannot decrypt the entries.
// 
// 
//    Rev 1.0   16 Apr 1997 15:51:46   pvenkat
// Initial revision.
//*
//**************************************************************************/

#include    <windows.h>
#include    <stdio.h>
#include    <tchar.h>

// Key
TCHAR szKey[] = TEXT ("NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221NAVWNT221");


//**************************************************************************/
//*
//* Encrypt
//*     To encrypt given string based on Key and return encrypted.
//*
//* Parameters:
//*     pszPlain        LPTSTR      String to encrypt
//*     pszEncrypted    LPTSTR      on return Encrypted string
//*     pszKey          LPTSTR      Key
//*
//* Returns:
//*     - None -
//*
//**************************************************************************/
//* 1/23/97     PVENKAT     Function Created.
//**************************************************************************/
void
Encrypt (LPTSTR pszPlain, LPTSTR pszEncrypted, LPTSTR pszKey)
{
    int nLen = (lstrlen (pszPlain)  + 1), i;
    LPTSTR pTemp = pszEncrypted;

    // Cleanup the output.
    memset (pszEncrypted, 0, 256);

    for (i = 0; i < nLen; i++)
    {
        *pszEncrypted = *pszKey;
        if (*pszPlain != TEXT ('\0'))
            *pszEncrypted += *pszPlain++;
        pszEncrypted++;
        pszKey++;
    }
    *pszEncrypted = TEXT ('\0');
}

#ifdef      DECRYPT
//**************************************************************************/
//*
//* Decrypt
//*     To decrypt given string based on Key and return encrypted.
//*
//* Parameters:
//*     pszPlain        LPTSTR      [OUT] Decrypted string
//*     pszEncrypted    LPTSTR      Encrypted String
//*     pszKey          LPTSTR      Key
//*
//* Returns:
//*     - None -
//*
//**************************************************************************/
//* 1/23/97     PVENKAT     Function Created.
//**************************************************************************/
void
Decrypt (LPTSTR pszPlain, LPTSTR pszEncrypted, LPTSTR pszKey)
{
    int len = lstrlen (pszEncrypted), i;
    LPTSTR pTemp = pszPlain;

    for (i = 0; i < len; i++)
    {
        TCHAR c1 = *pszEncrypted;
        TCHAR c2 = *pszKey;
        TCHAR c3 = c1 - c2;

        *pszPlain++ = c3;
        pszEncrypted++;
        pszKey++;
    }
    *pszPlain = '\0';
}
#endif


//**************************************************************************/
//*
//* GetString
//*     To read from the INI file.
//*
//* Parameters:
//*      szUser      LPTSTR      User Name
//*      szDomain    LPTSTR      Domain Name
//*      szPassword  LPTSTR      Password
//*      lpszFile    LPTSTR      Ini File.
//*
//* Returns:
//*      TRUE on success
//*      FALSE on FAILURE
//*
//**************************************************************************/
//* 1/23/97     PVENKAT     Function Created.
//**************************************************************************/
BOOL
GetString (LPTSTR szUser, LPTSTR szDomain, LPTSTR szPassword, LPTSTR lpszFile)
{
    BOOL    bRet = TRUE;    // always start with success;

    if (GetPrivateProfileString (TEXT ("Install/Update"),
                                TEXT ("ImpersonationUserName"),
                                TEXT (""),
                                szUser,
                                MAX_PATH,
                                lpszFile))
    {
        if (GetPrivateProfileString (TEXT ("Install/Update"),
                                    TEXT ("ImpersonationDomain"),
                                    TEXT (""),
                                    szDomain,
                                    MAX_PATH,
                                    lpszFile))
        {
            if (!GetPrivateProfileString (TEXT ("Install/Update"),
                                        TEXT ("ImpersonationPassword"),
                                        TEXT (""),
                                        szPassword,
                                        MAX_PATH,
                                        lpszFile))
                bRet = FALSE;
        }
        else
            bRet = FALSE;
    }
    else
        bRet = FALSE;

    return bRet;
}


//**************************************************************************/
//*
//* main
//*     Main entry point
//*
//* Parameters:
//*      argc  int      #of arguments
//*      argv  TCHAR ** Arguments
//*
//* Returns:
//*     - None -
//*
//**************************************************************************/
//* 1/23/97     PVENKAT     Function Created.
//**************************************************************************/
void
#ifdef   UNICODE
wmain (int argc, TCHAR **argv)
#else
main (int argc, char **argv)
#endif
{
    TCHAR    szUser[256], 
            szDomain[256], 
            szPassword[256], 
            szOut[256];
    LPTSTR  lpszFile = NULL;
    int     nOpt;

    if (argc != 2)
    {
        fprintf (stderr, "Usgae: MODUSR <NNS File>\n");
        exit (0);
    }
    else
        lpszFile = argv[1];

    nOpt = GetPrivateProfileInt (TEXT ("Install/Update"),
                                    TEXT ("ImpersonationModified"),
                                    0,
                                    lpszFile);

    if (GetString (szUser, szDomain, szPassword, lpszFile))
    {
        if (nOpt == 0)
        {
            Encrypt ((LPTSTR) szUser, (LPTSTR) szOut, (LPTSTR) szKey);
            WritePrivateProfileString (TEXT ("Install/Update"), 
                                TEXT ("ImpersonationUserName"), 
                                szOut, 
                                lpszFile);
            Encrypt ((LPTSTR)szDomain, (LPTSTR) szOut, (LPTSTR) szKey);
            WritePrivateProfileString (TEXT ("Install/Update"),
                                TEXT ("ImpersonationDomain"),
                                szOut,
                                lpszFile);

            Encrypt ((LPTSTR) szPassword, (LPTSTR) szOut, (LPTSTR) szKey);
            WritePrivateProfileString (TEXT ("Install/Update"),
                                TEXT ("ImpersonationPassword"),
                                szOut,
                                lpszFile);
            WritePrivateProfileString (TEXT ("Install/Update"),
                                TEXT ("ImpersonationModified"),
                                TEXT ("1"),
                                lpszFile);
        }
#ifdef   DECRYPT
        else
        {
            Decrypt (szOut, szUser, szKey);
            WritePrivateProfileString (TEXT ("Install/Update"), 
                                TEXT ("ImpersonationUserName"), 
                                szOut, 
                                lpszFile);
            Decrypt (szOut, szDomain, szKey);
            WritePrivateProfileString (TEXT ("Install/Update"),
                                TEXT ("ImpersonationDomain"),
                                szOut,
                                lpszFile);

            Decrypt (szOut, szPassword, szKey);
            WritePrivateProfileString (TEXT ("Install/Update"),
                                TEXT ("ImpersonationPassword"),
                                szOut,
                                lpszFile);
            WritePrivateProfileString (TEXT ("Install/Update"),
                                TEXT ("ImpersonationModified"),
                                TEXT ("0"),
                                lpszFile);
        }
#endif
    }
}
