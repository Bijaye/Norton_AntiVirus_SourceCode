// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif

#ifndef VIEWFMT_DEFS
#define VIEWFMT_DEFS

/*	This file contains the view format definitions which are used to read and
	write the format of a view in the View note.
	
	The structure of this data is as follows:

	[ VIEW_FORMAT_HEADER ]		

	[ VIEW_TABLE_FORMAT ]

	[ VIEW_COLUMN_FORMAT #0 ]	n columns, as specified in VIEW_TABLE_FORMAT.
	[ VIEW_COLUMN_FORMAT #1 ]
	...
	[ VIEW_COLUMN_FORMAT #n ]

	[ COLUMN DATA #0 ]			n columns, as specified in VIEW_TABLE_FORMAT.
	[ COLUMN DATA #1 ]
	...
	[ COLUMN DATA #n ]

	[VIEW_TABLE_FORMAT2]

	[ VIEW_COLUMN_FORMAT2 #0 ]	n columns, as specified in VIEW_TABLE_FORMAT.
	[ VIEW_COLUMN_FORMAT2 #1 ]
	...
	[ VIEW_COLUMN_FORMAT2 #n ]
*/
	


#ifndef FONTID_DEFS
#include "fontid.h"
#endif

#ifndef MISC_DEFS
#include "misc.h"
#endif

#ifndef NSF_DATA_DEFS
#include "nsfdata.h"
#endif

/* View on-disk format definitions */

#define VIEW_FORMAT_VERSION 1

#define VIEW_COLUMN_FORMAT_SIGNATURE	0x4356
#define VIEW_COLUMN_FORMAT_SIGNATURE2	0x4357

#define VIEW_CLASS_TABLE	(0 << 4)
#define VIEW_CLASS_CALENDAR	(1 << 4)
#define VIEW_CLASS_MASK		0xF0

#define CALENDAR_TYPE_DAY	0
#define CALENDAR_TYPE_WEEK	1
#define CALENDAR_TYPE_MONTH	2

#define VIEW_STYLE_TABLE	VIEW_CLASS_TABLE
#define VIEW_STYLE_DAY		(VIEW_CLASS_CALENDAR + 0)
#define VIEW_STYLE_WEEK		(VIEW_CLASS_CALENDAR + 1)
#define VIEW_STYLE_MONTH	(VIEW_CLASS_CALENDAR + 2)

/* View table format descriptor.  Followed by VIEW_COLUMN_FORMAT */
/* descriptors; one per column.  The column format descriptors are followed */
/* by the packed item name, title, formula, and constant values.  */
/* All of this is followed by a VIEW_TABLE_FORMAT2 data structure that */
/* is only present in views saved in V2 or later. */
/* All descriptors and values are packed into one item named $VIEWFORMAT. */

typedef struct
	{
	BYTE	Version;						/* Version number */
	BYTE	ViewStyle;						/* View Style - Table,Calendar */
	} VIEW_FORMAT_HEADER;

typedef struct
	{
	VIEW_FORMAT_HEADER Header;
	WORD 	Columns;								/* Number of columns */
	WORD	ItemSequenceNumber;						/* Sequence number for unique item names */
	WORD 	Flags;									/* Flags */
#define VIEW_TABLE_FLAG_COLLAPSED			0x0001	/* Default to fully collapsed */
#define VIEW_TABLE_FLAG_FLATINDEX			0x0002	/* Do not index hierarchically */
													/* If FALSE, MUST have */
													/* NSFFormulaSummaryItem($REF) */
													/* as LAST item! */
#define VIEW_TABLE_FLAG_DISP_ALLUNREAD		0x0004	/* Display unread flags in margin at ALL levels */
#define VIEW_TABLE_FLAG_CONFLICT			0x0008	/* Display replication conflicts */
													/* If TRUE, MUST have */
													/* NSFFormulaSummaryItem($Conflict) */
													/* as SECOND-TO-LAST item! */
#define VIEW_TABLE_FLAG_DISP_UNREADDOCS		0x0010	/* Display unread flags in margin for documents only */
#define VIEW_TABLE_GOTO_TOP_ON_OPEN			0x0020	/* Position to top when view is opened. */
#define VIEW_TABLE_GOTO_BOTTOM_ON_OPEN		0x0040	/* Position to bottom when view is opened. */
#define VIEW_TABLE_ALTERNATE_ROW_COLORING	0x0080	/* Color alternate rows. */
#define VIEW_TABLE_HIDE_HEADINGS			0x0100	/* Hide headings. */
#define VIEW_TABLE_HIDE_LEFT_MARGIN			0x0200	/* Hide left margin. */
#define VIEW_TABLE_SIMPLE_HEADINGS			0x0400	/* Show simple (background color) headings. */
#define VIEW_TABLE_VARIABLE_LINE_COUNT		0x0800	/* TRUE if LineCount is variable (can be reduced as needed). */

/*	Refresh flags.

	When both flags are clear, automatic refresh of display on update
	notification is disabled.  In this case, the refresh indicator will
	be displayed.
	
	When VIEW_TABLE_GOTO_TOP_ON_REFRESH is set, the view will fe refreshed from
	the top row of the collection (as if the user pressed F9 and Ctrl-Home).
	
	When VIEW_TABLE_GOTO_BOTTOM_ON_REFRESH is set, the view will be refreshed
	so the bottom row of the collection is visible (as if the user pressed F9
	and Ctrl-End).
	
	When BOTH flags are set (done to avoid using another bit in the flags),
	the view will be refreshed from the current top row (as if the user
	pressed F9). */

#define VIEW_TABLE_GOTO_TOP_ON_REFRESH		0x1000	/* Position to top when view is refreshed. */
#define VIEW_TABLE_GOTO_BOTTOM_ON_REFRESH	0x2000	/* Position to bottom when view is refreshed. */

/*	More flag(s). */

#define VIEW_TABLE_EXTEND_LAST_COLUMN		0x4000	/* TRUE if last column should be extended to fit the window width. */

	WORD	spare2;									/* spare 2 */
	} VIEW_TABLE_FORMAT;

/*  Additional (since V2) format info.  This structure follows the
	variable length strings that follow the VIEW_COLUMN_FORMAT structres */

typedef struct
	{
	WORD	Length;							/* Length of this structure */
	WORD	BackgroundColor;				/* Color of view's background. Pre-V4 compatible */
	WORD	V2BorderColor;					/* Archaic! Color of view's border lines. */
	FONTID	TitleFont;						/* Title and borders */
	FONTID	UnreadFont;						/* Unread lines */
	FONTID	TotalsFont;						/* Totals/Statistics */
	WORD	AutoUpdateSeconds;				/* interval b/w auto updates (zero for no autoupdate) */
	WORD	AlternateBackgroundColor;		/* Color of view's background for alternate rows. */

	/*	When wSig == VALID_VIEW_FORMAT_SIG, rest of struct is safe to use.  Bug
		in versions prior to V4 caused spare space in this structure to contain
		random stuff. */

	WORD	wSig;							
#define VALID_VIEW_FORMAT_SIG				0x2BAD
	BYTE	LineCount;						/* Number of lines per row.  1, 2, etc. */
#define VIEW_TABLE_MAX_LINE_COUNT			10
	BYTE	Spacing;						/* Spacing.  VIEW_TABLE_XXX_SPACE. */
#define VIEW_TABLE_SINGLE_SPACE				0
#define VIEW_TABLE_ONE_POINT_25_SPACE		1
#define VIEW_TABLE_ONE_POINT_50_SPACE		2
#define VIEW_TABLE_ONE_POINT_75_SPACE		3
#define VIEW_TABLE_DOUBLE_SPACE				4
	WORD	BackgroundColorExt;				/* Palette Color of view's background. */
#define VIEW_TABLE_COLOR_MASK	0x00FF		/* color is index into 240 element array */
	BYTE	HeaderLineCount;				/* Lines per header. */

	/* Spares.  Will be zero when wSig == VALID_VIEW_FORMAT_SIG. */

	BYTE	SpareByte;
	WORD	Spare[4];						
	} VIEW_TABLE_FORMAT2;

typedef struct
	{
	VIEW_FORMAT_HEADER Header;
	} VIEW_DAY_FORMAT;

typedef struct
	{
	VIEW_FORMAT_HEADER Header;
	} VIEW_WEEK_FORMAT;

typedef struct
	{
	VIEW_FORMAT_HEADER Header;
	} VIEW_MONTH_FORMAT;


/* 	Calendar View Format Information.  Introduced in build 141 (for 4.2).
	This is in Calendar Style Views only. */

#define VIEW_CALENDAR_FORMAT_VERSION	1

typedef struct
	{
	BYTE	Version;						/* Version Number */	
	BYTE	Formats;						/* Formats supported by this view VIEW_CAL_FORMAT_XXX.*/
#define VIEW_CAL_FORMAT_TWO_DAY		0x01
#define VIEW_CAL_FORMAT_ONE_WEEK	0x02
#define VIEW_CAL_FORMAT_TWO_WEEKS	0x04	
#define VIEW_CAL_FORMAT_ONE_MONTH	0x08	
#define VIEW_CAL_FORMAT_ONE_YEAR	0x10  	
#define VIEW_CAL_FORMAT_ALL			0xff

	FONTID	DayDateFont;					/* Day and Date display */
	FONTID	TimeSlotFont;					/* Time Slot display */
	FONTID	HeaderFont;					   	/* Month Headers */

	WORD	DaySeparatorsColor;				/* Lines separating days */
	WORD    TodayColor;						/* Color Today is displayed in */

	WORD	wFlags;							/* Misc Flags */
#define CAL_DISPLAY_CONFLICTS		0x0001	/* Display Conflict marks */
#define CAL_ENABLE_TIMESLOTS		0x0002	/* Disable Time Slots */
#define CAL_DISPLAY_TIMESLOT_BMPS	0x0004	/* Show Time Slot Bitmaps */
	
	WORD    BusyColor;						/* Color busy times are displayed in */

	WORD    wTimeSlotStart;				 	/* TimeSlot start time (in minutes from midnight) */
	WORD    wTimeSlotEnd;				 	/* TimeSlot end time (in minutes from midnight) */
	WORD    wTimeSlotDuration;			 	/* TimeSlot duration (in minutes) */

	WORD    unused;							
	DWORD	Spare[7];
	} VIEW_CALENDAR_FORMAT;

typedef VIEW_CALENDAR_FORMAT *PVIEW_CALENDAR_FORMAT;


/* View column format descriptor.  One per column. */

typedef struct tagVIEW_COLUMN_FORMAT
	{
	WORD	Signature;							/* VIEW_COLUMN_FORMAT_SIGNATURE */
	WORD	Flags1;
#define	VCF1_S_Sort						0		/* Add column to sort */
#define	VCF1_M_Sort						0x0001
#define	VCF1_S_SortCategorize			1		/* Make column a category */
#define	VCF1_M_SortCategorize			0x0002
#define	VCF1_S_SortDescending			2		/* Sort in descending order (ascending if FALSE) */
#define	VCF1_M_SortDescending			0x0004
#define	VCF1_S_Hidden					3		/* Hidden column */
#define	VCF1_M_Hidden					0x0008
#define	VCF1_S_Response					4		/* Response column */
#define	VCF1_M_Response					0x0010
#define	VCF1_S_HideDetail				5		/* Do not show detail on subtotalled columns */
#define	VCF1_M_HideDetail				0x0020
#define	VCF1_S_Icon						6		/* Display icon instead of text */
#define	VCF1_M_Icon						0x0040
#define	VCF1_S_NoResize					7		/* Resizable at run time. */
#define	VCF1_M_NoResize					0x0080
#define	VCF1_S_ResortAscending			8		/* Resortable in ascending order. */
#define	VCF1_M_ResortAscending			0x0100
#define	VCF1_S_ResortDescending			9		/* Resortable in descending order. */
#define	VCF1_M_ResortDescending			0x0200
#define	VCF1_S_Twistie					10		/* Show twistie if expandable. */
#define	VCF1_M_Twistie					0x0400
#define	VCF1_S_ResortToView				11		/* Resort to a view. */
#define	VCF1_M_ResortToView				0x0800
#define	VCF1_S_SecondResort				12		/* Secondary resort column set. */
#define	VCF1_M_SecondResort				0x1000
#define	VCF1_S_SecondResortDescending	13		/* Secondary column resort descending (ascending if clear). */
#define	VCF1_M_SecondResortDescending	0x2000
#define	VCF1_S_CaseInsensitiveSort		14		/* Case insensitive sorting. */
#define	VCF1_M_CaseInsensitiveSort		0x4000
#define	VCF1_S_AccentInsensitiveSort	15		/* Accent insensitive sorting. */
#define	VCF1_M_AccentInsensitiveSort	0x8000


#define	VCF1_M_spare					0xc000	/* Spare flags. */
	WORD	ItemNameSize;						/* Item name string size */
	WORD	TitleSize;							/* Title string size */
	WORD	FormulaSize;						/* Compiled formula size */
	WORD	ConstantValueSize;					/* Constant value size */
	WORD	DisplayWidth;						/* Display width - 1/8 ave. char width units */
	FONTID	FontID;								/* Display font ID */
	WORD	Flags2;
#define	VCF2_S_DisplayAlignment			0		/* Display alignment - VIEW_COL_ALIGN_XXX */
#define	VCF2_M_DisplayAlignment			0x0003
#define	VCF2_S_SubtotalCode				2		/* Subtotal code (NIF_STAT_xxx) */
#define	VCF2_M_SubtotalCode				0x003c
#define	VCF2_S_HeaderAlignment			6		/* Header alignment - VIEW_COL_ALIGN_XXX */
#define	VCF2_M_HeaderAlignment			0x00c0
#define	VCF2_S_SortPermute				8		/* Make column permuted if multi-valued */
#define	VCF2_M_SortPermute				0x0100
#define	VCF2_M_spare					0xfe00
	NFMT	NumberFormat;						/* Number format specification */
	TFMT	TimeFormat;							/* Time format specification */
	WORD	FormatDataType;						/* Last format data type */
	WORD	ListSep;							/* List Separator */
	} VIEW_COLUMN_FORMAT;	

/* View column display alignment.  */
/*		Note: order and values are assumed in VIEW_ALIGN_XXX_ID's. */

#define VIEW_COL_ALIGN_LEFT		0				/* Left justified */
#define VIEW_COL_ALIGN_RIGHT	1				/* Right justified */
#define VIEW_COL_ALIGN_CENTER	2				/* Centered */

/* Simple format data types, used to initialize dialog box to last "mode". */

#define VIEW_COL_NUMBER 0
#define VIEW_COL_TIMEDATE 1
#define VIEW_COL_TEXT 2

/*	Extended View column format descriptor.  One per column as of Notes V4.
	NOTE:  If you add variable data to this structure, store the packed,
	variable data AFTER the array of structures. */

typedef struct
	{
	WORD	Signature;						/* VIEW_COLUMN_FORMAT_SIGNATURE2 */
	FONTID	HeaderFontID;					/* FontID of column header. */
	UNID	ResortToViewUNID;				/* UNID of view to switch to. */
	WORD	wSecondResortColumnIndex;		/* 0 based index of secondary resort column. */
	WORD	wSpare;
	DWORD	dwSpare[5];
	} VIEW_COLUMN_FORMAT2;

#endif /* VIEWFMT_DEFS */


#ifdef __cplusplus
}
#endif

