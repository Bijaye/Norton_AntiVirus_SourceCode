// EventData.h: interface for the CEventData class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#pragma pack( push, 8 )

// Disable identifier truncation warning.  Thank you STL...
#pragma warning( disable: 4786 )

#pragma warning( push, 3 )
#include <map>
#include <vector>
#include <xstring>

#include "ItemBag.h"

//
// This class uses int's for the indexes and implements serialization.
//
class CEventData : 
    public CItemBag<long>
{
public:    
    CEventData();
	CEventData( SAFEARRAY* psa );
	virtual ~CEventData();

	// Returns this object serialized to a SAFEARRAY of VT_UI1.
	// Destroy this by calling the WIN32 SafeArrayDestory() function.
	SAFEARRAY* Serialize();
    void SerializeToVariant ( VARIANT& v );

    ULONG GetSize () const; // Size in bytes of the class data

    // Serialize to/from LPVOID
    bool LoadDataFromStream( const LPVOID pData );

    bool SaveDataToStream( LPVOID pvData, 
						   ULONG ulSize, 
						   ULONG& ulWritten) const;

    // For nesting!
    bool GetNode( long lIndex, CEventData& eventData /*out*/ );
    void SetNode( long lIndex, CEventData& eventData /*in*/ ); 

    // Copy
    CEventData( const CEventData& other );
	CEventData& operator=(const CEventData &other);

private:
	// Stream signature.
	enum { SIGNATURE = 0xDEADBEEF };
	
	// Stream header information.
	struct _HEADER 
	{
		unsigned long ulSignature;			// 0xDEADBEEF
		unsigned long ulItemCount;			// Number of items in stream.
	};

	// Item descriptor.
	struct _DESCRIPTOR 
	{
		int	Index;							// ID for this data.
		int iDataType;						// Type of data.
		unsigned long ulSize;				// Length of the data.
	};

	void buildMapFromStream( _HEADER* pHeader );
    bool writeToBuffer ( BYTE* pData ) const;
};

#pragma pack(pop)
