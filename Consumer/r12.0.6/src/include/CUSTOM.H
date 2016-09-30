// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/Custom.h_v   1.1   10 Jun 1997 11:08:42   DBUCHES  $
//
// Description:
//      This include file defines the Custom Response options structure.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/Custom.h_v  $
// 
//    Rev 1.1   10 Jun 1997 11:08:42   DBUCHES
// Added 1 byte packing pragma.
// 
//    Rev 1.0   01 May 1997 15:09:10   JBRENNA
// Initial revision.
//
//************************************************************************

#ifndef CUSTOM_H_INCLUDED
#define CUSTOM_H_INCLUDED

#ifdef SYM_WIN
#pragma pack(push, 1)
#endif
typedef struct tagCUSTOMRESPONSE
{
	BOOL bSelected;		//TRUE if the custom response was selected from the list
    BYTE uchMacro;
    BYTE uchFile;
    BYTE uchBoot;
} CUSTOMRESPONSE, * PCUSTOMRESPONSE, FAR* LPCUSTOMRESPONSE;

#ifdef SYM_WIN
#pragma pack(pop)
#endif

#endif // CUSTOM_H_INCLUDED
