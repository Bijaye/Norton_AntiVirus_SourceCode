//************************************************************************
//
// $Header:   S:/NAVEX/VCS/gdefines.h_v   1.1   15 Dec 1998 12:15:42   DCHI  $
//
// Description:
//  Contains global definitions that would be in the MAK file.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/gdefines.h_v  $
// 
//    Rev 1.1   15 Dec 1998 12:15:42   DCHI
// Added PP_SCAN definition for NAVEX15 and SYM_WIN.
// 
//    Rev 1.0   09 Dec 1998 17:44:52   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _GDEFINES_H_

#define _GDEFINES_H_

#if !defined(NOMACRO)

    #if defined(NAVEX15) || defined(SYM_DOSX) || defined(SYM_WIN16)

        #define MACROHEU    // Macro heuristics

    #endif

    #if defined(NAVEX15)

        #define MACRODAT    // Macro data files
        #define MSX         // Macro scan exclusions

    #endif

    #if defined(NAVEX15) || defined(SYM_WIN)

        #define PP_SCAN     // PowerPoint scanning

    #endif

#endif // #if !defined(NOMACRO)

#endif // #ifndef _GDEFINES_H_

