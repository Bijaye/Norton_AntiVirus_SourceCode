// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/****************************************************************************

    FILE:       lapiplat.h

    PURPOSE:    platform-specific macro definitions for toolkit samples

    DESCRIPTION:
    Notes API Toolkit version definitions and macro definitions for Notes API
    Toolkit samples to allow platform-independent sample code.  The following
    macros must be defined for each platform:

        LAPI_MAIN
        LAPI_INIT(status)
        LAPI_INIT_ERROR
        LAPI_RETURN(status)
        LAPI_DUMP_ERROR(error)

****************************************************************************/

#ifndef LAPIPLAT_DEFS
#define LAPIPLAT_DEFS

#ifdef __cplusplus
extern "C" {
#endif

#define NOTESAPI_VERSION   "4.5"
#define NOTESAPI_MAJOR_VERSION 4
#define NOTESAPI_MINOR_VERSION 5

#ifndef GLOBAL_DEFS
#include <global.h>
#endif

#ifndef OSMISC_DEFS
#include <osmisc.h>
#endif

#if defined(MAC)
#define LPSTR char *
#endif

/****************************************************************************
*
*       LAPI_MAIN
*
****************************************************************************/

#if (defined(DOS) && !defined(W)) || defined(MAC)
/*
 *  QuickWindows, Macintosh
 */
#define LAPI_MAIN  void main(int argc, char *argv[])

#else
/*
 *  DEFAULT
 */
#define LAPI_MAIN  STATUS LNPUBLIC NotesMain(int argc, char far *argv[])

#endif

/****************************************************************************
*
*       LAPI_INIT
*
****************************************************************************/

#if (defined(DOS) && !defined(W))
/*
 *  QuickWindows
 *
 *  1) Call NotesInitExtended() returning the errorcode
 *  2) Set the display buffer to as large as memory will allow
 */
#define LAPI_INIT(x)  {x = NotesInitExtended(argc,argv); \
                       if (!x) _wsetscreenbuf(_wgetfocus(),_WINBUFINF);}

#elif defined(MAC)
/*
 *  Macintosh
 *
 *  1) Do some basic Macintosh Toolbox initialization
 *  2) Maximize the stack and the heap zone
 *  3) Allocate additional master pointers
 *  4) Intialize the Library Manager; on error exit application
 *  5) Initialize Notes
*/
#ifdef __PPCC__
#define LAPI_INIT(x)  {InitGraf(&thePort); InitFonts();\
                       InitWindows(); InitDialogs(0);\
                       SetApplLimit(GetApplLimit()-8192); MaxApplZone();\
                       MoreMasters(); MoreMasters(); MoreMasters();\
                       if (NotesInitExtended == NULL)\
                         x = (STATUS)TRUE;\
                       else\
                         x = NotesInitExtended( argc, argv );}
#else
#define LAPI_INIT(x)  {InitGraf(&thePort); InitFonts();\
                       InitWindows(); InitDialogs(0);\
                       SetApplLimit(GetApplLimit()-8192); MaxApplZone();\
                       MoreMasters(); MoreMasters(); MoreMasters();\
                       if (InitLibraryManager(0,kCurrentZone,kNormalMemory))\
                         ExitToShell();\
                       LoadFunctionSet("nOTE$NotesLib", FALSE);\
                       x = NotesInitExtended( argc, argv );}
#endif

#else
/*
 *  DEFAULT
 */
#define LAPI_INIT(x)

#endif

/****************************************************************************
*
*       LAPI_INIT_ERROR
*
****************************************************************************/

#if (defined(DOS) && !defined(W))
/*
 *  QuickWindows
 *
 *  NotesInitExtended failed - print error message and return
 */
#define LAPI_INIT_ERROR {printf("\nUnable to initialize Notes\n");\
                         return;}

#elif defined(MAC)
/*
 *  Macintosh
 *
 *  NotesInitExtended failed 
 *  1) Print error message 
 *  2) Function set unloading and Library Manager Cleanup 
 */
#ifdef __PPCC__
#define LAPI_INIT_ERROR {printf("\nUnable to initialize Notes\n");\
                         return;}
#else
#define LAPI_INIT_ERROR {printf("\nUnable to initialize Notes\n");\
                         UnloadFunctionSet("nOTE$NotesLib");\
                         CleanupLibraryManager();\
                         return;}
#endif

#else
/*
 *  DEFAULT
 */
#define LAPI_INIT_ERROR

#endif

/****************************************************************************
*
*       LAPI_RETURN
*
****************************************************************************/

#if (defined(DOS) && !defined(W))
/*
 *  QuickWindows
 *
 *  1) If error code exists, then dump text to stdout.
 *  2) Call NotesTerm().
 */
#define LAPI_RETURN(x)  {if (x) LAPI_DUMP_ERROR(x);NotesTerm();return;}

#elif defined(MAC)
/*
 *  Macintosh
 *
 *  1) Dump error text to stdout.
 *  2) Call NotesTerm().
 *  3) Library Manager Cleanup and Function set unloading.
 *  4) Symatec Project Manager version of SIOW does not prompt user to quit.
 *     It quits automatically unless we do something like create a loop.
 */
#if defined(MAC_SPM) || defined(THINK_C)

#ifdef __PPCC__
#define LAPI_RETURN(x)  {if (x) LAPI_DUMP_ERROR(x);\
                         NotesTerm();\
                         printf("\n\n\n*** click mouse button to quit ***\n"); while ( ! Button() );\
                         return;}
#else
#define LAPI_RETURN(x)  {if (x) LAPI_DUMP_ERROR(x);\
                         NotesTerm();\
                         UnloadFunctionSet("nOTE$NotesLib");\
                         CleanupLibraryManager();\
                         printf("\n\n\n*** click mouse button to quit ***\n"); while ( ! Button() );\
                         return;}
#endif

#else

#ifdef __PPCC__
#define LAPI_RETURN(x)  {if (x) LAPI_DUMP_ERROR(x);\
                         NotesTerm();\
                         return;}
#else
#define LAPI_RETURN(x)  {if (x) LAPI_DUMP_ERROR(x);\
                         NotesTerm();\
                         UnloadFunctionSet("nOTE$NotesLib");\
                         CleanupLibraryManager();\
                         return;}
#endif

#endif

#else
/*
 *  DEFAULT
 */
#define LAPI_RETURN(x)   return(x)

#endif

/****************************************************************************
*
*       LAPI_DUMP_ERROR
*
*       Use in LAPI_RETURN or as a debugging tool for dumping the associated
*       error to stdout.
*
****************************************************************************/

#if (defined(DOS) && !defined(W)) || defined (MAC)
/*
 *  QuickWindows, Macintosh
 */
#define LAPI_DUMP_ERROR(x) {char e[128];OSLoadString(0L,ERR(x),e,sizeof(e));\
                            printf("\n%s\n",e);}

#else
/*
 *  DEFAULT
 */
#define LAPI_DUMP_ERROR(x)

#endif

#ifdef __cplusplus
}
#endif

#endif
