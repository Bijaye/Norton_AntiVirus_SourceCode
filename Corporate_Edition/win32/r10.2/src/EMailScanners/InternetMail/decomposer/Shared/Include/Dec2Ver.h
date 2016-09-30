// Dec2Ver.h : Decomposer 2 Version Resource Header
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// The following VERSION_ symbols should be updated for each build.

#define VERSION_MAJOR			3
#define VERSION_MINOR			02
#define VERSION_INLINE          10
#define VERSION_BUILD_NUMBER    03
#define VERSION_STRING          "3.02.11.03"


/////////////////////////////////////////////////////////////////////////////
// The following VR_ symbols should be defined before this header file
// is included into a resource script (.rc) file.
//
// #define VR_INTERNALNAME         "FILENAME"
// #define VR_ORIGINALFILENAME     "FILENAME.DLL"
// #define VR_FILEDESCRIPTION      "File Description"


/////////////////////////////////////////////////////////////////////////////
// The following VR_ symbols should be set once for the product.

#define VR_PUBLISHERNAME	"Symantec Corporation"
#define VR_PRODUCTNAME		"File Decomposer"
#define VR_COPYRIGHT		"Copyright 2001 - 2006 Symantec Corporation."
#ifndef VR_FILEDESCRIPTION
  #define VR_FILEDESCRIPTION "File Decomposer Component"
#endif


/////////////////////////////////////////////////////////////////////////////
// The following VR_ symbols are standard for any product.

#define VR_FILEVERSION			VERSION_MAJOR, VERSION_MINOR, VERSION_INLINE, VERSION_BUILD_NUMBER
#define VR_PRODVERSION			VERSION_MAJOR, VERSION_MINOR, VERSION_INLINE, VERSION_BUILD_NUMBER
#define VR_FILEFLAGSMASK		0
#define VR_FILEFLAGS			0
#define VR_FILEOS				VOS__WINDOWS32
#ifndef _WINDLL
  #define VR_FILETYPE			VFT_APP
#else
  #define VR_FILETYPE			VFT_DLL
#endif
#define VR_FILESUBTYPE			VFT2_UNKNOWN
#define VR_FILEVERSIONSTRING	VERSION_STRING
#define VR_PRODUCTVERSIONSTRING VERSION_STRING
#define VR_LANGCHARSET			0x409, 0x4E4   // U.S. English, Multilingual Windows
#define VR_LANGCHARSETSTRING	"040904E4"     // U.S. English, Multilingual Windows


/////////////////////////////////////////////////////////////////////////////
// The following creates a version resource from all the VR_ symbols.
// For some reason, this will not work inside a header file included
// by an .rc file, but only when placed directly in an .rc file.
// So, make sure this is pasted into every component's .rc file
// below this header file.

#if 0

VS_VERSION_INFO VERSIONINFO
  FILEVERSION		VR_FILEVERSION
  PRODUCTVERSION	VR_PRODVERSION
  FILEFLAGSMASK		VR_FILEFLAGSMASK
  FILEFLAGS			VR_FILEFLAGS
  FILEOS			VR_FILEOS
  FILETYPE			VR_FILETYPE
  FILESUBTYPE		VR_FILESUBTYPE
BEGIN
  BLOCK "StringFileInfo"
  BEGIN
	BLOCK VR_LANGCHARSETSTRING
	BEGIN
	 VALUE "CompanyName",      VR_PUBLISHERNAME "\0"
	 VALUE "FileDescription",  VR_FILEDESCRIPTION "\0"
	 VALUE "FileVersion",      VR_FILEVERSIONSTRING "\0"
	 VALUE "InternalName",     VR_INTERNALNAME "\0"
	 VALUE "LegalCopyright",   VR_COPYRIGHT "\0"
	 VALUE "OriginalFilename", VR_ORIGINALFILENAME "\0"
	 VALUE "ProductName",      VR_PRODUCTNAME "\0"
	 VALUE "ProductVersion",   VR_PRODUCTVERSIONSTRING "\0"
	END
  END
  BLOCK "VarFileInfo"
  BEGIN
	VALUE "Translation", VR_LANGCHARSET
  END
END

#endif
