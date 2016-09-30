// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "DecPlat.h"
#include "Dec2.h"
#include "IMIME.h"
#include "bstricmp.h"
#include <stdio.h>

inline bool	CMIMEAttributeCollection::ValidIndex(size_t index) const
{
	return (index < m_nBufferSize);
}

inline 	unsigned char	CMIMEAttributeCollection::ConvertToLowerCase(unsigned char aChar) const
{
	return  ( ( aChar >= 0x41 && aChar <= 0x5A ) ? (aChar | 0x20) : (aChar) );
}

IDecAttributeCollection::DerivedType CMIMEAttributeCollection::GetType() const 
{ 
	return IDecAttributeCollection::eCMIMEAttributeCollection; 
}


const CDecAttributeKey* CMIMEAttributeCollection::GetKeyAt(size_t index ) const 
{ 
	const CDecAttributeKey* result = NULL;
	if ( m_nValuePairIndex > 0 && index < m_nValuePairIndex && m_pHeaderBuffer != NULL ) 
	{
		CMIMEAttributeCollection* nonConstThis = (CMIMEAttributeCollection*)this;

		CKeyValue* keyValuePtr = nonConstThis->m_keyValuePairs[index];
		result = keyValuePtr->GetKey();
	}
	return result; 
}

IDecAttribute* CMIMEAttributeCollection::GetAttributeAt ( const CDecAttributeKey& key,  size_t index ) const 
{ 
	CMIMEAttributeCollection* nonConstThis = (CMIMEAttributeCollection*)this;
	CDecTextGeneric* pResult = NULL;

	size_t foundAt = 0;

	if (m_pHeaderBuffer != NULL && m_nValuePairIndex > 0 )
	{
		for ( size_t i = 0; i < m_nValuePairIndex; i++)
		{
			CKeyValue* keyValuePtr = nonConstThis->m_keyValuePairs[i];

			if ( key == keyValuePtr->GetKey() )
			{
				foundAt++;
				if ( foundAt == index+1 )
				{
					pResult = keyValuePtr->GetAttribute();
					if (	pResult == NULL
						&&	ValidIndex(keyValuePtr->GetKeyOffset())
						&&	ValidIndex(keyValuePtr->GetValueOffset()) )
					{
						const char* keyPtr = &m_pHeaderBuffer[keyValuePtr->GetKeyOffset()];
						size_t keyLength = keyValuePtr->GetKeyLength();

						const char* valuePtr = &m_pHeaderBuffer[keyValuePtr->GetValueOffset()];
						size_t valueLength = keyValuePtr->GetValueLength();
						pResult = new CDecTextGeneric(	keyValuePtr->GetKey(), IDecAttribute::Alias,
														valuePtr, valueLength,IDecAttribute::Alias);
						keyValuePtr->SetAttribute(pResult);
					}
					break;
				}
			}
		}
	}


	return pResult;
}


size_t CMIMEAttributeCollection::GetNumberOfValuesForKey( const CDecAttributeKey& key ) const 
{ 
	size_t numberOfValues = 0;
	
	if (m_pHeaderBuffer != NULL && m_nValuePairIndex > 0 )
	{

		CMIMEAttributeCollection* nonConstThis = (CMIMEAttributeCollection*)this;

		for ( size_t i = 0; i < m_nValuePairIndex; i++)
		{
			CKeyValue* keyValuePtr = nonConstThis->m_keyValuePairs[i];

			if ( key == keyValuePtr->GetKey() )
			{
				numberOfValues++;
			}
		}
	}

	return numberOfValues;
}


CMIMEAttributeCollection::CMIMEAttributeCollection() 
:	IDecAttributeCollection(),
	m_pHeaderBuffer(NULL),
	m_nBufferSize(0),
	m_nHeaderBaseOffset(0),
	m_keyValuePairs(),
	m_nValuePairIndex(0),
	m_nContentTypeIndex(-1),
	m_nContentTypeSlashOffset(-1),
	m_nContentTypeBoundaryOffset(-1),
	m_nContentTypeCharsetOffset(-1),
	m_nContentTypeLength(0),
	m_nContentTypePtr(NULL),
	m_nContentTypeBoundaryLength(0),
	m_nContentTypeBoundaryPtr(NULL),
	m_nContentTypeCharsetLength(0),
	m_nContentTypeCharsetPtr(NULL)
{
	m_keyValuePairs.initializeMemory(true);
	m_keyValuePairs.reset();
}

void CMIMEAttributeCollection::SetContentTypeSlashSeparatorOffset(size_t nOffset)
{
	if (	 nOffset > m_nHeaderBaseOffset 
		&& ( m_nBufferSize != 0 ? nOffset < m_nBufferSize : true ) )
	{
		m_nContentTypeSlashOffset = nOffset-m_nHeaderBaseOffset;
	}
}

void CMIMEAttributeCollection::SetContentTypeBoundaryOffset(size_t nOffset)
{
	if (	 nOffset > m_nHeaderBaseOffset 
		&& ( m_nBufferSize != 0 ? nOffset < m_nBufferSize : true ) )
	{
		m_nContentTypeBoundaryOffset = nOffset-m_nHeaderBaseOffset;
	}
}

void CMIMEAttributeCollection::SetContentTypeCharset(size_t nOffset,const char* pCharset,size_t nLength)
{
	if (	 nOffset > m_nHeaderBaseOffset 
		&& ( m_nBufferSize != 0 ? nOffset < m_nBufferSize : true ) )
	{
		if ( m_nContentTypeCharsetPtr != NULL )
		{
			free (m_nContentTypeCharsetPtr);
		}
		m_nContentTypeCharsetPtr = (char*)malloc(nLength+1);
		if (m_nContentTypeCharsetPtr != NULL )
		{
			memcpy(m_nContentTypeCharsetPtr,pCharset,nLength);
			m_nContentTypeCharsetPtr[nLength] = 0;
			m_nContentTypeCharsetLength = nLength;
		}
	}
}

CMIMEAttributeCollection::~CMIMEAttributeCollection() 
{ 
	AdoptHeaderBuffer(NULL,0);
	for ( size_t i = 0; i < m_keyValuePairs.getSize(); i++ )
	{
		delete m_keyValuePairs[i];
	}
	
	if ( m_nContentTypeCharsetPtr != NULL )
	{
		free(m_nContentTypeCharsetPtr);
		m_nContentTypeCharsetPtr = NULL;
	}

	if ( m_nContentTypeBoundaryPtr != NULL )
	{
		free(m_nContentTypeBoundaryPtr);
		m_nContentTypeBoundaryPtr = NULL;
	}

	if ( m_nContentTypePtr != NULL )
	{
		free(m_nContentTypePtr);
		m_nContentTypePtr = NULL;
	}
}

CMIMEAttributeCollection& CMIMEAttributeCollection::operator=(const CMIMEAttributeCollection& rhs) 
{ 
	return *this; 
}

CMIMEAttributeCollection::CMIMEAttributeCollection(const CMIMEAttributeCollection& rhs) 
{
}

void CMIMEAttributeCollection::AdoptHeaderBuffer(char* pBuffer, size_t nBufferSize)
{
	if ( m_pHeaderBuffer != NULL )
	{
		delete m_pHeaderBuffer;
	}

	m_pHeaderBuffer = pBuffer;
	m_nBufferSize = nBufferSize;
	
	if ( m_pHeaderBuffer != NULL )
	{
		AdjustKeyValues();
	}
}

// This function strips white space from the keys and values stored in this collection.
// The white space is removed by updating the offsets and lengths of the keys and values.

void CMIMEAttributeCollection::AdjustKeyValues() 
{
	if ( m_pHeaderBuffer != NULL )
	{

		for ( size_t i = 0; i < m_nValuePairIndex; i++ )
		{
			m_keyValuePairs[i]->SetBaseAddress(m_pHeaderBuffer);
			CKeyValue value = *(m_keyValuePairs[i]);
			
			size_t	keyOffset	= value.GetKeyOffset();
			size_t	keyLength	= value.GetKeyLength();

			size_t	valueOffset	= value.GetValueOffset();
			size_t	valueLength	= value.GetValueLength();

			if (   ValidIndex(keyOffset) 
				&& ValidIndex(keyOffset+keyLength)
				&& ValidIndex(valueOffset)
				&& ValidIndex(valueOffset+valueLength) )
			{
				char *	valuePtr	= &m_pHeaderBuffer[valueOffset];
				char *	keyPtr		= &m_pHeaderBuffer[keyOffset];

				// Convert the key and value to 'C' strings.

				m_pHeaderBuffer[keyOffset+keyLength] = 0;
				m_pHeaderBuffer[valueOffset+valueLength] = 0;

				// Trim white space from the key value pair if needed
				{
					bool replaceValue = false;
					
					size_t whiteSpaceInFront = 0;
					size_t whiteSpaceAtRear = 0;

					if ( HasWhiteSpace(keyPtr,keyLength,whiteSpaceInFront,whiteSpaceAtRear) )
					{
						value.SetKeyOffset(keyOffset+whiteSpaceInFront);
						value.SetKeyLength(keyLength-whiteSpaceInFront-whiteSpaceAtRear);
						
						// Move the null terminator as needed to truncate the 'C' string.

						m_pHeaderBuffer[value.GetKeyOffset()+value.GetKeyLength()] = 0;
						replaceValue = true;
					}
			
					if ( HasWhiteSpace(valuePtr,valueLength,whiteSpaceInFront,whiteSpaceAtRear) )
					{
						value.SetValueOffset(valueOffset+whiteSpaceInFront);
						value.SetValueLength(valueLength-whiteSpaceInFront-whiteSpaceAtRear);

						// Move the null terminator as needed to truncate the 'C' string.

						m_pHeaderBuffer[value.GetValueOffset()+value.GetValueLength()] = 0;
						replaceValue = true;
					}

					if ( replaceValue )
					{
						delete m_keyValuePairs[i];
						m_keyValuePairs[i] = new CKeyValue(value);
					}
				}
			}
		}
	}

}
 

bool CMIMEAttributeCollection::HasWhiteSpace(char* pString,size_t nStringLength, size_t& whiteSpaceInFront, size_t& whiteSpaceAtRear) const
{
	whiteSpaceAtRear = 0;
	whiteSpaceInFront = 0;
	

	if ( nStringLength > 0 )
	{
		int i = nStringLength-1;
		while ( pString[i] == 0x20 || pString[i] == 0x09 )
		{
			whiteSpaceAtRear++;
			i--;
		}

		i = 0;
		while ( pString[i] == 0x20 || pString[i] == 0x09 )
		{
			whiteSpaceInFront++;
			i++;
		}
	}

	return (whiteSpaceInFront > 0 || whiteSpaceAtRear > 0 );
}

const char* CMIMEAttributeCollection::GetRawHeaderBuffer(size_t& nbufferSizeFillin) const
{
	nbufferSizeFillin = m_nBufferSize;
	return m_pHeaderBuffer;
}


const char*	CMIMEAttributeCollection::GetContentType(size_t& nLengthFillin) const
{
	nLengthFillin = m_nContentTypeLength;
	char* result = m_nContentTypePtr;
	
	if (	m_nContentTypePtr == NULL 
		&&	m_nContentTypeSlashOffset != -1 
		&&	m_pHeaderBuffer != NULL
		&&	ValidIndex(m_nContentTypeSlashOffset))
	{
		// Point to the slash first and then walk backward
		result = &m_pHeaderBuffer[m_nContentTypeSlashOffset];
	
		char oneChar;

		while (	(oneChar = *result) != 0x20		// space
				&&  oneChar != 0				// C string terminator
				&&	oneChar != 0x09				// Tab
				&&	oneChar != 0x3A				// :
				&&	result != m_pHeaderBuffer )
		{
			result--;
			nLengthFillin++;
		}

		// Point to the start of the content type value.
		result++;

		char* ptr = &m_pHeaderBuffer[m_nContentTypeSlashOffset+1];
		size_t index = 0;

		while (	(oneChar = ptr[index]) != 0x20	// space
				&&	oneChar != 0				// C string terminator
				&&	oneChar != 0x09				// Tab
				&&	oneChar != 0x3A				// :
				&&	oneChar != 0x3B				// ;
				&&	oneChar != 0x0D				// CR
				&&	oneChar != 0x0A				// LF 
				&&	ValidIndex(index) )
		{
			index++;
			nLengthFillin++;
		}

		((CMIMEAttributeCollection*)this)->m_nContentTypeLength = nLengthFillin;
		// copy the boundary to a new buffer.  We copy the boundary to preserve
		// the original content-type field.

		((CMIMEAttributeCollection*)this)->m_nContentTypePtr = (char*) malloc(nLengthFillin+1);

		if ( m_nContentTypePtr != NULL )
		{
			memcpy(	((CMIMEAttributeCollection*)this)->m_nContentTypePtr,
					result,
					nLengthFillin);
			result = ((CMIMEAttributeCollection*)this)->m_nContentTypePtr;
			result[nLengthFillin] = 0;
		}
		else
		{
			result = NULL;
		}
	}
	return result;
}

const char*		CMIMEAttributeCollection::GetContentTypeCharset(size_t& nLengthFillin) const
{
	nLengthFillin = m_nContentTypeCharsetLength;
	return m_nContentTypeCharsetPtr;
}


const char*		CMIMEAttributeCollection::GetContentTypeBoundary(size_t& nLengthFillin) const
{
	nLengthFillin = m_nContentTypeBoundaryLength;
	char* result = m_nContentTypeBoundaryPtr;
	
	if (	m_nContentTypeBoundaryPtr == NULL
		&&	m_nContentTypeBoundaryOffset != -1 
		&&	m_pHeaderBuffer != NULL
		&&	ValidIndex(m_nContentTypeBoundaryOffset+1))
	{
		// Point to the = after boundary
		result = &m_pHeaderBuffer[m_nContentTypeBoundaryOffset+1];

		char oneChar;

		while (		(oneChar = result[nLengthFillin]) != 0x20	// space
				&&	oneChar != 0				// 'C' string terminator
				&&	oneChar != 0x09				// Tab
				&&	oneChar != 0x3A				// :
				&&	oneChar != 0x3B				// ;
				&&	oneChar != 0x0D				// CR
				&&	oneChar != 0x0A				// LF 
				&&	ValidIndex(nLengthFillin) )
		{
			nLengthFillin++;
		}

		((CMIMEAttributeCollection*)this)->m_nContentTypeBoundaryLength = nLengthFillin;
		// copy the boundary to a new buffer.  We copy the boundary to preserve
		// the original content-type field.

		((CMIMEAttributeCollection*)this)->m_nContentTypeBoundaryPtr = (char*) malloc(nLengthFillin+1);

		if ( m_nContentTypeBoundaryPtr != NULL )
		{
			memcpy(	((CMIMEAttributeCollection*)this)->m_nContentTypeBoundaryPtr,
					result,
					nLengthFillin);
			result = ((CMIMEAttributeCollection*)this)->m_nContentTypeBoundaryPtr;		
			result[nLengthFillin] = 0;
		}
		else
		{
			result = NULL;
		}
	}
	return result;
}

void CMIMEAttributeCollection::SetHeaderBaseOffset(size_t nBaseOffset) 
{
	m_nHeaderBaseOffset = nBaseOffset;
} 

bool CMIMEAttributeCollection::GetStartOfHeaderOffset(size_t& nOffsetFillin) const
{
	nOffsetFillin = m_nHeaderBaseOffset;
	return ( m_pHeaderBuffer != NULL );
}

void CMIMEAttributeCollection::AddKeyValue(int nKeyID,size_t nKeyOffset,size_t nValueOffset,size_t nValueLength)
{

	m_keyValuePairs[m_nValuePairIndex++] = new CKeyValue(nKeyID,
														 nKeyOffset-m_nHeaderBaseOffset,
														 (nValueOffset-1)-nKeyOffset,
														 nValueOffset-m_nHeaderBaseOffset,
														 nValueLength,NULL);

	switch ( nKeyID )
	{
		case MIME_TOKEN_CONTENT_TYPE: {
											m_nContentTypeIndex = m_nValuePairIndex-1;
											break;
									  }

		case MIME_TOKEN_RETURN_PATH:
		case MIME_TOKEN_RECEIVED:
		case MIME_TOKEN_REPLY_TO:
		case MIME_TOKEN_FROM:
		case MIME_TOKEN_SENDER:
		case MIME_TOKEN_DATE:
		case MIME_TOKEN_TO:
		case MIME_TOKEN_CC:
		case MIME_TOKEN_BCC:
		case MIME_TOKEN_MESSAGE_ID:
		case MIME_TOKEN_IN_REPLY_TO:
		case MIME_TOKEN_REFERENCES:
		case MIME_TOKEN_KEYWORDS:
		case MIME_TOKEN_SUBJECT:
		case MIME_TOKEN_COMMENTS:
		case MIME_TOKEN_ENCRYPTED:
		case MIME_TOKEN_CONTENT_DISPOSITION:
		case MIME_TOKEN_CONTENT_DESCRIPTION:
		case MIME_TOKEN_CONTENT_TRANSFER_ENCODING:
		case MIME_TOKEN_CONTENT_ID:
		case MIME_TOKEN_MIME_VERSION:
		case MIME_TOKEN_IMPORTANCE:
		case MIME_TOKEN_ORGANIZATION:
		case MIME_TOKEN_AUTHOR:
		case MIME_TOKEN_PRIORITY:
		case MIME_TOKEN_DELIVERED_TO:
		case MIME_TOKEN_CONTENT_LOCATION:
		case MIME_TOKEN_THREAD_TOPIC:
		case MIME_TOKEN_THREAD_INDEX:
		case MIME_TOKEN_MAILER:
		case MIME_TOKEN_CONTENT_LENGTH:
		case MIME_TOKEN_HELO:
		case MIME_TOKEN_DATA:
		case MIME_TOKEN_ACCEPT_LANGUAGE:
		case MIME_TOKEN_MSMAIL_PRIORITY:
		case MIME_TOKEN_USER_AGENT:
		case MIME_TOKEN_APPARENTLY_TO:
		case MIME_TOKEN_ID:
		case MIME_TOKEN_EGROUPS_RETURN:
		case MIME_TOKEN_LIST_UNSUBSCRIBE:
		case MIME_TOKEN_LIST_SUBSCRIBE:
		case MIME_TOKEN_AUTO_SUBMITTED:
		case MIME_TOKEN_ORIGINATING_IP:
		case MIME_TOKEN_MAILER_VERSION:
		case MIME_TOKEN_LIST_ID:
		case MIME_TOKEN_LIST_POST:
		case MIME_TOKEN_LIST_HELP:
		case MIME_TOKEN_ERRORS_TO:
		case MIME_TOKEN_MAILING_LIST:
		case MIME_TOKEN_MS_HAS_ATTACH:
		case MIME_TOKEN_MS_TNEF_CORRELATOR:
		case MIME_TOKEN_MIME_AUTOCONVERTED:
		case MIME_TOKEN_CONTENT_CLASS:
		case MIME_TOKEN_PRECEDENCE:
		case MIME_TOKEN_ORIGINALARRIVALTIME:
		case MIME_TOKEN_MIMEOLE:
		case MIME_TOKEN_LIST_ARCHIVE:
		case MIME_TOKEN_UNKNOWN_FIELD:
			{
				break;
			}
	}
}

CDecTextGeneric*	CMIMEAttributeCollection::CKeyValue::GetAttribute() const
{
	return m_pAttribute;
}

void	CMIMEAttributeCollection::CKeyValue::SetAttribute(CDecTextGeneric* pAttribute)
{
	if ( m_pAttribute != NULL )
	{
		delete m_pAttribute;
	}
	m_pAttribute = pAttribute;
}

CMIMEAttributeCollection::CKeyValue::CKeyValue(int nKeyID,size_t nKeyOffset,size_t nKeyLength, size_t nValueOffset,size_t nValueLength,CDecTextGeneric* pAttribute)
:	m_nKeyID(nKeyID),
	m_nKeyOffset(nKeyOffset),
	m_nKeyLength(nKeyLength),
	m_pAttribute(pAttribute),
	m_nValueOffset(nValueOffset),
	m_nValueLength(nValueLength),
	m_pBaseAddress(0),
	m_bDeleteKey(false),
	m_pKey(0)
{
}

CMIMEAttributeCollection::CKeyValue::CKeyValue()
:	m_nKeyID(0),
	m_nKeyOffset(0),
	m_nKeyLength(0),
	m_nValueOffset(0),
	m_nValueLength(0),
	m_pAttribute(0),
	m_pBaseAddress(0),
	m_bDeleteKey(false),
	m_pKey(0)
{
}

CMIMEAttributeCollection::CKeyValue::~CKeyValue()
{
	if ( m_pAttribute != NULL )
	{
		delete m_pAttribute;
	}

	if ( m_bDeleteKey )
	{
		delete m_pKey;
	}
}

int		CMIMEAttributeCollection::CKeyValue::GetKeyID () const
{
	return m_nKeyID;
}

size_t	CMIMEAttributeCollection::CKeyValue::GetKeyOffset() const
{
	return m_nKeyOffset;
}

size_t	CMIMEAttributeCollection::CKeyValue::GetKeyLength() const
{
	return m_nKeyLength;
}

size_t	CMIMEAttributeCollection::CKeyValue::GetValueOffset() const
{
	return m_nValueOffset;
}

size_t	CMIMEAttributeCollection::CKeyValue::GetValueLength() const
{
	return m_nValueLength;
}

void	CMIMEAttributeCollection::CKeyValue::SetKeyOffset(size_t nValue)
{
	m_nKeyOffset = nValue;
}

void	CMIMEAttributeCollection::CKeyValue::SetKeyLength(size_t nValue)
{
	m_nKeyLength = nValue;
}

void	CMIMEAttributeCollection::CKeyValue::SetValueOffset(size_t nValue)
{
	m_nValueOffset = nValue;
}

void	CMIMEAttributeCollection::CKeyValue::SetValueLength(size_t nValue)
{
	m_nValueLength = nValue;
}

void	CMIMEAttributeCollection::CKeyValue::SetBaseAddress(void* baseAddress)
{
	m_pBaseAddress = baseAddress;
}


const CDecAttributeKey*	CMIMEAttributeCollection::CKeyValue::GetKey() const
{
	if (m_pKey == NULL &&  m_pBaseAddress != NULL )
	{
		CKeyValue* ptr = (CKeyValue*)this;
		ptr->m_bDeleteKey = false;
		switch ( m_nKeyID )
		{
			case MIME_TOKEN_CONTENT_TYPE:				{	ptr->m_pKey = &CDecAttributeKey::kContentType;			 	break;	}
			case MIME_TOKEN_RETURN_PATH:				{	ptr->m_pKey = &CDecAttributeKey::kReturnPath;				break;	}
			case MIME_TOKEN_RECEIVED:					{	ptr->m_pKey = &CDecAttributeKey::kReceived; 				break;	}
			case MIME_TOKEN_REPLY_TO:					{	ptr->m_pKey = &CDecAttributeKey::kReplyTo;					break;	}
			case MIME_TOKEN_FROM:						{	ptr->m_pKey = &CDecAttributeKey::kFrom; 					break;	}
			case MIME_TOKEN_SENDER:						{	ptr->m_pKey = &CDecAttributeKey::kSender;					break;	}
			case MIME_TOKEN_DATE:						{	ptr->m_pKey = &CDecAttributeKey::kDate;						break;	}
			case MIME_TOKEN_TO:							{	ptr->m_pKey = &CDecAttributeKey::kTo;						break;	}
			case MIME_TOKEN_CC:							{	ptr->m_pKey = &CDecAttributeKey::kCc;						break;	}
			case MIME_TOKEN_BCC:						{	ptr->m_pKey = &CDecAttributeKey::kBcc;						break;	}
			case MIME_TOKEN_MESSAGE_ID:					{	ptr->m_pKey = &CDecAttributeKey::kMessageId;				break;	}
			case MIME_TOKEN_IN_REPLY_TO:				{	ptr->m_pKey = &CDecAttributeKey::kInReplyTo;				break;	}
			case MIME_TOKEN_REFERENCES:					{	ptr->m_pKey = &CDecAttributeKey::kReferences;				break;	}
			case MIME_TOKEN_KEYWORDS:					{	ptr->m_pKey = &CDecAttributeKey::kKeywords;					break;	}
			case MIME_TOKEN_SUBJECT:					{	ptr->m_pKey = &CDecAttributeKey::kSubject;					break;	}
			case MIME_TOKEN_COMMENTS:					{	ptr->m_pKey = &CDecAttributeKey::kComments;					break;	}
			case MIME_TOKEN_ENCRYPTED:					{	ptr->m_pKey = &CDecAttributeKey::kEncrypted;				break;	}
			case MIME_TOKEN_CONTENT_DISPOSITION:		{	ptr->m_pKey = &CDecAttributeKey::kContentDisposition;		break;	}
			case MIME_TOKEN_CONTENT_DESCRIPTION:		{	ptr->m_pKey = &CDecAttributeKey::kContentDescription;		break;	}
			case MIME_TOKEN_CONTENT_TRANSFER_ENCODING:	{	ptr->m_pKey = &CDecAttributeKey::kContentTransferEncoding; 	break;	}
			case MIME_TOKEN_CONTENT_ID:					{	ptr->m_pKey = &CDecAttributeKey::kContentId;				break;	}
			case MIME_TOKEN_MIME_VERSION:				{	ptr->m_pKey = &CDecAttributeKey::kMimeVersion;				break;	}
			case MIME_TOKEN_IMPORTANCE:					{	ptr->m_pKey = &CDecAttributeKey::kImportance;				break;	}
			case MIME_TOKEN_ORGANIZATION:				{	ptr->m_pKey = &CDecAttributeKey::kOrganization;			 	break;	}
			case MIME_TOKEN_AUTHOR:						{	ptr->m_pKey = &CDecAttributeKey::kAuthor;					break;	}
			case MIME_TOKEN_PRIORITY:					{	ptr->m_pKey = &CDecAttributeKey::kPriority;					break;	}
			case MIME_TOKEN_DELIVERED_TO:				{	ptr->m_pKey = &CDecAttributeKey::kDeliveredTo;				break;	}
			case MIME_TOKEN_CONTENT_LOCATION:			{	ptr->m_pKey = &CDecAttributeKey::kContentLocation; 			break;	}
			case MIME_TOKEN_THREAD_TOPIC:				{	ptr->m_pKey = &CDecAttributeKey::kThreadTopic; 				break;	}
			case MIME_TOKEN_THREAD_INDEX:				{	ptr->m_pKey = &CDecAttributeKey::kThreadIndex;				break;	}
			case MIME_TOKEN_MAILER:						{	ptr->m_pKey = &CDecAttributeKey::kMailer;					break;	}
			case MIME_TOKEN_CONTENT_LENGTH:				{	ptr->m_pKey = &CDecAttributeKey::kContentLength;			break;	}
			case MIME_TOKEN_ACCEPT_LANGUAGE:			{	ptr->m_pKey = &CDecAttributeKey::kAcceptLanguage;			break;	}
			case MIME_TOKEN_MSMAIL_PRIORITY:			{	ptr->m_pKey = &CDecAttributeKey::kMSMailPriority;			break;	}
			case MIME_TOKEN_APPARENTLY_TO:				{	ptr->m_pKey = &CDecAttributeKey::kApparentlyTo;				break;	}
			case MIME_TOKEN_ID:							{	ptr->m_pKey = &CDecAttributeKey::kId; 						break;	}
			case MIME_TOKEN_EGROUPS_RETURN:				{	ptr->m_pKey = &CDecAttributeKey::kEgroupsReturn;			break;	}
			case MIME_TOKEN_LIST_UNSUBSCRIBE:			{	ptr->m_pKey = &CDecAttributeKey::kListUnsubscribe;			break;	}
			case MIME_TOKEN_LIST_SUBSCRIBE:				{	ptr->m_pKey = &CDecAttributeKey::kListSubscribe;			break;	}
			case MIME_TOKEN_AUTO_SUBMITTED:				{	ptr->m_pKey = &CDecAttributeKey::kAutoSubmitted;			break;	}
			case MIME_TOKEN_ORIGINATING_IP:				{	ptr->m_pKey = &CDecAttributeKey::kOriginatingIp;			break;	}
			case MIME_TOKEN_MAILER_VERSION:				{	ptr->m_pKey = &CDecAttributeKey::kMailerVersion;			break;	}
			case MIME_TOKEN_LIST_ID:					{	ptr->m_pKey = &CDecAttributeKey::kListId;					break;	}
			case MIME_TOKEN_LIST_POST:					{	ptr->m_pKey = &CDecAttributeKey::kListPost;					break;	}
			case MIME_TOKEN_LIST_HELP:					{	ptr->m_pKey = &CDecAttributeKey::kListHelp;					break;	}
			case MIME_TOKEN_ERRORS_TO:					{	ptr->m_pKey = &CDecAttributeKey::kErrorsTo;					break;	}
			case MIME_TOKEN_MAILING_LIST:				{	ptr->m_pKey = &CDecAttributeKey::kMailingList; 				break;	}
			case MIME_TOKEN_MS_HAS_ATTACH:				{	ptr->m_pKey = &CDecAttributeKey::kMSHasAttach;				break;	}
			case MIME_TOKEN_MS_TNEF_CORRELATOR:			{	ptr->m_pKey = &CDecAttributeKey::kMSTnefCorrelator; 		break;	}
			case MIME_TOKEN_MIME_AUTOCONVERTED:			{	ptr->m_pKey = &CDecAttributeKey::kMimeAutoconverted;		break;	}
			case MIME_TOKEN_CONTENT_CLASS:				{	ptr->m_pKey = &CDecAttributeKey::kContentClass; 			break;	}
			case MIME_TOKEN_PRECEDENCE:					{	ptr->m_pKey = &CDecAttributeKey::kPrecedence;				break;	}
			case MIME_TOKEN_ORIGINALARRIVALTIME:		{	ptr->m_pKey = &CDecAttributeKey::kOriginalarrivaltime; 		break;	}
			case MIME_TOKEN_MIMEOLE:					{	ptr->m_pKey = &CDecAttributeKey::kMimeOLE; 					break;	}
			case MIME_TOKEN_LIST_ARCHIVE:				{	ptr->m_pKey = &CDecAttributeKey::kListArchive;				break;	}
			default:
				{
					
					ptr->m_pKey = new CDecAttributeKey(	CDecAttributeKey::eNoAccelerator , 
														(const char*) m_pBaseAddress+m_nKeyOffset,
														m_nKeyLength, 
														CDecAttributeKey::Alias);
					ptr->m_bDeleteKey = true;
					break;
				}
		}
	}

	return m_pKey;
}
