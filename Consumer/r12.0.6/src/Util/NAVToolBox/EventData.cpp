// EventData.cpp: implementation of the CEventData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <crtdbg.h>
#include "EventData.h"
#include "comdef.h"
using namespace std;

//////////////////////////////////////////////////////////////////////
// CEventData::CEventData()

CEventData::CEventData()
{
	// Intentionally empty.
}

CEventData::~CEventData()
{
	// Intentionally empty.
}

//////////////////////////////////////////////////////////////////////
// CEventData::CEventData()

CEventData::CEventData( SAFEARRAY* psa )
{
	if( psa == NULL )
		throw runtime_error( "Invalid Argument" );

	_HEADER* pHeader = NULL;
	try
	{
	    // Get pointer to data
	    HRESULT hr;
	    hr = SafeArrayAccessData( psa, (void HUGEP**)&pHeader );
	    if( FAILED( hr ) )
		    throw runtime_error( "Invalid Argument" );

	    // Check signature.
	    if( pHeader->ulSignature != SIGNATURE )
		    throw runtime_error( "Not a valid stream." );

	    // Ok, process the stream.
	    buildMapFromStream( pHeader );

	    // Clean up.
	    SafeArrayUnaccessData( psa );
    }
	catch(...)
	{
		if( pHeader )
		{
			// Clean up.
			SafeArrayUnaccessData( psa );
		}
		throw;
	}
}

//////////////////////////////////////////////////////////////////////
// CEventData::GetSize()

ULONG CEventData::GetSize () const
{
    // First, compute size needed for the data.
	_MAP::const_iterator it;
	size_t size = m_ItemMap.size() * sizeof( _DESCRIPTOR ) + sizeof( _HEADER ); 
	for( it = m_ItemMap.begin(); it != m_ItemMap.end(); ++it )
	{
		if( (*it).second->iDataType == TYPE_STRING )
			size += (*it).second->sItemData.length() * sizeof( WCHAR );
		else if ( (*it).second->iDataType == TYPE_BINARY )
            size += (*it).second->vBinaryData.size();
        else if ( (*it).second->iDataType == TYPE_DWORD )
			size += sizeof( long );
		else    //Unrecognized type...
            _ASSERT( false );
	}
    
    return (ULONG) size;
}

//////////////////////////////////////////////////////////////////////
// CEventData::Serialize()

SAFEARRAY* CEventData::Serialize()
{
	// Create the SAFEARRAY object.
	SAFEARRAY *pRet;
	pRet = SafeArrayCreateVector(VT_UI1, 1, GetSize ());
	if( pRet == NULL )
		return NULL;

	// Get pointer to data.
	HRESULT hr;
	BYTE* pData;
	hr = SafeArrayAccessData( pRet, (void HUGEP**)&pData );
	if( FAILED( hr ) )
	{
		SafeArrayDestroy( pRet );
		return NULL;
	}

    writeToBuffer ( pData );

	// Unlock SAFEARRAY.
	SafeArrayUnaccessData( pRet );
	return pRet;
}

void CEventData::SerializeToVariant ( VARIANT& v )
{
    // Initialize the variant data.
	v.vt = VT_ARRAY | VT_UI1;
	v.parray = Serialize();
	_ASSERT( v.parray );
}

bool CEventData::writeToBuffer ( BYTE* pData ) const
{
    if ( !pData )
        return false;

    // Write the header.
	_HEADER header;
	header.ulSignature = SIGNATURE;
	header.ulItemCount = ULONG( m_ItemMap.size() );
	CopyMemory( pData, &header, sizeof( _HEADER ) );
	pData += sizeof( _HEADER );

	// Copy data to SAFEARRAY
    _MAP::const_iterator it;

	for( it = m_ItemMap.begin(); it != m_ItemMap.end(); ++it )
	{
		long lValue;
		const void* pDataToWrite;
		_DESCRIPTOR descriptor;

		// Populate descriptor object with data.
		descriptor.Index = (*it).first;
		if( (*it).second->iDataType == TYPE_STRING )
		{
			descriptor.iDataType = TYPE_STRING;
			descriptor.ulSize = ULONG( (*it).second->sItemData.length() * sizeof( WCHAR ) );
			pDataToWrite = (*it).second->sItemData.c_str();
		}
        else if ( (*it).second->iDataType == TYPE_BINARY )
        {
            descriptor.iDataType = TYPE_BINARY;
            descriptor.ulSize = ULONG( (*it).second->vBinaryData.size() );
            pDataToWrite = &( (*it).second->vBinaryData[0] );
        }
		else if ( (*it).second->iDataType == TYPE_DWORD )
		{
			descriptor.iDataType = TYPE_DWORD;
			descriptor.ulSize = sizeof( long );
			lValue = (*it).second->lItemData;
			pDataToWrite = &lValue;
		}
        else 
		{	
			//Unrecognized data type.
            _ASSERT( false );
			continue;
		}

		// Finally, write data to SAFEARRAY.
		CopyMemory( pData, &descriptor, sizeof( _DESCRIPTOR ) );
		pData += sizeof( _DESCRIPTOR );
		CopyMemory( pData, pDataToWrite, descriptor.ulSize ); 
		pData += descriptor.ulSize;
	}

    return true;
}

//////////////////////////////////////////////////////////////////////
// CEventData::buildMapFromStream()

void CEventData::buildMapFromStream( _HEADER* pHeader )
{
    // Get pointer to first descriptor.  Ugly, but neccessary.
	_DESCRIPTOR *p = reinterpret_cast<_DESCRIPTOR*>( reinterpret_cast<BYTE*>( pHeader)  +
														sizeof( _HEADER ) );

	// Add each item to the map
	for( ULONG i=0; i < pHeader->ulItemCount; ++i )
	{
		if( p->iDataType == TYPE_STRING )
		{
			// Add string data.
			const WCHAR* pszData = reinterpret_cast<const wchar_t*>( reinterpret_cast<BYTE*>(p) + 
																sizeof( _DESCRIPTOR ) ); 
            setData( p->Index, wstring( pszData, p->ulSize / sizeof( wchar_t ) ) ); 
		}
		else if ( p->iDataType == TYPE_BINARY )
        {
            const BYTE * pData = reinterpret_cast<BYTE *>(p) + sizeof( _DESCRIPTOR );

            SetData( p->Index, pData, p->ulSize );
        }
        else if ( p->iDataType == TYPE_DWORD )
		{
			long* plData = reinterpret_cast<long*>( reinterpret_cast<BYTE*>(p) + 
																sizeof( _DESCRIPTOR ) ); 
			SetData( p->Index, *plData ); 
		}
        else
            _ASSERT( false );

		// Increment pointer to next descriptor.
		p = reinterpret_cast<_DESCRIPTOR*>( reinterpret_cast<BYTE*>(p) + 
																sizeof( _DESCRIPTOR ) + p->ulSize ); 
    }
}

bool CEventData::LoadDataFromStream( const LPVOID pData )
{
	if( pData == NULL )
		return false;

	_HEADER* pHeader = NULL;
	try
	{
		// Get pointer to data
		pHeader = (_HEADER*) pData;

		// Ok, process the stream.
		buildMapFromStream( pHeader );
		
		return true;
	}
	catch(...)
	{
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
// CEventData::SaveDataToStream()

bool CEventData::SaveDataToStream( LPVOID pvData, 
							       ULONG ulSize, 
								   ULONG& ulWritten) const
{
    // First, compute size needed for the data.
    //
	if ( GetSize () > ulSize )
        return false;

    ulWritten = GetSize ();

    // Get pointer to data.
    BYTE* pData = (BYTE*) pvData; // get around the const
    return writeToBuffer ( pData );
}

bool CEventData::GetNode( long lIndex, CEventData& eventData /*out*/ )
{
    std::vector <BYTE> vecBuffer;
    long lLength = 0;

    // This will fail and tell us how big the buffer is
    GetData ( lIndex, (BYTE*) &vecBuffer[0], lLength );

    if ( lLength )
    {
        vecBuffer.resize ( lLength );

        if ( GetData ( lIndex, (BYTE*) &vecBuffer[0], lLength ))
        {
            return eventData.LoadDataFromStream ( &vecBuffer[0] );
        }
    }

    return false;
}


void CEventData::SetNode( long lIndex, CEventData& eventData /*in*/ )
{
    std::vector <BYTE> vecBuffer;
    ULONG ulLength = eventData.GetSize ();
    ULONG ulWritten = 0;

    if ( ulLength )
    {
        vecBuffer.resize ( ulLength);

        // Copy into a temp buffer
        if ( eventData.SaveDataToStream ( &vecBuffer[0], ulLength, ulWritten ))
        {
            // Now put the buffer into a new event
            SetData ( lIndex, &vecBuffer[0], ulLength );
        }
    }
}

//////////////////////////////////////////////////////////////////////
// CEventData::CEventData() copy constructor.

CEventData::CEventData( const CEventData& other )
{
    std::vector <BYTE> vecBuffer;
    ULONG ulLength = other.GetSize ();
    ULONG ulWritten = 0;

    if ( ulLength )
    {
        vecBuffer.resize ( ulLength );

        // Copy into a temp buffer
        if ( other.SaveDataToStream ( &vecBuffer[0], ulLength, ulWritten ))
        {
            // Now put the buffer into a new event
            LoadDataFromStream ( &vecBuffer[0] );
        }
    }
}


//////////////////////////////////////////////////////////////////////
// CEventData::operator=()

CEventData& CEventData::operator=(const CEventData &other)
{
	if( &other != this )
    {
		std::vector <BYTE> vecBuffer;
        ULONG ulLength = other.GetSize ();
        ULONG ulWritten = 0;

        if ( ulLength )
        {
            vecBuffer.resize ( ulLength );

            // Copy into a temp buffer
            if ( other.SaveDataToStream ( &vecBuffer[0], ulLength, ulWritten ))
            {
                // Now put the buffer into a new event
                LoadDataFromStream ( &vecBuffer[0] );
            }
        }
    }
	return *this;	
}