////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "cceventutil.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// Begining of Load Map
/////////////////////////////////////////////////////////////////////////////////////////////
#define BEGIN_SERIALIZE_LOAD_MAP_INSIDE(ClassName)                                          \
public:                                                                                     \
    ccEvtMgr::CError::ErrorType Load(const LPVOID pData, DWORD dwSize, DWORD& dwRead)       \
    {                                                                                       \
        LPCTSTR szClassName = _T(#ClassName);                                               \
        /* Destroy existing data */                                                         \
        Destroy();                                                                          \
                                                                                            \
        /* Attach to memory and load from memory */                                         \
        ccLib::CMemoryArchive archive(ccLib::CArchive::eReadArchive);                       \
        archive.GetMemory().Reference(pData, dwSize);                                                   \
                                                                                            \
        /* Version */                                                                       \
        long nVersion = 0;                                                                  \
        if(archive.Read(nVersion) == FALSE)                                                 \
        {                                                                                   \
            TRACE_READ_FAIL("nVersion");                                                    \
        }                                                                                   \
        switch (nVersion)                                                                   \
        {

//*****************************************************************************************//
// Save Macros
//*****************************************************************************************//
/////////////////////////////////////////////////////////////////////////////////////////////
// Call save to get the size of our archived data
/////////////////////////////////////////////////////////////////////////////////////////////
#define DECLARE_SERIALIZE_GETMAXSIZE_INSIDE(ClassName, BaseClass)                           \
public:                                                                                     \
ccEvtMgr::CError::ErrorType GetSizeMax(DWORD& dwSize) const                                 \
{                                                                                           \
    LPCTSTR szClassName = _T(#ClassName);                                                   \
    /* Save to dummy archive to get the size */                                             \
    ccLib::CDummyArchive archive(ccLib::CArchive::eWriteArchive);                           \
    if (Save(archive) == FALSE)                                                             \
    {                                                                                       \
        CCTRACEE(_T("%s::GetSizeMax() : Save() == FALSE\n"), szClassName);                  \
        return ccEvtMgr::CError::eFatalError;                                               \
    }                                                                                       \
    dwSize += DWORD(archive.GetSize());                                                     \
    return BaseClass::GetSizeMax(dwSize);                                                   \
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Public save method
/////////////////////////////////////////////////////////////////////////////////////////////
#define DECLARE_SERIALIZE_PUBLIC_SAVE_INSIDE(ClassName, BaseClass)                          \
public:                                                                                     \
    ccEvtMgr::CError::ErrorType Save(LPVOID pData, DWORD dwSize, DWORD& dwWritten) const    \
    {                                                                                       \
        LPCTSTR szClassName = _T(#ClassName);                                               \
        /* Attach to memory and save to memory */                                           \
        ccLib::CMemoryArchive archive(ccLib::CArchive::eWriteArchive);                      \
        archive.GetMemory().Reference(pData, dwSize);                                                   \
        if (Save(archive) == FALSE)                                                         \
        {                                                                                   \
            CCTRACEE(_T("%s::Save() : Save() == FALSE\n"), szClassName);                    \
            return ccEvtMgr::CError::eFatalError;                                           \
        }                                                                                   \
                                                                                            \
        /* Set correct size and offset */                                                   \
        dwWritten += DWORD(archive.GetSize());                                              \
        dwSize -= DWORD(archive.GetSize());                                                 \
        LPBYTE pOffset = LPBYTE(pData);                                                     \
        pOffset += archive.GetSize();                                                       \
                                                                                            \
        /* Call base */                                                                     \
        return BaseClass::Save(pOffset, dwSize, dwWritten);                                 \
    }

/////////////////////////////////////////////////////////////////////////////////////////////
// Refs the two declares above
/////////////////////////////////////////////////////////////////////////////////////////////
#define DECLARE_SERIALIZE_SAVE_INSIDE(ClassName, BaseClass)                                 \
     DECLARE_SERIALIZE_PUBLIC_SAVE_INSIDE(ClassName, BaseClass)                             \
     DECLARE_SERIALIZE_GETMAXSIZE_INSIDE(ClassName, BaseClass)

/////////////////////////////////////////////////////////////////////////////////////////////
// Begin save map
/////////////////////////////////////////////////////////////////////////////////////////////
#define BEGIN_SERIALIZE_SAVE_MAP_INSIDE( ClassName, BaseClass )                   \
    DECLARE_SERIALIZE_SAVE_INSIDE(ClassName, BaseClass)                           \
                                                                                  \
protected:                                                                        \
    BOOL Save(ccLib::CArchive& Archive) const                                     \
    {                                                                             \
        LPCTSTR szClassName = _T(#ClassName);                                     \
        /* Version */                                                             \
        if(Archive.Write(long(ClassName::Version)) == FALSE)                      \
        {                                                                         \
            TRACE_WRITE_FAIL(ClassName::Version)                                  \
        }                                                                                   
