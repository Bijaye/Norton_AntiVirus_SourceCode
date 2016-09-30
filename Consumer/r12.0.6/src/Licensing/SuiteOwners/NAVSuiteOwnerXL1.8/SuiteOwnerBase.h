// NAVSuiteOwner.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_NAVSuiteOwner_H__389D63C0_68BD_4827_8434_5B8B43101FA6__INCLUDED_)
#define _NAVSuiteOwner_H__389D63C0_68BD_4827_8434_5B8B43101FA6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////
// ISuiteOwner
class ISuiteOwner: public SIMON::IInterfaceManagement
{
public:
    SIMON_STDMETHOD(SomeMethod)(DWORD) = 0;
};

// {8C20674F-42E7-4092-A56F-37233E231110}
DEFINE_SIMON_GUID(IID_ISuiteOwner, 0x8C20674F,0x42E7,0x4092,0xA5,0x6F,0x37,0x23,0x3E,0x23,0x11,0x10);

//====================================================================================================
//
// CLSID's of Objects
//
//====================================================================================================

// {B78BD375-E0B0-4942-9256-A061583F3495}
DEFINE_SIMON_GUID(CLSID_CSuiteOwner, 0xB78BD375,0xE0B0,0x4942,0x92,0x56,0xA0,0x61,0x58,0x3F,0x34,0x95);


#endif // !defined(_NAVSuiteOwner_H__389D63C0_68BD_4827_8434_5B8B43101FA6__INCLUDED_)
