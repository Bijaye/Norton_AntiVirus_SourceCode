// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef _REGEDIT_h_
#define _REGEDIT_h_


#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <conio.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <errno.h>
#include <ctype.h>

#ifndef NLM   // gdf CW conversion 
#include <share.h>
#endif  // gdf CW conversion 

#include "nlm.h"

#include "ACTA.h"
#include "nwreg.h"
#include "vpcommon.h"
#include "nlmui.h"


#define nSUBKEYS	128				// Max number of subkeys for a particular key
#define nVALUES		128				// Max number of values for a particular key



#define CURR parentKey[openKey]		// the currently open key

// the following are used in RegCommandMenu
#define NUM_ADD_KEY			1
#define NUM_ADD_VALUE		2
#define NUM_DELETE_KEY		3
#define NUM_DELETE_VALUE	4
#define NUM_OPEN_SUBKEY		5
#define NUM_CLOSE_KEY		6
#define NUM_SHOW_DATA		7
#define NUM_EDIT_DATA		8
#define NUM_QUIT_PROG		9

#define CMD_ESCAPE			255

#define LEFT		2				// The left text margin
#define RIGHT		79				// The right margin
#define MIDDLE		40				// The middle of the screen

#define KEYPATHY	1				// row where the key path is printed
#define SUBKEYY		2				// row where subkeys and values begin printing
#define COMY		20				// row where commands begin listing
#define STATY		23				// row where status is printed
#define PROMPTY		23				// row where question prompts are printed

#define KEYPATH_XY	LEFT,KEYPATHY		// location where key path is printed
#define SUBKEY_XY 	LEFT,SUBKEYY		// Location where subkeys begin printing
#define VALUE_XY 	MIDDLE+1,SUBKEYY	// location where values begin printing
#define COM_XY		LEFT,COMY			// location where commands start printing
#define STAT_XY		LEFT,STATY			// location where status is printed
#define PROMPT_XY 	LEFT,PROMPTY		// location where question prompts are printed
#define PROMPT_YX	PROMPTY,LEFT		// location of prompt in row/column order

#define KEYLEN		128				// Max length of Key names
#define VALLEN		128				// Max length of Value names
#define DATALEN		256				// Max length of value data
#define KEYPATHLEN	32				// Max depth of key path

// The following are ASCII values returned by getch() for the various items
#define RETURN 		13
#define BACKSPACE 	8
#define ZERO 		0
#define DELETE 		83
#define RIGHT_ARROW 77
#define CTRL_RIGHT	116
#define LEFT_ARROW 	75
#define CTRL_LEFT	115
#define END			79
#define HOME		71

typedef struct ValueStruct {
	struct ValueStruct* next;
	DWORD type;
	char name[VALLEN];
} Value;

#endif
