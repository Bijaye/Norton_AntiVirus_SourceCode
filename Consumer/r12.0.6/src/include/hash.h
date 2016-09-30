///////////////////////////////////////////////////////////
// 
// Classes to wrap Windows hash functions. 
// by Garret Polk -  Dec. 15, 2003 - SYMANTEC CORP.
//
// CHashProvider - takes a file HANDLE and fills the CDigest*
//                 with the hash value.
//
// Use CHashProvider to scope loading the crypto DLL's.
// Use the CDigest derived classes to store data.
//
///////////////////////////////////////////////////////////
/*
Sample code:

void HashIt (const char* pszFileName )
{
    CSHA1 hashFile;
    CHashProvider hashProvider;

    HANDLE hFile = ::CreateFile ( pszFileName,
                                  GENERIC_READ, 
                                  FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
                                  NULL,
                                  OPEN_EXISTING,
                                  FILE_FLAG_SEQUENTIAL_SCAN,
                                  NULL );

    if ( hFile == INVALID_HANDLE_VALUE )
        return;

    m_hashProvider.SetHash ( hFile, &hashCurrentFile );
}
*/


#pragma once

#include "windows.h"
#include <wincrypt.h>	// Cryptographic API Prototypes and Definitions
#include <vector>

namespace NAVToolbox
{
// Base class for all hash types
//
class CDigest
{
public:
    typedef std::vector<BYTE> vecBytes;

    CDigest(void){};
    virtual ~CDigest(void){};
    virtual bool IsValid ();
    virtual ALG_ID GetAlgID () = 0;
    static int GetHashSize ();  // Size of the digest in bytes. Used to read in from file so make it const.
    vecBytes m_hash;
};

// 160-bit (20 byte) SHA1 hash.
//
class CSHA1 : public CDigest
{
public:
    CSHA1(void){};
    virtual ~CSHA1(void){};
    static int GetHashSize (){return 20;};
    ALG_ID GetAlgID (){return CALG_SHA1;};
};

// 128-bit (16 byte) MD5 hash.
//
class CMD5 : public CDigest
{
public:
    CMD5(void){};
    virtual ~CMD5(void){};
    static int GetHashSize (){return 16;};
    ALG_ID GetAlgID (){return CALG_MD5;};
};

// 512-bit ( 64 byte) SHA512 hash.
//
class CSHA512 : public CDigest
{
public:
    CSHA512(void){};
    virtual ~CSHA512(void){};
    static int GetHashSize (){return 64;};
    ALG_ID GetAlgID (){return CALG_SHA_512;};
};

// Engine that does the hashing
//
class CHashProvider
{
public:    
    CHashProvider (void);
    virtual ~CHashProvider (void);
    bool SetHash ( HANDLE hFile, CDigest* pDigest );

protected:
    HCRYPTPROV m_hCryptProv;
    std::vector<BYTE> m_byBuffer; // Prefer vector so memory is allocated on heap not stack.
};
}; // end namespace NAVToolbox