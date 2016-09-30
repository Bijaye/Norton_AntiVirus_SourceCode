
This module is no longer used, and should be disposed of.  It was
incorporated into TypeMgr.PM, since we couldn't conceive of anyone
else having any use for it.


####################################################################
#                                                                  #
# Program name:  EXTList.pm                                        #
#                                                                  #
# Package name:  EXTList.pm                                        #
#                                                                  #
# Description:                                                     #
#                                                                  #
# Statement:     Licensed Materials - Property of IBM              #
#                (c) Copyright IBM Corp. 1998-1999                 #
#                                                                  #
# Author:        Till Teichmann                                    #
#                                                                  #
#                U.S. Government Users Restricted Rights - use,    #
#                duplication or disclosure restricted by GSA ADP   #
#                Schedule Contract with IBM Corp.                  #
#                                                                  #
#                                                                  #
####################################################################
#                                                                  #
# Function:                                                        #
#                                                                  #
#                                                                  #
#                                                                  #
####################################################################


package EXTList;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(@EXT);
@EXPORT_OK = @EXPORT;


@EXT=(
"RIFF,RIFF,0,-1,4,2,0",
"ARJ,\\x60\\xea,0,-1,2,1,0",
"ARJ,aRJsf,0,30,5,1,0",
"AIF,DOCTYPE HTML PUBLIC,0,100,-1,1,0",
"AU,.SND,0,-1,4,1,0",
"AU,.snd,0,-1,4,1,0",
"ANI,ACON,0,100,-1,3,0",
"ANI,anih,0,100,-1,3,0",
"AVI,AVI,0,100,-1,2,0",
"BAT,\@ECHO OFF,0,100,-1,3,0",
"AT,\@echo off,0,100,-1,3,0",
"BAT,REM ,0,100,-1,3,3",
"BAT,rem ,0,100,-1,3,3",
"BGI,pk\\b\\bBGI Device Driver,0,100,-1,1,0",
"BGI,pk\\b\\bBGI Device Driver,0,100,-1,1,0",
"BMP Windows,BM,0,-1,2,1,0",
"BMP OS\/2,BA,0,-1,2,1,0",
"BMP OS\/2,CI,0,-1,2,1,0",
"BMP OS\/2,CP,0,-1,2,1,0",
"BMP OS\/2,IC,0,-1,2,1,0",
"BMP OS\/2,PT,0,-1,2,1,0",
"BBK,ADLIB-,0,100,-1,1,0",
"BOOTSECTOR,\\x55\\xaa,510,510,2,1,0",
"CAB,MSCF,0,-1,4,1,0",
"CAC,\\x3f\\x5f\\x03\\x00,0,-1,4,1,0",
"CAL,\\xb5\\xa2\\xb0\\xb3\\xb3\\xb0\\xa2\\xb5,0,100,-1,1,0",
"CDA,CDDA,0,100,-1,1,0",
"CDR,MGC,0,-1,3,1,1",
"CGI,\\xffFONT  ,0,-1,7,1,0",
"CHR,PK\\b\\bBGI,0,-1,9,1,0",
"CUR,\\x00\\x00\\x02\\x00\\x01\\x00\\x20\\x20\\x00\\x00,0,100,-1,1,0",
"Compress5,SZDD,0,-1,4,1,0",
"Compress6,KWAJ,0,-1,4,1,0",
"DCX,\\x98\\x76\\x54\\x32,0,-1,4,1,0",
"EXE,MZ,0,-1,2,1,0",
"EXE,ZM,0,-1,2,1,0",
"GEM,\\x00\\x01\\x00\\x08,0,-1,4,1,0",
"GEM,\\xff\\xff\\x18\\x00,0,-1,4,1,0",
"GIF87a,GIF87a,0,-1,6,1,0",
"GIF89a,GIF89a,0,-1,6,1,0",
"GRP,PMC,0,100,-1,1,0",
"GZIP,\\x1f\\x8b,0,-1,2,1,0",
"HLP,\\x3f\\x5f,0,-1,2,2,0",
"HLP,\\xff\\xff\\xff\\xff,0,100,-1,2,0",
"HTML,<HTML>,0,100,-1,2,0",
"HTML,<HEAD>,0,100,-1,2,0",
"HTML,<P>,0,100,-1,3,0",
"HTML,HREF=\",0,100,-1,3,0",
"HTML,<H2 ,0,100,-1,3,0",
"IBK,\\x49\\x42\\x3b\\x1a,0,100,-1,1,0",
"ICO,\\x00\\x00\\x01\\x00\\x02\\x00\\x20\\x20\\x10\\x00\\x00\\x00\\x00\\x00\\xe8\\x02\\x00\\x00\\x26\\x00,0,-1,20,1,0",
"ICO,\\x00\\x00\\x01\\x00\\x02\\x00\\x20\\x20\\x10\\x00\\x00\\x00\\x00\\x00\\xe8\\xe2\\x00\\x00\\x26\\x00,0,-1,20,1,0",
"JAVA,\\xca\\xfe\\xba\\xbe,0,100,-1,1,0",
"JPG,JFIF,0,100,-1,2,0",
"JPG,\\xff\\xd8\\xff\\xe0,0,100,-1,2,0",
"LIB,!<arch>,0,-1,7,1,0",
"LHZ,-lh,0,-1,2,1,0",
"LHZ,-lz,0,-1,2,1,0",
"LNK,\\x3c\\x00\\x00\\x00\\x01\\x14\\x02\\x00\\x00\\x00\\x00\\x0c,0,-1,12,1,0",
"MIDI,MThd,0,-1,4,2,0",
"MIDI,MTrk,0,100,-1,2,0",
"MOV,mdat,0,100,-1,2,0",
"MOV,moov,0,100,-1,2,0",
"MPG,\\xff\\xfd\\x60\\xcc,0,100,-1,1,0",
"MPG,\\xff\\xfa\\x80\\x04,0,-1,4,1,0",
"MPG,\\xff\\xfb\\x80\\x04,0,-1,4,1,0",
"MPG,\\xff\\xfa\\x80\\x44,0,-1,4,1,0",
"MPG,\\xff\\xfb\\x80\\x44,0,-1,4,1,0",
"MPG,\\xff\\xfa\\x80\\x64,0,-1,4,1,0",
"MPG,\\xff\\xfb\\x80\\x64,0,-1,4,1,0",
"MPG,\\xff\\xfa\\x90\\x04,0,-1,4,1,0",
"MPG,\\xff\\xfb\\x90\\x04,0,-1,4,1,0",
"MPG,\\xff\\xfa\\x90\\x44,0,-1,4,1,0",
"MPG,\\xff\\xfb\\x90\\x44,0,-1,4,1,0",
"MPG,\\xff\\xfa\\x90\\x64,0,-1,4,1,0",
"MPG,\\xff\\xfb\\x90\\x64,0,-1,4,1,0",
"MPG,\\xff\\xfa\\x90\\x04,0,-1,4,1,0",
"MPG,\\xff\\xfb\\x90\\x04,0,-1,4,1,0",
"MPG,\\xff\\xfa\\x90\\x44,0,-1,4,1,0",
"MPG,\\xff\\xfb\\x90\\x44,0,-1,4,1,0",
"MPG,\\xff\\xfa\\x90\\x64,0,-1,4,1,0",
"MPG,\\xff\\xfb\\x90\\x64,0,-1,4,1,0",
"MPG,\\xff\\xfb\\x90\\x04,0,-1,4,1,0",
"MPG,\\xff\\xfb\\x58\\xc4,0,-1,4,1,0",
"MPG Video,\\x00\\x00\\x01\\xb3,0,-1,4,1,0",
"OLE,\\xd0\\xcf\\x11\\xe0\\xa1\\xb1\\x1a\\xe1,0,-1,8,1,0",
"PAL,AH,0,-1,2,2,0",
"PAL,Dr. Halo,0,100,-1,2,0",
"PCX,\\x0a\\x00,0,-1,2,2,0",
"PCX,\\x0a\\x02,0,-1,2,2,0",
"PCX,\\x0a\\x03,0,-1,2,2,0",
"PCX,\\x0a\\x05,0,-1,2,2,0",
"PCX,XMIN,0,100,-1,4,1,0",
"PCX,YMIN,0,100,-1,4,1,0",
"PCX,XMAX,0,100,-1,4,1,0",
"PCX,YMAX,0,100,-1,4,1,0",
"PDF,%PDF,0,100,-1,1,0",
"PFB,PS-AdobeFont,0,100,-1,1,0",
"PSD,8BPS,0,100,-1,1,0",
"PIC,\\x12\\x34,0,-1,2,1,0",
"RAM,\\x2e\\x52\\x4d\\x46\\x00\\x00\\x00\\x12\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x05\\x50\\x52\\x4f\\x50,0,100,-1,1,0",
"RTF,\\x7b\\x5c\\x72\\x74\\x66,0,100,-1,1,0",
"RTF,\\x7b\\x72\\x74\\x66,0,100,-1,1,0",
"RTF,\{\\rtf,0,-1,4,1,0",
"SBI,\\x53\\x42\\x49\\x1a,0,-1,4,1,0",
"SMF,\\x4d\\x54\\x68\\x64,0,-1,4,1,0",
"VBScript,script language=\"VBScript\",0,100,-1,2,0",
"VOC,Creative Voice File,0,-1,11,1,0",
"WAV,WAVE,0,100,-1,2,0",
"WMF,\\xd7\\xcd\\xc6\\x9a\\x00\\x00,0,-1,6,1,0",
"WRI,\\x31\\xbe\\x00\\x00\\x00\\xab\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00,0,-1,13,1,0",
"WRI,\\x32\\xbe\\x00\\x00\\x00\\xab\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00,0,-1,13,1,0",
"ZIP,\\x50\\x4b\\x03\\x04,0,-1,4,1,0",
"ZIP,\\x50\\x4b\\x05\\x06,0,-1,4,1,0",
"ZOO,ZOO 2.00 Archive,0,100,-1,1,0",
"ZOO,ZOO 2.10 Archive,0,100,-1,1,0",
"ZOO,\\xfd\\xc4\\xa7\\xdc,0,-1,4,1,0");

1;
