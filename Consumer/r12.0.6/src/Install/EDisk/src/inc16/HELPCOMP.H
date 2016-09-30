/* Copyright 1991 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/INCLUDE/VCS/HELPCOMP.H_V   1.2   14 Mar 1992 22:51:28   TORSTEN  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************ 
 * $Log:   S:/INCLUDE/VCS/HELPCOMP.H_V  $ *
 * 
 *    Rev 1.2   14 Mar 1992 22:51:28   TORSTEN
 * Added support for the #SETINDEX command.
 * 
 *    Rev 1.1   29 Feb 1992 15:30:06   TORSTEN
 * Added the wMaxDynamicMemory field to the HELP_SEGMENTHEADER structure.
 * 
 *    Rev 1.0   11 Dec 1991 18:50:36   BRUNO
 * Initial revision.
 ************************************************************************/


#ifndef _HELPCOMP_H_

#define _HELPCOMP_H_

#include    <types.h>


/************************************************************************
 * The magic number appearing in the help segment header                *
 ************************************************************************/

#define HELPMAGIC_NUMBER    0x2E63


/************************************************************************
 * Help system contraints                                               *
 ************************************************************************/

#define MAX_COPYRIGHT       128
#define MAX_PRODUCTNAME     8
#define MAX_TOPIC           60
#define MAX_HELPLINE        65
#define MAX_DEFINENAME      30


/************************************************************************
 * Help topic flags                                                     *
 ************************************************************************/

#define HTFLAG_NORMAL       0x00        // normal topics
#define HTFLAG_NOINDEX      0x01        // don't include in index
#define HTFLAG_ADVISE       0x02        // ADVISE topic
#define HTFLAG_ADVANCED     0x04        // item which only appears in advanced help


/************************************************************************
 * Data structures                                                      *
 ************************************************************************/

typedef struct {
    Byte        szCopyright[MAX_COPYRIGHT + 1];         // copyright notice
    Byte        szProductName[MAX_PRODUCTNAME + 1];     // short product name
    Word        wVersionNumber;                         // product version number
    Word        wMagicNumber;                           // help segment identifier
    Word        wIndexEntries;                          // the number of topics
    Word        wAdvancedIndexEntries;                  // the number of topics only in advanced help
    Word        wNoIndexEntries;                        // the number of topics not in index
    Ulong       ulHelpSegmentSize;                      // length of this help segment
    Boolean     bCompressed;                            // TRUE if topics and index are compressed
    Word        wIndexSize;                             // size of compressed index section
    Word        wMaxDynamicMemory;                      // max dynamic memory required
    Word        wIndexTopicID;                          // the ID of the index topic (typically 0)
    } HELP_SEGMENTHEADER;

typedef struct {
    Byte        byTopicHeaderLength;                    // length of topic header
    Byte        byFlags;                                // help topic flags
    Word        wTopicLength;                           // the original help text length
    Word        wCompressedLength;                      // the compressed help text length
    Word        wLinks;                                 // the number of links in this topic
    Ulong       ulTopicOffset;                          // offset to links and help text from beginning of help segment
    Word        wHelpLines;                             // the number of lines in the topic
    Byte        szTopicName[MAX_TOPIC + 1];             // the topic name (must be the last field, variable sized!)
    } HELP_TOPICHEADER;

typedef struct {
    Word        wLine;                                  // the line
    Word        wColumn;                                // the column
    Word        wLength;                                // the length
    Word        wTargetTopicID;                         // the target topic
    } HELP_LINKNODE;


/************************************************************************
 * Help text attributes                                                 *
 ************************************************************************/

#define HELPATTR_ESCAPE     '\\'
#define HELPATTR_META       '^'
#define HELPATTR_BOLD       'b'
#define HELPATTR_NORMAL     'n'
#define HELPATTR_HEXCHAR    'c'
#define HELPATTR_HEXATTR    'a'
#define HELPATTR_LINKSTART  '['
#define HELPATTR_LINKFENCE  ':'
#define HELPATTR_LINKEND    ']'

/************************************************************************
 * Help display module attributes                                       *
 ************************************************************************/

#define HELPDISP_META       '^'
#define HELPDISP_BOLD       'b'
#define HELPDISP_REVERSE    'r'
#define HELPDISP_UNDERLINE  'u'
#define HELPDISP_NORMAL     'n'
#define HELPDISP_LINK       'h'
#define HELPDISP_ACTIVELINK 'l'
#define HELPDISP_HEXCHAR    'c'
#define HELPDISP_HEXATTR    'a'

#endif


