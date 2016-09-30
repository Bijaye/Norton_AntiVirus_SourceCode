//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/endutils.h_v   1.4   21 Nov 1996 17:14:42   AOONWAL  $
//
// Description:
//      Contains ConvertEndianLong() and ConvertEndianShort() functions
//      for endian-enabling Word macro virus scan/repair engine.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/endutils.h_v  $
// 
//    Rev 1.4   21 Nov 1996 17:14:42   AOONWAL
// No change.
// 
//    Rev 1.3   29 Oct 1996 12:59:50   AOONWAL
// No change.
// 
//    Rev 1.2   28 Aug 1996 15:52:28   DCHI
// Correction of MACINTOSH return value for AVDEREF_DWORD().
// Added AVASSIGN_WORD() and AVASSIGN_DWORD() macros.
// 
//    Rev 1.1   14 Aug 1996 17:20:02   DCHI
// Added AVDEREF_WORD and AVDEREF_DWORD and BIG_ENDIAN #ifdef.
// 
//    Rev 1.0   03 May 1996 13:22:18   DCHI
// Initial revision.
// 
//************************************************************************

/*ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
	
	© 1995 Symantec
	
	FileName:	EndianUtils.h
	
	01/09/96	Stuart Davison	Initial rev
	
ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ*/

#include "platform.h"

#ifdef __MACINTOSH__
	/*ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		ConvertEndianLong:	Does an endian conversion on theLong and returns it
	ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ*/
	inline ulong ConvertEndianLong( const ulong theLong )
	{
		ulong	result = 0;
		
		((char*)&result)[0] = ((char*)&theLong)[3];
		((char*)&result)[1] = ((char*)&theLong)[2];
		((char*)&result)[2] = ((char*)&theLong)[1];
		((char*)&result)[3] = ((char*)&theLong)[0];
		
		return result;
	}
	
	
	/*ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
        ConvertEndianShort:  Does an endian conversion on theShort and returns it
	ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ*/
    inline ushort ConvertEndianShort( const ushort theShort )
	{
		ushort	result = 0;
		
		((char*)&result)[0] = ((char*)&theShort)[1];
		((char*)&result)[1] = ((char*)&theShort)[0];
		
		return result;
	}


    /*ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
        AVDEREF_WORD:  Does a dereference of a WORD value at the pointer
	ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ*/
    inline ushort AVDEREF_WORD( const LPBYTE lpby )
	{
        ushort  result = 0;
        
        ((char*)&result)[0] = lpby[1];
        ((char*)&result)[1] = lpby[0];
        
		return result;
	}
	
	
	/*ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
        AVDEREF_DWORD:  Does a dereference of a DWORD value at the pointer
	ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ*/
    inline ulong AVDEREF_DWORD( const LPBYTE lpby )
	{
        ulong   result = 0;
        
        ((char*)&result)[0] = lpby[3];
        ((char*)&result)[1] = lpby[2];
        ((char*)&result)[2] = lpby[1];
        ((char*)&result)[3] = lpby[0];
        
        return result;
	}
#else

    #define ConvertEndianLong( theLong )    DWENDIAN(theLong)
    #define ConvertEndianShort( theShort )  WENDIAN(theShort)

    #ifdef  BIG_ENDIAN

        #define WENDIAN(w)   ( (WORD)((WORD)(w) >> 8) | (WORD)((WORD)(w) << 8) )
        #define DWENDIAN(dw) ( (DWORD)((DWORD)(dw) >> 24) | \
                               (DWORD)((DWORD)dw << 24)  | \
                               (DWORD)(((DWORD)(dw) >> 8) & 0xFF00U) | \
                               (DWORD)(((DWORD)(dw) << 8) & 0xFF0000UL) )

        #define AVDEREF_WORD(lpvBuf) ((WORD) \
            (((LPBYTE)(lpvBuf))[0] | (((LPBYTE)(lpvBuf))[1] << 8)))

        #define AVDEREF_DWORD(lpvBuf) ((DWORD) \
            (((LPBYTE)(lpvBuf))[0] | (((LPBYTE)(lpvBuf))[1] << 8) | \
             (((LPBYTE)(lpvBuf))[2] << 16) | (((LPBYTE)(lpvBuf))[3] << 24)))

        #define AVASSIGN_WORD(lpvBuf,w) \
            (*(LPBYTE)(lpvBuf) = (w) & 0xFF, \
             *((LPBYTE)(lpvBuf)+1) = ((w) >> 8) & 0xFF, \
             (w))

        #define AVASSIGN_DWORD(lpvBuf,dw) \
            (*(LPBYTE)(lpvBuf) = (dw) & 0xFF, \
             *((LPBYTE)(lpvBuf)+1) = ((dw) >> 8) & 0xFF, \
             *((LPBYTE)(lpvBuf)+2) = ((dw) >> 16) & 0xFF, \
             *((LPBYTE)(lpvBuf)+3) = ((dw) >> 24) & 0xFF, \
             (dw))

    #else   // if LITTLE_ENDIAN (default)

        #define WENDIAN(w)   (w)
        #define DWENDIAN(dw) (dw)

        #define AVDEREF_WORD(lpvBuf) (*(LPWORD)(lpvBuf))

        #define AVDEREF_DWORD(lpvBuf) (*(LPDWORD)(lpvBuf))

        #define AVASSIGN_WORD(lpvBuf,w) (*(LPWORD)(lpvBuf) = (w))

        #define AVASSIGN_DWORD(lpvBuf,dw) (*(LPDWORD)(lpvBuf) = (dw))

    #endif

#endif

