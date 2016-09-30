// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/enumfile.h_v   1.1   11 Dec 1997 23:33:46   VGLOSTE  $
//
// ENUMFILE.H
//
// Class for enumerating all files in a folder and its subfolders.
// Also has helper classes CEnumSingleDir and CDirNode.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/enumfile.h_v  $
// 
//    Rev 1.1   11 Dec 1997 23:33:46   VGLOSTE
// Added two new classes, CEnumLinkedItem and CEnumLinkedList, 
// required by changes to the CEnumSingleDir class.
// 
//    Rev 1.0   13 May 1997 18:02:18   DALLEE
// Initial revision.
//************************************************************************

#ifndef __ENUMFILE_H
#define __ENUMFILE_H

// The classes CEnumLinkedItem and CEnumLinkedList were added to 
// implement a linked list that stores the contents of the folder
class CEnumLinkedItem
{
private:
	FINDDATA        m_data;
	CEnumLinkedItem *m_pNext;

public:
	CEnumLinkedItem() 
	{ memset(&m_data, 0, sizeof( m_data )); m_pNext = NULL; };
	CEnumLinkedItem(const FINDDATA *pData) 
	{ memcpy(&m_data, pData, sizeof( m_data )); m_pNext = NULL; };
	inline void SetNext(CEnumLinkedItem *pNext) { m_pNext = pNext; };
	inline CEnumLinkedItem *GetNext() { return m_pNext; };
	inline FINDDATA *GetData() { return &m_data; };
};

class CEnumLinkedList
{
private:
	CEnumLinkedItem *m_pHead;
	CEnumLinkedItem *m_pTail;
	CEnumLinkedItem *m_pCurrent;
public:
	CEnumLinkedList() { m_pHead = m_pTail = m_pCurrent = NULL; };
	~CEnumLinkedList();
	inline void Reset() { m_pCurrent = m_pHead; };
	FINDDATA *Next();
	BOOL Add(const FINDDATA *pData);
	void Clear();
	inline BOOL IsEmpty() { return (m_pHead == NULL); };
};

class CEnumSingleDir
{
// Constructor
public:
    CEnumSingleDir();

// Operations
public:
    int Init( LPCTSTR lpszFileSpec );
    int Next();
    int Reset();

// Attributes
public:
    const FINDDATA  m_rFind;

    LPCTSTR     GetLongFile();
    LPCTSTR     GetShortFile();

    LPCTSTR     GetFileSpec();

// Implementation
public:
    ~CEnumSingleDir();

protected:
    LPTSTR          m_pszFileSpec;
	CEnumLinkedList m_list;

    int Clear();
    int CloseAll();
};


class CDirNode
{
// Constructor
public:
    CDirNode();

// Implementation
public:
    ~CDirNode();

    CEnumSingleDir  m_EnumDir;
    CDirNode        *m_pNext;
};


class CEnumFiles
{
// Constructor
public:
    CEnumFiles();

// Operations
public:
    int Init( LPCTSTR lpszFileSpec, UINT uMaxPath = MAX_PATH );

    int Next();
    int Reset();

    int Enter();
    int Leave();

// Attributes
public:
    const FINDDATA  m_rFind;

    LPCTSTR         GetLongName();
    LPCTSTR         GetShortName();

    LPCTSTR         GetLongFile();
    LPCTSTR         GetShortFile();

    LPCTSTR         GetLongDir();
    LPCTSTR         GetShortDir();

    LPCTSTR         GetFileSpec();

// Implementation
public:
    ~CEnumFiles();

protected:
    CDirNode        *m_pDirNode;

    UINT            m_uMaxPath;
    LPTSTR          m_pszFileSpec;
    LPTSTR          m_pszFileMask;

    LPTSTR          m_pszLongName;
    LPTSTR          m_pszShortName;

    LPTSTR          m_pszLongDir;
    LPTSTR          m_pszShortDir;

    int Clear();
    int CloseAll();
};

#endif // __ENUMFILE_H

