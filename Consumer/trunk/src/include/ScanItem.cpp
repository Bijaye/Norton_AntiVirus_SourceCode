////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanItem.cpp : implementation file
//
#include "stdafx.h"
#include "ScanItem.h"

using namespace avScanTask;

//-------------------------------------------------------------------------
CScanItem::CScanItem()
{
	m_Type = typeNone;
	m_SubType = subtypeNone;
	m_ulVID = 0;
}
//-------------------------------------------------------------------------
CScanItem::~CScanItem()
{
}
//-------------------------------------------------------------------------
void CScanItem::SetItemType(avScanTask::SCANITEMTYPE type, avScanTask::SCANITEMSUBTYPE subtype)
{
	m_Type = type;
	m_SubType = subtype;
}
//-------------------------------------------------------------------------
void CScanItem::GetItemType(avScanTask::SCANITEMTYPE& type, avScanTask::SCANITEMSUBTYPE& subtype) const
{
	type = m_Type;
	subtype = m_SubType;
}
//-------------------------------------------------------------------------
bool CScanItem::SetDrive(const WCHAR chDrive)
{
	if (m_Type != typeDrive)
		return false;

	m_chDrive = chDrive;

	return true;
}
//-------------------------------------------------------------------------
WCHAR CScanItem::GetDrive() const
{
	if(m_Type != typeDrive)
		return (WCHAR) -1;

	return m_chDrive;
}
//-------------------------------------------------------------------------
bool CScanItem::SetPath(LPCWSTR szPath)
{
	if((m_Type != typeFile) && 
		(m_Type != typeFolder) && 
		(m_Type != typeFileVID) && 
		(m_Type != typeFileVIDCompressed))
		return false;

	m_sPath = szPath;

	return true;
}
//-------------------------------------------------------------------------
LPCWSTR CScanItem::GetPath() const
{
	if((m_Type != typeFile) && 
		(m_Type != typeFolder) && 
		(m_Type != typeFileVID) && 
		(m_Type != typeFileVIDCompressed))
		return NULL;

	return m_sPath;
}
//-------------------------------------------------------------------------
bool CScanItem::SetVID(const ULONG ulVID)
{
	if((m_Type != typeFileVID) && 
		(m_Type != typeFileVIDCompressed))
		return false;

	m_ulVID = ulVID;

	return true;
}
//-------------------------------------------------------------------------
ULONG CScanItem::GetVID() const
{
	if((m_Type != typeFileVID) && 
		(m_Type != typeFileVIDCompressed))
		return 0;

	return m_ulVID;
}
//-------------------------------------------------------------------------
bool CScanItem::operator==(const CScanItem& rhs) const
{
	return ((this == &rhs) ||
		(m_Type == rhs.m_Type && (typeDrive == m_Type && m_chDrive == rhs.m_chDrive ||
		typeDrive != m_Type && 0 == m_sPath.Compare(rhs.m_sPath))));
}

