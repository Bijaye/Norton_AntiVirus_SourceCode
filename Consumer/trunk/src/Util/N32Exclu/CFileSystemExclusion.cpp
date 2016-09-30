////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/***
 * Implementation for path-based exclusion object; subclass of IExclusion.
 * Adds certain methods directly concerning filesystem exclusions as well
 * as methods that require consideration of implementation specifics.
 * Also adds some helper methods to assist in dealing with paths.
 ***/

#pragma once

#ifndef __N32EXCLU_cfilesystemexclusion_cpp__
#define __N32EXCLU_cfilesystemexclusion_cpp__
#pragma warning( disable : 4290 )

#include <wchar.h>
#include <tchar.h>
#include "ccString.h"
#include "ccSymKeyValueCollectionImpl.h"
#include "ccStringInterface.h"
#include "ccSymStringImpl.h"
#include "CFileSystemExclusion.h"

#define __MY_INTERFACE IFileSystemExclusion
#define __MY_CLASS CFileSystemExclusion
#define __MY_CLASSNAME "CFileSystemExclusion"
#define __N32EXCLU_IMPLEMENTATION_

namespace NavExclusions
{

    CFileSystemExclusion::CFileSystemExclusion()
    {
        if(this->initMe())
        {
            m_spProperties->SetValue(IExclusion::Property::EType, 
                       (ULONG)IExclusion::ExclusionType::FILESYSTEM);
        }
    }

    CFileSystemExclusion::CFileSystemExclusion(cc::IKeyValueCollection* rhs) 
    {
        this->m_spProperties = rhs;
        this->m_bInitialized = true;
    }

    CFileSystemExclusion::~CFileSystemExclusion() {this->uninitMe();}

    ExResult CFileSystemExclusion::setFSPath(const cc::IString* myPath)
    {
        CStringW cswBuffer;
        cc::IStringPtr spBuffer;

        if(!this->isInitialized()) 
        { 
            CCTRACEE(_T("CFileSystemExclusion::setFSPath - Not initialized."));
            return NavExclusions::NotInitialized; 
        }

        if(!myPath)
        {
            CCTRACEE(_T("CFileSystemExclusion::setFSPath - Received NULL arg."));
            return NavExclusions::InvalidArgument;
        }

        cswBuffer = myPath->GetStringW();
        if(cswBuffer[cswBuffer.GetLength()-1] == '\\')
        {
            cswBuffer.Delete(cswBuffer.GetLength()-1,1);
            spBuffer = ccSym::CStringImpl::CreateStringImpl(cswBuffer.GetString());
        }
        else
        {
            spBuffer = myPath;
        }

        if(!m_spProperties->SetValue(IFileSystemExclusion::Property::FSPath, 
                                     spBuffer))
        {
            CCTRACEE(_T("CFileSystemExclusion::setFSPath - Could not put path ")\
                        _T("into bag."));
            return NavExclusions::Fail;
        }

        return NavExclusions::Success;
    }

    ExResult CFileSystemExclusion::getFSPath(cc::IString*& pStrPath)
    {
        return this->getStringFromBag((size_t)IFileSystemExclusion::Property::FSPath,
                                      pStrPath);
    }

    ExResult CFileSystemExclusion::doesIncludeSubdirs(const IExclusion::ExclusionState requiredState)
    {
        IExclusion::ExclusionState eState = this->getState();
        IExclusion::ExclusionState sdirType = IExclusion::ExclusionState::NOT_EXCLUDED;

        // Exclude viral subdirs only if we are performing a viral match
        // and can match virally.
        if(eState & IExclusion::ExclusionState::EXCLUDE_VIRAL && 
            requiredState & IExclusion::ExclusionState::EXCLUDE_VIRAL)
            sdirType = (IExclusion::ExclusionState) (sdirType | IExclusion::ExclusionState::EXCLUDE_SUBDIRS_VIRAL);

        // Analogous to above.
        if(eState & IExclusion::ExclusionState::EXCLUDE_NONVIRAL && 
            requiredState & IExclusion::ExclusionState::EXCLUDE_NONVIRAL)
            sdirType = (IExclusion::ExclusionState) (sdirType | IExclusion::ExclusionState::EXCLUDE_SUBDIRS_NONVIRAL);

        if(eState & sdirType)
            return NavExclusions::Success;
        else
            return NavExclusions::False;
    }

    ExResult CFileSystemExclusion::isMatch(const void* pPath, const void* pRequiredState)
    {
        wchar_t myVol, argVol;
        bool bMatch;
        ExResult eResult;
        cc::IStringPtr spStrMyFilename, spStrArgFilename;
        cc::IStringPtr spStrMyPath, spStrArgPath;
        cc::IStringPtr spStrMyFS;
        cc::IString* pStrArgFS;
        IExclusion::ExclusionState requiredState;

        if(!pPath) 
        { 
            CCTRACEE(_T("CFileSystemExclusion::isMatch - Received a NULL path."));
            return NavExclusions::InvalidArgument; 
        }
        else
            pStrArgFS = (cc::IString*)(pPath);

        // Nasty hack so we know if we want subdirs or not
        if(!pRequiredState || (*((LONG*)pRequiredState) == 0))
            requiredState = IExclusion::ExclusionState::EXCLUDE_ALL;
        else
            requiredState = *((IExclusion::ExclusionState*)pRequiredState);

        if(!this->isInitialized())
        {
            CCTRACEE(_T("CFileSystemExclusion::isMatch - Not initialized."));
            return NavExclusions::NotInitialized;
        }

        if(this->getFSPath(spStrMyFS) != NavExclusions::Success)
        {
            CCTRACEE(_T("CFileSystemExclusion::isMatch - Could not retrieve ")\
                        _T("internal FS path; was it set?"));
            return NavExclusions::Fail;
        }

        if(spStrMyFS->GetLength() <= 0)
        {   // WTF?!
            CCTRACEW(_T("CFileSystemExclusion::isMatch - Match attempted on ")\
                        _T("exclusion with no FS path set or null FS path. ")\
                        _T("Auto-failing."));
            spStrMyFS.Release();
            return NavExclusions::Fail;
        }

        spStrMyFilename = spStrArgFilename = spStrMyPath = spStrArgPath = NULL;
        bMatch = false;

        myVol = this->getVolumeChar(spStrMyFS);
        argVol = this->getVolumeChar(pStrArgFS);

        if(myVol != 0)
        {   // If we have a volume, match 'em
            if(myVol != argVol)
            {
                spStrMyFS.Release();
                return NavExclusions::False;
            }
        }   // Else, implicitly pass since volume doesn't matter.

        // Get Filenames
        eResult = (this->getFilename(spStrMyFS, spStrMyFilename));
        if(eResult >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CFileSystemExclusion::isMatch - Error while ")\
                        _T("attempting to extract filename from internal FS path."));
            return eResult;
        }

        eResult = (this->getFilename(pStrArgFS, spStrArgFilename));
        if(eResult >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CFileSystemExclusion::isMatch - Error while ")\
                        _T("attempting to extract filename from argument."));
            spStrMyFilename.Release();
            spStrMyFS.Release();
            return eResult;
        }

        // Match directories.. this is messy.
        eResult = this->getPath(spStrMyFS, spStrMyPath);
        if(eResult >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CFileSystemExclusion::isMatch - Error while extracting ")\
                        _T("path from internal string."));
            spStrArgFilename.Release();
            spStrMyFilename.Release();
            spStrMyFS.Release();
            return eResult;
        }
        else if(eResult == NavExclusions::Success)
        {
            eResult = this->getPath(pStrArgFS, spStrArgPath);
            if(eResult >= NavExclusions::Fail)
            {
                CCTRACEE(_T("CFileSystemExclusion::isMatch - Error while ")\
                            _T("extracting path from argument FS path."));
                spStrMyPath.Release();
                spStrArgFilename.Release();
                spStrMyFilename.Release();
                spStrMyFS.Release();
                return eResult;
            }
            else if(eResult == NavExclusions::Success)
            {   // Horrendously complicated matching logic here!
                // This is fugly because we don't really know whether
                // anything after the last backslash is a directory or
                // a filename; hence, we need to treat it as a directory.
                // This ambiguity exists in both the input and internal paths.
                cc::IStringPtr spStrMyPathAndFilename, spStrArgPathAndFilename;

                if(!spStrMyFilename)
                    spStrMyPathAndFilename = spStrMyPath;
                else
                {
                    CStringW buf = spStrMyPath->GetStringW();
                    buf.Append(spStrMyFilename->GetStringW());
                    if(buf[buf.GetLength()-1] != '\\')  // if needed
                        buf.Append(L"\\");              // add trailing slash
                    spStrMyPathAndFilename.Attach(
                        ccSym::CStringImpl::CreateStringImpl(buf.GetString())
                                                 );
                }

                if(!spStrArgFilename)
                    spStrArgPathAndFilename = spStrArgPath;
                else
                {
                    CStringW buf = spStrArgPath->GetStringW();
                    buf.Append(spStrArgFilename->GetStringW());
                    if(buf[buf.GetLength()-1] != '\\')  // if needed
                        buf.Append(L"\\");              // add trailing slash
                    spStrArgPathAndFilename.Attach(
                        ccSym::CStringImpl::CreateStringImpl(buf.GetString())
                                                 );
                }

                // If myPath+Filename == myPath, we just want
                // to test myPath matching ArgPath/ArgPathAndFilename.
                if(spStrMyPathAndFilename != spStrMyPath && 
                   !this->hasWildcard(spStrMyFilename))
                {
                    // Now we need to test MyPath+MyFilename vs
                    // ArgPath+ArgFilename AND
                    // MyPath+MyFilename vs ArgPath alone.
                    // If either of these hit, we match.
                    if(this->matchPathHelper(spStrMyPathAndFilename,
                                             spStrArgPathAndFilename,
                                             requiredState) ||
                       this->matchPathHelper(spStrMyPathAndFilename,
                                                  spStrArgPath,
                                                  requiredState))
                    {   // That was it! Escape out.
                        spStrMyPathAndFilename.Release();
                        spStrArgPathAndFilename.Release();
                        spStrMyPath.Release();
                        spStrArgPath.Release();
                        spStrMyFilename.Release();
                        spStrArgFilename.Release();
                        spStrMyFS.Release();
                        return NavExclusions::Success;                       
                    }
                }
                
                // Test MyPath vs. ArgPath+ArgFilename (if applicable) AND
                // Test MyPath vs. ArgPath
                // If either hit, we need to check filenames.
                // If both miss, we can safely fail.
                if((spStrArgPathAndFilename != spStrArgPath &&
                    !this->matchPathHelper(spStrMyPath, spStrArgPathAndFilename, requiredState))
                    && !this->matchPathHelper(spStrMyPath, spStrArgPath, requiredState))
                {
                        spStrMyPathAndFilename.Release();
                        spStrArgPathAndFilename.Release();
                        spStrMyPath.Release();
                        spStrArgPath.Release();
                        spStrMyFilename.Release();
                        spStrArgFilename.Release();
                        spStrMyFS.Release();
                        return NavExclusions::False; 
                }

                spStrMyPathAndFilename.Release();
                spStrArgPathAndFilename.Release();
            }  // Else: We have a path and the argument doesn't? Can't match!
        }   // Else, we have no path... maybe just a file match?

        if(spStrMyFilename && spStrArgFilename)
        {
            bMatch = this->matchFilenames(spStrMyFilename, 
                                             spStrArgFilename);
        }
        else if(!spStrMyFilename) { bMatch = true; }

        if(spStrMyPath) { spStrMyPath.Release(); }
        if(spStrArgPath) { spStrArgPath.Release(); }
        if(spStrMyFS) { spStrMyFS.Release(); }
        if(spStrArgFilename) { spStrArgFilename.Release(); }
        if(spStrMyFilename) { spStrMyFilename.Release(); }

        if(!bMatch) { return NavExclusions::False; }
        else { return NavExclusions::Success; }
    }

    ExResult CFileSystemExclusion::getDisplayName(cc::IString*& pStrName)
    {
        return this->getFSPath(pStrName);
    }

    /*** Protected Methods ***/
    // Specialized getVolume for while we can still assume that
    // volumes are single paths.
    wchar_t CFileSystemExclusion::getVolumeChar(const cc::IString* pStrPath)
    {
        LPCWSTR pszPath;

        if(!pStrPath)
        {
            CCTRACEE(_T("CFileSystemExclusion::getVolumeChar - NULL input."));
            return (wchar_t)0;
        }

        if(pStrPath->GetLength() < 2) 
        { 
            CCTRACEE(_T("CFileSystemExclusion::getVolumeChar - Input string ")\
                        _T("is too short, can't possibly have a volume."));
            return (wchar_t)0; 
        }

        pszPath = pStrPath->GetStringW();
        if(!pszPath)
        {
            CCTRACEE(_T("CFileSystemExclusion::getVolumeChar - Could not ")\
                        _T("retrieve LPCWSTR from pointer."));
            return (wchar_t)0;
        }

        if(iswalpha(pszPath[0]) && pszPath[1] == L':')
        {   // WARNING: Currently only works with single-character volumes.
            if(wcslen(pszPath) == 2 || pszPath[2] == L'\\')
            {   // X: only, no path or file
                return towupper(pszPath[0]);
            }
        }
        
        return (wchar_t)0;
    }

    ExResult CFileSystemExclusion::getPath(const cc::IString* pStrPath,
                                           cc::IString* &pStrResult)
    {
        LPCWSTR lpCWStr = NULL;
        LPCWSTR lpFilename;
        size_t len;
        cc::IStringPtr spPathOnly;
        CStringW cswBuffer;

        if(!pStrPath || pStrPath->GetLength() <= 0)
            return NavExclusions::InvalidArgument;
        else
            lpCWStr = pStrPath->GetStringW();

        if(!lpCWStr)
        {
            CCTRACEE(_T("CFileSystemExclusion::getPath - Could not retrieve ")\
                        _T("raw string from input parameter."));
            return NavExclusions::Fail;
        }

        if(pStrResult != NULL)
        {
            CCTRACEI(_T("CFileSystemExclusion::getPath - Output parameter ")\
                        _T("was not set to NULL; auto-releasing."));
            pStrResult->Release();
            pStrResult = NULL;
        }

        // Two possible conditions: with or without volume
        if(this->getVolumeChar(pStrPath) != (WCHAR)0 || lpCWStr[0] == '\\')
        {
            if(lpCWStr[0] != '\\')     // If with volume...
                lpCWStr = lpCWStr + 2; // Move past volume.

            if(lpCWStr[0] == '\\')      // We should have a backslash now
            {   // Extract the path (everything from the first backslash to the
                // last backslash)
                lpFilename = wcsrchr((lpCWStr), '\\');
                len = (lpFilename - lpCWStr) + 1;
                cswBuffer.SetString(lpCWStr, (int)len);
                pStrResult = ccSym::CStringImpl::CreateStringImpl(cswBuffer.GetString());
                if(!pStrResult)
                {
                    CCTRACEE(_T("CFileSystemExclusion::getPath - Could not ")\
                                _T("allocate output buffer."));
                    return NavExclusions::OutOfMemory;
                }
                else
                {
                    return NavExclusions::Success;
                }
            }
        }

        return NavExclusions::False; // No path...
    }

    ExResult CFileSystemExclusion::getFilename(const cc::IString* pStrPath,
                                                   cc::IString* &pStrFilename)
    {
        LPCWSTR pszPath;
        CStringW cswBuffer;

        if(!pStrPath || pStrPath->GetLength() <= 0)
        {
            CCTRACEE(_T("CFileSystemExclusion::getFilename - Invalid path."));
            return NavExclusions::InvalidArgument;
        }

        if(pStrFilename != NULL)
        {
            CCTRACEI(_T("CFileSystemExclusion::getFilename - Output pointer ")\
                        _T("not set to NULL; autoreleasing."));
            pStrFilename->Release();
            pStrFilename = NULL;
        }

        pszPath = pStrPath->GetStringW();
        if(!pszPath)
        {
            CCTRACEE(_T("CFileSystemExclusion::getFilename - could not retrieve")\
                        _T(" raw string from input path."));
            return NavExclusions::Fail;
        }

        pszPath = wcsrchr(pszPath, '\\');
        if(pszPath && pszPath[1] != '\0')
        {   // If there's stuff after the last slash, we have a fname
            cswBuffer.SetString(pszPath+1);
            pStrFilename = ccSym::CStringImpl::CreateStringImpl(cswBuffer.GetString());
            if(!pStrFilename)
            {
                CCTRACEE(_T("CFileSystemExclusion::getFilename - could not allocate")\
                            _T(" a buffer for the filename portion of the path."));
                return NavExclusions::OutOfMemory;
            }
            else
                return NavExclusions::Success;
        }
        else if(!pszPath)
        {   // No backslash - either it's a volume OR copy the whole string
            pszPath = pStrPath->GetStringW();
            if(!pszPath)
            {
                CCTRACEE(_T("CFileSystemExclusion::getFilename - could not retrieve")\
                            _T(" raw string from input path."));
                return NavExclusions::Fail;
            }

            // Special case: Volume
            if(wcslen(pszPath) == 2 && iswalpha(pszPath[0]) && pszPath[1] == ':')
                return NavExclusions::False;

            pStrFilename = ccSym::CStringImpl::CreateStringImpl(pStrPath->GetStringW());
            if(!pStrFilename)
            {
                CCTRACEE(_T("CFileSystemExclusion::getFilename - could not allocate")\
                            _T(" a buffer for the filename portion of the path."));
                return NavExclusions::OutOfMemory;
            }
            else
                return NavExclusions::Success;
        }

        return NavExclusions::False; // No filename
    }

    bool CFileSystemExclusion::matchFilenames(const cc::IString* pStrPattern, 
                                              const cc::IString* pStrPath)
    {
        LPCWSTR pszPattern, pszPath, pszExtension;
        LPWSTR pszRevExt, pszRevPath;
        size_t extensionLength;

        if(!pStrPattern || !pStrPath) { return false; }
        pszPattern = pszPath = NULL;
        pszPattern = pStrPattern->GetStringW();
        pszPath = pStrPath->GetStringW();
        if(!pszPattern || !pszPath) { return false; }
        pszExtension = wcsrchr(pszPattern, '*');
        if(!pszExtension)
            return wcsicmp(pszPattern, pszPath) == 0;
        else
        {
            if(wcsncmp(pszPattern, L"*.*", 3) == 0)
            {   // Added 5 Apr 2005: *.* matches everything :)
                return true;
            }

            pszExtension += 1; // Advance beyond *.
            // Duplicate and reverse strings.
            pszRevExt = wcsrev(wcsdup(pszExtension));
            if(!pszRevExt)
                return false; // memory allocation error!
            pszRevPath = wcsrev(wcsdup(pszPath));
            if(!pszRevPath)
            {
                free(pszRevExt);
                return false;
            }

            extensionLength = wcslen(pszRevExt);
            if(wcsnicmp(pszRevExt, pszRevPath, extensionLength) == 0)
            {
                // The last N characters of the path match the extension
                // N = length of the extension
                // If the path is shorter than the extension, then it
                // cannot possibly match; wcsnicmp handles this case.
                free(pszRevExt);
                free(pszRevPath);
                return true;
            }
            free(pszRevExt);
            free(pszRevPath);
            return false;
        }
    }

    bool CFileSystemExclusion::hasWildcard(const cc::IString* pStrPath)
    {
        LPCWSTR pszPath;
        if(!pStrPath || pStrPath->GetLength() <= 0) { return false; }

        pszPath = pStrPath->GetStringW();
        if(!pszPath) { return false; }

        return (wcsrchr(pszPath, '*') != NULL);
    }
    
    bool CFileSystemExclusion::matchPathHelper(const cc::IString* pStrPattern, 
                                               const cc::IString* pStrPath,
                                               const IExclusion::ExclusionState requiredState)
    {
        size_t sizPattern, sizPath;
        LPCWSTR pszPattern, pszPath;
        pszPattern = pszPath = NULL;
        if(!pStrPattern || !pStrPath) { return false; }

        sizPattern = pStrPattern->GetLength();
        sizPath = pStrPath->GetLength();
        pszPattern = pStrPattern->GetStringW();
        pszPath = pStrPath->GetStringW();
        if(!pszPattern || !pszPath) { return false; } // WTF?!

        if((sizPattern > sizPath) || (sizPattern < sizPath && 
                this->doesIncludeSubdirs(requiredState) != NavExclusions::Success))
        {   // Impossible match conditions:
            // Pattern longer than path OR pattern shorter than path,
            // but don't want to match subdirs.
                return false;
        }
        else    // Path Length == Pattern Length OR
        {       // (Include Subdirs AND Pattern Length < Path Length)
            return (wcsnicmp(pszPattern, pszPath, sizPattern) == 0);
        }
    }

    ExResult CFileSystemExclusion::isExactMatch(const cc::IString* pStrPath)
    {
        cc::IStringPtr spMyPath;
        LPCWSTR pszArgPath, pszMyPath;

        if(!pStrPath)
        {
            CCTRACEW(_T("CFileSystemExclusion::isExactMatch - Received a ")\
                        _T("NULL input string; autofailing."));
            return NavExclusions::InvalidArgument;
        }

        if(this->getFSPath(spMyPath) != NavExclusions::Success)
        {
            CCTRACEE(_T("CFileSystemExclusion::isExactMatch - Could not ")\
                        _T("retrieve FS Path internally."));
            return NavExclusions::Fail;
        }

        pszArgPath = pStrPath->GetStringW();
        pszMyPath = spMyPath->GetStringW();
        if(!pszArgPath || !pszMyPath)
        {
            CCTRACEE(_T("CFileSystemExclusion::isExactMatch - Could not ")\
                        _T("retrieve constant string from argument or internal ")\
                        _T("FS path."));
            spMyPath.Release();
            return NavExclusions::Fail;
        }

        if(wcsicmp(pszArgPath, pszMyPath) == 0)
        {
            spMyPath.Release();
            return NavExclusions::Success;
        }
        else
        {
            spMyPath.Release();
            return NavExclusions::False;
        }
    }

    IFileSystemExclusion* CFileSystemExclusion::clone()
    {
        cc::IStringPtr spStr;
        time_t tExpiry;
        ExResult res;
        cc::IKeyValueCollection* pUserData;

        CFileSystemExclusion* ptr = new(std::nothrow) CFileSystemExclusion();
        ptr->AddRef();
        if(!ptr->m_spProperties->SetCloneInterface(true))
        {
            CCTRACEE(_T("CFileSystemExclusion::clone - Could not set clone interface."));
            delete ptr;
            return NULL;
        }

        if(ptr->setActive(this->isActive() == NavExclusions::Success) != NavExclusions::Success)
        {
            CCTRACEE(_T("CFileSystemExclusion::clone - Could not set Active state."));
            delete ptr;
            return NULL;
        }

        if(ptr->setDeleted(this->isDeleted() == NavExclusions::Success) != NavExclusions::Success)
        {
            CCTRACEE(_T("CFileSystemExclusion::clone - Could not set Deleted state."));
            delete ptr;
            return NULL;
        }

        if(ptr->setState(this->getState()) != NavExclusions::Success)
        {
            CCTRACEE(_T("CFileSystemExclusion::clone - Could not set exclusion state."));
            delete ptr;
            return NULL;
        }

        res = this->getFSPath(spStr);
        if(res >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CExclusionExclusion::clone - Failure when getting FS path."));
            delete ptr;
            return NULL;
        }
        else if(res == NavExclusions::Success)
        {
            if(ptr->setFSPath(spStr) != NavExclusions::Success)
            {
                CCTRACEE(_T("CExclusionExclusion::clone - Failure when setting FS path."));
                delete ptr;
                return NULL;
            }
            spStr.Release();
        }
        else
        {
            CCTRACEE(_T("CFileSystemExclusion::clone - No FS path! Illegal exclusion."));
            delete ptr;
            return NULL;
        }

        
        res = this->getExclusionGuid(spStr);
        if(res >= NavExclusions::Fail)
        {
            CCTRACEE(_T("CExclusionExclusion::clone - Failure when getting Exclusion guid."));
            delete ptr;
            return NULL;
        }
        else if(res == NavExclusions::Success)
        {
            if(ptr->setExclusionGuid(spStr) != NavExclusions::Success)
            {
                CCTRACEE(_T("CExclusionExclusion::clone - Failure when setting Exclusion guid."));
                delete ptr;
                return NULL;
            }
            spStr.Release();
        }
        else
        {
            CCTRACEE(_T("CFileSystemExclusion::clone - No exclusion GUID! Illegal exclusion."));
            delete ptr;
            return NULL;
        }


        if((tExpiry = this->getExpiryStamp()) > 0)
        {
            if(ptr->setExpiryStamp(tExpiry) != NavExclusions::Success)
            {            
                CCTRACEE(_T("CFileSystemExclusion::clone - Could not copy exclusion timestamp."));
                delete ptr;
                return NULL;
            }
        }

        if(this->m_spProperties->GetValue(IExclusion::Property::UserData, 
            (ISymBase*&)pUserData))
        {
            if(!ptr->m_spProperties->SetValue(IExclusion::Property::UserData, 
                (ISymBase*&)pUserData))
            {
                CCTRACEE(_T("CFileSystemExclusion::clone - Could not copy user data."));
                delete ptr;
                return NULL;
            }
        }


        if(ptr->setID(this->getID()) != NavExclusions::Success)
        {
            CCTRACEE(_T("CFileSystemExclusion::clone - Could not set Exclusion ID."));
            delete ptr;
            return NULL;
        }

        if(!ptr->m_spProperties->SetCloneInterface(false))
        {
            CCTRACEE(_T("CFileSystemExclusion::clone - Could not unset clone interface."));
            delete ptr;
            return NULL;
        }

        return ptr;
    }

/*** MACRO THIS ***/
#include "GenericExclusion.h"
/*** END MACRO THIS ***/
#undef __MY_INTERFACE
#undef __MY_CLASS
#undef __MY_CLASSNAME

}

#endif