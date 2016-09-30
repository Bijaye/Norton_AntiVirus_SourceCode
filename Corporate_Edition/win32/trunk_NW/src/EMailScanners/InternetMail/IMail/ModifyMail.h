// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// ModifyMail.h:
//
// Implementation of the CIMailStorage notifications
//      Insertion of warning messages
//*************************************************************************

#ifndef __MODIFYMAIL_H
#define __MODIFYMAIL_H


#define     MAX_RFC821_LINE     1001    // Spec is for 1000 (including CRLF) + 1 for encoding leading '.'

typedef enum _MAIL_MODIFICATION_ACTION {
    MODIFICATION_DELETE = 1,
    MODIFICATION_REPLACE,
    MODIFICATION_INSERT
} MAIL_MODIFICATION_ACTION;


typedef struct _MAIL_MODIFICATION_LIST
{
    MAIL_MODIFICATION_ACTION    nAction;

    DWORD       dwStart;
    DWORD       dwEnd;

    LPCTSTR     lpszNewData;

    struct _MAIL_MODIFICATION_LIST  *pNext;
} MAIL_MODIFICATION_LIST, * PMAIL_MODIFICATION_LIST;

DWORD InsertWarningMessage( LPCTSTR lpszMessageImage, LPCTSTR lpszWarning, LPCTSTR lpszNewSubject );

DWORD GetMailModificationInfo( LPCTSTR  szFileName,
                               LPDWORD  pdwSubjectStart,
                               LPDWORD  pdwSubjectEnd,
                               LPDWORD  pdwHeaderEnd );

DWORD ModifyMail( LPCTSTR szToFile,
                  LPCTSTR szFromFile,
                  PMAIL_MODIFICATION_LIST pModifications );

void EncodeSubject( LPSTR pszNewField, LPCSTR pszSubject, int cchNewField );

int ConvertWindowsString ( UINT         uSrcCodePage,
                           UINT         uDstCodePage,
                           LPCSTR       pszSrc,
                           LPSTR        pszDst,
                           int          cchDst );

UINT GetDefaultEmailCodePage( );

#endif // __MODIFYMAIL_H