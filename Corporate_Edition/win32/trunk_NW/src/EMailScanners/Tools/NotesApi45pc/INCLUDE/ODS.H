// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif


/* On-Disk Structure Definitions */

#ifndef ODS_DEFS
#define ODS_DEFS

#ifndef POOL_DEFS
#include "pool.h"				/* Get BLOCKID definition */
#endif



/* structures used to define and read the signatures 

		 0		   1
	+---------+---------+
	|   Sig   |  Length	|						Byte signature
	+---------+---------+

		 0		   1        2         3
	+---------+---------+---------+---------+
	|   Sig   |   ff	|		Length		|	Word signature
	+---------+---------+---------+---------+

		 0		   1        2         3          4         5
	+---------+---------+---------+---------+---------+---------+
	|   Sig   |   00	|                 Length				| DWord signature
	+---------+---------+---------+---------+---------+---------+

*/

#define	LONGRECORDLENGTH 0x0000
#define	WORDRECORDLENGTH 0xff00
#define BYTERECORDLENGTH 0		/* High byte contains record length */

typedef struct {
	WORD Signature;				/* ORed with DWORDRECORDLENGTH */
	DWORD Length;				/* (length is inclusive with this struct) */
} LSIG;

typedef struct {
	WORD Signature;				/* ORed with WORDRECORDLENGTH */
	WORD Length;				/* (length is inclusive with this struct) */
} WSIG;

typedef struct {
	BYTE Signature;				/* Signature */
	BYTE Length;				/* (length is inclusive with this struct) */
} BSIG;


/* Base ODS types */

#define	_SHORT					0
#define	_USHORT					_SHORT
#define	_WORD					_SHORT
#define	_BOOL					_SHORT
#define	_STATUS					_SHORT
#define	_UNICODE				_SHORT
#define	_LONG					1
#define _FLOAT 					2
#define	_DWORD					_LONG
#define _ULONG 					_LONG

/* ODS types that are the size of a base type */

#define	_NUMBER					_FLOAT
#define	_NOTEID					_LONG

/* Include the rest of the ODS types from odstypes.h */

#if defined(GCC) && defined(SUN)
#define odsmacro(name, num, size) enum{_/*##*/name = num};
#else
#define odsmacro(name, num, size) enum{_##name = num};
#endif
#include "odstypes.h"
#undef odsmacro

/*	All records begin with a signature word that indicates its type
	and record length.  The low order byte is the type, and the high
	order byte is the length.  If the indicated length is 0, then the
	next LONG word contains the record length.  If the indicated length
	is 0xff, the next WORD contains the record length.  Record lengths
	always INCLUDE the header portion of the record. */

#define SIG_INVALID				0


/* Signatures for Composite Records in items of data type COMPOSITE */

#define	SIG_CD_PARAGRAPH		(129 | BYTERECORDLENGTH)
#define	SIG_CD_PABDEFINITION	(130 | WORDRECORDLENGTH)
#define	SIG_CD_PABREFERENCE		(131 | BYTERECORDLENGTH)
#define SIG_CD_TEXT				(133 | WORDRECORDLENGTH)
#define	SIG_CD_HEADER			(142 | WORDRECORDLENGTH)
#define	SIG_CD_LINKEXPORT2		(146 | WORDRECORDLENGTH)
#define SIG_CD_BITMAPHEADER		(149 | LONGRECORDLENGTH)
#define SIG_CD_BITMAPSEGMENT    (150 | LONGRECORDLENGTH)
#define SIG_CD_COLORTABLE	    (151 | LONGRECORDLENGTH)
#define SIG_CD_GRAPHIC		    (153 | LONGRECORDLENGTH)
#define SIG_CD_PMMETASEG	    (154 | LONGRECORDLENGTH)
#define SIG_CD_WINMETASEG	    (155 | LONGRECORDLENGTH)
#define SIG_CD_MACMETASEG	    (156 | LONGRECORDLENGTH)
#define SIG_CD_CGMMETA		    (157 | LONGRECORDLENGTH)
#define SIG_CD_PMMETAHEADER	    (158 | LONGRECORDLENGTH)
#define SIG_CD_WINMETAHEADER    (159 | LONGRECORDLENGTH)
#define SIG_CD_MACMETAHEADER    (160 | LONGRECORDLENGTH)
#define SIG_CD_TABLEBEGIN		(163 | BYTERECORDLENGTH)
#define SIG_CD_TABLECELL		(164 | BYTERECORDLENGTH)
#define SIG_CD_TABLEEND			(165 | BYTERECORDLENGTH)
#define SIG_CD_STYLENAME		(166 | BYTERECORDLENGTH)
#define SIG_CD_STORAGELINK		(196 | WORDRECORDLENGTH)
#define SIG_CD_TRANSPARENTTABLE (197 | LONGRECORDLENGTH)

/* Signatures for Composite Records that are reserved internal records, */
/* whose format may change between releases. */

#define	SIG_CD_DOCUMENT_PRE_26	(128 | BYTERECORDLENGTH)
#define	SIG_CD_FIELD_PRE_36		(132 | WORDRECORDLENGTH)
#define	SIG_CD_FIELD			(138 | WORDRECORDLENGTH)
#define	SIG_CD_DOCUMENT			(134 | BYTERECORDLENGTH)
#define	SIG_CD_METAFILE			(135 | WORDRECORDLENGTH)
#define	SIG_CD_BITMAP			(136 | WORDRECORDLENGTH)
#define	SIG_CD_FONTTABLE		(139 | WORDRECORDLENGTH)
#define	SIG_CD_LINK				(140 | BYTERECORDLENGTH)
#define	SIG_CD_LINKEXPORT		(141 | BYTERECORDLENGTH)
#define	SIG_CD_KEYWORD			(143 | WORDRECORDLENGTH)
#define	SIG_CD_LINK2			(145 | WORDRECORDLENGTH)
#define	SIG_CD_CGM				(147 | WORDRECORDLENGTH)
#define	SIG_CD_TIFF				(148 | LONGRECORDLENGTH)
#define SIG_CD_PATTERNTABLE	    (152 | LONGRECORDLENGTH)
#define SIG_CD_DDEBEGIN			(161 | WORDRECORDLENGTH)
#define SIG_CD_DDEEND			(162 | WORDRECORDLENGTH)
#define SIG_CD_OLEBEGIN			(167 | WORDRECORDLENGTH)
#define SIG_CD_OLEEND			(168 | WORDRECORDLENGTH)
#define SIG_CD_HOTSPOTBEGIN		(169 | WORDRECORDLENGTH)
#define SIG_CD_HOTSPOTEND		(170 | BYTERECORDLENGTH)
#define SIG_CD_BUTTON			(171 | WORDRECORDLENGTH)
#define SIG_CD_BAR				(172 | WORDRECORDLENGTH)
#define SIG_CD_V4HOTSPOTBEGIN	(173 | WORDRECORDLENGTH)
#define SIG_CD_V4HOTSPOTEND		(174 | BYTERECORDLENGTH)
#define SIG_CD_EXT_FIELD		(176 | WORDRECORDLENGTH)
#define SIG_CD_LAYOUT			(183 | BYTERECORDLENGTH)
#define SIG_CD_LAYOUTTEXT		(184 | BYTERECORDLENGTH)
#define SIG_CD_LAYOUTEND		(185 | BYTERECORDLENGTH)
#define SIG_CD_LAYOUTFIELD		(186 | BYTERECORDLENGTH)
#define SIG_CD_PABHIDE			(187 | WORDRECORDLENGTH)
#define SIG_CD_PABFORMREF		(188 | BYTERECORDLENGTH)
#define SIG_CD_ACTIONBAR		(189 | BYTERECORDLENGTH)
#define SIG_CD_ACTION			(190 | WORDRECORDLENGTH)

#define SIG_CD_DOCAUTOLAUNCH	(191 | WORDRECORDLENGTH)
#define SIG_CD_LAYOUTGRAPHIC	(192 | BYTERECORDLENGTH)
#define SIG_CD_OLEOBJINFO		(193 | WORDRECORDLENGTH)
#define SIG_CD_LAYOUTBUTTON		(194 | BYTERECORDLENGTH)
#define SIG_CD_TEXTEFFECT		(195 | WORDRECORDLENGTH)

/*	Saved Query records for items of type TYPE_QUERY */

#define SIG_QUERY_HEADER		(129 | BYTERECORDLENGTH)
#define SIG_QUERY_TEXTTERM		(130 | WORDRECORDLENGTH)
#define SIG_QUERY_BYFIELD		(131 | WORDRECORDLENGTH)
#define SIG_QUERY_BYDATE		(132 | WORDRECORDLENGTH)
#define SIG_QUERY_BYAUTHOR		(133 | WORDRECORDLENGTH)
#define SIG_QUERY_FORMULA		(134 | WORDRECORDLENGTH)
#define SIG_QUERY_BYFORM		(135 | WORDRECORDLENGTH)
#define SIG_QUERY_BYFOLDER		(136 | WORDRECORDLENGTH)
#define SIG_QUERY_USESFORM		(137 | WORDRECORDLENGTH)
#define SIG_QUERY_TOPIC			(138 | WORDRECORDLENGTH)

/*	Save Action records for items of type TYPE_ACTION */

#define SIG_ACTION_HEADER		(129 | BYTERECORDLENGTH)
#define SIG_ACTION_MODIFYFIELD	(130 | WORDRECORDLENGTH)
#define SIG_ACTION_REPLY		(131 | WORDRECORDLENGTH)
#define SIG_ACTION_FORMULA		(132 | WORDRECORDLENGTH)
#define SIG_ACTION_LOTUSSCRIPT	(133 | WORDRECORDLENGTH)
#define SIG_ACTION_SENDMAIL		(134 | WORDRECORDLENGTH)
#define SIG_ACTION_DBCOPY		(135 | WORDRECORDLENGTH)
#define SIG_ACTION_DELETE		(136 | BYTERECORDLENGTH)
#define SIG_ACTION_BYFORM		(137 | WORDRECORDLENGTH)
#define SIG_ACTION_MARKREAD		(138 | BYTERECORDLENGTH)
#define SIG_ACTION_MARKUNREAD	(139 | BYTERECORDLENGTH)
#define SIG_ACTION_MOVETOFOLDER	(140 | WORDRECORDLENGTH)
#define SIG_ACTION_COPYTOFOLDER	(141 | WORDRECORDLENGTH)
#define SIG_ACTION_REMOVEFROMFOLDER	(142 | WORDRECORDLENGTH)
#define SIG_ACTION_NEWSLETTER	(143 | WORDRECORDLENGTH)
#define SIG_ACTION_RUNAGENT		(144 | WORDRECORDLENGTH)
#define SIG_ACTION_SENDDOCUMENT	(145 | BYTERECORDLENGTH)
#define SIG_ACTION_FORMULAONLY	(146 | WORDRECORDLENGTH)


/* Signatures for items of type TYPE_VIEWMAP_DATASET */

#define SIG_VIEWMAP_DATASET	(87 | WORDRECORDLENGTH)

/* Signatures for items of type TYPE_VIEWMAP */

#define SIG_CD_VMHEADER			(175 | BYTERECORDLENGTH)
#define SIG_CD_VMBITMAP			(176 | BYTERECORDLENGTH)
#define SIG_CD_VMRECT			(177 | BYTERECORDLENGTH)
#define SIG_CD_VMPOLYGON_BYTE	(178 | BYTERECORDLENGTH)
#define SIG_CD_VMPOLYLINE_BYTE	(179 | BYTERECORDLENGTH)
#define SIG_CD_VMREGION			(180 | BYTERECORDLENGTH)
#define SIG_CD_VMACTION			(181 | BYTERECORDLENGTH)
#define SIG_CD_VMELLIPSE		(182 | BYTERECORDLENGTH)
#define SIG_CD_VMRNDRECT		(184 | BYTERECORDLENGTH)
#define SIG_CD_VMBUTTON			(185 | BYTERECORDLENGTH)
#define SIG_CD_VMACTION_2		(186 | WORDRECORDLENGTH)
#define SIG_CD_VMTEXTBOX		(187 | WORDRECORDLENGTH)
#define SIG_CD_VMPOLYGON 		(188 | WORDRECORDLENGTH)
#define SIG_CD_VMPOLYLINE		(189 | WORDRECORDLENGTH)
#define SIG_CD_VMPOLYRGN		(190 | WORDRECORDLENGTH)
/* 191 is available */
#define SIG_CD_VMPOLYRGN_BYTE	(192 | BYTERECORDLENGTH)

void LNPUBLIC		ODSReadMemory(void far *ppSrc, WORD type, void far *pDest, WORD iterations);
void LNPUBLIC		ODSWriteMemory(void far *ppDest, WORD type, void far *pSrc, WORD iterations);
WORD LNPUBLIC		ODSLength(WORD type);

/* Paragraph delimiter flags for ConvertItemToComposite */

#define PARADELIM_BLANKLINE		0x01	/* New par on each blank line */
#define PARADELIM_INDENTEDLINE	0x02	/* New par on any indented line */
#define PARADELIM_ANYBLANKLINE	0x04	/* New par on each blank line, hard CRs on each line */
										/* Note: ANYBLANKLINE sets justification = None */

#define PARADELIM_ANYLINE		0x08	/* New par on each line */
#define PARADELIM_COLON			0x10	/* New par on fieldname: text */

STATUS LNPUBLIC	ConvertItemToText (BLOCKID ItemValue, DWORD ItemValueLength,
								char far *LineDelimiter,
								WORD CharsPerLine,
								HANDLE far *rethBuffer,
								DWORD far *retBufferLength,
								BOOL fStripTabs);

STATUS LNPUBLIC	ConvertItemToComposite (BLOCKID ItemValue, DWORD ItemValueLength,
								FONTID CompositeFont,
								char far *LineDelimiter,
								WORD ParaDelimiterFlags,
								HANDLE far *rethBuffer,
								DWORD far *retBufferLength,
								BOOL fTranslate, 
								char far *TranslateFile,
								int CDFileFD,
								BOOL fAppendToFile);

typedef STATUS	(LNCALLBACKPTR ActionRoutinePtr)(char far *, WORD, DWORD, void far *);

STATUS LNPUBLIC EnumCompositeBuffer(	BLOCKID	ItemValue,
										DWORD	ItemValueLength,
										ActionRoutinePtr ActionRoutine,
										void far *vContext);

STATUS LNPUBLIC EnumCompositeFile(char far *Filename, STATUS (LNCALLBACKPTR ActionRoutine)(char far *, WORD, DWORD, void far *), void far *);


/* End of On-Disk Structure Definitions */

#endif


#ifdef __cplusplus
}
#endif




