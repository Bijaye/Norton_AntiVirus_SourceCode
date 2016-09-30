// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////

/***
 * Interface header file for FileSystem exclusions. Primarily contains
 *  methods related to conversion/handling of paths.
 ***/
#pragma once

#ifndef __N32EXCLU_cfilesystemexclusion_h__
#define __N32EXCLU_cfilesystemexclusion_h__

#define __MY_INTERFACE IFileSystemExclusion
#define __MY_CLASS CFileSystemExclusion
#define __MY_CLASSNAME "CFileSystemExclusion"

#include <time.h>
#include "n32exclu.h"
#include "ExclusionInterface.h"

namespace NavExclusions
{

class CFileSystemExclusion: public IFileSystemExclusion,
                            public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    CFileSystemExclusion();
    CFileSystemExclusion(cc::IKeyValueCollection* rhs);
    virtual ~CFileSystemExclusion();

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(IID_IExclusion, IExclusion)
        SYM_INTERFACE_ENTRY(IID_IFileSystemExclusion, IFileSystemExclusion)
    SYM_INTERFACE_MAP_END()

    // From IFileSystemExclusion
    virtual NavExclusions::ExResult setFSPath(const cc::IString* myPath);
    virtual ExResult getFSPath(cc::IString*& pStrPath);

    virtual ExResult doesIncludeSubdirs(const IExclusion::ExclusionState requiredState);
    virtual ExResult isExactMatch(const cc::IString* pStrPath);

protected:
    // Returns a single-character volume, uppercase. Returns 0 if no volume.
    virtual wchar_t getVolumeChar(const cc::IString* pStrPath);

    // Returns the path. Returns 0 if no path.
    virtual ExResult getPath(const cc::IString* pStrPath, 
                                 cc::IString* &pStrResult);

    // Returns the filename. Returns 0 if no filename.
    virtual ExResult getFilename(const cc::IString* pStrPath, 
                                     cc::IString* &pStrFilename);

    // Performs filename matching, with or without wildcards.
    virtual bool matchFilenames(const cc::IString* pStrPattern,
                                const cc::IString* pStrPath);

    virtual bool hasWildcard(const cc::IString* pStrPath);
    virtual bool matchPathHelper(const cc::IString* pStrPattern, 
                                 const cc::IString* pStrPath,
                                 const IExclusion::ExclusionState requiredState);

/** MACRO THIS **/
#include "GenericExclusion.h"
/** END MACRO THIS **/
};

#undef __MY_INTERFACE
#undef __MY_CLASS
#undef __MY_CLASSNAME

}
#endif