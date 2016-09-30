/* $Header:   U:/NORTON/VCS/ADVSTR.C_V   1.6   04 Mar 1993 17:16:38   SKURTZ  $ */

/*----------------------------------------------------------------------*
 * Advise Library		     			       ADVISE.C	*
 *									*
 * This file contains text strings.					*
 *									*
 * Copyright 1991 by Symantec Corporation				*
 *----------------------------------------------------------------------*/
/* $Filename:  ADVSTR.C                                                 */
/* $Date:   04 Mar 1993 17:16:38  $ */
/* $Product:   Norton Utilities                                         */
/* $Codename:  Scorpio                                                  */
/* $Version:   4.0                                                      */
/* $Revision:   1.6  $  */
/* $Platform:  NortonDOS                                                */
/* $Domain:    UTILITY                                                  */
/* $FileType:  NortonDOS                                                */

/*
 * $Log:   U:/NORTON/VCS/ADVSTR.C_V  $
 * 
 *    Rev 1.6   04 Mar 1993 17:16:38   SKURTZ
 * Added advise topics for Excel and Qpro
 *
 *    Rev 1.5   02 Mar 1993 16:10:06   SKURTZ
 * Removed some obsolete advise topics
 *
 *    Rev 1.4   09 Feb 1993 10:04:26   BRIANF
 * Fixed Annotations
 *
 *    Rev 1.3   26 Jan 1993 11:07:20   SKURTZ
 * Added more WP error messages
 *
 *    Rev 1.2   05 Jan 1993 18:58:30   SKURTZ
 * Assigned hotkeys to buttons.
 *
 *    Rev 1.1   05 Jan 1993 15:46:48   SKURTZ
 * Added DIR_STR, CHKDSK_STR and NAV_STR
 *
 *    Rev 1.0   05 Jan 1993 12:03:40   SKURTZ
 * Initial revision.
 *
 *    Rev 1.2   29 Oct 1991 11:47:12   JOHN
 * Changed variable name "errorSource" to "adviseErrorSource"
 *
 *    Rev 1.1   17 Jul 1991 15:15:26   PETERD
 * Made NULL_STR an external reference.
 *
 *    Rev 1.0   19 Jun 1991 17:25:24   PETERD
 * Initial revision.
 */

#ifndef EMERGENCY

#include	"dialog.h"		/* Library headers		*/
#include	"main.h"
#include	"pulldown.h"
#include	"keys.h"
#include	"nuconfig.h"
#include	"password.h"
#include	"editbox.h"

#include	"adv.h"
#include	"defs.h"
#include	"trouble.h"

/*----------------------------------------------------------------------*
 * These are miscellaneous strings.					*
 *----------------------------------------------------------------------*/

extern Byte NULL_STR[];


/*----------------------------------------------------------------------*
 * List of Common Disk Problems and list of Symptom/Explanation text.	*
 *									*
 * These titles should match the text in the Troubleshooter section of	*
 * the Disk Explorer manual as closely as possible (some titles may	*
 * have to be shortened in order to fit in the lists).			*
 *----------------------------------------------------------------------*/

//@Block ID:AdviseDiskProb Type:DialogString
ErrorInfoRec diskProbData[] =
  {
    { "Computer will Not Boot Up (from Hard Disk)",	PROB_NO_BOOT_SYM,           ADV_PROB_NO_BOOT_SOL		},
    { "Cross-linked Files",				PROB_CROSS_LINK_SYM,        ADV_PROB_CROSS_LINK_SOL         },
    { "Erased Files",					PROB_ERASED_FILES_SYM,	    ADV_PROB_ERASED_FILES_SOL       },
    { "Erased Files (in a Removed Directory)",		PROB_ERASED_DIR_SYM,        ADV_PROB_ERASED_DIR_SOL		},
    { "Errors (reading/writing) Disk Drive",		PROB_READ_WRITE_DISK_SYM,   ADV_PROB_READ_WRITE_DISK_SOL    },
    { "File Allocation Table (FAT) is Bad", 		PROB_BAD_FAT_SYM,	    ADV_PROB_BAD_FAT_SOL            },
    { "Formatted Floppy Diskette (Accidental)",		PROB_FORMAT_FLOPPY_SYM,     ADV_PROB_FORMAT_FLOPPY_SOL      },
    { "Formatted Hard Disk (Accidental)",		PROB_FORMAT_HD_SYM,         ADV_PROB_FORMAT_HD_SOL          },
    { "Lost or Damaged Subdirectories",			PROB_LOST_SUBDIR_SYM,       ADV_PROB_LOST_SUBDIR_SOL        },
    { "Missing Files",					PROB_MISSING_FILES_SYM,	    ADV_PROB_MISSING_FILES_SOL      },
    { "Non-DOS Disk (Reported by CHKDSK)",		PROB_NON_DOS_DISK_SYM,      ADV_PROB_NON_DOS_DISK_SOL       },
    { "No Room for System on Destination Disk",		PROB_NO_ROOM_FOR_SYS_SYM,   ADV_PROB_NO_ROOM_FOR_SYS_SOL    },
    { "Overwritten Files",				PROB_OVERWRITTEN_FILES_SYM, ADV_PROB_OVERWRITTEN_FILES_SOL  },
    { "Running DOS's RECOVER Program",			PROB_DOS_RECOVER_SYM,       ADV_PROB_DOS_RECOVER_SOL        },
    { "Trashed Root Directory",				PROB_TRASHED_ROOT_DIR_SYM,  ADV_PROB_TRASHED_ROOT_DIR_SOL   },
    { "Unable to Read Drive",				PROB_UNABLE_READ_DRIVE_SYM, ADV_PROB_UNABLE_READ_DRIVE_SOL  },
    { NULL },
  };
//@EndBlock
/*----------------------------------------------------------------------*
 * List of DOS or CHKDSK errors (used for both purposes)		*
 *									*
 * These titles should match the text in the Troubleshooter section of	*
 * the Disk Explorer manual as closely as possible (some titles may	*
 * have to be shortened in order to fit in the lists).			*
 *----------------------------------------------------------------------*/

//@Block ID:AdvisedosErrs Type:DialogString
ErrorInfoRec dosErrorsData[] =
  {
    { "Abort, Retry, Ignore, Fail?",				0, ADV_DOS_ABORT_RETRY_IGNORE	},
    { "Access denied",						0, ADV_DOS_ACCESS_DENIED          },
    { "Bad command or file name",				0, ADV_DOS_BAD_COMMAND            },
    { "Bad or missing Command Interpreter",			0, ADV_DOS_MISSING_INTERPRETER    },
    { "Cannot find system files",				0, ADV_DOS_CANT_FIND_SYS_FILES    },
    { "Cannot load COMMAND, system halted",			0, ADV_DOS_CANNOT_LOAD_COMMAND    },
    { "Data error (reading/writing) ^Bdevice^N",		0, ADV_DOS_ERROR_READ_WRITE       },
    { "Disk boot failure",					0, ADV_DOS_DISK_BOOT_FAIL         },
    { "Divide overflow",					0, ADV_DOS_DIVIDE_OVERFLOW        },
    { "Drive not ready error (reading/writing) ^Bdevice^N",	0, ADV_DOS_DRIVE_NOT_READY        },
    { "Error in EXE file",					0, ADV_DOS_ERROR_IN_EXE_FILE      },
    { "Error loading operating system",				0, ADV_DOS_ERROR_LOAD_DOS         },
    { "Error writing fixed disk",				0, ADV_DOS_ERROR_WRITING_DISK     },
    { "Error writing partition table",				0, ADV_DOS_ERROR_PART_TBL         },
    { "File allocation drive bad, drive ^Bx^N",			0, ADV_DOS_FAT_BAD_A_R_I          },
    { "File creation error",					0, ADV_DOS_FILE_CREATE_ERR        },
    { "File not found",						0, ADV_DOS_FILE_NOT_FOUND         },
    { "General failure error (reading/writing) ^Bdevice^N",	0, ADV_DOS_GENERAL_ERROR          },
    { "Insufficient disk space",				0, ADV_DOS_OUT_OF_DISK_SPACE      },
    { "Invalid drive specification",				0, ADV_DOS_INVALID_DRIVE_SPEC     },
    { "Invalid partition table",				0, ADV_DOS_INVALID_PART_TBL       },
    { "Invalid path, not directory, or dir not empty",		0, ADV_DOS_INVALID_PATH_NOT_DIR   },
    { "Missing operating system",				0, ADV_DOS_MISSING_OS             },
    { "Non-DOS disk error (reading/writing) ^Bdevice^N",	0, ADV_DOS_NON_DOS_ERR            },
    { "Non-System disk or disk error",				0, ADV_DOS_NON_SYS_DISK           },
    { "No room for system on destination disk",			0, ADV_DOS_NO_ROOM_FOR_SYS        },
    { "Not ready error (reading/writing) ^Bdevice^N",		0, ADV_DOS_NOT_READY_ERROR        },
    { "Read fault error (reading/writing) ^Bdevice^N",		0, ADV_DOS_READ_FAULT_ERR         },
    { "Sector not found error (reading/writing) ^Bdevice^N",	0, ADV_DOS_SECTOR_NOT_FOUND       },
    { "Seek error (reading/writing) ^Bdevice^N",		0, ADV_DOS_SEEK_ERR               },
    { "Top level process aborted, cannot continue",		0, ADV_DOS_PROCESS_ABORT          },
    { "Write protect error (reading/writing) ^Bdevice^N",	0, ADV_DOS_WRITE_PROTECT_ERR      },
    { NULL },
  };
//@EndBlock
//@Block ID:AdvisechkdskErrs Type:DialogString
ErrorInfoRec chkdskErrorsData[] =
  {
    { "(.)(..) does not exist",					0, ADV_CHKDSK_DOTS                 },
    { "/^Bx^N Invalid parameter",				0, ADV_CHKDSK_INVALID_PARAM        },
    { "^Bxx^N lost clusters found in ^Bxx^N chains",		0, ADV_CHKDSK_LOST_CLUSTERS        },
    { "Allocation error, size adjusted",	    		0, ADV_CHKDSK_ALLOCATION_ERROR     },
    { "Cannot CHDIR to ^Bsubdirectory^N",			0, ADV_CHKDSK_CANNOT_CD            },
    { "Cannot CHDIR to root",					0, ADV_CHKDSK_CANNOT_CD_ROOT       },
    { "Cannot RECOVER Entry, Processing Continued",		0, ADV_CHKDSK_CANNOT_RECOVER_ENTRY },
    { "Cannot RECOVER (.) Entry, Processing Continued",		0, ADV_CHKDSK_RECOVER_DIR          },
    { "Cannot RECOVER (..) Entry",				0, ADV_CHKDSK_RECOVER_TOP_DIR      },
    { "CHDIR (..) Failed Trying Alternate Method",		0, ADV_CHKDSK_CD_TOP_FAIL          },
    { "Contains ^Bxxx^N non-contiguous blocks",			0, ADV_CHKDSK_NON_CONT_BLOCKS      },
    { "Convert directory to file (Y/N)?",			0, ADV_CHKDSK_CONVERT_DIR          },
    { "Directory is totally empty, no (.) or (..)",		0, ADV_CHKDSK_DIR_EMPTY            },
    { "Disk error reading FAT ^Bx^N",				0, ADV_CHKDSK_ERR_READING_FAT      },
    { "Disk error writing FAT ^Bx^N",				0, ADV_CHKDSK_ERR_WRITING_FAT      },
    { "Entry has a bad attribute (or size or link)",		0, ADV_CHKDSK_ENTRY_BAD_ATTR       },
    { "Error found, F parameter not specified.",		0, ADV_CHKDSK_ERR_FOUND_NO_F       },
    { "File contains non-contiguous blocks",			0, ADV_CHKDSK_FILE_NON_CONT_BLOCKS },
    { "File is cross-linked: on cluster ^Bxx^N",		0, ADV_CHKDSK_CROSS_LINKED_FILE    },
    { "First cluster is invalid, entry truncated",		0, ADV_CHKDSK_FIRST_NUMBER_INVALID },
    { "Has invalid cluster, file truncated",			0, ADV_CHKDSK_INVALID_CLUSTER      },
    { "Insufficient room in root directory",			0, ADV_CHKDSK_ROOT_DIR_FULL        },
    { "Invalid current directory",				0, ADV_CHKDSK_INVALID_CUR_DIR      },
    { "Invalid subdirectory",					0, ADV_CHKDSK_INVALID_SUBDIR       },
    { "Non-DOS diskette",					0, ADV_CHKDSK_NON_DOS_DISKETTE     },
    { "Probable non-DOS disk, Continue (Y/N)?",			0, ADV_CHKDSK_PROBABLE_NON_DOS     },
    { "Unrecoverable error on directory",			0, ADV_CHKDSK_UNRECVRABLE_DIR_ERR  },
    { NULL },
  };
//@EndBlock
//@Block ID:VariousERRs Type:DialogString
ErrorInfoRec wpErrorsData[] =
  {
    { "Disk error 19 ",		0, 	ADV_DOS_WRITE_PROTECT_ERR      },
    { "Disk error 20 ",		0, 	ADV_DOS_INVALID_DRIVE_SPEC     },
    { "Disk error 21 ",		0, 	ADV_DOS_DRIVE_NOT_READY        },
    { "Disk error 23 ",		0, 	ADV_DOS_ERROR_READ_WRITE       },
    { "Disk error 25 ",		0, 	ADV_DOS_SEEK_ERR               },
    { "Disk error 26 ",		0, 	ADV_DOS_NON_DOS_ERR            },
    { "Disk error 27 ",		0, 	ADV_DOS_SECTOR_NOT_FOUND       },
    { "Disk error 30 ",		0, 	ADV_DOS_READ_FAULT_ERR         },
    { "Disk error 31 ",		0, 	ADV_DOS_GENERAL_ERROR          },
//    { "Bad memory image of FAT",0, 	ADV_WP_BAD_MEMORY_FAT},
    { "Data error (30)",        0,      DOS_READ_FAULT_ERR},
    { "Device error (25)",      0,      DOS_SEEK_ERR},
    { "Device not ready (21)",  0,      DOS_NOT_READY_ERROR},
    { "Disk error (23)",        0,      DOS_ERROR_READ_WRITE},
    { "Drive not ready (20)",   0,      DOS_INVALID_DRIVE_SPEC},
//    { "FCB unavailable",        0,      ADV_WP_FCB_UNAVAILABLE},
    { "General failure (31)",   0,      DOS_GENERAL_ERROR},
//    { "Lock violation",         0,      ADV_WP_LOCK_VIOLATION},
//    { "Network device fault reading/writing drive",0,   ADV_WP_NET_DEVICE_FAULT},
    { "Sector not found (27)",  0,      DOS_SECTOR_NOT_FOUND},
    { "Write-protect error (19)",0,      DOS_WRITE_PROTECT_ERR},
    { NULL },
  };

ErrorInfoRec lotusErrorsData[] =
  {
    { "Cannot create file",			0, 	LOTUS_CANT_CREATE_FILE     },
    { "Cannot delete file",			0, 	ADV_DOS_ACCESS_DENIED          },
    { "Directory does not exist",		0, 	ADV_DOS_ACCESS_DENIED          },
    { "Disk Drive not ready",			0,	ADV_DOS_DRIVE_NOT_READY        },
    { "Disk Error",				0, 	ADV_DOS_ERROR_READ_WRITE       },
    { "File does not exist",			0, 	ADV_DOS_FILE_NOT_FOUND         },
    { "Invalid character in filename",		0, 	LOTUS_INVALID_CHARACTER    },
    { "Invalid disk name",			0, 	ADV_DOS_INVALID_DRIVE_SPEC     },
    { "No files of specified type on disk",	0, 	ADV_DOS_FILE_NOT_FOUND         },
    { "Not a valid worsheet file",       	0, 	LOTUS_NOT_VALID_WORKSHEET  },
    { "Part of file missing",			0, 	LOTUS_PART_OF_FILE_MISSING },
    { "Worksheet file revision is out of date", 0, 	LOTUS_REVISION_OUT_OF_DATE },
    { NULL },
  };

////// EXCEL ERROR MESSAGES ////////////////////////////////////////////
ErrorInfoRec excelErrorsData[] =
  {
    { "Unrecoverable Application Error",        0,      ADV_EXCEL_UAE                  },
    { "Some Data may be lost",                  0,      ADV_EXCEL_DATA_LOST},
    { "Unable to read file",                    0,      ADV_EXCEL_UNABLE_TO_READ_FILE},
    { NULL },
  };

////// QUATTRO PRO ERROR MESSAGES //////////////////////////////////////
ErrorInfoRec qProErrorsData[] =
  {
    { "Invalid spreadsheet file",               0,      ADV_QPRO_INVALID_SS},
    { "Error in reading file",                  0,      ADV_QPRO_ERROR_READ},

    { NULL },
  };

ErrorInfoRec dbaseErrorsData[] =
  {
    { "Beginning of file encountered",		0,	DBASE_BOF_ENCOUNTERED },
    { "Cannot write to a read-only file",	0,	ADV_DOS_ACCESS_DENIED },
    { "DBT file cannot be opened",		0,	DBASE_CANT_OPEN_DBT_FILE },
    { "Disk full when writing file:",		0,	ADV_DOS_OUT_OF_DISK_SPACE },
    { "End of file encountered",		0,	DBASE_EOF_ENCOUNTERED },
    { "Error in configuration value",		0,	DBASE_CONFIG_ERROR },
    { "File already exists",			0,	DBASE_FILE_ALREADY_EXISTS },
    { "File does not exist",			0,	ADV_DOS_FILE_NOT_FOUND },
    { "File is not accessible",			0,	DBASE_FILE_NOT_ACCESSIBLE },
    { "File was not LOADed",			0,	DBASE_FILE_NOT_LOADED },
    { "Internal error: CMDSET():",		0,	DBASE_INTERNAL_CMDSET },
    { "Internal error: Illegal opcode",		0,	DBASE_INTERNAL_OPCODE },
    { "Insufficient memory",			0,	DBASE_NO_MEMORY },
    { "Line exceeds maximum of 254 characters",	0,	DBASE_LINE_TOO_LONG },
    { "Maximum path length exceeded",		0,	DBASE_PATH_TOO_LONG },
    { "Maximum record length exceeded",		0,	DBASE_RECORD_TOO_BIG },
    { "Not a dBASE database",			0,	DBASE_NOT_DBASE_DATABASE },
    { "Record is out of range",			0,	DBASE_RECORD_OUT_OF_RANGE },
    { "Too many files are open",		0,	DBASE_TOO_MANY_FILES },
    { "Unsupported path given",			0,	DBASE_UNSUPPORTED_PATH },
    { NULL },
  };
//@EndBlock


/* these strings must be the same length, and there must be a space as
 * the last character */

//@Block ID:adviseErrsrc Type:Icon
Byte 	*adviseErrorSource[] =
    {
    "DOS     ",
    "CHKDSK  ",
    "WP      ",
    "123     ",
    "dBASE   ",
    "Excel   ",
    "Quattro ",
    };
//@EndBlock
/*----------------------------------------------------------------------*
 * Buttons for first level of Advise menu dialog boxes.	 		*
 *----------------------------------------------------------------------*/

Byte *lookupButtonStrings[] =    	     	/* Button strings	*/
    {
    "^Expand",
    " ^Done ",
    NULL_STR,
    };

ButtonsRec lookupButtons =			/* Dialog buttons	*/
    {
    lookupButtonStrings,	     		/* strings		*/
    NORMAL_BUTTON_FLAGS | USE_PAD,	 	/* flags		*/
    0,						/* value		*/
    0, 0,					/* row, col		*/
    1, 6,       				/* pad, gap		*/
    };



/*----------------------------------------------------------------------*
 * Search Dialog							*
 *----------------------------------------------------------------------*/

Byte	*startSearchStrs [] =
    {
    " ^Start Search ",
    "    ^Cancel    ",
    NULL_STR
    };

ButtonsRec	startSearchButtons =
    {
    startSearchStrs,
    BOUNCE_BACK,
    0,
    0,0,
    };

CheckBoxRec	searchForErrorsCB =
    {
    "^Include Application Error Messages",
    NULL,
    FALSE
    };

TERec searchForErrorsTE = 			/* Text-edit field	*/
    {
    0, 0,					/* row, col		*/
    KEY_LENGTH + 2,   	 			/* width		*/
    NULL,					/* string		*/
    KEY_LENGTH,    				/* max length		*/
    0, 0,    					/* insert pt, offset	*/
    NULL,    					/* validation function	*/
    TE_CONVERT_CR,   				/* flags		*/
    };

Byte *searchForErrorsDlgStrs[] =
    {
    DIALOG_EMPTY_LINE,
    "Enter part of the error message that you",
    "wish to search for, or some key words that",
    "appeared in the error message.",
    DIALOG_EMPTY_LINE,
    " %T1 ",
    DIALOG_EMPTY_LINE,
    DIALOG_LEFT,
    " %C2 ",
    DIALOG_EMPTY_LINE,
    DIALOG_CENTER,
    " %B3 ",
    DIALOG_END
    };

DialogRec searchForErrorsDlg = 			/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS, 			/* flags		*/
    " Search For Error Messages ",		/* title		*/
    searchForErrorsDlgStrs  			/* strings		*/
    };

/*----------------------------------------------------------------------*
 * Search Result Dialog	- uses the errorList list box			*
 *----------------------------------------------------------------------*/

Byte *newLookupButtonStrings[] =    	     	/* Button strings	*/
    {
    "  ^Expand  ",
    "^New Search",
    "   ^Done   ",
    NULL_STR,
    };

ButtonsRec newLookupButtons =			/* Dialog buttons	*/
    {
    newLookupButtonStrings,	     			/* strings	*/
    NORMAL_BUTTON_FLAGS | USE_PAD | BOUNCE_BACK,	/* flags	*/
    0,							/* value	*/
    0, 0,						/* row, col	*/
    1, 2,       					/* pad, gap	*/
    };

Byte *searchResultStrings[] =	 		/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
    "  %L1  ",
    DIALOG_SKIP_LINE,
    DIALOG_SKIP_LINE,
    DIALOG_EMPTY_LINE,
    "%B2",
    DIALOG_END,
    };

Byte	*applLegend 	= "%3aWP%1a - WordPerfect   %3a123%1a - Lotus 123   %3adBASE%1a - dBASE III/IV";
Byte	*applLegend2	= "%3aExcel%1a - Excel   %3aQuattro%1a - Quattro Pro";
Byte	*searchLegend 	= DIALOG_SKIP_LINE;

Byte	*applTitle 	= " Application Error Messages ";
Byte	*searchTitle 	= " Found Error Messages ";

DialogRec searchResultDlg =    			/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS | ALL_ACTIVE, 	 	/* flags		*/
    NULL_STR,			/* title		*/
    searchResultStrings,			/* strings		*/
    };

/*----------------------------------------------------------------------*
 * This dialog appears when no matching error messages were found	*
 *----------------------------------------------------------------------*/
Byte	*newSearchStrs [] =
    {
    " ^New Search ",
    "   ^Cancel   ",
    NULL_STR
    };

ButtonsRec	newSearchButtons =
    {
    newSearchStrs,
    BOUNCE_BACK,
    0,
    0,0,
    };

Byte *searchResultNoneStrings[] =	 	/* Dialog strings 	*/
    {
    DIALOG_EMPTY_LINE,
    "No DOS, CHKDSK, or application error messages",
    "were found that matched the search text:",
    DIALOG_EMPTY_LINE,
    "%3a%s1%1a",
    DIALOG_EMPTY_LINE,
    "%B2",
    DIALOG_END,
    };

DialogRec searchResultNoneDlg =    		/* Dialog box		*/
    {
    0, 0,					/* row, col		*/
    NORMAL_DIALOG_FLAGS | ALL_ACTIVE, 	 	/* flags		*/
    NULL_STR,					/* title		*/
    searchResultNoneStrings,			/* strings		*/
    };

Byte  DIR_STR[] 	= "DIR";
Byte  CHKDSK_STR[] 	= "CHKDSK.EXE";
Byte  NAV_STR[]		= "NAV";


#endif
