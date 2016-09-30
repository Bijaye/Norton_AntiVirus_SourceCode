///////////////////////////////////////////////////////////////////////////
// Copyright (c)1993-1997 - Bruce Hellstrom
///////////////////////////////////////////////////////////////////////////
//
//  $Header: Cwstring.h  Revision:1.2  Mon Jan 26 07:18:36 1998  BRUCEH $
//
//  CW_STRING.H   - This file contains class definitions for the CWString
//                  Class Objects.
//
//
//
///////////////////////////////////////////////////////////////////////////


#ifndef _CW_STRING_H_
#define _CW_STRING_H_

///////////////////////////////////////////////////////////////////////////
//  String size defines
///////////////////////////////////////////////////////////////////////////

#define CW_LARGE_STRING         512
#define CW_MAX_PATH             260
#define CW_MAX_FILENAME         256
#define CW_MIN_STRING           48

#define MIN_ALLOC_DEFAULT           265
#define MAX_CWSERRMSG_SIZE          256
#define MAX_CWSERRCAP_SIZE          128

///////////////////////////////////////////////////////////////////////////
//  Debugging support
///////////////////////////////////////////////////////////////////////////

#define STR_ASSERT_CAPTION  "CW_Class Library"
#define STR_ASSERT_FAILED   "Assertion Failed!\nFile: %s\nLine: %d\n"

void WINAPI CW_AssertFailedLine( LPCSTR lpszFileName, int nLine );


#ifdef _DEBUG

#define CWASSERT(f)     ((f) ? (void)0 : ::CW_AssertFailedLine(__FILE__, __LINE__))

#else

#define CWASSERT(f)     (void)0

#endif                                  // _DEBUG


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// The CWString class is used as almost invisible replacements
// for character strings defined on the stack frame.  CWStrings
// allocate their buffers from the heap instead of using valuable stack
// space.  While designed for frame allocated string replacements, they
// can be used equally well when declared on the heap.  The string classes
// offer automatic buffer cleanup upon destruction either when allocated
// on the frame or when the delete operator is used for objects allocated
// from the heap.
//
// CWStrings are allocated from the global heap and can be used in almost
// any case where a LPSTR is required.  CWNStrings are allocated from the
// local heap and can be used in almost any case where a PSTR is required,
// for things like C Run-time functions.  If the application is large model,
// CWStrings can be used for all cases and the CWNStrings should not be
// necessary.
//
// Most pointer operations can be used on CWStrings invisibly, however, some
// may not allowed due to improper lvalues.  Some examples are shown below:
//
// BOOL WINAPI MyFunction()
// {
//      char            cChar;
//      CWString        cwStr(260);
//      CWString        cwStr2(260);
//
//      *cwStr = 0;                      // supported
//      cwStr[0] = 0;                    // supported
//
//      cwStr = "This is a test string"; // supported
//      cChar = cwStr[10];               // supported
//      cChar = *cwStr;                  // supported
//      cChar = *(cwStr + 10);           // supported
//
//      cwStr2 = cwStr;                  // supported (equivalent to next example)
//      lstrcpy(cwStr2, cwStr);          // supported
//
//      return;
// }
//
// When an allocation error occurs, the application is terminated immediately
// by using the abort() function.  An application can put up an application
// defined message prior to termination by using the RegisterCWStrings() function
// and supplying the HINSTANCE and HWND of the application along with the
// resource IDs of the text and caption for the message.  This function also
// allows the definition of the default string size to be used if none is given.
// CWStrings do *not* dynamically resize their buffers!  The
// buffer is allocated in a fixed size during construction.  The size of the
// buffer is defined in one of three ways:
//
// 1) by passing the desired size when declaring the object;
//   i.e.    CWString   cwStr(260);
// 2) by passing a default size value to the RegisterCWStrings() function;
// 3) if no default is specifed in #2 above, a default of 265 is used.
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  Application registration
///////////////////////////////////////////////////////////////////////////

BOOL WINAPI RegisterCWStrings( HWND hWnd, HINSTANCE hInst, int nDefAlloc,
                               UINT uResText, UINT uResCaption );



///////////////////////////////////////////////////////////////////////////
//  CWString class definition
///////////////////////////////////////////////////////////////////////////

class CWString
{
public:
                                        // construction
    CWString();
    CWString( const CWString& strsrc );
    CWString( int buffsize );
                                        // destruction
    ~CWString();
                                        // public interface

    int  StrLen( void ) const;          // length in bytes
    int  GetLength( void ) const;       // length in bytes
    int  GetAllocSize( void ) const;
    void Empty();
    BOOL IsEmpty( void ) const;
    void FreeString( void );
    BOOL NewSize( int buffsize );
    LPTSTR GetPtr( void );
    LPTSTR GetPtrToChar( int nIndex );
    LPTSTR FindEndOfString();
    LPTSTR FindLastChar();

    CWString Left( int nChars );
    CWString Mid( int nStart, int nChars );
    CWString Right( int nChars );

    BOOL SetExtraData( DWORD dwData );
    DWORD GetExtraData( void );

    int   FindChar( UINT tcFind );
    int   FindCharRev( UINT tcFind );
    int   FindString( LPTSTR lpSubStr, BOOL bCaseSensitive = FALSE );
    int   FindString( int nStart, LPTSTR lpSubStr, BOOL bCaseSensitive = FALSE );
    TCHAR GetAt( int nIndex ) const;
    void  SetAt( int nIndex, TCHAR ch );
    void  MakeUpperAnsi( void );
    void  MakeLowerAnsi( void );
    void  StripEndWhiteSpace( void );
    void  StripBeginWhiteSpace( void );
    BOOL  StripFileName( void );
    BOOL  StripExtension( void );
    void  AppendFile( LPSTR lpFile );
    void _cdecl CWString::Format( LPTSTR lpFmt, ... );

    void  AnsiToOem( void );
    void  OemToAnsi( void );
                                        // overloaded operators
    TCHAR& operator[]( int nIndex );
    TCHAR& operator[]( WORD wIndex );

    operator LPTSTR();
    operator LPCTSTR() const;

    const CWString& operator+( const CWString& strAdd );
    const CWString& operator+( LPCTSTR lpAdd );
    const CWString& operator+( TCHAR cAdd );

    const CWString& operator+=( const CWString& strAdd );
    const CWString& operator+=( LPCTSTR lpAdd );
    const CWString& operator+=( TCHAR cAdd );

    const CWString& operator=( const CWString& strsrc );
    const CWString& operator=( TCHAR ch );
    const CWString& operator=( LPTSTR lpData );
    const CWString& operator=( LPCTSTR lpData );

    char&  operator*();

    friend LPTSTR WINAPI operator+( CWString& cwStr, int nIndex );
    friend LPTSTR WINAPI operator+( int nIndex, CWString& cwStr );

protected:
    int         m_nSize;
    LPDWORD     m_pdwExtra;
    LPTSTR      m_lpData;

    BOOL Init( int buffsize );
    BOOL AllocBuf( int buffsize );
    void DestroyBuf( void );
    void ZeroBuf( void );
    BOOL ReAllocBuf( int nNewSize );
    int  CWStringAssert();
    LPTSTR DBCSCopyChar( LPTSTR lpSrc, LPTSTR lpDest );

    BOOL CheckValid()
        { return( m_nSize && ( m_lpData != NULL ) ); }

    BOOL CheckValid() const
        { return( m_nSize && ( m_lpData != NULL ) ); }

    BOOL CWSIsDBCSLeadByte( BYTE ch );

};



///////////////////////////////////////////////////////////////////////////
//  Inline functions to mimic sizeof for these string objects
///////////////////////////////////////////////////////////////////////////

int _inline cwsizeof( const CWString& cwStr )
{
    return( cwStr.GetAllocSize() );
}



#endif                                  // ifndef _this_file_



///////////////////////////////////////////////////////////////////////////
//  $Log: G:\QVCS\LOGS\BHUtils32\Cwstring.i $
//  
//    
//  
//  Revision 1.2  by: BRUCEH  Rev date: Mon Jan 26 07:18:36 1998
//    1. Added new functionality to CWString
//  
//  Revision 1.1  by: BRUCEH  Rev date: Tue Aug 05 04:26:56 1997
//    Added List Object and TextFile stuff
//  
//  Revision 1.0  by: BRUCEH  Rev date: Sun Jun 29 01:03:20 1997
//    Initial revision.
//  
//  $Endlog$
//
//
//
///////////////////////////////////////////////////////////////////////////

