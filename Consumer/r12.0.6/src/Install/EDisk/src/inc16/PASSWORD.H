/* $Header:   V:/INCLUDE/VCS/PASSWORD.H_V   1.17   29 Oct 1992 17:43:06   JOHN  $ */

/*----------------------------------------------------------------------*
 * PASSWORD.H 								*
 *									*
 * This file contains stuff for program patching.  It used to be that	*
 * the only patched item was program password, hence the name of this	*
 * file.  Now there are all sorts of patch areas.			*
 *									*
 * Note: This file goes with PATCHSIG.INC.  Any changes to this file	*
 * 	 MUST also be reflected there.  Be particularly careful with	*
 *	 The PASSWORD_XOR value which is "encoded" in the PASSWORD_NONE	*
 *	 string.							*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   V:/INCLUDE/VCS/PASSWORD.H_V  $
 * 
 *    Rev 1.17   29 Oct 1992 17:43:06   JOHN
 * Added PWD_PROG_RESCUE
 * 
 *    Rev 1.16   19 Oct 1992 15:58:14   JOHN
 * Changed PWD_PROG_xxx constants from hex to decimal
 * 
 *    Rev 1.15   16 Oct 1992 20:44:16   JOHN
 * Major update to work with NU 7's new patch scheme.
 * 
 *    Rev 1.14   10 Sep 1992 17:03:56   BASIL
 * Removed NIPatch-related items, added INTegrator Patch symbols.
 * 
 *    Rev 1.13   04 Mar 1992 18:49:00   PETERD
 * Added old NI record
 * 
 *    Rev 1.12   26 Feb 1992 12:24:20   PETERD
 * Changed signature characters for new password and registration strucs
 * 
 *    Rev 1.11   05 Feb 1992 16:36:38   PETERD
 * Changed the user registration structure.  This one is used in Windows
 * programs too.  Changed XOR pattern to FF.
 * 
 *    Rev 1.10   05 Nov 1991 19:27:52   PETERD
 * Added PatchSearchDriver() to allow patching of device drivers.
 * 
 *    Rev 1.9   29 Oct 1991 16:34:04   PETERD
 * Added product variable name to PatchAreaRead and Write.
 * 
 *    Rev 1.8   16 Oct 1991 12:43:16   PETERD
 * Added PaswordClear() prototype, and PASSWORD_XOR definition.
 * 
 *    Rev 1.7   03 Oct 1991 15:04:54   PETERD
 * Added PasswordPatchRec.
 * 
 *    Rev 1.6   04 Sep 1991 13:21:02   PETERD
 * Generalized the user-name patch area structure, and changed the name
 * from NIPatch to UserPatch.
 * 
 *    Rev 1.5   19 Mar 1991 16:17:48   PETERD
 * Changed NICrypt() prototype.
 * 
 *    Rev 1.4   28 Jan 1991 16:23:22   JOHN
 * Added prototypes for PatchAreaRead() and PatchAreaWrite().
 * 
 *    Rev 1.3   28 Jan 1991 12:24:30   ED
 * Added a check for INCL_PASSWORD to prevent multiple inclusions
 * 
 *    Rev 1.2   27 Jan 1991 19:38:02   JOHN
 * Reworked for the new program patching system.  Almost everything has
 * been changed; use VDIFF if you need to know the exact differences.
 * 
 *    Rev 1.1   14 Sep 1990 16:59:52   DAVID
 * Increased MAX_SWITCHES.
 * 
 *    Rev 1.0   29 Aug 1990 11:24:42   DAVID
 * Initial revision.
 */

#ifndef	INCL_PASSWORD
#define	INCL_PASSWORD

#ifndef INCL_WINGLUE
#include <winglue.h>
#endif

/*----------------------------------------------------------------------*
 * Patch area signatures						*
 *----------------------------------------------------------------------*/

#define PASSWORD_SIG		((Byte) 'W')	/* New password area	*/
#define USER_SIG		((Byte) 'U')	/* User registration area */
#define INTEGRATOR_SIG		((Byte) 'I')	/* Norton Integrator area */

					/* Text is XOR'd with signature */
					/* byte to locate area in file	*/
#define PATCH_AREA_SIG_LEN	18
#define PATCH_AREA_SIG_TEXT	"PatchAreaSignature"

/*----------------------------------------------------------------------*
 * Password patch area							*
 *----------------------------------------------------------------------*/

#define PASSWORD_XOR    	0xFF    /* Password/User xor value */

#define PASSWORD_LEN		16 	/* Must be same as in PWPATCH.ASM */

					/* "NOPASSWORDISSET!" */
#define PASSWORD_NONE		"\xB1\xB0\xAF\xBE\xAC\xAC\xA8\xB0" \
				"\xAD\xBB\xB6\xAC\xAC\xBA\xAB\xDE"

#define PWD_PROG_LEN		32

#define PWD_PROG_CALIBRAT	1
#define PWD_PROG_DISKEDIT	2
#define PWD_PROG_DISKTOOL	3
#define PWD_PROG_FILEFIX	4
#define PWD_PROG_NDD		5
#define PWD_PROG_NDIAG		6
#define PWD_PROG_NUCONFIG	7
#define PWD_PROG_SFORMAT	8
#define PWD_PROG_SPEEDISK	9
#define PWD_PROG_UNERASE	10
#define PWD_PROG_UNFORMAT	11
#define PWD_PROG_WIPEINFO	12
#define PWD_PROG_RESCUE		13
#define PWD_PROG_NAV		14

typedef struct
    {
    Byte	szPassword[PASSWORD_LEN + 1];
    Word	wFlags;
    Byte	szProtectedPrograms[PWD_PROG_LEN + 1];
    }
    PasswordPatchRec;

/*----------------------------------------------------------------------*
 * User registration patch area						*
 *----------------------------------------------------------------------*/

#define IDENT_NAME_LEN		60
#define USER_NAME_LEN		60
#define COMPANY_NAME_LEN	60	/* Assumed to be the same	*/
#define SERIAL_NUMBER_LEN	32

typedef struct
    {
    char    szIDString[IDENT_NAME_LEN];   /* id string Install looks for */
    WORD    wStrucLen;                    /* Length of this structure    */
    char    szRegName[USER_NAME_LEN];     /* Registered user             */
    int     nRegNameLen;                  /* length of string            */
    char    szRegCompany[COMPANY_NAME_LEN];/* Registered company          */
    int     nRegCompanyLen;               /* length of string            */
    char    szSerialNum[SERIAL_NUMBER_LEN];/* Product serial number	 */
    int     nSerialNumLen;                /* length of string            */
    BOOL    bProcessedByInstall;          /* TRUE when touched by Install*/
    BOOL    bFlipped;                     /* TRUE when bits are flipped  */
    BYTE    byVersionMajor;               /* version number -- major     */
    BYTE    byVersionMinor;               /* version number -- minor     */
    BYTE    byVersionExtra1;              /* version number -- extra     */
    BYTE    byVersionExtra2;              /* version number -- extra     */
    }
    UserPatchRec;

/*----------------------------------------------------------------------*
 * Norton Integrator patch area						*
 *----------------------------------------------------------------------*/

// Bit assignments for Integrator patch record

#define  INT_PATCH_FLAG_MENU_EDIT   0x0001  // Enable/disable menu editing
#define  INT_PATCH_FLAG_DISKREET    0x0002  // Diskreet included (non-International)

typedef struct
    {
    Word  wFlags;                         // See bit assignments above
    }
    IntegratorPatchRec;

/*----------------------------------------------------------------------*
 * Patch area external declarations					*
 *----------------------------------------------------------------------*/

// extern PasswordPatchRec   far *lpPwdPatchArea;
// extern UserPatchRec       far *lpUserPatchArea;
// extern IntegratorPatchRec far *lpIntPatchArea;

/*----------------------------------------------------------------------*
 * Function Prototypes							*
 *----------------------------------------------------------------------*/

Boolean PASCAL PasswordCheck(Byte byProgramID);
Boolean PASCAL PasswordGet(Byte *pw);
void	PASCAL PasswordEncrypt(Byte far *pw);
Boolean PASCAL PasswordMatch(Byte far *s, Byte far *t);
void 	PASCAL PasswordClear(Byte far *lpszPassword);

void	PASCAL UserNameCrypt(UserPatchRec far *r);

Boolean	PASCAL UserNameRead(UserPatchRec far *r);
Boolean	PASCAL UserNameUpdate(UserPatchRec far *r);

Boolean PASCAL PatchAreaRead(Byte *pszProductVar,
			     Byte *pszFilename,
			     Byte byPatchAreaSig,
			     void far *lpPatchDataBuffer,
			     Word wPatchDataSize);
Boolean PASCAL PatchAreaUpdate(Byte *pszProductVar,
			       Byte *pszFilename,
			       void far *lpPatchDataBuffer,
			       Word wPatchDataSize);
Ulong PASCAL PatchAreaFind(Word wFileHandle, Byte byPatchAreaSig);
void PASCAL PatchAreaMakeSig(Byte byPatchAreaSig, Byte *pbySigBlock);

#endif /* INCL_PASSWORD */
