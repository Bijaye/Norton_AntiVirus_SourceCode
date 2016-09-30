// MD5.h: interface for the CMD5 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MD5_H__37FE2D89_9E62_4DC7_8A30_7533C11BB1D9__INCLUDED_)
#define AFX_MD5_H__37FE2D89_9E62_4DC7_8A30_7533C11BB1D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wincrypt.h>	// Cryptographic API Prototypes and Definitions
//#include <hash_compare> // For hash operations

class CMD5  
{
public:
    CMD5();
	virtual ~CMD5();

    bool SetHash ( HANDLE hFile );
    bool IsValid ();    // Is the hash value valid or empty?

    // Comparison operators
    //
    bool operator==(const CMD5&) const;
    bool operator>(const CMD5&) const;
    bool operator<(const CMD5&) const;

    // Copy constructors
    //
    CMD5 &operator=(const CMD5&);
    CMD5(const CMD5&);

    BYTE* pbyResultHash; // Hash result 

};

#endif // !defined(AFX_MD5_H__37FE2D89_9E62_4DC7_8A30_7533C11BB1D9__INCLUDED_)
