// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
/* Copyright 1991, 2005 by Symantec Corporation.  All rights reserved. */
/**********************************************************************
**
** $Header:   S:/SYMUTIL/VCS/EMSGINT.H_v   1.0   26 Jan 1996 20:19:12   JCUBBAG  $
**
** Description:
**    This file supplies all definitions and prototypes used only
**    internaly by the emsg component.
**
** Processor:  Microsoft C 6.0
** Operating System:  MS-Windows 3.0
***********************************************************************
**  $Log:   S:/SYMUTIL/VCS/EMSGINT.H_v  $
// 
//    Rev 1.0   26 Jan 1996 20:19:12   JCUBBAG
// Initial revision.
// 
//    Rev 1.3   15 Mar 1994 10:42:02   BRUCE
// Changed EXPORT to SYM_EXPORT
// 
//    Rev 1.2   25 Feb 1994 11:08:52   PGRAVES
// Global Quake changes.
// 
//    Rev 1.1   15 Feb 1993 18:58:04   ENRIQUE
// No change.
// 
//    Rev 1.0   07 Dec 1992 14:59:44   LCOHEN
// Initial revision.
// 
//    Rev 1.0   07 Dec 1992 11:52:12   LCOHEN
// Initial revision.
// 
//    Rev 1.1   15 Sep 1992 21:56:18   BRUCE
// Made SYM_EXPORT conversion for exports
// 
//    Rev 1.0   28 Jul 1991 10:45:54   MARKL
// Initial revision.
// 
//    Rev 1.3   10 Feb 1991  2:00:46   DAVIDH
// Added MAX_CAPFMT_LEN and ERR_SEV_INTERNAL.  Changed max string lengths
// to actual buffer sizes (by adding +1's) to reduce confusion in code.
**  11/16/90 BEM File creation.
**********************************************************************/

				/* These are the sizes of buffers used	*/
				/* to build error messages and caption	*/
				/* text. Sizes are expressed as a max	*/
				/* string length + 1 for a terminating	*/
				/* null character.			*/

#define MAX_STR_TBL_LEN	(255+1)	/* Buf size for loading message string 	*/
				/* from a string table.  This limit is	*/
				/* enforced by Windows.			*/

#define MAX_ERR_MSG_LEN (512+1) /* Buf size for expanded message (after	*/
				/* %n substitutions).			*/

#define MAX_CAPFMT_LEN	(60+1)  /* Buf size for loading error caption	*/
				/* formatting string from string table.	*/

#define MAX_CAP_LEN     (80+1)  /* Buf size for expanded caption (after	*/
				/* %n substitutions).			*/


				/* This must differ from all other 	*/
				/* ERR_SEV_??? constants in emsg.h.	*/
				/* It is used to deal with errors while	*/
				/* loading error message strings.	*/

#define ERR_SEV_INTERNAL	0xFFFF	
