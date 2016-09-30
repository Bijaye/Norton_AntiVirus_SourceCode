// MD5.cpp: implementation of the CMD5 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MD5.h"

#define HASH_SIZE 16

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMD5::CMD5()
{
    pbyResultHash = new BYTE[HASH_SIZE];
    ZeroMemory ( pbyResultHash, HASH_SIZE);
}

CMD5::~CMD5()
{
    if ( pbyResultHash )
    {
        delete [] pbyResultHash;
        pbyResultHash = NULL;
    }
}

bool CMD5::SetHash ( HANDLE hFile )
{
    ZeroMemory ( pbyResultHash, HASH_SIZE);

    HCRYPTPROV hCryptProv; 
    HCRYPTHASH hHash; 
    BYTE byBuffer [1024*8] = {0};   // 8K buffer, pick a size
    BYTE pbTempHash[16] = {0};

    DWORD dwBytesRead = 0;
    DWORD dwHashLen = HASH_SIZE; // The MD5 algorithm always returns 16 bytes. 
                                 // We save ourselves an extra call and memory allocation
                                 // by hard-coding this.

    BOOL bFileReadResult = FALSE;

    if(CryptAcquireContext(&hCryptProv, 
		NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET)) 
	{
		if(CryptCreateHash(hCryptProv, 
			CALG_MD5,	// algorithm identifier definitions see: wincrypt.h
			0, 0, &hHash)) 
		{
            // Read the next chunk from the file
            //
            while ( ::ReadFile ( hFile, &byBuffer, sizeof (byBuffer), &dwBytesRead, 0 ) &&
                    dwBytesRead > 0 )
            {
                if( !CryptHashData(hHash, byBuffer, dwBytesRead, 0))
                {
                    DWORD dwError = GetLastError();
                }
            }

            // Done, get the hash value
            //
            if(CryptGetHashParam(hHash, HP_HASHVAL, pbTempHash, &dwHashLen, 0)) 
                memcpy (pbyResultHash, pbTempHash, dwHashLen);
		}
		else 
            return false;//szDigest=_T("Error creating hash"); 
    }
    else 
        return false;//szDigest=_T("Error acquiring context"); 

    CryptDestroyHash(hHash); 
    CryptReleaseContext(hCryptProv, 0); 

    return true;
}

// *********************************
//
// Comparision operators
//
//
// Equality
//
bool CMD5::operator==(const CMD5& MD5) const
{
    return ( 0 == memcmp (MD5.pbyResultHash, pbyResultHash, HASH_SIZE)) ? true : false;
}

// Greater than
//
bool CMD5::operator>(const CMD5& MD5) const
{
    return ( 0 > memcmp (MD5.pbyResultHash, pbyResultHash, HASH_SIZE)) ? true : false;
}

// Less than
//
bool CMD5::operator<(const CMD5& MD5) const
{
    return ( 0 < memcmp (MD5.pbyResultHash, pbyResultHash, HASH_SIZE)) ? true : false;
}
// End comparison operators
//
// ***************************

// Copy operators/constructors
//
CMD5 &CMD5::operator=(const CMD5& MD5)
{
    memcpy ( pbyResultHash, MD5.pbyResultHash, HASH_SIZE);
    return *this;
}

CMD5::CMD5(const CMD5& MD5)
{
    pbyResultHash = new BYTE[HASH_SIZE];
    ZeroMemory ( pbyResultHash, HASH_SIZE);
    memcpy ( pbyResultHash, MD5.pbyResultHash, HASH_SIZE);
}

bool CMD5::IsValid ()
{
    CMD5 emptyMD5;
    return !( emptyMD5 == *this );
}