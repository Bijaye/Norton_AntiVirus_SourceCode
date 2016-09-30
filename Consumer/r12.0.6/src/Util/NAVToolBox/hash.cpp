#include "stdafx.h"
#include "hash.h"

namespace NAVToolbox
{
bool CDigest::IsValid ()
{
    return !m_hash.empty();
}

CHashProvider::CHashProvider (void)
{
    m_hCryptProv = NULL;
    m_byBuffer.resize(1024 * 64);

    if(!CryptAcquireContext(&m_hCryptProv, 
		                    NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET))
    {
    }
}

CHashProvider::~CHashProvider (void)
{
    CryptReleaseContext(m_hCryptProv, 0); 
}

bool CHashProvider::SetHash ( HANDLE hFile, CDigest* pDigest )
{
    if ( pDigest == NULL )
        return false;

    pDigest->m_hash.clear ();

    //
    // Because actual # of bytes read is always passed to CryptHashData, it's not necessary
    // to take the hit to zero this 64K buffer everytime we're called.
    //
    //ZeroMemory ( &m_byBuffer, 1024*64 );   // 64K buffer, pick a size
 
    HCRYPTHASH hHash;
    DWORD dwBytesRead = 0;
    BOOL bFileReadResult = FALSE;

	if(CryptCreateHash( m_hCryptProv, 
                        pDigest->GetAlgID(),	// algorithm identifier definitions see: wincrypt.h
			            0, 0,
                        &hHash)) 
	{
        // Read the next chunk from the file
        //
        while ( ::ReadFile ( hFile, &m_byBuffer, sizeof (m_byBuffer), &dwBytesRead, 0 ) &&
                dwBytesRead > 0 )
        {
            if( !CryptHashData(hHash, &m_byBuffer[0], dwBytesRead, 0))
            {
                DWORD dwError = GetLastError();
            }
        }

        // Done, get the hash value
        //
        DWORD        dwHashSize = 0;
        DWORD        dwHashLen = sizeof(DWORD);

        if ( CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&dwHashSize, &dwHashLen, 0))
        {
            pDigest->m_hash.resize ( dwHashSize, 0 );

            if ( !CryptGetHashParam(hHash, HP_HASHVAL, &(pDigest->m_hash[0]), &dwHashSize, 0))
                DWORD dwError = GetLastError();
        }
        else
            DWORD dwError = GetLastError();                
	}
	else 
        return false;//szDigest=_T("Error creating hash"); 

    CryptDestroyHash(hHash); 
    return true;
}
};// end namespace NAVToolbox
