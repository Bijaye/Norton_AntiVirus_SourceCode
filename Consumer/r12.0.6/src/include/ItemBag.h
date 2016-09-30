#pragma once

#include <map>
#include <vector>
#include <string>
#include <xstring> // wstring

#include <crtdbg.h>

template< class T >
class CItemBag
{
public:
    CItemBag(void);
    virtual ~CItemBag(void);

	// Data Accessors.
    bool GetData( T _Index, std::string & sOut );
    bool GetData( T _Index, std::wstring & sOut );

    bool GetData( T _Index, long & lOut );
	void SetData( T _Index, long lIn );

    // lLength will return the size of the buffer needed if you pass in lLength of NULL
    //
    bool GetData( T _Index, BYTE * pData, long& lLength );
    void SetData( T _Index, const BYTE * pData, long lLength );

    // C-Style
    bool GetData( T _Index, char* sOut );
    void SetData( T _Index, const char* sIn );

    bool GetData( T _Index, WCHAR* sOut );
    void SetData( T _Index, const WCHAR* sIn );

    // Number of entries in the map (useful for enum)
    long GetCount ();

    // Supported event data types.
	enum { TYPE_STRING = 0, TYPE_DWORD, TYPE_BINARY };
    
protected:
    // The compiler was putting wstring data into a single byte std::string
    // unless you specified SetData ( index, (LPWSTR) string);  This
    // caused defects on DBCS strings. See defect 1-3KRUMM.
    void setData( T _Index, const std::string & sIn );
    void setData( T _Index, const std::wstring & sIn );


	// Disallowed
	CItemBag( CItemBag& other );

    // In-memory representation of data.
	class CItemData
	{
	public:
		CItemData()
        {
            iDataType = 0;
            lItemData = 0;
        };
		~CItemData(){};

        int		iDataType;
        T       Index;
        
        // Only one of these will contain data
		std::wstring	sItemData;
		long	lItemData;
        std::vector<BYTE> vBinaryData;
	};
    
    void getItem ( T _Index, CItemData*& pItem );

	// Map of attributes.
    typedef std::map< T, CItemData* >	_MAP;
	_MAP							m_ItemMap;
};

//////////////////////////////////////////////////////////////////////
// CItemBag::CItemBag()
template< class T > 
CItemBag< T >::CItemBag()
{
}

template< class T > 
CItemBag< T >::~CItemBag()
{
    // Cleanup.
	_MAP::iterator it;
	for( it = m_ItemMap.begin(); it != m_ItemMap.end(); ++it )
	{
		if ( (*it).second )
        {
            delete (*it).second;
            (*it).second = NULL;
        }
	}
    
    m_ItemMap.clear ();
}

//////////////////////////////////////////////////////////////////////
// CItemBag::GetData()

template< class T > 
bool CItemBag< T >::GetData( T _Index, std::string & sOut )
{
   // Call the Wide Char Version of Get Data
	std::wstring wsTemp;
	if( GetData( _Index, wsTemp ) == false )
		return false;

	// Setup a temporary buffer
    std::vector<char> vAnsiBuff;
	vAnsiBuff.reserve( (wsTemp.size()+1)*sizeof(WCHAR) );

	// Convert the wstring to an ANSI string
    WideCharToMultiByte( CP_ACP, 0, wsTemp.c_str(), -1, &vAnsiBuff[0], int((wsTemp.size()+1)*sizeof(WCHAR)), NULL, NULL );
	sOut = &vAnsiBuff[0];

    return true;
}


//////////////////////////////////////////////////////////////////////
// CItemBag::GetData()

template< class T > 
bool CItemBag< T >::GetData( T _Index, std::wstring & sOut )
{
    // Lookup the item.
	_MAP::iterator it = m_ItemMap.find(_Index);
	if( it == m_ItemMap.end() )
		return false;

	// Verify the type
	if( (*it).second->iDataType != TYPE_STRING )
        throw std::runtime_error( "Incorrect type." );
    
	sOut = (*it).second->sItemData;
	return true;
}

//////////////////////////////////////////////////////////////////////
// CItemBag::GetData()

template< class T > 
bool CItemBag< T >::GetData( T _Index, long & lOut )
{
    _MAP::iterator it = m_ItemMap.find(_Index);
	if( it == m_ItemMap.end() )
		return false;

	// Verify the type
	if( (*it).second->iDataType != TYPE_DWORD )
        throw std::runtime_error( "Incorrect type." );

	lOut = (*it).second->lItemData;
	return true;
}

//////////////////////////////////////////////////////////////////////
// CItemBag::GetData()

template< class T > 
bool CItemBag< T >::GetData( T _Index, BYTE * pData, long& lLength )
{
    // Lookup the item.
	_MAP::iterator it = m_ItemMap.find(_Index);
	if( it == m_ItemMap.end() )
		return false;

	// Verify the type
	if( (*it).second->iDataType != TYPE_BINARY )
        return false;
        //throw std::runtime_error( "Incorrect type." );
    
	if ( lLength >= static_cast<long>( (*it).second->vBinaryData.size() ) )
    {
        lLength = long( (*it).second->vBinaryData.size() );
        CopyMemory( pData, &( (*it).second->vBinaryData[0] ), lLength );
    }
    else
    {
        // Tell the client the size
        lLength = static_cast<long>( (*it).second->vBinaryData.size() );
        return false;
    }

	return true;
}

//////////////////////////////////////////////////////////////////////
// CItemBag::setData()

template< class T > 
void CItemBag< T >::setData( T _Index, const std::string & sIn )
{
    // Convert to Wide chars
    std::vector<WCHAR> vWideChar;
	vWideChar.reserve( sIn.size() + 1 );

	MultiByteToWideChar( CP_ACP, 0, sIn.c_str(), -1, &vWideChar[0], int( sIn.size() + 1 ) );

	SetData( _Index, &vWideChar[0] );
}


//////////////////////////////////////////////////////////////////////
// CItemBag::setData()

template< class T > 
void CItemBag< T >::setData( T _Index, const std::wstring & sIn )
{
    CItemData *item;
    getItem ( _Index, item );

    // Insert item into map.  Overwrite existing data.
	item->iDataType = TYPE_STRING;
	item->Index = _Index;
	item->sItemData = sIn;
	m_ItemMap[_Index] = item;
}

//////////////////////////////////////////////////////////////////////
// CItemBag::SetData()

template< class T > 
void CItemBag< T >::SetData( T _Index, long lIn )
{
    CItemData *item;
    getItem ( _Index, item );

	item->iDataType = TYPE_DWORD;
	item->Index = _Index;
	item->lItemData = lIn;
	m_ItemMap[_Index] = item;
}

//////////////////////////////////////////////////////////////////////
// CItemBag::SetData()

template< class T > 
void CItemBag< T >::SetData( T _Index, const BYTE * pData, long lLength )
{
    CItemData *item;
    getItem ( _Index, item );

	item->iDataType = TYPE_BINARY;
	item->Index = _Index;
	item->vBinaryData.resize( lLength );
    CopyMemory( &(item->vBinaryData[0]), pData, lLength );
	m_ItemMap[_Index] = item;
}

// void CItemBag::getData ()
//
// Function will look up the item and return it.
// It will create a new one if it doesn't exist.
//
template< class T > 
void CItemBag< T >::getItem ( T _Index, CItemData*& pItem )
{
    _MAP::iterator it = m_ItemMap.find(_Index);
	if( it != m_ItemMap.end() )
    {
	    // Verify the type
        pItem = (*it).second;
    }
    else
    {
        pItem = new CItemData;
    }
}

template< class T >
bool CItemBag< T >::GetData( T _Index, WCHAR* sOut )
{
    std::wstring strOut;
    if ( GetData ( _Index, strOut ))
    {
        wcsncpy (sOut, strOut.c_str(), strOut.size());
		return true;
    }

    return false;
}

template< class T >
bool CItemBag< T >::GetData( T _Index, char* sOut )
{
    std::string strOut;
    if ( GetData ( _Index, strOut ))
    {
        strncpy (sOut, strOut.c_str(), strOut.size());
        return true;
    }

    return false;
}

template< class T >
void CItemBag< T >::SetData( T _Index, const char* sIn )
{
    std::string strIn;
    strIn = sIn;
    setData ( _Index, strIn );
}

template< class T >
void CItemBag< T >::SetData( T _Index, const WCHAR* sIn )
{
    std::wstring strIn;
    strIn = sIn;
    setData ( _Index, strIn );
}

template< class T >
long CItemBag< T >::GetCount ()
{
    return m_ItemMap.size();
}