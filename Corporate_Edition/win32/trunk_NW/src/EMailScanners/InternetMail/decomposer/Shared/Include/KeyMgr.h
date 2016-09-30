// KeyMgr.h : Key management Library
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

// Keys are indexed/catagorized by some kind of token-string.
// All keys stored in this database should be considered "compromised".
// As such, there is no need to secure this database in any way.
// Note that this library can be used to launch a dictionary type
// attack by creating an artificially-generated database.  This is actually
// a good thing since it demonstrates the need for strong (i.e. long)
// passwords/keys.

#if !defined(KEYMGR_H)
#define KEYMGR_H

#include "fixibmps.h"
#include <stdio.h>
#include <stdlib.h>

// define MAX_PATH if not already defined
#if !defined( MAX_PATH )
  #if defined(_MAX_PATH)
	#define MAX_PATH _MAX_PATH
  
  #elif defined(_POSIX_PATH_MAX)
	#define MAX_PATH _POSIX_PATH_MAX

  #elif defined( PATH_MAX )
	#define MAX_PATH PATH_MAX
  
  #else
	#error MAX_PATH not declared correctly!
  #endif
#endif

#define KEY_FLAG_SECTION	0x01  // When set, szKey field contains section name
#define KEY_FLAG_USED		0x02

#define MAX_KEY_LENGTH		512

//------------------------------------------------------------------------
// Definitions for key/password cracking mechanism
//------------------------------------------------------------------------
#define MAX_KEY_SIZE			  128

#define KEY_TYPE_PROMPT			  0
#define KEY_TYPE_DATABASE		  1
#define KEY_TYPE_ANY			  2
#define KEY_TYPE_ALPHA			  3
#define KEY_TYPE_ALPHANUMERIC	  4
#define KEY_TYPE_ASCII			  5
#define KEY_TYPE_HIGHASCII		  6

typedef struct tagBFKey
{
	unsigned char	byKey[MAX_KEY_SIZE];
	int				iKeyLength;
	int				iMaxKeyLength;
	int				iKeyType;
	int				iState;
	int				iSubState;
} BF_KEY, *PBF_KEY;


#if defined(_WINDOWS)
#pragma pack(1)
#elif defined(OS400)
#pragma pack(1)
#elif defined(IBMCXX)
#pragma option align=packed
#endif

// This is the on-disk structure of all data stored in the key file.
typedef struct tagKeyEntry
{
	unsigned char	Flags;
	unsigned int	uiKeyLength;  // Variable length of key/section name that follows
#if defined(__GNUC__)
} __attribute__ ((packed)) KEY_ENTRY;
#else
} KEY_ENTRY;
#endif

#if defined(_WINDOWS)
#pragma pack()
#elif defined(OS400)
#pragma pack()
#elif defined(IBMCXX)
#pragma option align=reset
#endif


class CKeyMgr
{
public:
	CKeyMgr();
	~CKeyMgr();

	int		SetKeyFile(char *pszKeyFile);
	int		AddKey(char *pszSection, unsigned char *pKey, unsigned int uiLength);
	int		DeleteKey(char *pszSection, unsigned char *pKey, unsigned int uiLength);

	unsigned int GetFirstKey(char *pszSection, unsigned char *pKey, unsigned int uiLength);
	unsigned int GetNextKey(unsigned char *pKey, unsigned int uiLength);

	// Brute-force password cracking functions
	bool	BF_GetFirstKey(BF_KEY *pkey, int iMinLength, int iMaxLength, int iType);
	bool	BF_GetNextKey(BF_KEY *pkey);
	bool	BF_GenerateNextKey(BF_KEY *pkey);
	void	BF_DestroyKey(BF_KEY *pkey);
	int		BF_GetFirstByte(int iType);
	int		BF_GetNextByte(unsigned char byValue, int iType);

private:
	int		CleanKeyFile(void);
	FILE	*m_fp;
	char	m_szKeyFile[MAX_PATH];
};

#endif // KEYMGR_H
