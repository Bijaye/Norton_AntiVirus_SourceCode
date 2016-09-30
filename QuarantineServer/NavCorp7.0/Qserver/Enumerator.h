// Enumerator.h: interface for the CEnumerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENUMERATOR_H__BA0D2476_8957_11D2_8E5F_2A0712000000__INCLUDED_)
#define AFX_ENUMERATOR_H__BA0D2476_8957_11D2_8E5F_2A0712000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 
// For QSERVERITEM
// 


class CEnumerator  
{
public:
    CEnumerator( LPCTSTR pszFolder );
    virtual ~CEnumerator();

    BOOL Enumerate();
    BOOL GetNext( LPTSTR pszFileName );
    void Reset(){ m_pCurrentPosition = m_pListHead; }
    ULONG GetCount(){ return m_ulCount; }

private:
    TCHAR       m_szFolder[ MAX_PATH + 1 ];

private:
    // 
    // Linked list of quarantine items
    // 
    class CQItem
        {
        public:
            CQItem*         pNext;
            LPTSTR          szFileName;

            CQItem() : pNext( NULL ), szFileName( NULL ) {};
            ~CQItem() 
                { 
                if( szFileName ) 
                    delete [] szFileName; 
                }
            CQItem( LPCTSTR pszFileName ) : pNext( NULL )
                { 
                szFileName = new TCHAR[ lstrlen( pszFileName ) + 1]; 
                lstrcpy( szFileName, pszFileName );
                }
            operator LPCTSTR() const  { return szFileName; }

        };

    CQItem*     m_pCurrentPosition;
    CQItem*     m_pListHead;
    ULONG       m_ulCount;

};

#endif // !defined(AFX_ENUMERATOR_H__BA0D2476_8957_11D2_8E5F_2A0712000000__INCLUDED_)
