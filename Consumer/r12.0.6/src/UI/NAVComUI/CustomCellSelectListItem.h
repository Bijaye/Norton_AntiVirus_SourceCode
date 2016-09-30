//****************************************************************************
// CustomCellSelectListItem.h: Interface for CCustomCellSelectListItem class.
//****************************************************************************
#pragma once

class CCustomCellSelectListItem
{
private:
    CString  string;
    UINT     data;

public:
    CCustomCellSelectListItem( const CString & initString, 
                               const UINT initData );
    CCustomCellSelectListItem( const CCustomCellSelectListItem & sourceItem);

    CCustomCellSelectListItem &
    operator=(const CCustomCellSelectListItem & sourceItem);

    const CString &  GetString() const;
    const UINT       GetData() const;
};
