// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif

#if	!defined(NLS_H)
#define	NLS_H


/********************************************************************
**			   		TYPES AND DEFINITIONS
*/

typedef	WORD	NLS_STATUS;
				
typedef void NLS_INFO;			
typedef void far *NLS_PINFO;			

/********************************************************************
**			   		PROTOS
*/

NLS_STATUS LNPUBLIC	 	NLS_find				(BYTE far * far * 	ppString, 
		   										WORD				Len, 
		   										BYTE far *	  		pSetOfChars, 
		   										WORD				ControlFlags,
		   										NLS_PINFO			pInfo);

NLS_STATUS LNPUBLIC	 	NLS_find_substr 		(BYTE far * far * 	ppString, 
		   										WORD				Len1, 
		   										BYTE far *	  		pSubString,	
		   										WORD				Len2, 
		   										NLS_PINFO			pInfo);

NLS_STATUS LNPUBLIC	 	NLS_get 				(BYTE far * far * 	ppString, 
												WORD				Len, 
												BYTE far *	  		pCharacter,	
												NLS_PINFO			pInfo);

WORD	 	LNPUBLIC	NLS_goto_next 			(BYTE far * far * 	ppString, 
												WORD				Len,
												NLS_PINFO			pInfo);
WORD	 	LNPUBLIC	NLS_goto_prev 			(BYTE far * far * 	ppString, 
												BYTE far *	  		pStrStart, 
												NLS_PINFO			pInfo);
WORD	 	LNPUBLIC	NLS_goto_next_word_end 	(BYTE far * far * 	ppString, 
												WORD				Len, 
												NLS_PINFO			pInfo);

WORD	 	LNPUBLIC	NLS_goto_next_word_start(BYTE far * far * 	ppString, 
												WORD				Len, 
												NLS_PINFO			pInfo);

WORD	 	LNPUBLIC	NLS_goto_next_break 	(BYTE far * far * 	ppString,
												WORD	   			Len,
												NLS_PINFO			pInfo);

WORD	 	LNPUBLIC	NLS_goto_prev_word_end 	(BYTE far * far * 	ppString, 
												BYTE far *		  	pStrStart, 
												NLS_PINFO			pInfo);

WORD	 	LNPUBLIC	NLS_goto_prev_word_start(BYTE far * far * 	ppString, 
												BYTE far *	  		pStrStart, 
												NLS_PINFO			pInfo);

NLS_STATUS	LNPUBLIC	NLS_isalpha 			(BYTE far *			pCharacter,
												NLS_PINFO			pInfo);

NLS_STATUS	LNPUBLIC	NLS_isalnum 			(BYTE far *  		pCharacter,
												NLS_PINFO			pInfo);

NLS_STATUS	LNPUBLIC	NLS_isarith 			(BYTE far *   		pCharacter,
												NLS_PINFO			pInfo);

NLS_STATUS	LNPUBLIC	NLS_iscntrl 			(BYTE far *   		pCharacter,
												NLS_PINFO			pInfo);

NLS_STATUS	LNPUBLIC	NLS_isdigit 			(BYTE far *   		pCharacter,
												NLS_PINFO			pInfo);

NLS_STATUS	LNPUBLIC	NLS_islower 			(BYTE far *   		pCharacter,
												NLS_PINFO			pInfo);

NLS_STATUS	LNPUBLIC	NLS_ispunct 			(BYTE far *   		pCharacter,
												NLS_PINFO			pInfo);

NLS_STATUS	LNPUBLIC	NLS_isspace 			(BYTE far *   		pCharacter,
												NLS_PINFO			pInfo);

NLS_STATUS	LNPUBLIC	NLS_isupper 			(BYTE far *   		pCharacter,
												NLS_PINFO			pInfo);

NLS_STATUS	LNPUBLIC	NLS_isleadbyte 			(BYTE	  			Character,
												NLS_PINFO			pInfo);

NLS_STATUS	LNPUBLIC	NLS_string_chars		(BYTE far *   		pString, 
												WORD				NumBytes,
												WORD far *  		pNumChars,
												NLS_PINFO			pInfo);

NLS_STATUS	LNPUBLIC	NLS_string_bytes		(BYTE far *   		pString,
												WORD				NumChars,
												WORD far *  		pNumBytes,
												NLS_PINFO			pInfo);
NLS_STATUS	LNPUBLIC    NLS_put 				(BYTE far * far * 	ppString, 
										 		BYTE far *	  		pCharacter,	
										 		NLS_PINFO			pInfo);

NLS_STATUS	LNPUBLIC    NLS_put_term			(BYTE far * far * 	ppString, 
										 		BYTE far *	  		pCharacter,	
										 		NLS_PINFO			pInfo);

/********************************************************************
**					RETURN CODES
*/
									
#define	NLS_SUCCESS					0x0
#define	NLS_BADPARM					0xABAD 
#define	NLS_BUFFERTOOSMALL			0xA000 
#define	NLS_CHARSSTRIPPED 			0x1000 
#define	NLS_ENDOFSTRING				0x1001 
#define	NLS_FALLBACKUSED 			0x1002 
#define	NLS_FILEINVALID				0xA001 
#define	NLS_FILENOTFOUND			0xA002 
#define	NLS_FINDFAILED				0x1005
#define	NLS_INVALIDCHARACTER		0xA003 
#define	NLS_INVALIDDATA				0xA004 
#define	NLS_INVALIDENTRY 			0xA005 
#define	NLS_INVALIDTABLE 			0xA006 
#define	NLS_PROPNOTFOUND 			0x1003 
#define	NLS_STARTOFSTRING 			0x1004 
#define	NLS_STRINGSIZECHANGED 	   	0x1006 
#define	NLS_TABLEHEADERINVALID		0xA007 
#define	NLS_TABLENOTFOUND			0x1007 

/********************************************************************
**			   		FLAGS
*/

/* 	NLS_find */
#define	NLS_FIND_CHARINSET	   		1
#define	NLS_FIND_CHARNOTINSET  		2

/********************************************************************
**			   		MISC DEFINES
*/

#define	NLS_NULLTERM			(WORD) -1

#define	NLS_SIZEOFNULL			1 /* for LMBCS implementation */

#define	NLS_MAXBYTESPERCHAR		3


#endif	 /*	if !defined(NLS_H) */

#ifdef __cplusplus
}
#endif

