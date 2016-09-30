//****************************************************************************
// CustomCellSelectListItem.cpp: Implementation of CCustomCellSelectListItem 
//                               class.
//****************************************************************************
#include "stdafx.h"
#include "CustomCellSelectListItem.h"

CCustomCellSelectListItem::CCustomCellSelectListItem( 
                                                    const CString & initString,
                                                    const UINT initData ) :
    string(initString),
    data(initData)
{
}

CCustomCellSelectListItem::CCustomCellSelectListItem( 
                               const CCustomCellSelectListItem & sourceItem ) :
    string(sourceItem.string),
    data(sourceItem.data)
{
}

CCustomCellSelectListItem & CCustomCellSelectListItem::operator=(
                                 const CCustomCellSelectListItem & sourceItem )
{
    string = sourceItem.string;
    data = sourceItem.data;

	return *this;
}

const CString & CCustomCellSelectListItem::GetString() const
{ 
    return string;
}

const UINT CCustomCellSelectListItem::GetData() const
{
    return data;
}
