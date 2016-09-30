//========================================================================
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.
//------------------------------------------------------------------------
//
// PASSWD.H -- Password header
//
// $Header:   S:/INCLUDE/VCS/PASSWD.H_v   1.0   26 Jan 1996 20:13:44   RJACKSO  $
//
//------------------------------------------------------------------------
// $Log:   S:/INCLUDE/VCS/PASSWD.H_v  $
// 
//    Rev 1.0   26 Jan 1996 20:13:44   RJACKSO
// Initial revision.
// 
//    Rev 1.4   26 Apr 1994 16:35:40   BRAD
// Removed PVCS Logs
//========================================================================

#ifndef _INC_PASSWD
#define _INC_PASSWD

#define MAXIMUM_PASSWORD_LEN  16

#define NO_PASSWORD 0xFFFFFFFF          /* do not change this value.    */
                                        /* -1 is the standard initial   */
                                        /* value for this CRC algorithm,*/
                                        /* thus NULL passwords will be  */
                                        /* encoded by -1.               */
#define INITIAL_CRC NO_PASSWORD         /* This should also be -1       */
//&?
/*
**  This is needed because of the conflict of interest in Vic & DavidD.
**  We are in trouble if 0 is actually a valid encoded password
**  (let's hope that that is not the case)...
*/
#define IS_PASSWORD(dw)     (dw != 0  &&  dw != NO_PASSWORD)

#ifdef __cplusplus                      // restarting _cplusplus block for rest of file
extern "C" {
#endif

/*
**  Function Prototypes:
*/

WORD SYM_EXPORT WINAPI EncodePassWord(LPSTR lpPassWord, LPDWORD lpdwEncodedPW);
WORD SYM_EXPORT WINAPI CRCFile(HFILE hFile, LPDWORD lpdwCRC);

#ifdef __cplusplus
}
#endif

#endif

