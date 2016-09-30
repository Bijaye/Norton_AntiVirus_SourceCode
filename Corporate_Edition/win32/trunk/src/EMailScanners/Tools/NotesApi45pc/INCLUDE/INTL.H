// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif

#ifndef INTL_DEFS
#define INTL_DEFS

/*	International Environment Parameter Definitions */

#define	CURRENCY_SUFFIX 	0x0001
#define	CURRENCY_SPACE		0x0002
#define	NUMBER_LEADING_ZERO	0x0004
#define	CLOCK_24_HOUR		0x0008
#define	DAYLIGHT_SAVINGS	0x0010
#define	DATE_MDY			0x0020
#define	DATE_DMY			0x0040
#define	DATE_YMD			0x0080

#define	ISTRMAX 5
#define	YTSTRMAX 32

typedef struct {
	WORD Flags;							/* Flags (see above) */
	BYTE CurrencyDigits;
	BYTE Length;						/* Length of this structure */
										/* THIS MUST BE SET TO THE EXACT */
										/* SIZE OF THE STRUCTURE WHEN ITS */
										/* POINTER IS PASSED AS AN ARGUMENT */
	int	 TimeZone;
	char AMString[ISTRMAX];
	char PMString[ISTRMAX];
	char CurrencyString[ISTRMAX];
	char ThousandString[ISTRMAX];
	char DecimalString[ISTRMAX];
	char DateString[ISTRMAX];
	char TimeString[ISTRMAX];
	char YesterdayString[YTSTRMAX];
	char TodayString[YTSTRMAX];
	char TomorrowString[YTSTRMAX];
} INTLFORMAT;

void	LNPUBLIC OSGetIntlSettings(INTLFORMAT far *retIntlFormat,
									WORD BufferSize);


/*	End of International Environment Parameter Definitions */

#endif

#ifdef __cplusplus
}
#endif

