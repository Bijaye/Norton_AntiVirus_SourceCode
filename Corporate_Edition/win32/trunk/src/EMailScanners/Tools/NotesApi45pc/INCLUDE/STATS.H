// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif

#ifndef STATS_DEFS
#define STATS_DEFS

/*	Cross-package definitions */

#define	STATPKG_OS "OS"
#define	STATPKG_STATS "Stats"
#define	STATPKG_OSMEM "Mem"
#define	STATPKG_OSSEM "Sem"
#define	STATPKG_OSSPIN "Spin"
#define	STATPKG_OSFILE "Disk"
#define	STATPKG_SERVER "Server"
#define	STATPKG_REPLICA "Replica"
#define	STATPKG_MAIL "Mail"
#define	STATPKG_COMM "Comm"
#define	STATPKG_NSF "Database"
#define	STATPKG_NIF "Database"
#define	STATPKG_TESTNSF "Testnsf"
#define STATPKG_OSIO "IO"
#define STATPKG_NET "NET"
#define STATPKG_OBJSTORE "Object"
#define STATPKG_AGENT "Agent"		/* used by agent manager */
#define STATPKG_WEB "Web"			/* used by Web retriever */
#define STATPKG_CAL	"Calendar"		/* used by schedule manager */

/*	Value type constants */

#define	VT_LONG		0
#define	VT_TEXT		1
#define	VT_TIMEDATE	2
#define	VT_NUMBER	3


/*	Flags for StatUpdate */

#define	ST_UNIQUE	0x0001		/* Statistic is unique */
#define	ST_ADDITIVE	0x0002		/* Add to VT_LONG statistic, don't replace */

/*	Prototypes */

STATUS LNPUBLIC	 StatUpdate (char far *Facility, char far *StatName, WORD Flags, WORD ValueType, void far *Value);
void LNPUBLIC	 StatDelete (char far *Facility, char far *StatName);
STATUS LNPUBLIC	 StatReset (char *Facility, char *StatName);
typedef STATUS (LNCALLBACKPTR STATTRAVERSEPROC)(void far *, char far *, char far *, WORD, void far *);
void LNPUBLIC	 StatTraverse (char far *Facility, char far *StatName, STATTRAVERSEPROC Routine, void far *Context);
void LNPUBLIC	 StatToText(char far *Facility, char far *StatName, WORD ValueType, void far *Value,
							char far *NameBuffer, WORD NameBufferLen,
							char far *ValueBuffer, WORD ValueBufferLen);
STATUS LNPUBLIC	 StatQuery(char far *HeaderString,
							char far *NamePrefix, char far *ValuePrefix,
							char far *LineSuffix,
							HANDLE far *rethStats, DWORD far *retStatsLength);

#endif

#ifdef __cplusplus
}
#endif

