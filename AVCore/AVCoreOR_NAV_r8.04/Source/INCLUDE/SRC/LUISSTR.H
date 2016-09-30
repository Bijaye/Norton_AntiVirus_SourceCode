// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/LUISSTR.H_v   1.0   21 Aug 1997 14:25:42   Ray  $
//
// Description:
//
//  This file contains the global string externs
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/LUISSTR.H_v  $
// 
//    Rev 1.0   21 Aug 1997 14:25:42   Ray
// Initial revision.
// 
//    Rev 1.1   19 May 1997 12:42:24   RAY
// Added strings
// 
//    Rev 1.0   07 May 1997 02:43:30   rpulint
// Initial revision.
// 
//
//************************************************************************

//  --IMPORTANT--READ--
//
//  This file is used in conjunction with LUISSTR.RC & LUIS.LIB
//  You need to do the following:
//  (1) Make sure you include both luisstr.rc and luisstr.h in your project.
//      (Obviously RC cannot be included in LUIS.LIB, that's the reason)
//  (2) Make sure the string ID values used in your RC file DO NOT conflict with
//      the ones used in the above mentioned files. (Range - 30001-13XXX)
//  (3) You will need to declare the following as a global variable :
//      char         **gszLUISStrings;  // array of pointers to all strings in LUIS project
//  (4) Call  LUISStringInit(); to initialize this global list of strings.
//  (5) Make sure you've done steps 1-4 :)
//

#ifndef _LUISSTR_H_

#define _LUISSTR_H_

//  LUIS_STRING_FIRST is the lowest string resource number in LUISSTR.RC
//  LUIS_STRING_LAST is the hightest string resource number in LUISSTR.RC
//              this must be the same value as the last string resource ID
#define LUIS_STRING_FIRST                          30001
#define LUIS_STRING_LAST                           30016

#if !defined(DEFINE_STRINGS)
extern char         **gszLUISStrings;       // array of pointers to all strings
#endif

//  The following are the string resource IDs used by the LUISSTR.RC file
//  and by the LUIS*.CPP source code to index into the gszLUISStrings[] array.
//  The #defines following each resource ID are used by the code for easy
//  readability.  Also, this way we don't have to change *any* of the code when
//  we move around the resources, just their IDs ... and then recompile.

#define LUIS_STR_REQCONTENTLENGTH		30001
#define gszLuisReqContentLength			gszLUISStrings[LUIS_STR_REQCONTENTLENGTH - LUIS_STRING_FIRST]

#define LUIS_STR_REFERRINGPAGE   		30002
#define gszLuisReferringPage			gszLUISStrings[LUIS_STR_REFERRINGPAGE - LUIS_STRING_FIRST]

#define LUIS_STR_NOTFOUND       		30003
#define gszLuisNotFound     			gszLUISStrings[LUIS_STR_NOTFOUND - LUIS_STRING_FIRST]

#define LUIS_STR_CANNOTLISTEN   		30004
#define gszLuisCannotListen             gszLUISStrings[LUIS_STR_CANNOTLISTEN - LUIS_STRING_FIRST]

#define LUIS_STR_VIEWOUTSIDERANGE		30005
#define gszLuisViewOutsideRange			gszLUISStrings[LUIS_STR_VIEWOUTSIDERANGE - LUIS_STRING_FIRST]

#define LUIS_STR_VIEWWRONGPSWD  		30006
#define gszLuisViewWrongPassword		gszLUISStrings[LUIS_STR_VIEWWRONGPSWD - LUIS_STRING_FIRST]

#define LUIS_STR_MODIFYOUTSIDERANGE		30007
#define gszLuisModifyOutsideRange		gszLUISStrings[LUIS_STR_MODIFYOUTSIDERANGE - LUIS_STRING_FIRST]

#define LUIS_STR_MODIFYWRONGPSWD		30008
#define gszLuisModifyWrongPassword		gszLUISStrings[LUIS_STR_MODIFYWRONGPSWD - LUIS_STRING_FIRST]

#define LUIS_STR_BADHTTPREQUEST 		30009
#define gszLuisBadHTTPHeader			gszLUISStrings[LUIS_STR_BADHTTPREQUEST - LUIS_STRING_FIRST]

#define LUIS_STR_CANNOTSTARTREQHANDLER	30010
#define gszLuisCannotStartReqHandler	gszLUISStrings[LUIS_STR_CANNOTSTARTREQHANDLER - LUIS_STRING_FIRST]

#define LUIS_STR_NOTENOUGHMEMORY		30011
#define gszLuisNotEnoughMemory			gszLUISStrings[LUIS_STR_NOTENOUGHMEMORY - LUIS_STRING_FIRST]

#define LUIS_STR_OUTOFMEMORY    		30012
#define gszLuisOutOfMemory  			gszLUISStrings[LUIS_STR_OUTOFMEMORY - LUIS_STRING_FIRST]

#define LUIS_STR_UNKNOWNERROR   		30013
#define gszLuisUnknownError				gszLUISStrings[LUIS_STR_UNKNOWNERROR - LUIS_STRING_FIRST]

#define LUIS_STR_URLNOTFOUND    		30014
#define gszLuisURLNotFound  			gszLUISStrings[LUIS_STR_URLNOTFOUND - LUIS_STRING_FIRST]

#define LUIS_STR_WINSOCKERROR   		30015
#define gszLuisWinsockError 			gszLUISStrings[LUIS_STR_WINSOCKERROR - LUIS_STRING_FIRST]

#define LUIS_STR_AUTHORIZATION          30016
#define gszLuisAuthorization            gszLUISStrings[LUIS_STR_AUTHORIZATION - LUIS_STRING_FIRST]

// this is the resource ID of the error message string - it is not included in
// the list because it is *only* loaded in the event of an error.

#define LUIS_STR_ERROR_ACCESSSTRING         ( LUIS_STRING_LAST + 1)
#define gszLuisErrorAccessstring              gszLUISStrings[LUIS_STR_ERROR_ACCESSSTRING  - LUIS_STRING_FIRST]

// function prototypes

BOOL         LUISStringInit(void);

#endif // #ifndef _LUISSTR_H_
