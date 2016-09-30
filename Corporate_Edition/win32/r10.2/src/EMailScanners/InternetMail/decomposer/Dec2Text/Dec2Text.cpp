// Dec2Text.cpp : Decomposer Engine for text-based data formats
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// Headers

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "SymSaferStrings.h"

// Project headers
#define DEC2TEXT_CPP
#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"
#include "DecIO.h"
#include "TextFile.h"
#include "Dec2Text.h"

/////////////////////////////////////////////////////////////////////////
// CTextEngine::SetMaxExtractSize()

DECRESULT CTextEngine::SetMaxExtractSize(DWORD dwMaxSize)
{
	if(!dwMaxSize || dwMaxSize > MAX_FILE_SIZE)
	{
		dwMaxSize = MAX_FILE_SIZE;
	}

	TextSetMaxExtractSize(dwMaxSize);
	return (DEC_OK);
}


/////////////////////////////////////////////////////////////////////////
// CTextEngine::AbortProcess()

DECRESULT CTextEngine::AbortProcess(bool bAbort)
{
	TextAbortProcess(bAbort);
	return (DEC_OK);
}


/////////////////////////////////////////////////////////////////////////
// CTextEngine::SetIOCallback()

DECRESULT CTextEngine::SetIOCallback(IDecIOCB *pIOCallback)
{
	DecSetIOCallback(pIOCallback);
	return (DEC_OK);
}


DECRESULT CTextEngine::TextCreateTempFile(const char *szExtension, char *pszFile, IDecomposerEx *pDecomposer)
{
	return (CreateTempFile(szExtension, pszFile, pDecomposer));
}


/////////////////////////////////////////////////////////////////////////
// CTextEngine::Process()

DECRESULT CTextEngine::Process(
	IDecomposerEx		*pDecomposer,
	IDecContainerObjectEx *pObject,
	IDecEventSink		*pSink,
	WORD				*pwResult,
	char				*pszNewDataFile,
	bool				*pbIdentified,
	bool				*pbContinue)
{
	DECRESULT	hr, hr2;
	DWORD		dwType;
	FILE		*fp = NULL;
	char		*pszDataFile;
	bool		bContinueWithChildren;
	void		*pInterface = NULL;
	CTextFile	archive;

	// Initialize all outputs.
	*pbIdentified = false;
	*pbContinue = false;
	*pwResult = DECRESULT_UNCHANGED;
	*pszNewDataFile = '\0';

	pszDataFile = (char *)malloc(MAX_PATH);
	if (!pszDataFile)
		return DECERR_OUT_OF_MEMORY;

	// Get the data file.
	pObject->GetDataFile(pszDataFile, NULL);

	// Open the file.
	fp = pObject->OpenDataFile(pszDataFile, "rb", NULL, NULL, NULL);
	if (fp == NULL)
	{
		hr = DECERR_CONTAINER_OPEN;
		goto done_process;
	}

	if (FAILED(pDecomposer->GetOption(DEC_OPTION_EVENTSINK_VERSION, &m_dwEventSinkVersion)))
	{
		//
		// If this property is not present, then default to EVENT_SINK_VERSION_1.
		//
		m_dwEventSinkVersion = EVENT_SINK_VERSION_1;
	}

	// Check if the Decomposer data object
	// represents a data file we know how to process.
	// If successful, this will open the Text file.
	dwType = DEC_TYPE_UNKNOWN;
	hr = OpenTextFile(pDecomposer, pObject, &archive, fp, &dwType);
	if (FAILED(hr))
		goto done_process;

	// If we don't recognize the data file,
	// return and allow other engines to try.
	if (dwType == DEC_TYPE_UNKNOWN)
	{
		*pbContinue = true;
		hr = DEC_OK;
		goto done_process;
	}

	*pbIdentified = true;

	// Set the container type identifier.
	pObject->SetContainerType(dwType);

	archive.GetInterface(&pInterface);
	pObject->SetEngineInterface(pInterface);

	
#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	// We don't support attributes so finalize the current list.

	pObject->FinalizeAttributes();
#endif //*** End AS400 Attribute API Removal *** 

	// Make an event sink callback.
	bContinueWithChildren = true;
	hr = pSink->OnContainerIdentified(pObject, &bContinueWithChildren);
	if (FAILED(hr) || !bContinueWithChildren)
		goto done_process;

	// Process each contained data file with Decomposer.
	hr = ProcessChildren(pDecomposer,
						pObject,
						pSink,
						&archive,
						pwResult,
						pszNewDataFile);
	if (SUCCEEDED(hr))
	{
		// Notify the caller that we have finished processing all items
		// inside the container and give the caller an opportunity to
		// insert new items.
		hr = pSink->OnContainerEnd(pObject, NULL);
	}

done_process:
	// Close the Text file
	hr2 = CloseTextFile(&archive, pwResult, pszNewDataFile);
	if (FAILED(hr2) && !FAILED(hr))
		hr = DECERR_CONTAINER_CLOSE;

	if (pszDataFile)
		free(pszDataFile);

	fp = NULL;
	return hr;
}


/////////////////////////////////////////////////////////////////////////
// CTextEngine::ProcessChildren

DECRESULT CTextEngine::ProcessChildren(
	IDecomposerEx		*pDecomposer,
	IDecContainerObjectEx *pObject,
	IDecEventSink		*pSink,
	CTextFile			*pTextFile,
	WORD				*pwResult,
	char				*pszNewDataFile)
{
	char			*pszChildName = NULL;
	char			*pszSecondaryName = NULL;
	TEXT_FIND_DATA	tfd;
	memset(&tfd,0,sizeof(tfd));
	DECRESULT		hr = DEC_OK;

	size_t	attributeIndex = 0;

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 

	size_t count = pObject->GetNumberOfAttributeCollections();

	IDecAttributeCollection* attrPtr = NULL;

	// Find the second MIME attribute collection.  The first MIME
	// attribute collection represents US. The following ones represent
	// out children
	for ( ; attributeIndex < count; attributeIndex++)
	{
		attrPtr = pObject->GetAttributeCollectionAt(attributeIndex);
		if ( attrPtr != NULL && attrPtr->GetType() == IDecAttributeCollection::eCMIMEAttributeCollection )
		{
			attributeIndex++;
			break;
		}
	}
#endif //*** End AS400 Attribute API Removal *** 

try_next_engine:
	// Get the first Text file entry.
	hr = pTextFile->GetFirstEntry(&tfd);
	if (hr == DECERR_CHILD_ACCESS)
	{
		hr = DEC_OK;  // There are no more engines that have identified the data.
		goto cleanUp;
	}

	if (hr != DEC_OK)
		goto try_next_engine;

	// While we have a valid entry...
	while (hr == DEC_OK)
	{
		// Make an event sink callback.
		hr = pSink->OnBusy();
		if (FAILED(hr))
		{
			goto cleanUp;
		}
 
		// Get the name of the entry.
		dec_assert(tfd.pszName);
		pszChildName = (char *)malloc(strlen(tfd.pszName) + 1);
		if (!pszChildName)
		{
			hr = DECERR_OUT_OF_MEMORY;
			goto cleanUp;
		}

		strcpy(pszChildName, tfd.pszName);

		// Get the secondary name if present...
		if (tfd.pszSecondaryName)
		{
			pszSecondaryName = (char *)malloc(strlen(tfd.pszSecondaryName) + 1);
			if (!pszSecondaryName)
			{
				hr = DECERR_OUT_OF_MEMORY;
				goto cleanUp;
			}

			strcpy(pszSecondaryName, tfd.pszSecondaryName);
			pObject->SetSecondaryName(pszSecondaryName);
		}
		else
		{
			pObject->SetSecondaryName("");
		}

		pObject->SetNameCharacterSet(tfd.dwNameCharset);
		pObject->SetDate(tfd.dwYear, tfd.dwMonth, tfd.dwDay);
		pObject->SetTime(tfd.dwHour, tfd.dwMinute, tfd.dwSecond, tfd.dwMillisecond);

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
		DWORD dwAttrs;
		dwAttrs = tfd.dwAttributes;
		pObject->SetAttributes(dwAttrs, tfd.dwAttrType);
#endif //*** End AS400 Attribute API Removal *** 

		// Get the uncompressed file size of this entry so that we can
		// make this information available to the client.
		DWORDLONG dwlUncompressedSize = DEC_CHILD_SIZE_UNKNOWN;
		DWORDLONG dwlCompressedSize = DEC_CHILD_SIZE_UNKNOWN;

		if (tfd.nUncompressedSize != (size_t)-1)
			dwlUncompressedSize = (DWORDLONG)tfd.nUncompressedSize;
		if (tfd.nCompressedSize != (size_t)-1)
			dwlCompressedSize = (DWORDLONG)tfd.nCompressedSize;

		// Save this information for the current container object.
		// The client can call GetUncompressedSize to retrieve this
		// information during the OnChildFound callback.
		pObject->SetUncompressedSize (dwlUncompressedSize);
		pObject->SetCompressedSize (dwlCompressedSize);

		// Make an event sink callback.
		bool bContinueWithChild = true;
		if (EVENT_SINK_VERSION_1 == m_dwEventSinkVersion)
		{
			hr = pSink->OnChildFound(pObject,
									tfd.dwType,
									pszChildName,
									tfd.dwNameCharset,
									&bContinueWithChild);
		}
		else if (EVENT_SINK_VERSION_2 == m_dwEventSinkVersion)
		{
			hr = pSink->OnChildFound2(pObject,
									 tfd.dwType,
									 pszChildName,
									 DEC_NAMETYPE_EXTERNAL,
									 tfd.dwNameCharset,
									 &bContinueWithChild);
		}
		else
		{
			hr = DECERR_INTERFACE_NOT_SUPPORTED;
		}

		if (FAILED(hr))
		{
			goto cleanUp;
		}

		bool bContinueWithChildren = true;
		if (bContinueWithChild)
		{
			// Process the Text file entry.
			hr = ProcessChild(pDecomposer,
							 pObject,
							 pSink,
							 pTextFile,
							 &tfd,
							 pszChildName,
							 pwResult,
							 pszNewDataFile,
							 &bContinueWithChildren,
							 attributeIndex);
			
			if (FAILED(hr))
			{
				goto cleanUp;
			}

			if (!bContinueWithChildren || *pwResult == DECRESULT_TO_BE_DELETED)
				break;
		}
		else
		{
			// Skip over the child's data so we can get to the next entry.
			hr = pTextFile->SkipExtractEntry(&tfd);
			if (FAILED(hr))
			{
				goto cleanUp;
			}
		}

		// Clean up for the next child
		FreeFindData(&tfd);
		
		if (pszChildName)
		{
			free(pszChildName);
			pszChildName = NULL;
		}

		if (pszSecondaryName)
		{
			free(pszSecondaryName);
			pszSecondaryName = NULL;
		}

		// Get the next Text file entry.
		hr = pTextFile->GetNextEntry(&tfd);
	}

	// OK, we have finished enumerating all the items for one engine that
	// identified the data.  Now loop back to the top and see if any other
	// engine also identified the data.  This loop is terminated when
	// the GetFirstEntry call fails to return any more items.
	goto try_next_engine;

cleanUp:
	
	// Clean up for the next child
	FreeFindData(&tfd);

	if (pszChildName)
	{
		free(pszChildName);
		pszChildName = NULL;
	}

	if (pszSecondaryName)
	{
		free(pszSecondaryName);
		pszSecondaryName = NULL;
	}

	return hr;
}


void CTextEngine::FreeFindData(TEXT_FIND_DATA *ptfd)
{
	if (ptfd->pszName)
	{
		free(ptfd->pszName);
		ptfd->pszName = NULL;
	}

	if (ptfd->pszSecondaryName)
	{
		free(ptfd->pszSecondaryName);
		ptfd->pszSecondaryName = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////
// CTextEngine::ProcessChild

DECRESULT CTextEngine::ProcessChild(
	IDecomposerEx		*pDecomposer,
	IDecContainerObjectEx *pObject,
	IDecEventSink		*pSink,
	CTextFile			*pTextFile,
	TEXT_FIND_DATA		*pfd,
	const char			*pszChildName,
	WORD				*pwResult,
	char				*pszNewDataFile,
	bool				*pbContinueWithChildren,
	size_t&				attributeIndex)
{
	DECRESULT			 hr = DEC_OK;

	*pbContinueWithChildren = false;

	// Create a temporary file to serve as the data file for the child object.
	// Use the extension from the child's name, as it may be significant
	// for the virus scanning engine.
	char szExtension[MAX_PATH];
	szExtension[0] = 0;
	FindNativeExtension(pszChildName, DEC_NAMETYPE_EXTERNAL, szExtension);

	char szChildDataFile[MAX_PATH];
	szChildDataFile[0] = 0;
	hr = CreateTempFile(szExtension, szChildDataFile, pDecomposer);
	if (FAILED(hr))
		return DECERR_TEMPFILE_CREATE;

	// Extract the child from its container to the temporary file.
	hr = pTextFile->ExtractEntry(szChildDataFile);
	if (FAILED(hr))
	{
		// Delete the child's temporary data file.
		dec_remove(szChildDataFile);

		// Make an event sink callback.
		if (EVENT_SINK_VERSION_1 == m_dwEventSinkVersion)
		{
			hr = pSink->OnChildBad(pObject, hr, pszChildName, pbContinueWithChildren);
		}
		else if (EVENT_SINK_VERSION_2 == m_dwEventSinkVersion)
		{
			hr = pSink->OnChildBad2(pObject, hr, pszChildName, DEC_NAMETYPE_EXTERNAL, pbContinueWithChildren);
		}
		else
		{
			hr = DECERR_INTERFACE_NOT_SUPPORTED;
		}
		
		return hr;
	}

	bool bCanReplace;
	pObject->GetCanReplace(&bCanReplace);

	// Create a Decomposer data object for the child.
	// We can only create a replacement Text file,
	// so we can only accept changes or replacements to the child
	// if we have permission to replace the parent.
	CDecObj ChildObject(pObject);
	ChildObject.SetName(pszChildName);

	// Here we copy the secondary name from the parent object to the
	// new child object.  The only reason for doing this is so that when
	// we call OnObjectBegin during processing of the child, the secondary
	// name is immediately available via a call to GetSecondaryName.
	char szChildNewDataFile[MAX_PATH];
	pObject->GetSecondaryName(szChildNewDataFile, NULL, NULL);
	ChildObject.SetSecondaryName(szChildNewDataFile);
	ChildObject.SetNameType(DEC_NAMETYPE_EXTERNAL);
	ChildObject.SetDataFile(szChildDataFile);
	ChildObject.SetCanDelete(bCanReplace);
	ChildObject.SetCanModify(bCanReplace);
	ChildObject.SetCanReplace(bCanReplace);
	ChildObject.SetCanRename(bCanReplace);

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	CGenericAttributeCollection* childAttr = (CGenericAttributeCollection*) ChildObject.GetAttributeCollectionAt(0);

	if ( childAttr != NULL )
	{
		IDecAttributeCollection* baseAttr = pObject->GetAttributeCollectionAt(attributeIndex);

		if ( baseAttr != NULL && baseAttr->GetType() == IDecAttributeCollection::eCMIMEAttributeCollection )
		{
			CMIMEAttributeCollection* MIMEAttr = (CMIMEAttributeCollection*)baseAttr;

			unsigned long parentId = MIMEAttr->GetId();
			char idString[32];
			size_t idStringLength = (size_t)sssnprintf(&idString[0],sizeof(idString),"%d",parentId);

			CDecTextGeneric* idAttr = new CDecTextGeneric(CDecAttributeKey::kParentAttributeCollectionId,
															&idString[0],idStringLength,IDecAttribute::CopyAndAdopt);
			if ( idAttr != NULL )
			{
				childAttr->AddAttribute(idAttr);
			}
		}
	}

	attributeIndex++;
#endif //*** End AS400 Attribute API Removal *** 

	// Set the character set for this object's name.
	ChildObject.SetNameCharacterSet(pTextFile->m_dwFilenameCharset);

	// Set the item type
	ChildObject.SetItemType(pTextFile->m_dwItemType);

	// Process the child with Decomposer.
	WORD wChildResult;
	hr = pDecomposer->DecProcess((IDecContainerObjectEx*)&ChildObject,
								pSink,
								&wChildResult,
								szChildNewDataFile);

	// Take the child processing results and
	// make any required changes to the parent.
	bool bKeepChildDataFile = false;
	if (SUCCEEDED(hr))
	{
		// Save the ChildObject's container type.
		// This allows the MIME engine to know when
		// it does not need to re-encode the data with base64.
		ChildObject.GetContainerType(&pTextFile->m_dwChildType);

		hr = ApplyChildResults(pDecomposer,
							  pObject,
							  pTextFile,
							  pfd,
							  szChildDataFile,
							  wChildResult,
							  szChildNewDataFile,
							  pwResult,
							  pszNewDataFile,
							  &bKeepChildDataFile);
		if (SUCCEEDED(hr))
			*pbContinueWithChildren = true;
	}

	// Delete the child's temporary data file.
	if(!bKeepChildDataFile)
		dec_remove(szChildDataFile);

	return hr;
}


/////////////////////////////////////////////////////////////////////////
// CTextEngine::ApplyChildResults()

DECRESULT CTextEngine::ApplyChildResults(
	IDecomposerEx		*pDecomposer,
	IDecContainerObjectEx *pObject,
	CTextFile			*pTextFile,
	TEXT_FIND_DATA		*pfd,
	const char			*szChildDataFile,
	WORD				wChildResult,
	const char			*szChildNewDataFile,
	WORD				*pwResult,
	char				*pszNewDataFile,
	bool				*pbKeepChildDataFile)
{
	DECRESULT			hr = DEC_OK;

	bool				bCanReplace;
	bool				bCanDelete, bCanDeleteAncestor;
	DWORD				dwDepth;

	pObject->GetCanReplace(&bCanReplace);
	pObject->GetCanDelete(&bCanDelete);
	pObject->GetCanDeleteAncestor(&bCanDeleteAncestor, &dwDepth);

	switch (wChildResult)
	{
	case DECRESULT_UNCHANGED:
		break;

	case DECRESULT_CHANGED:
		if (bCanReplace)
		{
			hr = PrepareNewDataFile(pDecomposer, pObject, pTextFile, pwResult, pszNewDataFile);
			if (SUCCEEDED(hr))
			{
				char	*pszNewName = NULL;
				char	*pszContentType = NULL;
				TEXT_CHANGE_DATA tcd;

				// See if the client wants us to rename the item as well...
				pObject->GetRenameItemName (&pszNewName);

				if (!pszNewName || (pszNewName && *pszNewName == '\0'))
				{
					tcd.bRenameItem = false;
					pszNewName = pfd->pszName;
				}
				else
				{
					tcd.bRenameItem = true;
				}

				// And/or change the Content-Type...?
				pObject->GetMIMEItemContentType (&pszContentType);

				// Copy information from the TEXT_FIND_DATA structure
				// into the TEXT_CHANGE_DATA structure to give all this
				// data back to the sub-engine so that it can re-construct
				// the item with the correct information.
				tcd.dwType = pfd->dwType;
				tcd.dwYear = pfd->dwYear;
				tcd.dwMonth = pfd->dwMonth;
				tcd.dwDay = pfd->dwDay;
				tcd.dwHour = pfd->dwHour;
				tcd.dwMinute = pfd->dwMinute;
				tcd.dwSecond = pfd->dwSecond;
				tcd.dwMillisecond = pfd->dwMillisecond;
				tcd.dwAttributes = pfd->dwAttributes;
				tcd.pszFilename = szChildDataFile;
				tcd.pszName = pszNewName;
				tcd.pszContentType = pszContentType;
				tcd.pszData = NULL;
				tcd.bRawChange = false;
				hr = pTextFile->ReplaceEntry(&tcd, pbKeepChildDataFile, NULL);
				if (SUCCEEDED(hr))
					*pwResult = DECRESULT_TO_BE_REPLACED;
				else
					hr = DECERR_CONTAINER_UPDATE;
			}
		}
		else
			hr = DECERR_INVALID_CHANGE;
		break;

	case DECRESULT_TO_BE_REPLACED:
		if (bCanReplace)
		{
			//
			// As of now, only the HQX engine uses this flag.  That engine always wants
			// to preserve the pszChildNewDataFile during a replace operation.
			//
			bool bKeepChildNewDataFile = false;

			hr = PrepareNewDataFile(pDecomposer, pObject, pTextFile, pwResult, pszNewDataFile);
			if (SUCCEEDED(hr))
			{
				char	*pszNewName = NULL;
				char	*pszContentType = NULL;
				TEXT_CHANGE_DATA tcd;

				// See if the client wants us to rename the item as well...
				pObject->GetRenameItemName (&pszNewName);
				if (!pszNewName || (pszNewName && *pszNewName == '\0'))
				{
					tcd.bRenameItem = false;
					pszNewName = pfd->pszName;
				}
				else
				{
					tcd.bRenameItem = true;
				}

				// And/or change the Content-Type...?
				pObject->GetMIMEItemContentType (&pszContentType);

				// Copy information from the TEXT_FIND_DATA structure
				// into the TEXT_CHANGE_DATA structure to give all this
				// data back to the sub-engine so that it can re-construct
				// the item with the correct information.
				tcd.dwType = pfd->dwType;
				tcd.dwYear = pfd->dwYear;
				tcd.dwMonth = pfd->dwMonth;
				tcd.dwDay = pfd->dwDay;
				tcd.dwHour = pfd->dwHour;
				tcd.dwMinute = pfd->dwMinute;
				tcd.dwSecond = pfd->dwSecond;
				tcd.dwMillisecond = pfd->dwMillisecond;
				tcd.dwAttributes = pfd->dwAttributes;
				tcd.pszFilename = szChildNewDataFile;
				tcd.pszName = pszNewName;
				tcd.pszContentType = pszContentType;
				tcd.pszData = NULL;
				tcd.bRawChange = false;
				hr = pTextFile->ReplaceEntry(&tcd, NULL, &bKeepChildNewDataFile);
				if (SUCCEEDED(hr))
					*pwResult = DECRESULT_TO_BE_REPLACED;
				else
					hr = DECERR_CONTAINER_UPDATE;
			}

			if (!bKeepChildNewDataFile)
				dec_remove(szChildNewDataFile);
		}
		else
			hr = DECERR_INVALID_REPLACE;
		break;

	case DECRESULT_TO_BE_DELETED:
		if (bCanReplace)
		{
			hr = PrepareNewDataFile(pDecomposer, pObject, pTextFile, pwResult, pszNewDataFile);
			if (SUCCEEDED(hr))
			{
				TEXT_CHANGE_DATA	tcd;

				tcd.dwType = pfd->dwType;
				tcd.dwYear = pfd->dwYear;
				tcd.dwMonth = pfd->dwMonth;
				tcd.dwDay = pfd->dwDay;
				tcd.dwHour = pfd->dwHour;
				tcd.dwMinute = pfd->dwMinute;
				tcd.dwSecond = pfd->dwSecond;
				tcd.dwMillisecond = pfd->dwMillisecond;
				tcd.dwAttributes = pfd->dwAttributes;
				tcd.pszFilename = NULL;
				tcd.pszName = NULL;
				tcd.pszData = NULL;
				tcd.bRawChange = false;
				hr = pTextFile->DeleteEntry(&tcd);
				if (SUCCEEDED(hr))
					*pwResult = DECRESULT_TO_BE_REPLACED;
				else
					hr = DECERR_CONTAINER_UPDATE;
			}
		}
		else if (bCanDelete || bCanDeleteAncestor)
		{
			*pwResult = DECRESULT_TO_BE_DELETED;
			hr = DEC_OK;
		}
		else
			hr = DECERR_INVALID_DELETE;
		break;

	default:
		hr = DECERR_GENERIC;
		break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////
// CTextEngine::PrepareNewDataFile()
//
// Called just before every change to pTextFile.
// Create the pTextFile output and temporary file, if not already done.

DECRESULT CTextEngine::PrepareNewDataFile(
	IDecomposerEx		*pDecomposer,
	IDecContainerObjectEx *pObject,
	CTextFile			*pTextFile,
	WORD				*pwResult,
	char				*pszNewDataFile)
{
	if (*pwResult == DECRESULT_UNCHANGED)
	{
		char szName[MAX_PATH];
		szName[0] = 0;
		pObject->GetName(szName, NULL, NULL);

		DWORD dwNameType = DEC_NAMETYPE_UNKNOWN;
		pObject->GetNameType(&dwNameType);

		char szExtension[MAX_PATH];
		szExtension[0] = 0;
		FindNativeExtension(szName, dwNameType, szExtension);

		// Create a file to serve as the replacement Text file.
		DECRESULT hr = CreateTempFile(szExtension, pszNewDataFile, pDecomposer);
		if (FAILED(hr))
			return DECERR_TEMPFILE_CREATE;

		// Assign the name of the replacement output file for this Text file.
		hr = pTextFile->BeginChanges(pszNewDataFile);
		if (FAILED(hr))
			return DECERR_CONTAINER_UPDATE;

		return DEC_OK;
	}
	else
		return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////
// CTextEngine::OpenTextFile()

DECRESULT CTextEngine::OpenTextFile(
	IDecomposerEx		*pDecomposer,
	IDecContainerObjectEx *pObject,
	CTextFile			*pTextFile,
	FILE				*fp,
	DWORD				*pdwType)
{
	DECRESULT	hr = DEC_OK;
	DWORD		dwValue;

	dec_assert(pdwType);
	*pdwType = DEC_TYPE_UNKNOWN;

	// Set any global Text engine options here...
	dwValue = 0;
	if (SUCCEEDED(pDecomposer->GetOption(DEC_OPTION_ENHANCED_TEXT_ID, &dwValue)))
		pTextFile->m_dwEnhancedID = dwValue;

	dwValue = 0;
    if (SUCCEEDED(pDecomposer->GetOption(DEC_OPTION_LOG_WARNINGS, &dwValue)))
        TextSetLogWarnings(dwValue ? true : false);

	// Performance enhancement.  Normally we want to run this check, but it can be
	// disabled via the TEXT_OPTION_NO_TEXT_FORMAT_FILTER option.
	if (!(pTextFile->m_dwEnhancedID & TEXT_OPTION_NO_TEXT_FORMAT_FILTER))
	{
		// If another engine has already set the object's type property
		// and the type is not in our supported list,
		// then don't process this object.
		DWORD types[] = { DEC_TYPE_MIME, DEC_TYPE_UUE, DEC_TYPE_HQX, DEC_TYPE_MBOX,
						 DEC_TYPE_TEXT, 0 };
		bool  bContinue = false;  // Assume the engine will not need to process the object.
		DWORD dwType;

		// If the object has a type property...
		if (SUCCEEDED(pObject->GetProperty("_GUESSTYPE", dwType)) &&
			dwType != DEC_TYPE_UNKNOWN)
		{
			// If the object's type is in the engine's support type list,
			// then the engine will need to process the object.
			for (int i = 0; types[i] != 0; i++)
			{
				if (dwType == types[i])
				{
					bContinue = true;
					break;
				}
			}
		}
		else
		{
			// The engine will need to process the object.
			bContinue = true;
		}

		if (!bContinue)
			return DEC_OK;
	}

	// Attempt to identify the data.  Return values:
	// S_OK = Data is recognized.
	// S_FALSE = Data is not recognized.
	// Anything else = An internal error occurred.
	hr = pTextFile->Open(fp, pDecomposer, this, pObject, pdwType);
	if (hr == S_OK)
		hr = DEC_OK;
	else if (hr == S_FALSE)
		hr = DEC_OK;
	else
		hr = DECERR_CONTAINER_OPEN;

	return hr;
}


/////////////////////////////////////////////////////////////////////////
// CTextEngine::CloseTextFile()

DECRESULT CTextEngine::CloseTextFile(
	CTextFile *pTextFile,
	WORD *pwResult,
	char *pszNewDataFile)
{
	DECRESULT			 hr;

	if (!pTextFile)
		return DECERR_OBJECT_FILE_INVALID;

	hr = pTextFile->Close(pwResult, pszNewDataFile);
	if (FAILED(hr))
		return DECERR_CONTAINER_CLOSE;

	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////
// DecNewTextEngine()

DECTEXTLINKAGE DECRESULT DecNewTextEngine(IDecEngine **ppEngine)
{
	try
	{
		*ppEngine = new CTextEngine;
		return DEC_OK;
	}
	catch(...)
	{
		*ppEngine = NULL;
		return DECERR_OUT_OF_MEMORY;
	}
}


/////////////////////////////////////////////////////////////////////////
// DecDeleteTextEngine()

DECTEXTLINKAGE DECRESULT DecDeleteTextEngine(IDecEngine *pEngine)
{
#if defined(UNIX)
	delete (CTextEngine*)(pEngine);
#else
	delete static_cast<CTextEngine*>(pEngine);
#endif
	return DEC_OK;
}


#if defined(SYM_PLATFORM_HAS_DLLS) || defined(SYM_PLATFORM_HAS_SHAREDOBJS)
DECTEXTLINKAGE DECRESULT DecNewEngine(IDecEngine **ppEngine)
{
	try
	{
		*ppEngine = new CTextEngine;
		return DEC_OK;
	}
	catch(...)
	{
		*ppEngine = NULL;
		return DECERR_OUT_OF_MEMORY;
	}
}


DECTEXTLINKAGE DECRESULT DecDeleteEngine(IDecEngine *pEngine)
{
#if defined(UNIX)
	delete (CTextEngine*)(pEngine);
#else
	delete static_cast<CTextEngine*>(pEngine);
#endif
	return DEC_OK;
}
#endif

