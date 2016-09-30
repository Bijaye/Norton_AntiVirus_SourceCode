// Copyright 2000 Symantec, Core Technology Group
//************************************************************************
//
// Description:
//      Routines for translating between ASCII and EBCDIC
//
// Contents:
//      StrAscToEbc()
//      StrEbcToAsc()	Commented out for now, until needed
//
// See Also:
//************************************************************************
// RGOUGH 7/27/2000
// Initial revision.
//************************************************************************

#include "platform.h"
/*
static const BYTE Ascii[256] = {0x00, 0x01, 0x02, 0x03, 0x20, 0x09, 0x20, 0x7F,
                                0x20, 0x20, 0x20, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                0x10, 0x11, 0x12, 0x13, 0x20, 0x20, 0x08, 0x20,
                                0x18, 0x19, 0x20, 0x20, 0x20, 0x1D, 0x1E, 0x1F,
                                0x20, 0x20, 0x1C, 0x20, 0x20, 0x0A, 0x17, 0x1B,
                                0x20, 0x20, 0x20, 0x20, 0x20, 0x05, 0x06, 0x07,
                                0x20, 0x20, 0x16, 0x20, 0x20, 0x20, 0x20, 0x04,
                                0x20, 0x20, 0x20, 0x20, 0x14, 0x15, 0x20, 0x1A,
                                0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                0x20, 0x20, 0x60, 0x2E, 0x3C, 0x28, 0x2B, 0x7C,
                                0x26, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                0x20, 0x20, 0x21, 0x24, 0x2A, 0x29, 0x3B, 0x5E,
                                0x2D, 0x2F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                0x20, 0x20, 0x20, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,
                                0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                0x20, 0x60, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,
                                0x20, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
                                0x68, 0x69, 0x20, 0x7B, 0xF3, 0x20, 0x20, 0xC5,
                                0x20, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
                                0x71, 0x72, 0x20, 0xAF, 0xDB, 0x20, 0xF1, 0xFE,
                                0x20, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
                                0x79, 0x7A, 0x20, 0x1C, 0xA9, 0x5B, 0xF3, 0xFA,
                                0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                0x20, 0x20, 0x20, 0xD9, 0xBF, 0x5D, 0xBE, 0x20,
                                0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                                0x48, 0x49, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
                                0x51, 0x52, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                0x5C, 0x20, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
                                0x59, 0x5A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                                0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                0x38, 0x39, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
*/
static const BYTE Ebcdic [256] = {0X00, 0X01, 0X02, 0X03, 0X37, 0X2D, 0X2E, 0X2F,
                                  0X16, 0X05, 0X25, 0X0B, 0X0C, 0X0D, 0X0E, 0X0F,
                                  0X10, 0X11, 0X12, 0X13, 0X3C, 0X3D, 0X32, 0X26,
                                  0X18, 0X19, 0X3F, 0X27, 0X22, 0X1D, 0X18, 0X19,
                                  0X40, 0X5A, 0X7F, 0X7B, 0X5B, 0X6C, 0X50, 0X7D,
                                  0X4D, 0X5D, 0X5C, 0X4E, 0X6B, 0X60, 0X4B, 0X61,
                                  0XF0, 0XF1, 0XF2, 0XF3, 0XF4, 0XF5, 0XF6, 0XF7,
                                  0XF8, 0XF9, 0X7A, 0X5E, 0X4C, 0X7E, 0X6E, 0X6F,
                                  0X7C, 0XC1, 0XC2, 0XC3, 0XC4, 0XC5, 0XC6, 0XC7,
                                  0XC8, 0XC9, 0XD1, 0XD2, 0XD3, 0XD4, 0XD5, 0XD6,
                                  0XD7, 0XD8, 0XD9, 0XE2, 0XE3, 0XE4, 0XE5, 0XE6,
                                  0XE7, 0XE8, 0XE9, 0XAD, 0XE0, 0XBD, 0X5F, 0X6D,
                                  0X4A, 0X81, 0X82, 0X83, 0X84, 0X85, 0X86, 0X87,
                                  0X88, 0X89, 0X91, 0X92, 0X93, 0X94, 0X95, 0X96,
                                  0X97, 0X98, 0X99, 0XA2, 0XA3, 0XA4, 0XA5, 0XA6,
                                  0XA7, 0XA8, 0XA9, 0XC0, 0X4F, 0XD0, 0XA1, 0X07,
                                  0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40,
                                  0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40,
                                  0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40,
                                  0X40, 0X40, 0X40, 0X4A, 0X40, 0X40, 0X40, 0X40,
                                  0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40,
                                  0X40, 0XAC, 0X5F, 0X40, 0X40, 0X40, 0X40, 0X40,
                                  0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40,
                                  0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0XBE, 0XBC,
                                  0X40, 0X40, 0X40, 0X40, 0X6D, 0X8F, 0X40, 0X40,
                                  0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40,
                                  0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40,
                                  0X40, 0XBB, 0X40, 0X9C, 0X40, 0X40, 0X40, 0X40,
                                  0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40,
                                  0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40, 0X40,
                                  0X40, 0X9E, 0XAE, 0X8C, 0X40, 0X40, 0X40, 0X40,
                                  0XA1, 0X40, 0XAF, 0X40, 0X40, 0X40, 0X9F, 0X40};

//************************************************************************
// Code
//************************************************************************

//*************************************************************************
// StrAscToEbc()
//
// int StrAscToEbc (LPCSTR lpszAscii, LPSTR lpszEbcdic )
//
// This routine converts a zero terminated ASCII string to EBCDIC
//
// No check on available buffer length
// NULL source string will result in zero-length destination string
// Same string may be used for destination as source
//
// Returns:
//      length of string converted
//*************************************************************************
// 7/27/00 RGOUGH, created.
//*************************************************************************

int StrAscToEbc(LPCSTR lpszAscii, LPSTR lpszEbcdic)
{
	SYM_ASSERT(lpszAscii);
	SYM_ASSERT(lpszEbcdic);

	int nLength;
	nLength = 0;

	if (NULL != lpszEbcdic)
	{
		if (NULL == lpszAscii)
		{
			lpszEbcdic[0] = (char)0;
		}
		else
		{
			while (*lpszEbcdic++ = Ebcdic[*lpszAscii++]) nLength++;
		}
	}
	return nLength;
}
/*
//*************************************************************************
// StrEbcToAsc()
//
// int StrEbcToAsc (LPCSTR lpszEbcdic, LPSTR lpszAscii )
//
// This routine converts a zero terminated EBCDIC string to ASCII
//
// No check on available buffer length
// NULL source string will result in zero-length destination string
// Same string may be used for destination as source
//
// Returns:
//      length of string converted
//*************************************************************************
// 7/27/00 RGOUGH, created.
//*************************************************************************

int StrEbcToAsc(LPCSTR lpszEbcdic, LPSTR lpszAscii)
{
	SYM_ASSERT(lpszEbcdic);
	SYM_ASSERT(lpszAscii);

	int nLength;
	nLength = 0;

	if (NULL != lpszAscii)
	{
		if (NULL == lpszEbcdic)
		{
			lpszAscii[0] = (char)0;
		}
		else
		{
			while (*lpszAscii++ = Ascii[*lpszEbcdic++]) nLength++;
		}
	}
	return nLength;
}
*/