// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

/*	Miscellaneous Package Global Definitions */

#ifndef MISC_DEFS
#define MISC_DEFS

#ifdef __cplusplus
extern "C" {
#endif


/*	Some useful constants */

#define	MAXALPHANUMBER		40
#define	MAXALPHATIMEDATE	80
#define	MAXALPHATIMEDATEPAIR (80 * 2 + 3)
#define MAXSPRINTF			256
#define MAXSOUNDEX			(4+1)		/* 4 chars plus null terminator */

/*	Flags for IntlTextCompare */

#define INTL_ACCENT_SENSITIVE	0x0001
#define INTL_CASE_SENSITIVE		0x0002


/* time structure - NOTE: These fields MUST BE int's, NOT UNSIGNEDS! */

typedef struct {
	int year;					/* 1-32767 */
	int month;					/* 1-12 */
	int day;					/* 1-31 */
	int weekday;				/* 1-7, Sunday is 1 */
	int hour;					/* 0-23 */
	int minute;					/* 0-59 */
	int second;					/* 0-59 */
	int hundredth;				/* 0-99 */
	int dst;					/* FALSE or TRUE */
	int zone;					/* -11 to +11 */
	TIMEDATE GM;
} TIME;

/*	Special encodings for TIMEDATE fields.  Note that these are all in
	HOST-SPECIFIC format, and must be used with TimeConstruct or
	TimeExtractJulianDate or TimeExtractTicks! */

#define ALLDAY 0xffffffffL		/* put this in the TIME field */
#define ANYDAY 0xffffffffL		/* put this in the DATE field */
#define TICKS_IN_DAY 8640000L	/* 10msec ticks in a day */
#define TICKS_IN_HOUR 360000L	/* 10msec ticks in an hour */
#define TICKS_IN_MINUTE 6000L	/* 10msec ticks in a minute */
#define TICKS_IN_SECOND 100L	/* 10msec ticks in a second */
#define SECS_IN_DAY 86400L		/* seconds in a day */


/*	Number Format */

#define	NFMT_GENERAL		0		/* Number Formats */
#define	NFMT_FIXED			1
#define	NFMT_SCIENTIFIC		2
#define	NFMT_CURRENCY		3

#define	NATTR_PUNCTUATED	0x0001	/* Number Attributes */
#define	NATTR_PARENS		0x0002
#define	NATTR_PERCENT		0x0004

typedef struct {
	BYTE Digits;					/* Number of decimal digits */
	BYTE Format;					/* Display Format */
	BYTE Attributes;				/* Display Attributes */
	BYTE Unused;
} NFMT;

/*	Time Format */

#define	TDFMT_FULL			0		/* year, month, and day */
#define	TDFMT_CPARTIAL		1		/* month and day, year if not this year */
#define	TDFMT_PARTIAL		2		/* month and day */
#define	TDFMT_DPARTIAL		3		/* year and month */
#define	TTFMT_FULL			0		/* hour, minute, and second */
#define	TTFMT_PARTIAL		1		/* hour and minute */
#define	TTFMT_HOUR			2		/* hour */
#define	TZFMT_NEVER			0		/* all times converted to THIS zone */
#define	TZFMT_SOMETIMES		1		/* show only when outside this zone */
#define	TZFMT_ALWAYS		2		/* show on all times, regardless */
#define	TSFMT_DATE			0		/* DATE */
#define	TSFMT_TIME			1		/* TIME */
#define	TSFMT_DATETIME		2		/* DATE TIME */
#define	TSFMT_CDATETIME		3		/* DATE TIME or TIME Today or TIME Yesterday */

typedef struct {
	BYTE Date;						/* Date Display Format */
	BYTE Time;						/* Time Display Format */
	BYTE Zone;						/* Time Zone Display Format */
	BYTE Structure;					/* Overall Date/Time Structure */
} TFMT;


/* function templates */

BOOL LNPUBLIC		TimeLocalToGM (TIME far *Time);
BOOL LNPUBLIC		TimeGMToLocal (TIME far *Time);
BOOL LNPUBLIC		TimeGMToLocalZone (TIME far *Time);
int LNPUBLIC		TimeDateCompare (TIMEDATE far *t1, TIMEDATE far *t2);
int LNPUBLIC		TimeDateCollate (TIMEDATE far *t1, TIMEDATE far *t2);
DWORD LNPUBLIC		TimeExtractJulianDate (TIMEDATE far *);
DWORD LNPUBLIC		TimeExtractDate (TIMEDATE far *);
DWORD LNPUBLIC		TimeExtractTicks (TIMEDATE far *);
void LNPUBLIC		TimeExtractLocal (TIMEDATE far *Time, BOOL fTime, TIMEDATE far *retTime);
void LNPUBLIC		TimeConstruct (DWORD Date, DWORD Time, TIMEDATE far *result);


#define	TIMEDATE_MINIMUM	0
#define	TIMEDATE_MAXIMUM	1
#define	TIMEDATE_WILDCARD	2
void LNPUBLIC		TimeConstant(WORD, TIMEDATE far *);

#define TimeDateEqual(a,b) ((BOOL)TimeDateCollate(a,b)==0)
#define TimeDateClear(a) (TimeConstant(TIMEDATE_MINIMUM, a))
STATUS LNPUBLIC		TimeDateIncrement (TIMEDATE far *Time, LONG Interval);
LONG LNPUBLIC		TimeDateDifference (TIMEDATE far *t1, TIMEDATE far *t2);
void LNPUBLIC		TimeDateDifferenceFloat (TIMEDATE far *t1,
						TIMEDATE far *t2, NUMBER far *difference);
BOOL LNPUBLIC		TimeDateAdjust (TIMEDATE far *Time, int seconds, int minutes, int hours, int days, int months, int years);
int LNPUBLIC		IntlTextCompare(void far *Str1, WORD Str1Len,
									void far *Str2, WORD Str2Len, DWORD Flags);
STATUS LNPUBLIC		ConvertTextToTIMEDATE (void far *IntlFormat,
										TFMT far *TextFormat,
										char far * far *Text,
										WORD MaxLength,
										TIMEDATE far *retTIMEDATE);
STATUS LNPUBLIC		ConvertTextToTIMEDATEPAIR (void far *IntlFormat,
										TFMT far *TextFormat,
										char far * far *Text,
										WORD MaxLength,
										TIMEDATE_PAIR far *retTIMEDATE);
STATUS LNPUBLIC		ConvertTIMEDATEToText (void far *IntlFormat,
										TFMT far *TextFormat,
										TIMEDATE far *InputTime,
										char far *retTextBuffer,
										WORD TextBufferLength,
										WORD far *retTextLength);
STATUS LNPUBLIC		ConvertTIMEDATEPAIRToText (void far *IntlFormat,
										TFMT far *TextFormat,
										TIMEDATE_PAIR far *InputTime,
										char far *retTextBuffer,
										WORD TextBufferLength,
										WORD far *retTextLength);
STATUS LNPUBLIC		ConvertTextToFLOAT (void far *IntlFormat,
										NFMT far *NumberFormat,
										char far * far *ppInputText,
										WORD InputTextMaxLength,
										NUMBER far *retNumber);
STATUS LNPUBLIC		ConvertFLOATToText (void far *IntlFormat,
										NFMT far *NumberFormat,
										NUMBER far *Number,
										char far *retTextBuffer,
										WORD TextBufferLength,
										WORD far *retTextLength);
void LNPUBLIC Soundex (char far *String, WORD StringLength, char far *retSoundexString);
STATUS LNPUBLIC CLSRead(char far *CLSFileName, HANDLE far *rethCLSTable, DWORD far *retCLSTableSize);

/* Abstract */

/* chunk separator parameters */
#define ABS_CHUNKBEGIN "ChunkBegin="
#define ABS_CHUNKSEP "ChunkSep="
#define ABS_CHUNKEND "ChunkEnd="

/* abstraction commands */
#define ABS_TEXTONLY		"textonly "
#define ABS_COUNTWORDS		"countwords "
#define ABS_SAVE			"save "
#define ABS_RESTORE			"restore "
#define ABS_TRYFIT			"tryfit "
#define ABS_ABBREV			"abbrev "
#define ABS_SORTCHUNKS		"sortchunks "
#define ABS_NOSTOPLIST		"nostoplist "
#define ABS_NOSIGLIST		"nosiglist "
#define ABS_USEDICT			"ab-usedict "
#define ABS_NODICT			"ab-usedict=0 "
#define ABS_DROPVOWELS		"ab-dropvowels "
#define ABS_KEEPVOWELS		"ab-dropvowels=0 "
#define ABS_TRIMWHITE		"ab-trimwhite "
#define ABS_NOTRIMWHITE		"ab-trimwhite=0 "
#define ABS_TRIMPUNCT		"ab-trimpunct "
#define ABS_NOTRIMPUNCT		"ab-trimpunct=0 "
#define ABS_DROPFIRSTVOWEL	"ab-dropfirstvowels "
#define ABS_KEEPFIRSTVOWEL	"ab-dropfirstvowels=0 "


STATUS LNPUBLIC Abstract(char far *szKeywords,
						char far *szText, 
						DWORD maxAbstract,
						char far *szAbstract,
						DWORD far *retSize);


#ifdef __cplusplus
}
#endif

#endif /* MISC_DEFS */

