//************************************************************************
//
// $Header:   S:/NAVEX/VCS/vba5dir.h_v   1.0   14 Mar 1997 16:32:54   DCHI  $
//
// Description:
//      Contains definitions for the dir stream of the VBA storage.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/vba5dir.h_v  $
// 
//    Rev 1.0   14 Mar 1997 16:32:54   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _VBADIR_H_

#define _VBADIR_H_

/**************************************************************************

Here is the design of the intermediate file format for use to load projects.

There will be multiple streams -- a single directory stream, followed by
multiple streams for each code module.


Code module streams:
====================
The code module streams are pure ascii code, saved exactly in the same format
as the current VBA would do a "save as text" (including attributes associated
with each function, etc).  These streams are compressed to save space.

The streams names are in general the same name as the module.
Note that it really doesn't matter what this name is.  The only 
restrictions are that each module's stream be unique and that 
it be a valid stream name in the docfile implementation

The directory stream:
=====================
The name of the directory stream is defined with the SZ_DIRECTORY_STREAM
constant defined in this file.

The directory stream consists of multiple variable-length records:
16-bit tag
32-bit byte count (# of bytes that follow -- 0 if none)
<data>

Following is a description of the records which may appear in the
directory stream.  When loading the directory stream, records with
unrecognized tags should be ignored.  In describing the data portion
of the record, all numbers are stored in Intel (not Motorola) format.
Anything called a BSTR is stored as a 32-bit length (in bytes) followed
by the string data.  Note that we do not store all strings as BSTRs because
the data record length may imply the length of the string.  All strings are
ANSI (DBCS).

BIN_PROJ_SYSKIND     syskind (32 bit)
BIN_PROJ_LCID        lcid (32 bit)
BIN_PROJ_CODEPAGE    codepage (16 bit)
BIN_PROJ_LCIDINVOKE  LCID to be used for Invoke calls (typically English).
BIN_PROJ_NAME        The programmatic name of the project.
BIN_PROJ_DOCSTRING   The project's doc string
BIN_PROJ_HELPFILE    The project's helpfile name
BIN_PROJ_HELPCONTEXT The project's help context id (32 bit)
BIN_PROJ_LIBFLAGS    wLibFlags (32 bit, strip off unknown ones upon load
                     of cannonical format)
BIN_PROJ_VERSION     m_ulgtliboleMajorId (32 bit)
                     m_ugtliboleMinorId (16 bit)
BIN_PROJ_TYPELIB_VERSION     wVerMajor (16 bit)
                             wVerMinor (16 bit)
BIN_PROJ_GUID        project's guid (16 byte)
BIN_PROJ_PROPERTIES  for each property:
                       BSTR for property name
                       BSTR for property value
                       four byte zero to mark end of list
BIN_PROJ_CONSTANTS   String of conditional compilation constants.
BIN_PROJ_COMPAT_EXE  Compatible exe information
		     fMakeCompatible (16-bit)
		     fShowEditDlg (16-bit)
		     BSTR for exe name

BIN_PROJ_COMMAND_LINE  The Command Line arguments (BSTR)
BIN_PROJ_CONDCOMP      The Conditional Compilation arguments (BSTR)

The following two tags are used to represent the references the project
has to other projects and typelibs.  They must appear in the order that
the references should appear in the project.  The "default" references
should not be in this list.

BIN_PROJ_LIBID_REGISTERED
                     BSTR for registered or foreignregistered libid
                     major (32 bit) and minor (16 bit) id of referenced
                     project/typelib (ulgtliboleMajorId/ugtliboleMinorId).
BIN_PROJ_LIBID_PROJ  BSTR for project libid with absolute path
                     BSTR for libid with relative path
                     major (32 bit) and minor (16 bit) timestamp id of
                     referenced project (ulgtliboleMajorId/ugtliboleMinorId).

BIN_PROJ_MODULECOUNT number of modules (16 bit)

For each module, you get the following tags.  The BIN_MOD_NAME
tag must appear first.

BIN_MOD_NAME         the module's name
BIN_MOD_STREAM       the corresponding module stream's name
BIN_MOD_DOCSTRING    the module's doc string
BIN_MOD_HELPFILE     the module's helpfile name
BIN_MOD_HELPCONTEXT  the module's help context id (32 bit)
BIN_MOD_PROPERTIES   the module's property count (16 bit), then for each property:
                        BSTR for property name
                        BSTR for property value

The filtflag bits.  No data associated with any of these.
If the tag is present, the bit is 1, else 0:
BIN_MOD_FBASMOD_StdMods
BIN_MOD_FBASMOD_Classes
BIN_MOD_FBASMOD_Creatable
BIN_MOD_FBASMOD_NoDisplay
BIN_MOD_FBASMOD_NoEdit
BIN_MOD_FBASMOD_RefLibs
BIN_MOD_FBASMOD_NonBasic
BIN_MOD_FBASMOD_Private
BIN_MOD_FBASMOD_Internal
BIN_MOD_END 	     last record in a given module's collection of records

BIN_PROJ_EOF 	     last record in the directory stream

Each module stream contains the module's Basic code interspersed
with Attribute statements.  VBA93 generates attribute statements
for each function that provide the helpid and doc string (only
generated if there is a helpid and/or a doc string).

**************************************************************/

#define BIN_SZ_DIRECTORY_STREAM (OLESTR("dir"))

// bxlat - The header structure for records in the directory stream.
typedef struct tagBINXLATRECORD
{
    unsigned short usTag;       // One of the BIN_* constants.
    unsigned long  cbData;      // The number of bytes of data in rgbData.
} BINXLATRECORD;

#define BIN_PROJ_SYSKIND                 1
#define BIN_PROJ_LCID                    2
#define BIN_PROJ_CODEPAGE                3
#define BIN_PROJ_NAME                    4
#define BIN_PROJ_DOCSTRING               5
#define BIN_PROJ_HELPFILE                6
#define BIN_PROJ_HELPCONTEXT             7
#define BIN_PROJ_LIBFLAGS                8
#define BIN_PROJ_VERSION                 9
#define BIN_PROJ_GUID                   10
#define BIN_PROJ_PROPERTIES             11
#define BIN_PROJ_CONSTANTS              12

#define BIN_PROJ_LIBID_REGISTERED       13
#define BIN_PROJ_LIBID_PROJ             14

#define BIN_PROJ_MODULECOUNT            15
#define BIN_PROJ_EOF                    16
#define BIN_PROJ_TYPELIB_VERSION        17
#define BIN_PROJ_COMPAT_EXE             18
#define BIN_PROJ_COOKIE                 19
#define BIN_PROJ_LCIDINVOKE             20
#define BIN_PROJ_COMMAND_LINE           21
#define BIN_PROJ_REFNAME_PROJ           22

#define BIN_MOD_NAME                    25
#define BIN_MOD_STREAM                  26
#define BIN_MOD_DOCSTRING               28
#define BIN_MOD_HELPFILE                29
#define BIN_MOD_HELPCONTEXT             30
#define BIN_MOD_PROPERTIES              32
#define BIN_MOD_TEXTOFFSET              49

#define BIN_MOD_FBASMOD_StdMods         33
#define BIN_MOD_FBASMOD_Classes         34
#define BIN_MOD_FBASMOD_Creatable       35
#define BIN_MOD_FBASMOD_NoDisplay       36
#define BIN_MOD_FBASMOD_NoEdit          37
#define BIN_MOD_FBASMOD_RefLibs         38
#define BIN_MOD_FBASMOD_NonBasic        39
#define BIN_MOD_FBASMOD_Private         40
#define BIN_MOD_FBASMOD_Internal        41
#define BIN_MOD_FBASMOD_AllModTypes     42
#define BIN_MOD_END                     43
#define BIN_MOD_COOKIETYPE              44
#define BIN_MOD_BASECLASSNULL           45
#define BIN_MOD_BASECLASS               46

#define BIN_PROJ_LIBID_TWIDDLED         47
#define BIN_PROJ_LIBID_EXTENDED         48

#define BIN_PROJ_UNICODE_CONSTANTS      60
#define BIN_PROJ_UNICODE_HELPFILE       61
#define BIN_PROJ_UNICODE_REFNAME_PROJ   62
#define BIN_PROJ_UNICODE_COMMAND_LINE   63
#define BIN_PROJ_UNICODE_DOCSTRING      64

#define BIN_MOD_UNICODE_STREAM          50
#define BIN_MOD_UNICODE_NAME            71
#define BIN_MOD_UNICODE_DOCSTRING       72
#define BIN_MOD_UNICODE_HELPFILE        73

#endif // #ifndef _VBADIR_H_

