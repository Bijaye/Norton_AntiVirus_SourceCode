// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef NSF_OLEAPI_DEFS
#define NSF_OLEAPI_DEFS

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OLE_ODS_DEFS
#include "oleods.h"
#endif


/*	
	API Utility functions used to extract, create and delete OLE2 object storage blobs from Notes documents.
	These functions only deal with the storage-related objects that comprise OLE object storage in Notes documents-
	and do NOT address any Rich-text references or representations of the OLE objects.  It is assumed that the
	Rich Text (CDOLEBEGIN/CDOLEEND) objects are dealt with externally and separately from these functions.
*/


STATUS LNPUBLIC NSFNoteExtractOLE2Object(
								NOTEHANDLE hNote, 
								char *ObjectName,
								char *FileName,
								ENCRYPTION_KEY *pEncryptionKey,
								BOOL fOverwrite,
								DWORD Flags);
/*	Create a copy of an OLE2 object structured storage file, serialized to
	an On-disk file. This is The OLE2 structured storage file that Notes uses to contain a
	single OLE object.  The resulting structured storage file contains the "normal" OLE streams
	and a single child sub-storage for the OLE object.  The child storage is named by it's OLE ProgID
	which is what Notes uses to locate the storage when loading the object.

	Input:

		hNote
			Note handle of open Note

		ObjectName
			Name of the OLE $FILE object which is the "master" extendable file object ("ext***") in the
			set of $FILE objects that comprise this OLE object.

		char *FileName
			The file name, including path, into which the Storage file will be dumped.

		pEncryptionKey
			The Note's bulk data encryption key, acquired from NSFNoteDecrypt(...)

		fOverwrite
			TRUE to overwrite the file, if it already exists

		Flags
			Additional flags, unused at this time, must be 0

*/


STATUS LNPUBLIC NSFNoteDeleteOLE2Object(
								NOTEHANDLE hNote,
								char *ObjectName,
								BOOL fDeleteObjInfo,
								ENCRYPTION_KEY *pEncryptionKey,
								DWORD Flags);

/*	Delete an OLE2 structured storage object, which in Notes is stored as
	an extendable file object.  Also, optionally delete the associated
	$OLEOBJINFO item 

	Input:

		hNote
			Note handle of open Note

		ObjectName
			Name of the OLE $FILE object which is the "master" extendable file object ("ext***") in the
			set of $FILE objects that comprise this OLE object.

		fDeleteObjInfo
			True if the associated $OLEOBJINFO with this object should also be deleted

		pEncryptionKey
			The Note's bulk data encryption key, acquired from NSFNoteDecrypt(...). Necessary to decrypt
			the $FILE extent table.

		Flags
			Additional flags, unused at this time, must be 0

*/


STATUS LNPUBLIC NSFNoteAttachOLE2Object(
								NOTEHANDLE hNote,
								char *FileName,
								char *ObjectName,
								BOOL fCreateInfoItem,
								char *ObjDescription,
	                            char *FieldName,
								OLE_GUID  *OleObjClass,
								WORD DisplayFormat,
								BOOL fScripted,
								BOOL fOleControl,
								DWORD Flags);
/*
	Attach an OLE structured storage object to the specified Note, creating the
	OLE $FILE objects using the specified storage file.  Also, optionally create an
	$OLEOBJINFO note item using the specified parameters 

	Input:

		NOTEHANDLE hNote
			NSF Note Handle to open note

		char *FileName
			Input file name containing the OLE structured storage file

		char *ObjectName
			The name of the NSF $FILE extendable file object to create. This MUST
			match the one created in the CDOLEBEGIN record in the body of the
			document.

		BOOL fCreateInfoItem
			TRUE to create a $OLEOBJINFO item.  The remaining parameters are propogated
			to the OLEOBJINFO item:

		char *ObjDescription
			Object user description, i.e. "My Worksheet"

        char *FieldName
			Field name in which the OLE object resides within the document.

		OLE_GUID  *OleObjClass  (see OLEODS.H)
			The OLE object's GUID

		WORD DisplayFormat
			Visual rendering format, like bitmap, metafile,  DDEFORMAT_* from editods.h

		BOOL fScripted
			True if object is scripted (for ActiveX).  If this is true, it's up to the caller to attach
			the associated Lotus script source and object code to this note, using the following
			naming convention <xxxx>.lso for the Lotus Script compiled object code and <xxxx>.lss for the
			Lotus Script source, where "xxxx" is identical to the the ObjectName used above.  If Object name
			is "foo" then "foo.lss" has the lotus script source, and "foo.lso" has the lotus script object code.

		BOOL fOleControl
			True if this object is registered an OLE ActiveX

		DWORD Flags
			Must Be 0; future use
*/


#ifdef __cplusplus
}
#endif

#endif /* NSF_OLEAPI_DEFS */
