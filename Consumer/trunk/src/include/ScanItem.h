////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanItem.h : header/implementation file
//
#pragma once

#include "ScanTaskInterface.h"

class CScanItem
{
public:
	// Constructor.
	CScanItem();
	virtual ~CScanItem();

public:
	// Sets the item type/subtype.
	void SetItemType(avScanTask::SCANITEMTYPE type, avScanTask::SCANITEMSUBTYPE subtype);

	// Retrieves the item type/subtype.
	void GetItemType(avScanTask::SCANITEMTYPE& type, avScanTask::SCANITEMSUBTYPE& subtype) const;

	// Sets the drive letter (if the item is a drive).
	bool SetDrive(const WCHAR chDrive);

	// Gets the drive letter (if the item is a drive).
	WCHAR CScanItem::GetDrive() const;

	// Sets the items path (if the item is a file or folder).
	bool CScanItem::SetPath(LPCWSTR szPath);

	// Returns the items path (if the item is a file or folder).
	LPCWSTR CScanItem::GetPath() const;

	// Sets the items VID (if the item is a filename\VID combo).
	bool CScanItem::SetVID(const ULONG ulVID);

	// Returns the items VID (if the item is a filename\VID combo).
	ULONG CScanItem::GetVID() const;

	// for find()
	bool CScanItem::operator==(const CScanItem& rhs) const;

protected:
	// Type of item.
	avScanTask::SCANITEMTYPE m_Type;

	// Sub-type of item.
	avScanTask::SCANITEMSUBTYPE m_SubType;

	// Uppercase drive letter (if the item is a drive).
	WCHAR m_chDrive;

	// Item path (if the item is a file or folder).
	CStringW m_sPath;

	// VID if its a filename\VID pair
	ULONG m_ulVID;
};
