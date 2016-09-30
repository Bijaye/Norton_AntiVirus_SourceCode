// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CERTLIBN/VCS/dbcsrtns.cpv   1.0   13 May 1996 08:46:42   YVANG  $
//
// Description:
//      This file contains functions to handle DBCS enabling.
//
//       DBCS_DBCSIsLeadByte(): Same as DBCSIsLeadByte(), but CERTLIBN 
//                                 version.
//       DBCS_AnsiNext():       Same as AnsiNext(), but CERTLIBN version.
//
//************************************************************************
// $Log:   S:/CERTLIBN/VCS/dbcsrtns.cpv  $
// 
//    Rev 1.0   13 May 1996 08:46:42   YVANG
// Initial revision.
//************************************************************************

#include "platform.h"
#include "ctsn.h"
#include "dbcs.h"

#if defined(__cplusplus)
    extern "C" {
#endif

//@API:*****************************************************************
//@Declaration:
//LPSTR WINAPI CTSN_LOADDS DBCS_AnsiNext(LPSTR lpsz);
//
//@Description:
//This function is the same as AnsiNext(), but used by CERTLIBN.
//
//@Parameters:
//$lpsz$       Points to a character in a null-terminated string.
//
//@Returns: If successful, the return value is a pointer to the next character
//          in the string, or to the terminating null character if at the end 
//          of the string.
//@See:
//@Include:   ctsn.inc, dbcs.h
//**********************************************************************
// 5/13/96 YVANG Function Created.                                   
//**********************************************************************
LPSTR WINAPI CTSN_LOADDS DBCS_AnsiNext(LPSTR lpsz)
{
    return((LPSTR)AnsiNext((LPCTSTR)lpsz));
}

//@API:*****************************************************************
//@Declaration:
//BOOL WINAPI CTSN_LOADDS DBCS_DBCSIsLeadByte(BYTE byCh);
//
//@Description:
//This function is the same as DBCSIsLeadByte(), but used by CERTLIBN.
//
//@Parameters:
//$byCh$       Byte to check.
//
//@Returns: If it is a lead byte, returns TRUE.  Otherwise FALSE.
//@See:
//@Include:   ctsn.inc, dbcs.h
//**********************************************************************
// 5/13/96 YVANG Function Created.                                   
//**********************************************************************
BOOL WINAPI CTSN_LOADDS DBCS_DBCSIsLeadByte(BYTE byCh)
{
    return(DBCSIsLeadByte(byCh));
}

#if defined(__cplusplus)
}
#endif

// End of File, dbcsrtns.cpp

