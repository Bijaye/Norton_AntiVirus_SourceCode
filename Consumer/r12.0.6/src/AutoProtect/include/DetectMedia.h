// DetectMedia.h: interface for the CDetectMedia class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DETECTMEDIA_H__6ED7C5E5_3517_4743_AB6B_A575032AE46A__INCLUDED_)
#define AFX_DETECTMEDIA_H__6ED7C5E5_3517_4743_AB6B_A575032AE46A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDetectMedia  
{
public:
	static bool IsMediaPresent( char chDriveLetter, bool bLongTimeOut );

private:	
	static unsigned __stdcall isMediaPresentProc( void *pvDrive );
};

#endif // !defined(AFX_DETECTMEDIA_H__6ED7C5E5_3517_4743_AB6B_A575032AE46A__INCLUDED_)
