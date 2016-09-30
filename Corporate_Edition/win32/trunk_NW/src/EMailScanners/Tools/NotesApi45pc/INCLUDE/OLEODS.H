// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*	This header file contains typedefs and structures stored on disk
	by Notes and used in OLE */


#ifndef OLE_ODS_DEFS
#define OLE_ODS_DEFS


#ifndef ODS_DEFS
#include "ods.h"
#endif

#ifndef EDIT_ODS_DEFS
#include "editods.h"	/* for DDEFORMAT_??? definitions */
#endif

/*	Name of a form autolaunch item.  This optional item is created when
	designing a Notes form using the auto launch options. */

#define	FORM_AUTOLAUNCH_ITEM "$AUTOLAUNCH"

/*	Name of an OLE object item.  One of these is created for every
	OLE embedded object that exists in a Notes document.  This item
	is used to access OLE objects witout having to parse the
	Rich Text item within the document to find an OLE CD record */

#define OLE_OBJECT_ITEM "$OLEOBJINFO"


/*	On-disk structure of an OLE GUID.  This is taken directly from 
	OLE's compobj.h.  The reason it's copied rather than included
	here is to eliminate inclusion of the OLE2 header files, which
	without great pain, only compile on OLE platforms.  This header file
	is included on ALL Notes platforms, so we don't want to mess with the
	whole of OLE just for the GUID typedef... */

typedef struct 
	{
    DWORD Data1;
    WORD  Data2;
    WORD  Data3;
    BYTE  Data4[8];
	} OLE_GUID;


/*	Format of an on-disk autolaunch item. Most of the info contained in
	this structure refer to OLE autolaunching behaviors, but there are
	some  */

typedef struct
	{
	WSIG Header;			/* Signature and length of this record */
	DWORD ObjectType;		/* Type of object to launch, see OBJECT_TYPE_??? */
	DWORD HideWhenFlags; 	/* HIDE_ flags below */
	DWORD LaunchWhenFlags;	/* LAUNCH_ flags below */
	DWORD OleFlags;			/* OLE Flags below */
	DWORD CopyToFieldFlags; 	/* Field create flags below */
	DWORD Spare1;
	DWORD Spare2;
	WORD  FieldNameLength;  /* If named field, length of field name */
	OLE_GUID  OleObjClass;  	/* ClassID GUID of OLE object, if create new */
	/* Field Name, if used, goes here */	
	} CDDOCAUTOLAUNCH;


/* Autolaunch Object type flags */

#define AUTOLAUNCH_OBJTYPE_NONE			0x00000000
#define AUTOLAUNCH_OBJTYPE_OLE_CLASS	0x00000001 /* OLE Class ID (GUID) */
#define AUTOLAUNCH_OBJTYPE_OLEOBJ		0x00000002 /* First OLE Object */
#define AUTOLAUNCH_OBJTYPE_DOCLINK		0x00000004 /* First Notes doclink */
#define AUTOLAUNCH_OBJTYPE_ATTACH		0x00000008 /* First Attachment */
#define AUTOLAUNCH_OBJTYPE_URL			0x00000010 /* AutoLaunch the url in the URL field */


/*	Hide-when flags */

#define HIDE_OPEN_CREATE	0x00000001 /* Hide when opening flags */
#define HIDE_OPEN_EDIT		0x00000002
#define HIDE_OPEN_READ		0x00000004
#define HIDE_CLOSE_CREATE	0x00000008	/* Hide when closing flags */
#define HIDE_CLOSE_EDIT		0x00000010
#define HIDE_CLOSE_READ		0x00000020

/* 	Launch-when flags */

#define LAUNCH_WHEN_CREATE	0x00000001
#define LAUNCH_WHEN_EDIT	0x00000002
#define LAUNCH_WHEN_READ	0x00000004

/* OLE Flags */

#define OLE_EDIT_INPLACE	0x00000001
#define OLE_MODAL_WINDOW	0x00000002
#define OLE_ADV_OPTIONS		0x00000004

/* Field Location Flags */

#define FIELD_COPY_NONE		0x00000001	/* Don't copy obj to any field (V3 compatabile) */
#define FIELD_COPY_FIRST	0x00000002  /* Copy obj to first rich text field */
#define FIELD_COPY_NAMED	0x00000004  /* Copy obj to named rich text field */


typedef struct
	{
	WSIG Header;					/* Signature and length of this record */
	WORD FileObjNameLength;			/* Name of extendable $FILE object containing object data */
	WORD DescriptionNameLength;		/* Length of description of Object */
	WORD FieldNameLength;			/* Length of field name in which object resides */
	WORD TextIndexObjNameLength;	/* Name of the $FILE object containing LMBCS text for object */
	OLE_GUID  OleObjClass;  		/* OLE ClassID GUID of OLE object */
	WORD StorageFormat;				/* See below OLE_STG_FMT_??? */	
	WORD DisplayFormat;				/* Object's display format within document, DDEFORMAT_??? */
	DWORD Flags;					/* Object information flags, see OBJINFO_FLAGS_??? */
	WORD StorageFormatAppearedIn;	/* Version # of Notes, high byte=major, low byte=minor, for display purposes */
	WORD Reserved1;					/* Unused, must be 0 */
	WORD Reserved2;					/* Unused, must be 0 */
	WORD  Reserved3;				/* Unused, must be 0 */
	DWORD Reserved4;				/* Unused, must be 0 */
	/* The variable length portions go here in the following order:
	   FileObjectName
	   DescriptionName
	   Field Name in Document in which this object resides
	   Full Text index $FILE object name
	*/
	}CDOLEOBJ_INFO;

#define OBJINFO_FLAGS_SCRIPTED		0x00000001  /* Object is scripted */
#define OBJINFO_FLAGS_RUNREADONLY	0x00000002	/* Object is run in read-only mode */
#define OBJINFO_FLAGS_CONTROL		0x00000004	/* Object is a control */
#define OBJINFO_FLAGS_FITTOWINDOW	0x00000008	/* Object is sized to fit to window */


#define OLE_STG_FMT_STRUCT_STORAGE	1 /* OLE "Docfile" structured storage format */
#define OLE_STG_FMT_BENTO			2 /* Bento storage format */


#endif	/* OLE_ODS_DEFS */

