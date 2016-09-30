// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

/*	Notes Rich Text On-Disk Structure Definitions -
	Record format used in the NSF data type TYPE_COMPOSITE. */


#ifndef EDIT_ODS_DEFS
#define EDIT_ODS_DEFS

#ifndef ODS_DEFS
#include "ods.h"
#endif
	
#ifndef NSF_DATA_DEFS
#include "nsfdata.h"					/* for NOTELINK */
#endif

#ifndef FONTID_DEFS
#include "fontid.h"						/* for FONTID and MAXFACESIZE */
#endif

#ifndef MISC_DEFS
#include "misc.h"						/* for NFMT and TFMT */
#endif



/*	Paragraph Record - Defines the start of a new paragraph */
	
typedef struct {
	BSIG Header;
} CDPARAGRAPH;

/*	Paragraph Attribute Block Definition Record */

#define MAXTABS 20				/* maximum number of stops in tables */
#define MAX_STYLE_NAME 35
#define MAX_STYLE_USERNAME 128

typedef struct {
	WSIG Header;
	WORD PABID;					/* ID of this PAB */
	WORD JustifyMode;			/* paragraph justification type */
	WORD LineSpacing;			/* (2 * (Line Spacing - 1)) (0:1,1:1.5,2:2,etc) */
	WORD ParagraphSpacingBefore; /* no. of LineSpacing units above paragraph */
	WORD ParagraphSpacingAfter;	/* no. of LineSpacing units below paragraph */
	WORD LeftMargin;			/* leftmost margin, twips rel to abs left */
								/* (16 bits = about 44") */
	WORD RightMargin;			/* rightmost margin, twips rel to abs right */
								/* (16 bits = about 44") */
								/* Special value "0" means right margin */
								/* will be placed 1" from right edge of */
								/* paper, regardless of paper size. */
	WORD FirstLineLeftMargin;	/* leftmost margin on first line */
								/* (16 bits = about 44") */
	WORD Tabs;					/* number of tab stops in table */
	SWORD Tab[MAXTABS];			/* table of tab stop positions, negative
								   value means decimal tab */
								/* (15 bits = about 22") */
	WORD Flags;					/* paragraph attribute flags */
	DWORD TabTypes;				/* 2 bits per tab */
	WORD Spare[1];

} CDPABDEFINITION;


/*	New PAB record for V4 -hide when formula.	*/

typedef struct
	{
	WSIG	Header;
	WORD	PABID;
	BYTE	Reserverd [8];
	} CDPABHIDE;
	/* Follows is the actual formula.		*/

/*	PAB Reference Record -

	This record is output in two situations:  First, at the start of every
	item of type Composite.  Second, at the start of every paragraph.  If,
	when reading this record during a note READ operation, the paragraph
	in question has NO runs in it, this defines the PAB to use for the
	paragraph.  If the paragraph has runs already in it, ignore this record. */
	
typedef struct {
	BSIG Header;
	WORD PABID;					/* ID number of the PAB being referenced */
} CDPABREFERENCE;


/*	This record is similar to a pab reference record but applies
	only to the pab's hide when formula which is new for V4
	so we make a new type that v3 can safely ignore.
*/

typedef struct 
{
	BSIG Header;
	WORD SourcePABID;				/* ID number of the source PAB 
										containing the formula.	*/
	WORD DestPABID;					/*	ID number of the dest PAB */
} CDPABFORMULAREF;


/*	Style Name Record */

#define STYLE_FLAG_FONTID		0x00000001L
#define STYLE_FLAG_INCYCLE		0x00000002L
#define STYLE_FLAG_PERMANENT	0x00000004L
typedef struct
	{
	BSIG Header;
	DWORD Flags;				/* Currently unused, but reserve some flags */
	WORD PABID;					/* ID number of the PAB being named */
	char StyleName[MAX_STYLE_NAME+1];	/* The style name. */
	/*	If STYLE_FLAG_FONTID, a FONTID follows this structure. */
	} CDSTYLENAME;

/*	Begin Table Record -

	This record specifies the beginning of a table.  It contains interesting
	information about the format and size of the table. */

typedef struct
	{
	BSIG Header;
	WORD LeftMargin;					/*	TWIPS */
	WORD HorizInterCellSpace;			/*	TWIPS */
	WORD VertInterCellSpace;			/*	TWIPS */
/* NOTE! all items below this comment are NOT guaranteed to have been zeroed if
	created in V2; all items are zeroed before use in V4 */
	WORD V4HorizInterCellSpace;			/*	TWIPS -- this field was spare in v3 */
	WORD V4VertInterCellSpace;			/*	TWIPS -- this field was spare in v3 */	
	WORD Flags;							/* Flags (CDTABLE_xxx) */
#define	CDTABLE_AUTO_CELL_WIDTH		0x0001	/* True if automatic cell width calculation */
#define CDTABLE_V4_BORDERS			0x0002  /* True if the table was created in v4 */
#define CDTABLE_3D_BORDER_EMBOSS	0x0004  /* True if the table uses embossed borders */
#define CDTABLE_3D_BORDER_EXTRUDE	0x0008  /* True if the table uses extruded borders */
	} CDTABLEBEGIN;

typedef struct
	{
	BSIG Header;
	BYTE Row;						/*	Row number (0 based) */
	BYTE Column;					/*	Column number (0 based) */
	WORD LeftMargin;				/*	Twips */
	WORD RightMargin;				/*	Twips */
	WORD FractionalWidth;			/*	20" (in twips) * CellWidth / TableWidth
										Used only if AutoCellWidth is
										specified in the TABLEBEGIN. */
	BYTE Border;					/*	4 cell borders, each 2 bits wide */
									/*	(see shift and mask CDTC_xxx values) */
									/*	Value of each cell border is one of */
									/*	TABLE_BORDER_xxx. */
#define	CDTC_S_Left		0
#define	CDTC_M_Left		0x0003
#define	CDTC_S_Right	2
#define	CDTC_M_Right	0x000c
#define	CDTC_S_Top		4
#define	CDTC_M_Top		0x0030
#define	CDTC_S_Bottom	6
#define	CDTC_M_Bottom	0x00c0
#define TABLE_BORDER_NONE	0
#define TABLE_BORDER_SINGLE	1
#define TABLE_BORDER_DOUBLE	2

	BYTE Flags;						
#define CDTABLECELL_USE_BKGCOLOR	0x01	/* True if background color */
#define CDTABLECELL_USE_V42BORDERS	0x02	/* True if version 4.2 or after */
#define CDTABLECELL_INVISIBLEH		0x04	/* True if cell is not spanned */
#define CDTABLECELL_INVISIBLEV		0x08	/* True if cell is not spanned */

	WORD v42Border;				/* wider borders */
#define	CDTC_S_V42_Left		0
#define	CDTC_M_V42_Left		0x000f
#define	CDTC_S_V42_Right	4
#define	CDTC_M_V42_Right	0x00f0
#define	CDTC_S_V42_Top		8
#define	CDTC_M_V42_Top		0x0f00
#define	CDTC_S_V42_Bottom	12
#define	CDTC_M_V42_Bottom	0xf000

	BYTE RowSpan;
	BYTE ColumnSpan;
	WORD BackgroundColor;			/* Color of background of cell */
	} CDTABLECELL;

typedef struct
	{
	BSIG Header;
	WORD SpareWORD;
	WORD SpareFlags;
	} CDTABLEEND;


/*	CDFIELD - Field Reference Record, used in forms ($BODY) to define a field. */

typedef struct {
	WSIG Header;
	WORD Flags;					/* Field Flags (see Fxxx flags below) */
	WORD DataType;				/* Alleged NSF Data Type */
	WORD ListDelim;				/* List Delimiters (LDELIM_xxx and LDDELIM_xxx) */
	NFMT NumberFormat;			/* Number format, if applicable */	
	TFMT TimeFormat;			/* Time format, if applicable */
	FONTID FontID;				/* displayed font */
	WORD DVLength;				/* Default Value Formula */
	WORD ITLength;				/* Input Translation Formula */
	WORD Unused1;				/* Unused */
	WORD IVLength;				/* Input Validity Check Formula */
	WORD NameLength;			/* NSF Item Name */
	WORD DescLength;			/* Description of the item */
	WORD TextValueLength;		/* (Text List) List of valid text values */
								/* Now comes the variable part of the struct... */
} CDFIELD;

/*	CDFIELD List Delimeters (ListDelim) */

#define	LDELIM_SPACE		0x0001	/* low three nibbles contain delim flags */
#define	LDELIM_COMMA		0x0002
#define	LDELIM_SEMICOLON	0x0004
#define	LDELIM_NEWLINE		0x0008
#define	LDELIM_BLANKLINE	0x0010
#define	LD_MASK				0x0fff

#define	LDDELIM_SPACE		0x1000	/* high nibble contains the display type */
#define	LDDELIM_COMMA		0x2000
#define	LDDELIM_SEMICOLON	0x3000
#define	LDDELIM_NEWLINE		0x4000
#define	LDDELIM_BLANKLINE	0x5000
#define	LDD_MASK			0xf000

/*	CDFIELD Flags Definitions */

#define	V3SPARESTOCLEAR		0x0075	/* Clear these if FOCLEARSPARES is TRUE */

#define	FREADWRITERS		0x0001	/* Field contains read/writers */
#define	FEDITABLE			0x0002	/* Field is editable, not read only */
#define	FNAMES				0x0004	/* Field contains distinguished names */
#define	FSTOREDV			0x0008	/* Store DV, even if not spec'ed by user */
#define	FREADERS			0x0010	/* Field contains document readers */
#define FSECTION			0x0020	/* Field contains a section */
#define	FSPARE3				0x0040	/* can be assumed to be clear in memory, V3 & later */
#define	FV3FAB				0x0080	/* IF CLEAR, CLEAR AS ABOVE */
#define	FCOMPUTED			0x0100	/* Field is a computed field */
#define	FKEYWORDS			0x0200	/* Field is a keywords field */
#define	FPROTECTED			0x0400	/* Field is protected */
#define	FREFERENCE			0x0800	/* Field name is simply a reference to a shared field note */
#define	FSIGN				0x1000	/* sign field */
#define	FSEAL				0x2000	/* seal field */
#define	FKEYWORDS_UI_STANDARD		0x0000	/* standard UI */
#define	FKEYWORDS_UI_CHECKBOX		0x4000	/* checkbox UI */
#define	FKEYWORDS_UI_RADIOBUTTON	0x8000	/* radiobutton UI */
#define	FKEYWORDS_UI_ALLOW_NEW		0xc000	/* allow doc editor to add new values */

/*	CDEXTFIELD - Extended Field Reference Record, used in forms ($BODY) to define a field. */

typedef struct {
	WSIG Header;
	DWORD Flags1;				/* Field Flags (see FEXT_xxx flags below) */
	DWORD Flags2;
	WORD EntryHelper;			/* Field entry helper type (see FIELD_HELPER_XXX below) */
	WORD EntryDBNameLen;		/* Entry helper DB name length */
	WORD EntryViewNameLen;		/* Entry helper View name length */
	WORD EntryColumnNumber;		/* Entry helper column number */
								/* Now comes the variable part of the struct... */
} CDEXTFIELD;

/*	Flags for CDEXTFIELD Flags1.  Note that the low word in Flags1 is not used. */

#define FEXT_LOOKUP_EACHCHAR		0x00010000L	/* lookup name as each char typed */
#define FEXT_KWSELRECALC			0x00020000L	/* recalc on new keyword selection */
#define FEXT_KWHINKYMINKY			0x00040000L	/* suppress showing field hinky minky */
#define FEXT_AFTERVALIDATION		0x00080000L	/* recalc after validation */
#define FEXT_ACCEPT_CARET			0x00100000L	/* the first field with this bit set will accept the caret */
/*	These bits are in use by the	0x02000000L
	column value.  The result of	0x04000000L
	the shifted bits is (cols - 1)	0x08000000L */
#define FEXT_KEYWORD_COLS_SHIFT		25
#define FEXT_KEYWORD_COLS_MASK		0x0E000000L
#define FEXT_KEYWORD_FRAME_3D		0x00000000L
#define FEXT_KEYWORD_FRAME_STANDARD	0x10000000L
#define FEXT_KEYWORD_FRAME_NONE		0x20000000L
#define FEXT_KEYWORD_FRAME_MASK		0x30000000L
#define FEXT_KEYWORD_FRAME_SHIFT	28
#define FEXT_KEYWORDS_UI_COMBO		0x40000000L
#define FEXT_KEYWORDS_UI_LIST		0x80000000L

/*	Flags for CDEXTFIELD Flags2. */

#define FEXT_KW_CHOICE_RECALC		0x00000001 /* TRUE to recalc the value choices. */



/*	The following identifiers indicate the type of helper in use by the 
	Keyword and the Name helper/pickers */
/* these define the VarDataFlags signifying variable length data following struct */
#define	CDEXTFIELD_KEYWORDHELPER	0x0001
#define	CDEXTFIELD_NAMEHELPER		0x0002
#define 	FIELD_HELPER_NONE			0
#define 	FIELD_HELPER_ADDRDLG		1
#define 	FIELD_HELPER_ACLDLG			2
#define 	FIELD_HELPER_VIEWDLG		3
	
/*	(*** OBSOLETE ***) Pre-V1 Field Reference Record, used in Forms (*** OBSOLETE ***) */

typedef struct {
	WSIG Header;
	WORD Flags;					/* Field Flags */
	WORD DataType;				/* Alleged NSF Data Type */
	WORD ListDelim;				/* List Delimiters */
	NFMT NumberFormat;			/* Number format, if applicable */	
	TFMT TimeFormat;			/* Time format, if applicable */
	FONTID FontID;				/* displayed font */
	WORD DVLength;				/* Default Value Formula */
	WORD ITLength;				/* Input Translation Formula */
	WORD Unused1;				/* Unused */
	WORD IVLength;				/* Input Validity Check Formula */
	WORD NameLength;			/* NSF Item Name */
	WORD DescLength;			/* Description of the item */
								/* Now comes the variable part of the struct... */

} CDFIELD_PRE_36;				/* List of Text Values was added... */


/*	CDTEXT - 8-bit text string record */

typedef struct {
	WSIG	Header;				/* Tag and length */
	FONTID	FontID;				/* Font ID */
								/* The 8-bit text string follows... */
} CDTEXT;

/*	CDTEXT - 8-bit text string record */

typedef struct {
	WSIG	Header;				/* Tag and length */
	FONTID	FontEffectID;				/* Font ID */
} CDTEXTEFFECT;


/*	CDLINK2 - Link record */

typedef struct	{
	WSIG Header;
	WORD LinkID;				/* Index into array in $LINKS/$FORMLINKS field of this document */
								/* Now comes the display comment... */
} CDLINK2;


/*	CDLINKEXPORT - This record is used in the case of exporting
	a note to the clipboard, where the NSF item describing the links
	cannot be generated. */


typedef struct	{
	WSIG Header;
	NOTELINK NoteLink;
								/* Now comes the display comment... */
} CDLINKEXPORT2;


/*	CDKEYWORD - Keyword Record */

#define CDKEYWORD_RADIO				0x0001
/*	These bits are in use by the	0x0002
	column value.  The result of	0x0004
	the shifted bits is (cols - 1)	0x0008 */
#define CDKEYWORD_COLS_SHIFT		1
#define CDKEYWORD_COLS_MASK			0x000E
#define CDKEYWORD_FRAME_3D			0x0000
#define CDKEYWORD_FRAME_STANDARD	0x0010
#define CDKEYWORD_FRAME_NONE		0x0020
#define CDKEYWORD_FRAME_MASK		0x0030
#define CDKEYWORD_FRAME_SHIFT		4		


typedef struct {
	WSIG	Header;				/* Tag and length */
	FONTID	FontID;				/* Font ID */
	WORD	Keywords;			/* number of keywords */
	WORD	Flags;
/*	char	OnOff[];			   array of '1' or '0' indicating state */
/*	char	TextValues[];		   packed buffer of keyword text, fab->pTextValues format */
} CDKEYWORD;



/*	Here is a description of Notes bitmap encoding.
*
*	COLOR FORMATS:
*
*	Notes displays 3 types of bitmaps: monochrome, color and grey scale. 
*	All monochome bitmaps are one bit per pixel.  Color bitmaps can be either 8 bits per Pel (color
*	mapped) or 16 bits per pel "quasi true" color.  Grey scale bitmaps 
*	are simply treated as "color" bitmaps, using the 8 bits per Pel format
*	with a color table whose RGB tuples range from [0,0,0] through 
*	[255,255,255].
*
*	GEOMETRY:
*
*	All bitmaps are single plane encoding. 8 bit color/grey scale must have
*	color tables provided. Monochome bitmaps and 16 bit "quasi true" color
*	bitmaps don't need a color table.  
*
*	RASTER LINE ENCODING:
*
*	For those programmers using Notes API, raster lines are encoded using
*	a simple run-length encoding format, where each raster line of the 
*	bitmap is encoded separately (i.e. run length won't exceed length of
*	a raster line). Also, each raster line is NOT padded to any particular
*	boundary; each scanline ends on the byte boundary which is defined by the
*	width of the bitmap. The following section describes how each raster line
*	is encoded using a simple run-length encoding scheme.
*
*
*		Notes bitmap compression scheme description
*		--------------------------------------------		
*		We have devised a scheme which does a good job compressing 
*		monochrome, color (both mapped and RGB) and gray scale 
*		bitmaps, and a secondary encoding to allow "raw" uncompressed
*		scanlines for those scanlines which actually "expand" by using
*		the run-length scheme (this happens in dithered images).  
*		In the run length encoding scheme, there an escape codes followed by either 
*		a run length byte which is then followed by the 
*		byte(s) to repeat. Note that the byte(s) to repeat may be either 
*		one or two bytes depending on the color format. For monochrome,
*		8 bit color and 8 bit grey scale, use one byte. For 16 bit color
*		the PEL is two bytes long, so the two byte quantity is repeated. 
*
*			MSB<---------->LSB
*			+-----------------------------------+
*		(1)	| 1 1 c c c c c c | r r r r r r r r |
*			+-----------------------------------+
*				cccccc = six bit repeat count
*				rrrrrrrr = PELS to repeat
*
*
*		In the following non-compressed encoding, the escape code is 
*		followed by a 6 bit repeat count of raw PELs (one or two byte).
*
*
*			MSB<---------->LSB
*			+------------------------------------------------------
*		(2)	| 0 0 c c c c c c | r r r r r r r r |[r r r r r r r r]|...
*			+------------------------------------------------------
*				cccccc = 6 bit repeat count
*				r[cccccc] = 1 or more raw uncompressed PELS							
*
*/


/*	A color table (used in CDBITMAPHEADER) is an array of packed colors.
	Each color is stored in 3 bytes (Red,Green,Blue), packed without any
	intervening pad bytes. */

#define CT_ENTRY_SIZE		3  /* Always 3 bytes, packed */
#define CT_RED_OFFSET		0
#define CT_GREEN_OFFSET		1
#define CT_BLUE_OFFSET		2

#define CT_REDVALUE(x) (x[CT_RED_OFFSET])
#define CT_GREENVALUE(x) (x[CT_GREEN_OFFSET])
#define CT_BLUEVALUE(x) (x[CT_BLUE_OFFSET])

#define CT_NEXT(x) (x+=CT_ENTRY_SIZE)
#define CT_ENTRY_PTR(x,ElmNum) (&x[CT_ENTRY_SIZE*ElmNum])

/*	A pattern table is a fixed-size color table used for patterns by
	CDBITMAPHEADER (patterns are used to compress the bitmap).
	A entry in the pattern table is 8 (PELS_PER_PATTERN) packed colors
	(3 bytes per color as above). */

#define PELS_PER_PATTERN 8

/* Maximum number of patterns we will ever store in a CDBITMAPHEADER. */

#define MAXPATTERNS 64

/* Maximum number of colors in a color table (8 bit mapped color) */

#define MAXCOLORS 256

/*	The CDBITMAPHEADER record must be present for all bitmaps. It must
	follow the CDGRAPHIC record, but come before any of the other bitmap
	CD records. */

typedef struct
	{
	LSIG	Header;				/* Signature and Length */
	RECTSIZE Dest;				/* dest bitmap height and width in PELS */
	RECTSIZE Crop;				/* crop destination dimensions in PELS (UNUSED) */
	WORD  Flags;				/* CDBITMAP_FLAGS Valid only in CDGRAPHIC_VERSION2 and later */
	WORD  wReserved;			/* Reserved for future use */
	DWORD lReserved;			/* Reserved for future use */
	WORD Width;					/* Width of bitmap in PELS */
	WORD Height;				/* Height "				 " */
	WORD BitsPerPixel;			/* Bits per PEL, must be 1,8 or 16 */
	WORD SamplesPerPixel;		/* For 1 or 8 BPP, set to 1. For 16 BBP, set to 3 */
	WORD BitsPerSample;			/* For 1 BPP, set to 1. For 8 BPP, set to 8. For 16 BPP, set to 5 */
	WORD SegmentCount;			/* Number of CDBITMAPSEGMENTS */
	WORD ColorCount;			/* Number of entries in CDCOLORTABLE. 0-256 */
	WORD PatternCount;			/* Number of entries in CDPATTERNTABLE. Set to 0 if using Notes API. */
	} CDBITMAPHEADER;

/* Bitmap Uses > 16 colors or > 4 grey scale levels */

#define CDBITMAP_FLAG_REQUIRES_PALETTE	1	

/* Initialized by import code for "first time" importing of bitmaps
	from clipboard or file, to tell Notes that it should compute whether
	or not to use a color palette or not.  All imports and API programs
	should initially set this bit to let the Editor compute whether it
	needs the palette or not. */

#define CDBITMAP_FLAG_COMPUTE_PALETTE	2	


/* 	Bitmap Transparency Table (optionally one per bitmap).  The colors
	in this table specify the bitmap colors that are "transparent".  The
	pixels in the bitmap whose colors are in this table will not affect
	the background; the background will "bleed through" into the bitmap.
	The entries in the transparency table should be in the same format
	as entries in the color table.  If a transparency table is used
	for a bitmap, it must immediately follow the CDBITMAPHEADER. */

typedef struct
	{
	LSIG Header;				/* Signature and length */
	WORD Reserved;				/* Reserved for future use */
	WORD TransparentCount;		/* Count of entries that follow (0-256) */
	/* One or more transparent color table entries */
	} CDTRANSPARENTTABLE;


/* 	Each of the following CDBITMAP segments contains the compressed raster
	data of the bitmap.  It is recommended that each segment be no larger
	than 10K for optimal use within Notes, but try to keep the segments as
	large as possible to increase painting speed. A scanline must not
	span a segment. A bitmap must contain at least one segment, but may have 
	many segments. */

typedef struct
	{
	LSIG	Header;				/* Signature and Length */
	DWORD Reserved[2];			/* Reserved for future use */
	WORD ScanlineCount;			/* Number of compressed scanlines in seg */
	WORD DataSize;				/* Size, in bytes, of compressed data */
	/* Comressed raster data for the segment follows right here */
	} CDBITMAPSEGMENT;


/* 	Bitmap Color Table. If the bitmap is 8 bit color or grey scale, you
	must have a color table.  However, you only need as many entries as
	you have colors, i.e. if a 16 color bitmap was converted to 8 bit
	form for Notes, the color table would only require 16 entries even
	though 8 bit color implies 256 entries.  The number of entries must
	match that specified in the CDBITMAPHEADER ColorCount. */

typedef struct
	{
	LSIG Header;
	/* One or more color table entries go here */
	} CDCOLORTABLE;

/* Bitmap Pattern Table (optionally one per bitmap) */

typedef struct
	{
	LSIG Header;
	/* One or more pattern table entries */
	} CDPATTERNTABLE;


/*	Crop rectangle used in graphic run */

typedef struct
	{
	WORD left;
	WORD top;
	WORD right;
	WORD bottom;	
	}CROPRECT;

/*	The Graphic combination record is used to store one or more graphic objects. 
	This record marks the beginning of a graphic composite item, and MUST
	be present for any graphic object to be loaded/displayed.  A
	graphic composite item can be one or more of the following CD
	record types: BITMAPHEADER, BITMAPSEGMENT, COLORTABLE, CGMMETA,
	WINMETA,WINMETASEG,PMMETAHEADER,PMMETASEG,MACMETAHEADER,MACMETASEG. If
	there is more than one graphic object, Notes will display only one object 
	using the following order: CGM Metafile, Native Metafile (i.e. Windows,
	PM,Mac),Bitmap. */

typedef struct
	{
	LSIG	Header;		/* Signature and Length */
	RECTSIZE DestSize;	/* Destination Display size in twips (1/1440 inch) */
	RECTSIZE CropSize;	/* Width and Height of crop rect in twips. Currently unused */
	CROPRECT CropOffset;/* Crop rectangle offset from bottom left of Dest (in twips).Currently unused */
	WORD	fResize;	/* Set to true if object has been resized by user. */
	BYTE    Version;	/* CDGRAPHIC_VERSION */
	BYTE	bFlags;		/* Ignored before CDGRAPHIC_VERSION3 */
	WORD	wReserved;
	} CDGRAPHIC;

/* Version control of graphic header */
#define CDGRAPHIC_VERSION1		0		/* Created by Notes version 2 */
#define CDGRAPHIC_VERSION2 		1		/* Created by Notes version 3 */
#define CDGRAPHIC_VERSION3 		2		/* Created by Notes version 4.5 */

/*	The following flag indicates that the DestSize field contains
	pixel values instead of twips. */

#define CDGRAPHIC_FLAG_DESTSIZE_IS_PIXELS	1

/*	CGM Metafile Record. This record follows the CDGRAPHIC record. It can
	contain the entire contents of a CGM metafile, and must be <= 64K Bytes
	in length. */

typedef struct
	{
	LSIG	Header;				/* Signature and Length */
	SWORD	mm;					/* see above CGM_MAPMODE_??? */
	SWORD	xExt,yExt;			/* Extents of drawing in world coordinates */
	RECTSIZE OriginalSize;		/* Original display size of metafile in twips */
	/*	CGM Metafile Bits Follow, must be <= 64K bytes total */
	} CDCGMMETA;

#define CGM_MAPMODE_ABSTRACT	0	/* Virtual coordinate system. This is default */
#define CGM_MAPMODE_METRIC		1	/*	Currently unsupported */


/*	Windows Metafile Record. This record follows the CDGRAPHIC record and
	contains the entire contents of a Windows GDI metafile.  Since these
	metafiles tend to be large, they may be segmented in chunks of any
	arbitrary size, as long as each segment is <= 64K bytes. */

typedef struct
	{
	LSIG	Header;				/* Signature and Length */
	SWORD	mm;					/* Windows mapping mode */
	SWORD	xExt,yExt;			/* size in mapping mode units */
	RECTSIZE OriginalDisplaySize; /* Original display size of metafile in twips */
	DWORD	MetafileSize;		/* Total size of metafile raw data in bytes */
	WORD 	SegCount;			/*	Number of CDWINMETASEG records */
	/*	Metafile segments Follow */
	} CDWINMETAHEADER;

typedef struct
	{
	LSIG	Header;				/* Signature and Length */
	WORD 	DataSize;			/* Actual Size of metafile bits in bytes, ignoring any filler */
	WORD 	SegSize; 			/*  Size of segment, is equal to or larger than DataSize 
									if filler byte added to maintain word boundary */
	/* Windows Metafile Bits for this segment. Each segment must be
	   <= 64K bytes. */
	} CDWINMETASEG;


/*	PM Metafile Record. This record follows the CDGRAPHIC record and
	contains the entire contents of a PM GPI metafile.  Since these
	metafiles tend to be large, they may be segmented in chunks of any
	arbitrary size, as long as each segment is <= 64K bytes. */

typedef struct
	{
	LSIG	Header;				/* Signature and Length of this record */
	SWORD	mm;					/* PM mapping mode, i.e. PU_??? */
	SWORD	xExt,yExt;			/* size in mapping mode units */
	RECTSIZE OriginalDisplaySize; /* Original display size of metafile in twips */
	DWORD	MetafileSize;		/* Total size of metafile raw data in bytes */
	WORD 	SegCount;			/*	Number of CDPMMETASEG records */
	} CDPMMETAHEADER;

typedef struct
	{
	LSIG	Header;				/* Signature and Length */
	WORD 	DataSize;			/* Actual Size of metafile bits in bytes, ignoring any filler */
	WORD 	SegSize; 			/*  Size of segment, is equal to or larger than DataSize 
									if filler byte added to maintain word boundary */
	/* PM Metafile Bits for this segment. Must be <= 64K bytes. */
	} CDPMMETASEG;

/* MACIntosh Metafile (PICT) */

typedef struct
	{
	LSIG	Header;				/* Signature and Length */
	RECTSIZE OriginalDisplaySize; /* Original display size of metafile in twips */
	DWORD	MetafileSize;		/* Total size of metafile raw data in bytes */
	WORD 	SegCount;			/*	Number of CDMACMETASEG records */
	/*	Metafile segments Follow */
	} CDMACMETAHEADER;

typedef struct
	{
	LSIG	Header;				/* Signature and Length */
	WORD 	DataSize;			/* Actual Size of metafile bits in bytes, ignoring any filler */
	WORD 	SegSize; 			/*  Size of segment, is equal to or larger than DataSize 
									if filler byte added to maintain word boundary */		
	/* Metafile Bits for this segment */
	} CDMACMETASEG;




/*	Document Record stored in $INFO field of a document.  This contains
	document-wide attributes. */
	/* for FormFlags */
#define	TPL_FLAG_REFERENCE	0x0001		/* Use Reference Note */
#define	TPL_FLAG_MAIL		0x0002		/* Mail during DocSave */
#define TPL_FLAG_NOTEREF	0x0004		/* Add note ref. to "reference note" */
#define TPL_FLAG_NOTEREF_MAIN 0x0008	/* Add note ref. to main parent of "reference note" */
#define TPL_FLAG_RECALC		0x0010		/* Recalc when leaving fields */
#define TPL_FLAG_BOILERPLATE 0x0020		/* Store form item in with note */
#define	TPL_FLAG_FGCOLOR	0x0040		/* Use foreground color to paint */
#define TPL_FLAG_SPARESOK	0x0080		/* Spare DWORDs have been zeroed */
#define TPL_FLAG_ACTIVATE_OBJECT_COMP	0x0100 /* Activate OLE objects when composing a new doc */	
#define TPL_FLAG_ACTIVATE_OBJECT_EDIT	0x0200 /* Activate OLE objects when editing an existing doc */	
#define TPL_FLAG_ACTIVATE_OBJECT_READ	0x0400 /* Activate OLE objects when reading an existing doc */	
#define TPL_FLAG_SHOW_WINDOW_COMPOSE	0x0800 /* Show Editor window if TPL_FLAG_ACTIVATE_OBJECT_COMPOSE */
#define TPL_FLAG_SHOW_WINDOW_EDIT	0x1000 /* Show Editor window if TPL_FLAG_ACTIVATE_OBJECT_EDIT */
#define TPL_FLAG_SHOW_WINDOW_READ	0x2000 /* Show Editor window if TPL_FLAG_ACTIVATE_OBJECT_READ */
#define	TPL_FLAG_UPDATE_RESPONSE	0x4000	/* V3 Updates become responses */
#define	TPL_FLAG_UPDATE_PARENT		0x8000	/* V3 Updates become parents */
	/* for FormFlags2 */
#define	TPL_FLAG_INCLUDEREF			0x0001	/* insert copy of ref note */
#define	TPL_FLAG_RENDERREF			0x0002	/* render ref (else it's a doclink) */
#define TPL_FLAG_RENDCOLLAPSE		0x0004	/* render it collapsed? */
#define TPL_FLAG_EDITONOPEN			0x0008	/* edit mode on open */
#define TPL_FLAG_OPENCNTXT			0x0010	/* open context panes */
#define TPL_FLAG_CNTXTPARENT		0x0020	/* context pane is parent */
#define TPL_FLAG_MANVCREATE			0x0040	/* manual versioning */
#define TPL_FLAG_UPDATE_SIBLING		0x0080	/* V4 versioning - updates are sibblings */
#define	TPL_FLAG_ANONYMOUS			0x0100	/* V4 Anonymous form */
#define	TPL_FLAG_NAVIG_DOCLINK_IN_PLACE 0x0200	/* Doclink dive into same window */
#define	TPL_FLAG_INTERNOTES			0x0400	/* InterNotes special form */
#define TPL_FLAG_DISABLE_FX			0x0800	/* Disable FX for this doc*/
#define TPL_FLAG_NOMENUS			0x1000	/* Disable menus for this DOC */
#define TPL_FLAG_CHECKDISPLAY		0x2000	/* check display before displaying background */


typedef struct {
	BSIG Header;
	WORD PaperColor;			/* Color of the paper being used */
	WORD FormFlags;				/* Form Flags */
	WORD NotePrivileges;		/* Privs for notes created when using form */

	/*	WARNING!!! Fields below this comment were not zeroed in builds
		prior to 100.  A mechanism has been set up to use them however.
		dload checks the TPL_FLAG_SPARESOK bit in the flags word.  If it
		is not set, all of the storage after this comment is zeroed.  On
		save, dsave makes sure the unused storage is zero and sets the bit. */

	WORD FormFlags2;			/* more Form Flags */
	WORD InherFieldNameLength;	/* Length of the name, which follows this struct */

	WORD PaperColorExt;			/* Palette Color of the paper being used. New in V4. */
#define ODS_COLOR_MASK	0x00FF	/* Palette color is an index into a 240 entry table */

	WORD Spare[5];
								/* ... now the Inherit Field Name string */
								/* ... now the Text Field Name string indicating
										which field to append version number to */
} CDDOCUMENT;


/*	Header/Footer Record, stored in $HEADER and $FOOTER fields of a
	document.  This contains the header and footer used in the document. */

typedef struct {
	WSIG	Header;
	BYTE	FontPitchAndFamily;
	char	FontName[MAXFACESIZE];
	FONTID	Font;
	WORD	HeadLength;			/* total header string length */
								/* ... now comes the string */
} CDHEADER;

/*	Font Table Record, stored in the $FONTS field of a document.
	This contains the list of "non-standard" fonts used in the
	document. */

typedef struct {
	WSIG	Header;				/* Tag and length */
	WORD	Fonts;				/* Number of CDFACEs following */
} CDFONTTABLE;					/* Now come the CDFACE records... */

typedef struct {
	BYTE Face;					/* ID number of face */
	BYTE Family;				/* Font Family */
	char Name[MAXFACESIZE];
} CDFACE;


/* Print settings data structure - (stored in desktop file per icon) */

typedef struct {

	WORD Flags;					/* PS_ flags below */

	WORD StartingPageNum;		/* Starting page number */
	WORD TopMargin;				/* Height between main body & top of page (TWIPS) */
	WORD BottomMargin;			/* Height between main body & bottom of page (TWIPS) */
	WORD ExtraLeftMargin;		/* Extra left margin width (TWIPS) */
								/* (beyond whats already specified in document) */
	WORD ExtraRightMargin;		/* Extra right margin width (TWIPS) */
								/* (beyond whats already specified in document) */
	WORD HeaderMargin;			/* Height between header & top of page (TWIPS) */
	WORD FooterMargin;			/* Height between footer & bottom of page (TWIPS) */
	WORD PageWidth;				/* Page width override (TWIPS) */
								/* (0 = "use printer's page width") */
	WORD PageHeight;			/* Page height override (TWIPS) */
								/* (0 = "use printer's page height") */
	WORD BinFirstPage;			/* Index of bin for 1st page */
	WORD BinOtherPage;			/* Index of bin for other pages */
	DWORD spare[3];				/* (spare words) */
	} PRINTNEW_SETTINGS;

typedef struct {

	WORD Flags;
#ifdef LITTLE_ENDIAN_ORDER
	#define PS_Initialized		0x0001	/* Print settings have been initialized */
	#define PS_HeaderFooterOnFirst 0x0002 /* Print header/footer on first page */
	#define PS_CropMarks		0x0004	/* Print crop marks */
	#define PS_ChangeBin		0x0008	/* Paper source should be set for 1st & Other Pg. */
#else
	#define PS_Initialized		0x8000
	#define PS_HeaderFooterOnFirst 0x4000
	#define PS_CropMarks		0x2000
	#define PS_ChangeBin		0x1000
#endif

	WORD StartingPageNum;		/* Starting page number */
	WORD TopMargin;				/* Height between main body & top of page (TWIPS) */
	WORD BottomMargin;			/* Height between main body & bottom of page (TWIPS) */
	WORD ExtraLeftMargin;		/* Extra left margin width (TWIPS) */
								/* (beyond whats already specified in document) */
	WORD ExtraRightMargin;		/* Extra right margin width (TWIPS) */
								/* (beyond whats already specified in document) */
	WORD HeaderMargin;			/* Height between header & top of page (TWIPS) */
	WORD FooterMargin;			/* Height between footer & bottom of page (TWIPS) */
	WORD PageWidth;				/* Page width override (TWIPS) */
								/* (0 = "use printer's page width") */
	WORD PageHeight;			/* Page height override (TWIPS) */
								/* (0 = "use printer's page height") */
	WORD BinFirstPage;
	WORD BinOtherPage;
	DWORD spare[3];				/* (spare words) */
	} PRINT_SETTINGS;

	
	
/*	Header/Footer data structure - passed into import/export modules */

typedef struct {
	BYTE	FontPitchAndFamily;
	char	FontName[MAXFACESIZE];
	FONTID	Font;
	WORD	HeadLength;			/* string length not including '\0' */
								/* Header string (ASCIIZ) follows */
} HEAD_DESC;

#define	MAXHEADERSTRING 256		/* maximum header string size */
typedef struct {				/* used for stack-local ones */
	HEAD_DESC Desc;
	char String[MAXHEADERSTRING]; /* Must be terminated by '\0' */
} HEAD_DESC_BUFFER;



/*	DDE composite data On Disk structures */

#define DDESERVERNAMEMAX	32
#define DDEITEMNAMEMAX		64
#define DDESERVERCOMMANDMAX	256

typedef struct
	{
	WSIG Header;						/* Signature and length of this record */
	char ServerName[DDESERVERNAMEMAX];  /* Null terminated server name */
	char TopicName[100];				/* Null terminated DDE Topic (usually a file name) */
	char ItemName[DDEITEMNAMEMAX];		/* Null terminated Place reference string */
	DWORD Flags;						/* See DDEFLAGS_xxx flag definitions below */
	char PasteEmbedDocName[80];			/*  only used on when making new link during Paste Special */
	WORD EmbeddedDocCount;				/* Number of embedded docs for this link */
										/* (MUST BE 0 or 1) */
	WORD ClipFormat;					/* Clipboard format with which data should be rendered */
										/* (DDEFORMAT_xxx defined below) */
	/* Null terminated embedded document name which is attached to the note follows.. */
	}CDDDEBEGIN;

/*	CDDDEBEGIN flags */

#define DDEFLAGS_AUTOLINK		0x01	/* Link type == Automatic (hot) */
#define DDEFLAGS_MANUALLINK		0x02	/* Link type == Manual (warm) */
#define DDEFLAGS_EMBEDDED		0x04	/* Embedded document exists */
#define DDEFLAGS_INITIATE		0x08	/* Used on paste to indicate not to
										   prompt user to initiate link */	
#define DDEFLAGS_CDP			0x10	/* Used on paste to indicate that
										   server uses Compound Document
										   protocol */
#define DDEFLAGS_NOTES_LAUNCHED 0x20	/* Used on CDP paste/load to indicate that
											Notes lauched app to establish
											original conversation. */

#define DDEFLAGS_CONV_ACTIVE 	0x40	/* Used on non-CDP paste/load to indicate that
										   conversation is already active 
										   for this embedded doc */
#define DDEFLAGS_EMBEDEXTRACTED	0x80	/* Used on non-CDP paste/load to indicate
											that Notes extracted the embedded
											file so that we may later close
											it */

#define DDEFLAGS_NEWOBJECT		0x100	/*	Set if this DDE Range is a new
											inserted object which contains
											no embedded object yet, i.e. a
											"blank" object */											
		
/*	These remappings of Native clipboard formats are used because we can't
	use Windows or PM constants because they are different */

#define DDEFORMAT_TEXT			0x01	/* CF_TEXT */
#define DDEFORMAT_METAFILE		0x02	/* CF_METAFILE or CF_METAFILEPICT */
#define DDEFORMAT_BITMAP		0x03	/* CF_BITMAP */
#define DDEFORMAT_RTF			0x04	/* Rich Text Format */
#define DDEFORMAT_OWNERLINK		0x06	/* OLE Ownerlink (never saved in CD_DDE or CD_OLE: used at run time) */
#define DDEFORMAT_OBJECTLINK	0x07	/* OLE Objectlink (never saved in CD_DDE or CD_OLE: used at run time) */
#define DDEFORMAT_NATIVE		0x08	/* OLE Native (never saved in CD_DDE or CD_OLE: used at run time) */
#define DDEFORMAT_ICON			0x09	/* Program Icon for embedded object */

/*	Total number of DDE format types supported.  Increment this if 
	one is added above */

#define DDEFORMAT_TYPES			5


typedef struct
	{
	WSIG Header;	/* Signature and length of this record */	
	DWORD Flags;	/* Currently unused, but reserve some flags */
	}CDDDEEND;


/*	On-disk format for an OLE object.  Both Links and
	embedded objects actually have an attached  $FILE "object"
	which is the variable length portion of the data which follows
	the CDOLEBEGIN record. */

typedef struct
	{
	WSIG Header;					/* Signature and length of this record */
	WORD Version;					/* Notes OLE implementation version */
	DWORD Flags;					/* See OLEREC_FLAG_xxx flag definitions below */
	WORD ClipFormat;				/* Clipboard format with which data should be rendered */
									/* (DDEFORMAT_xxx defined above) */
	WORD AttachNameLength;			/* Attached file name length */
	WORD ClassNameLength;			/* Used during Insert New Object, but never saved to disk */
	WORD TemplateNameLength;		/* User during Insert New Object, but never saved to disk */
	/* The Attachment Name (length "AttachNameLength") always follows... */
	/* The Classname, optional, then follows... */
	/* The Template Name, optional, then follows... */
	} CDOLEBEGIN;

typedef struct
	{
	WSIG Header;	/* Signature and length of this record */	
	DWORD Flags;	/* Currently unused, but reserve some flags */
	}CDOLEEND;

/*	Current OLE Version */

#define NOTES_OLEVERSION1	1
#define NOTES_OLEVERSION2	2

#define	OLEREC_FLAG_OBJECT		0x01	/* The data is an OLE embedded OBJECT */
#define OLEREC_FLAG_LINK		0x02    /* The data is an OLE Link */
#define OLEREC_FLAG_AUTOLINK	0x04	/* If link, Link type == Automatic (hot) */
#define OLEREC_FLAG_MANUALLINK	0x08	/* If link, Link type == Manual (warm) */
#define OLEREC_FLAG_NEWOBJECT	0x10	/* New object, just inserted */
#define OLEREC_FLAG_PASTED		0x20	/* New object, just pasted */
#define OLEREC_FLAG_SAVEOBJWHENCHANGED 0x40 /* Object came from form and should be saved 
											every time it changes in server */
#define OLEREC_FLAG_NOVISUALIZE	0x80	/* Object inherited from form, so don't visualize or
										   object incabable of rendering itself. */ 

/*	On-disk format for HotSpots. */

/*	HOTSPOT_RUN Types */

#define HOTSPOTREC_TYPE_POPUP			1
#define HOTSPOTREC_TYPE_HOTREGION		2
#define HOTSPOTREC_TYPE_BUTTON			3
#define HOTSPOTREC_TYPE_FILE			4
#define HOTSPOTREC_TYPE_SECTION			7
#define HOTSPOTREC_TYPE_ANY				8
#define HOTSPOTREC_TYPE_HOTLINK			11
#define HOTSPOTREC_TYPE_BUNDLE			12
#define HOTSPOTREC_TYPE_V4_SECTION		13
#define HOTSPOTREC_TYPE_SUBFORM			14
#define HOTSPOTREC_TYPE_ACTIVEOBJECT	15
/*	HOTSPOT_RUN Flags */

#define HOTSPOTREC_RUNFLAG_BEGIN		0x00000001L
#define HOTSPOTREC_RUNFLAG_END			0x00000002L
#define HOTSPOTREC_RUNFLAG_BOX			0x00000004L
#define HOTSPOTREC_RUNFLAG_NOBORDER		0x00000008L
#define HOTSPOTREC_RUNFLAG_FORMULA		0x00000010L	/*	Popup is a formula, not text. */
#define HOTSPOTREC_RUNFLAG_MOVIE		0x00000020L /*	File is a QuickTime movie. */
#define HOTSPOTREC_RUNFLAG_IGNORE		0x00000040L /*	Run is for backward compatibility
														(i.e. ignore the run)
													*/
#define HOTSPOTREC_RUNFLAG_ACTION		0x00000080L	/*	Hot region executes a canned action	*/
#define HOTSPOTREC_RUNFLAG_SCRIPT		0x00000100L	/*	Hot region executes a script.	*/
#define HOTSPOTREC_RUNFLAG_INOTES  		0x00001000L
#define HOTSPOTREC_RUNFLAG_ISMAP    	0x00002000L
#define HOTSPOTREC_RUNFLAG_INOTES_AUTO	0x00004000L
#define HOTSPOTREC_RUNFLAG_ISMAP_INPUT	0x00008000L

#define HOTSPOTREC_RUNFLAG_SIGNED  		0x00010000L
#define HOTSPOTREC_RUNFLAG_ANCHOR  		0x00020000L


typedef struct
	{
	WSIG Header;	/* Signature and length of this record */	
	WORD Type;
	DWORD Flags;
	WORD DataLength;
	/*	Data Follows. */
	/*  if HOTSPOTREC_RUNFLAG_SIGNED, WORD SigLen then SigData follows. */
	} CDHOTSPOTBEGIN;

typedef struct
	{
	BSIG Header;	/* Signature and length of this record */	
	} CDHOTSPOTEND;

/*	On-disk flags for CDBAR (Collapsible Sections)	*/

#define BARREC_DISABLED_FOR_NON_EDITORS 	1L
#define BARREC_EXPANDED 					2L
#define BARREC_PREVIEW 						4L

#define BARREC_BORDER_INVISIBLE 			0x1000L
#define BARREC_ISFORMULA					0x2000L
#define BARREC_HIDE_EXPANDED				0x4000L

/* Auto expand/collapse properties.	*/

#define BARREC_AUTO_EXP_READ	0x10L
#define BARREC_AUTO_EXP_PRE		0x20L
#define BARREC_AUTO_EXP_EDIT	0x40L
#define BARREC_AUTO_EXP_PRINT	0x80L

#define BARREC_AUTO_EXP_MASK	0xf0L

#define BARREC_AUTO_COL_READ	0x100L
#define BARREC_AUTO_COL_PRE		0x200L
#define BARREC_AUTO_COL_EDIT	0x400L
#define BARREC_AUTO_COL_PRINT	0x800L
#define BARREC_AUTO_COL_MASK	0xF00L

#define BARREC_AUTO_PRE_MASK	(BARREC_AUTO_COL_PRE | BARREC_AUTO_EXP_PRE)
#define BARREC_AUTO_READ_MASK	(BARREC_AUTO_COL_READ | BARREC_AUTO_EXP_READ)
#define BARREC_AUTO_EDIT_MASK	(BARREC_AUTO_COL_EDIT | BARREC_AUTO_EXP_EDIT)
#define BARREC_AUTO_PRINT_MASK	(BARREC_AUTO_COL_PRINT | BARREC_AUTO_EXP_PRINT)

/*	We will make use (in the code) of the fact that the auto expand/collapse
	flags for editors are simply shifted left twelve bits from the normal
	expand/collapse flags.
*/

#define BARREC_AUTO_ED_SHIFT		12

#define BARREC_AUTO_ED_EXP_READ		0x10000L
#define BARREC_AUTO_ED_EXP_PRE		0x20000L
#define BARREC_AUTO_ED_EXP_EDIT		0x40000L
#define BARREC_AUTO_ED_EXP_PRINT	0x80000L

#define BARREC_AUTO_ED_EXP_MASK		0xf00000L

#define BARREC_AUTO_ED_COL_READ		0x100000L
#define BARREC_AUTO_ED_COL_PRE		0x200000L
#define BARREC_AUTO_ED_COL_EDIT		0x400000L
#define BARREC_AUTO_ED_COL_PRINT	0x800000L
#define BARREC_AUTO_ED_COL_MASK		0xF00000L

#define BARREC_AUTO_ED_PRE_MASK		(BARREC_AUTO_ED_COL_PRE | BARREC_AUTO_ED_EXP_PRE)
#define BARREC_AUTO_ED_READ_MASK	(BARREC_AUTO_ED_COL_READ | BARREC_AUTO_ED_EXP_READ)
#define BARREC_AUTO_ED_EDIT_MASK	(BARREC_AUTO_ED_COL_EDIT | BARREC_AUTO_ED_EXP_EDIT)
#define BARREC_AUTO_ED_PRINT_MASK	(BARREC_AUTO_ED_COL_PRINT | BARREC_AUTO_ED_EXP_PRINT)

#define BARREC_INTENDED				0x1000000L
#define BARREC_HAS_COLOR			0x4000000L

#define BARREC_BORDER_MASK			0x70000000L
#define GETBORDERTYPE(a) ((DWORD)((a) & BARREC_BORDER_MASK) >> 28)
#define SETBORDERTYPE(a,b) a = ((DWORD)((a) & ~BARREC_BORDER_MASK) | ((DWORD)(b) << 28))

#define	BARREC_BORDER_SHADOW		0
#define BARREC_BORDER_NONE			1
#define BARREC_BORDER_SINGLE		2
#define	BARREC_BORDER_DOUBLE		3
#define BARREC_BORDER_TRIPLE		4
#define	BARREC_BORDER_TWOLINE		5

#define BARREC_INDENTED		0x80000000L

/*	Indicate explicitly those bits that we want to save on-disk
	so that we insure that the others are zero when we save to
	disk so that we can use later.
*/
 
#define BARREC_ODS_MASK 0xF4FF6FF7

/*	On-disk format for Object Bars.	*/

#define BARREC_IS_EXPANDED			0x0001
#define BARREC_IS_DISABLED			0x0002

typedef struct
	{
	WSIG	Header;
	DWORD	Flags;
	FONTID	FontID;
	/*	Caption and name follow	*/
	} CDBAR;

/*	On-disk format for Buttons.	*/

#define BUTTONREC_IS_DOWN			0x0002
#define BUTTONREC_IS_EDITABLE		0x0004
#define BUTTONREC_FLAG_CARET_ON		0x0008
#define	BUTTONREC_FLAG_RESIZE_ON	0x0010
#define	BUTTONREC_FLAG_DISABLED		0x0020

typedef struct
	{
	WSIG 	Header;	/* Signature and length of this record. */
	WORD 	Flags;	
	WORD 	Width;
	WORD 	Height;
	WORD	Lines;
	FONTID 	FontID;
	/* Button Text Follows	*/
	} CDBUTTON;

/*	On-disk format for form layout objects */

#define LAYOUT_FLAG_SHOWBORDER		0x00000001
#define LAYOUT_FLAG_SHOWGRID		0x00000002
#define LAYOUT_FLAG_SNAPTOGRID		0x00000004
#define LAYOUT_FLAG_3DSTYLE			0x00000008

typedef struct
	{
	BSIG	Header;
	WORD	wLeft;
	WORD	wWidth;
	WORD	wHeight;
	DWORD	Flags;
	WORD	wGridSize;
	BYTE	Reserved[14];
	} CDLAYOUT;

typedef struct
	{
	WORD	wLeft;
	WORD	wTop;
	WORD	wWidth;
	WORD	wHeight;
	FONTID	FontID;				/* Font ID */
	BYTE	byBackColor;		/*	Background color */
	BYTE	bSpare[7];
	} ELEMENTHEADER;

/*	The following flags must be the same as LAYOUT_FIELD_FLAG_ equiv's. */
#define LAYOUT_TEXT_FLAG_TRANS		0x10000000
#define LAYOUT_TEXT_FLAG_LEFT		0x00000000
#define LAYOUT_TEXT_FLAG_CENTER		0x20000000
#define LAYOUT_TEXT_FLAG_RIGHT		0x40000000
#define LAYOUT_TEXT_FLAG_ALIGN_MASK	0x60000000
#define LAYOUT_TEXT_FLAG_VCENTER	0x80000000
#define LAYOUT_TEXT_FLAGS_MASK		0xF0000000

typedef struct
	{
	BSIG			Header;
	ELEMENTHEADER	ElementHeader;
	DWORD			Flags;
	BYTE			Reserved[16];
	/*	For records save with builds prior to 134 the 8-bit text string follows... */
	} CDLAYOUTTEXT;

#define LAYOUT_FIELD_TYPE_TEXT	0
#define LAYOUT_FIELD_TYPE_CHECK	1
#define LAYOUT_FIELD_TYPE_RADIO	2
#define LAYOUT_FIELD_TYPE_LIST	3
#define LAYOUT_FIELD_TYPE_COMBO	4

#define LAYOUT_FIELD_FLAG_SINGLELINE	0x00000001
#define LAYOUT_FIELD_FLAG_VSCROLL		0x00000002
/*	The following flag must not be sampled by any design mode
	code.  It is, in effect, "write only" for design elements.
	Play mode elements, on the other hand, can rely on its value. */
#define LAYOUT_FIELD_FLAG_MULTISEL		0x00000004
#define LAYOUT_FIELD_FLAG_STATIC		0x00000008
#define LAYOUT_FIELD_FLAG_NOBORDER		0x00000010
#define LAYOUT_FIELD_FLAG_IMAGE			0x00000020
/*	The following flags must be the same as LAYOUT_TEXT_FLAG_ equiv's. */
#define LAYOUT_FIELD_FLAG_TRANS			0x10000000
#define LAYOUT_FIELD_FLAG_LEFT			0x00000000
#define LAYOUT_FIELD_FLAG_CENTER		0x20000000
#define LAYOUT_FIELD_FLAG_RIGHT			0x40000000
#define LAYOUT_FIELD_FLAG_VCENTER		0x80000000

typedef struct
	{
	BSIG			Header;
	ELEMENTHEADER	ElementHeader;
	DWORD			Flags;
	BYTE			bFieldType;
	BYTE			Reserved[15];
	} CDLAYOUTFIELD;

#define LAYOUT_GRAPHIC_FLAG_BUTTON		0x00000001
typedef struct
	{
	BSIG			Header;
	ELEMENTHEADER	ElementHeader;
	DWORD			Flags;
	BYTE			Reserved[16];
	} CDLAYOUTGRAPHIC;

typedef struct
	{
	BSIG			Header;
	ELEMENTHEADER	ElementHeader;
	DWORD			Flags;
	BYTE			Reserved[16];
	} CDLAYOUTBUTTON;

typedef struct
	{
	BSIG	Header;
	BYTE	Reserved[16];
	} CDLAYOUTEND;

#define STORAGE_LINK_TYPE_OBJECT		1
#define STORAGE_LINK_TYPE_NOTE			2
#define STORAGE_LINK_TYPE_URL_CONVERTED	3
#define STORAGE_LINK_TYPE_URL_MIME		4

#define STORAGE_LINK_LOAD_DEFERRED	1
#define STORAGE_LINK_LOAD_ON_DEMAND	2

/*	Structure for externally stored objects */

typedef struct {
	WSIG Header;
	WORD StorageType; 			/* Type of object (Object, Note, URL, etc.) */
	WORD LoadType;				/* How to load (deferred, on demand, etc.) */
	WORD Flags;					/* Currently not used */
	WORD DataLength;			/* Length of data following */
	WORD Reserved[6];			/* Currently not used */
								/* Storage data follows... */
} CDSTORAGELINK;

/* Structure for alternate CD records */
/* by definition these are alternates to what immediately preceeds them */
typedef struct
	{
	WSIG Header;			/* Signature and length of this record  */
	WORD Type;				/* Unused at this time */
	DWORD SequenceNumber;	/* ID/Sequence number should match  what's
							 * in some ACTIVEOBJECT in the doc
							 */
	DWORD Flags;			/* Unused at this time 					*/
	WORD DataLength;		/* Unused at this time 					*/
	/*	Data Follows. */
	} CDALTERNATEBEGIN;

typedef struct
	{
	BSIG Header;		/* Signature and length of this record */
	DWORD Flags;		/* Unused at this time				   */
	} CDALTERNATEEND;


/*	End of Edit Package On Disk Structure Definitions */

#endif /* EDIT_ODS_DEFS */


