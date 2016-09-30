// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif


/*	Notes On-Disk Structure Definitions - for the actions facility
	Currently contains only a single CD record that completely
	describes a customizable menu action.
*/
#ifndef ACT_ODS_DEFS
#define ACT_ODS_DEFS

#ifndef ODS_DEFS
#include "ods.h"
#endif


/*	CDACTION -  Action Record, used in forms ($ACTION) to 
	define a customizable menu action.
*/

typedef struct {
	LSIG	Header;				/* Signature and Length */
	WORD	Type;				/* Type of action (formula, script, etc.)*/
	WORD	IconIndex;			/* Index into array of icons.	*/
	DWORD Flags;				/* Action flags		*/
	WORD TitleLen;				/* Length (in bytes) of action's title*/
								/*	Variable portion of the record -
								  	TitleLen bytes of action's title
								  	followed by Size - Titlelen bytes
									of formula, script, etc. depending
									on the action type.
								*/
/* Six bytes of spares cause we don't know what else we might need.*/
	WORD	FormulaLen;
	DWORD	Spare2;
									
} CDACTION;

typedef struct {
	BSIG	Header;				/*	Signature and Length		*/
	WORD	BackColor;			/*	Background color index.	*/
	WORD	LineColor;			/*	Line color index.		*/
	WORD	LineStyle;			/*	Style of line.			*/
	WORD	BorderStyle;		/*	Border style.			*/
	WORD	BorderWidth;		/*	Border width (twips)	*/
	
	DWORD	dwFlags;
	DWORD	Spare2;
	DWORD	Spare3;
	DWORD	Spare4;

}	CDACTIONBAR;

#define ACTION_BAR_FLAG_NO_SYS_COLOR	0x00000001

/*	End of Action Package On Disk Structure Definitions */

#endif

#ifdef __cplusplus
}
#endif


