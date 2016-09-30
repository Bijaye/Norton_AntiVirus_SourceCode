/* $Header:   S:/navc/VCS/defs.h_v   1.0   06 Feb 1997 20:56:36   RFULLER  $ */

/*----------------------------------------------------------------------*
 * Norton AntiVirus User Interface                      DEFS.H          *
 * This file contains constants, structures, and type definitions.      *
 *                                                                      *
 * This file contains prototypes                                        *
 *                                                                      *
 *----------------------------------------------------------------------*
 * Copyright 1991 by Symantec                                           *
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/navc/VCS/defs.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:56:36   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:25:34   MKEATIN
// Initial revision.
// 
//    Rev 1.0   02 Oct 1996 12:35:06   JBELDEN
// Initial revision.
 */

#ifndef INCL_NAVDEFS_H
#define INCL_NAVDEFS_H

/*----------------------------------------------------------------------*
 * These are the buttons that the 'selectColorSetDlg' dialog box uses.	*
 * (Used in the SelectColorDisplay() module.				*
 *----------------------------------------------------------------------*/
#define		COLOR_DISPLAY_BUTTON	0	/* Color display color	*/
#define		BW_DISPLAY_BUTTON	1	/* B&W display color 	*/
#define		BW_COLOR_RET_BUTTON	2	/* Return to DOS	*/


                                        //============================
                                        // Date width xx/xx/xx
                                        //============================
#define DATE_WIDTH              (8)
                                        //============================
                                        // YES/NO or OK/CANCEL values
                                        //============================
#define OK_BUTTON               0
#define CANCEL_BUTTON           1

                                        //============================
                                        // Virus Info Buttons
                                        //============================
#define VIRUS_INFO_CLOSE        0
#define VIRUS_INFO_PRINT        1
#define VIRUS_INFO_HELP         2
#define VIRUS_INFO_PREV         3
#define VIRUS_INFO_NEXT         4
                                        //============================
                                        // Activity Log Buttons
                                        //============================
#define ACTIVITY_LOG_CLOSE      0
#define ACTIVITY_LOG_PRINT      1
#define ACTIVITY_LOG_FILTER     2
#define ACTIVITY_LOG_CLEAR      3
#define ACTIVITY_LOG_HELP       4
                                        //============================
                                        // Activity Log Clear Buttons
                                        //============================
#define CLEAR_YES               0
#define CLEAR_NO                1
#define CLEAR_HELP              2
                                        //============================
                                        // Activity Log Filter Buttons
                                        //============================
#define FILTER_OK               0
#define FILTER_CANCEL           1
#define FILTER_HELP             2
                                        //============================
                                        // Dated
                                        //============================
#define DATED_ON                0
#define DATED_NOT_ON            1
#define DATED_BEFORE            2
#define DATED_BEFORE_OR_ON      3
#define DATED_AFTER             4
#define DATED_AFTER_OR_ON       5
#define DATED_BETWEEN           6
#define DATED_NOT_BETWEEN       7
                                        //============================
                                        // Print to Device stuff
                                        //============================
#define MAX_PRINTER_TYPES       10      // Allow upto 10 printers in combo
#define MAX_PRINTER_WIDTH       10      // Longest printer string ("LPT1", "LPT2")

#define VIRUS_COMMENT_WIDTH	(60)    // width of comment in virus list
					// information dialog
#endif
