// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "DecPlat.h"
#include "Dec2.h"
#include <ctype.h>

char static_AcceptLanguage			[] = "\x61\x63\x63\x65\x70\x74\x2D\x6C\x61\x6E\x67\x75\x61\x67\x65";
char static_ApparentlyTo			[] = "\x61\x70\x70\x61\x72\x65\x6E\x74\x6C\x79\x2D\x74\x6F";
char static_Author					[] = "\x61\x75\x74\x68\x6F\x72";
char static_AutoSubmitted			[] = "\x61\x75\x74\x6F\x2D\x73\x75\x62\x6D\x69\x74\x74\x65\x64";
char static_Bcc						[] = "\x62\x63\x63";
char static_Cc						[] = "\x63\x63";
char static_Comments				[] = "\x63\x6F\x6D\x6D\x65\x6E\x74\x73";
char static_ContentClass			[] = "\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x63\x6C\x61\x73\x73";
char static_ContentDescription 		[] = "\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x64\x65\x73\x63\x72\x69\x70\x74\x69\x6F\x6E";
char static_ContentDisposition 		[] = "\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x64\x69\x73\x70\x6F\x73\x69\x74\x69\x6F\x6E";
char static_ContentId				[] = "\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x69\x64";
char static_ContentLength			[] = "\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x6C\x65\x6E\x67\x74\x68";
char static_ContentLocation			[] = "\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x6C\x6F\x63\x61\x74\x69\x6F\x6E";
char static_ContentTransferEncoding	[] = "\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x74\x72\x61\x6E\x73\x66\x65\x72\x2D\x65\x6E\x63\x6F\x64\x69\x6E\x67";
char static_ContentType				[] = "\x63\x6F\x6E\x74\x65\x6E\x74\x2D\x74\x79\x70\x65";
char static_Date					[] = "\x64\x61\x74\x65";
char static_DeliveredTo				[] = "\x64\x65\x6C\x69\x76\x65\x72\x65\x64\x2D\x74\x6F";
char static_EgroupsReturn			[] = "\x65\x67\x72\x6F\x75\x70\x73\x2D\x72\x65\x74\x75\x72\x6E";
char static_Encrypted				[] = "\x65\x6E\x63\x72\x79\x70\x74\x65\x64";
char static_ErrorsTo				[] = "\x65\x72\x72\x6F\x72\x73\x2D\x74\x6F";
char static_From					[] = "\x66\x72\x6F\x6D";
char static_Id						[] = "\x69\x64";
char static_Importance				[] = "\x69\x6D\x70\x6F\x72\x74\x61\x6E\x63\x65";
char static_InReplyTo				[] = "\x69\x6E\x2D\x72\x65\x70\x6C\x79\x2D\x74\x6F";
char static_Keywords				[] = "\x6B\x65\x79\x77\x6F\x72\x64\x73";
char static_ListArchive				[] = "\x6C\x69\x73\x74\x2D\x61\x72\x63\x68\x69\x76\x65";
char static_ListHelp				[] = "\x6C\x69\x73\x74\x2D\x68\x65\x6C\x70";
char static_ListId					[] = "\x6C\x69\x73\x74\x2D\x69\x64";
char static_ListPost				[] = "\x6C\x69\x73\x74\x2D\x70\x6F\x73\x74";
char static_ListSubscribe			[] = "\x6C\x69\x73\x74\x2D\x73\x75\x62\x73\x63\x72\x69\x62\x65";
char static_ListUnsubscribe			[] = "\x6C\x69\x73\x74\x2D\x75\x6E\x73\x75\x62\x73\x63\x72\x69\x62\x65";
char static_Mailer					[] = "\x6D\x61\x69\x6C\x65\x72";
char static_MailerVersion			[] = "\x6D\x61\x69\x6C\x65\x72\x2D\x76\x65\x72\x73\x69\x6F\x6E";
char static_MailingList				[] = "\x6D\x61\x69\x6C\x69\x6E\x67\x2D\x6C\x69\x73\x74";
char static_MessageId				[] = "\x6D\x65\x73\x73\x61\x67\x65\x2D\x69\x64";
char static_MimeOLE					[] = "\x6D\x69\x6D\x65\x6F\x6C\x65";
char static_MimeAutoconverted		[] = "\x6D\x69\x6D\x65\x2D\x61\x75\x74\x6F\x63\x6F\x6E\x76\x65\x72\x74\x65\x64";
char static_MimeVersion				[] = "\x6D\x69\x6D\x65\x2D\x76\x65\x72\x73\x69\x6F\x6E";
char static_MSMailPriority			[] = "\x6D\x73\x6D\x61\x69\x6C\x2D\x70\x72\x69\x6F\x72\x69\x74\x79";
char static_MSHasAttach				[] = "\x6D\x73\x2D\x68\x61\x73\x2D\x61\x74\x74\x61\x63\x68";
char static_MSTnefCorrelator		[] = "\x6D\x73\x2D\x74\x6E\x65\x66\x2D\x63\x6F\x72\x72\x65\x6C\x61\x74\x6F\x72";
char static_Name					[] = "\x6E\x61\x6D\x65";
char static_Organization			[] = "\x6F\x72\x67\x61\x6E\x69\x7A\x61\x74\x69\x6F\x6E";
char static_Originalarrivaltime		[] = "\x6F\x72\x69\x67\x69\x6E\x61\x6C\x61\x72\x72\x69\x76\x61\x6C\x74\x69\x6D\x65";
char static_OriginatingIp			[] = "\x6F\x72\x69\x67\x69\x6E\x61\x74\x69\x6E\x67\x2D\x69\x70";
char static_Precedence				[] = "\x70\x72\x65\x63\x65\x64\x65\x6E\x63\x65";
char static_Priority				[] = "\x70\x72\x69\x6F\x72\x69\x74\x79";
char static_Received				[] = "\x72\x65\x63\x65\x69\x76\x65\x64";
char static_References				[] = "\x72\x65\x66\x65\x72\x65\x6E\x63\x65\x73";
char static_ReplyTo					[] = "\x72\x65\x70\x6C\x79\x2D\x74\x6F";
char static_ReturnPath				[] = "\x72\x65\x74\x75\x72\x6E\x2D\x70\x61\x74\x68";
char static_Sender					[] = "\x73\x65\x6E\x64\x65\x72";
char static_Subject					[] = "\x73\x75\x62\x6A\x65\x63\x74";
char static_ThreadIndex				[] = "\x74\x68\x72\x65\x61\x64\x2D\x69\x6E\x64\x65\x78";
char static_ThreadTopic				[] = "\x74\x68\x72\x65\x61\x64\x2D\x74\x6F\x70\x69\x63";
char static_To						[] = "\x74\x6F";
char static_Pathname				[] = "\x70\x61\x74\x68\x6E\x61\x6D\x65";
char static_ParentAttributeCollectionId	[] = "\x70\x61\x72\x65\x6E\x74\x61\x74\x74\x72\x69\x62\x75\x74\x65\x63\x6F\x6C\x6C\x65\x63\x74\x69\x6F\x6E\x69\x64";

// General Keys

const CDecAttributeKey CDecAttributeKey::kName			(eAcceptLanguage,static_Name,sizeof(static_Name)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kPathname		(eAcceptLanguage,static_Pathname,sizeof(static_Pathname)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kParentAttributeCollectionId	(eParentAttributeCollectionId,static_ParentAttributeCollectionId,sizeof(static_ParentAttributeCollectionId)-1,Alias);

// MIME Keys

const CDecAttributeKey CDecAttributeKey::kAcceptLanguage			(eAcceptLanguage,static_AcceptLanguage,sizeof(static_AcceptLanguage)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kApparentlyTo				(eApparentlyTo,static_ApparentlyTo,sizeof(static_ApparentlyTo)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kAuthor					(eAuthor,static_Author,sizeof(static_Author)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kAutoSubmitted				(eAutoSubmitted,static_AutoSubmitted,sizeof(static_AutoSubmitted)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kBcc						(eBcc,static_Bcc,sizeof(static_Bcc)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kCc						(eCc,static_Cc,sizeof(static_Cc)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kComments					(eComments,static_Comments,sizeof(static_Comments)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kContentClass				(eContentClass,static_ContentClass,sizeof(static_ContentClass)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kContentDescription		(eContentDescription,static_ContentDescription,sizeof(static_ContentDescription)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kContentDisposition		(eContentDisposition,static_ContentDisposition,sizeof(static_ContentDisposition)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kContentId					(eContentId,static_ContentId,sizeof(static_ContentId)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kContentLength				(eContentLength,static_ContentLength,sizeof(static_ContentLength)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kContentLocation			(eContentLocation,static_ContentLocation,sizeof(static_ContentLocation)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kContentTransferEncoding	(eContentTransferEncoding,static_ContentTransferEncoding,sizeof(static_ContentTransferEncoding-1),Alias);
const CDecAttributeKey CDecAttributeKey::kContentType				(eContentType,static_ContentType,sizeof(static_ContentType)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kDate						(eDate,static_Date,sizeof(static_Date)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kDeliveredTo				(eDeliveredTo,static_DeliveredTo,sizeof(static_DeliveredTo)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kEgroupsReturn				(eEgroupsReturn,static_EgroupsReturn,sizeof(static_EgroupsReturn)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kEncrypted					(eEncrypted,static_Encrypted,sizeof(static_Encrypted)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kErrorsTo					(eErrorsTo,static_ErrorsTo,sizeof(static_ErrorsTo)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kFrom						(eFrom,static_From,sizeof(static_From)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kId						(eId,static_Id,sizeof(static_Id)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kImportance				(eImportance,static_Importance,sizeof(static_Importance)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kInReplyTo					(eInReplyTo,static_InReplyTo,sizeof(static_InReplyTo)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kKeywords					(eKeywords,static_Keywords,sizeof(static_Keywords)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kListArchive				(eListArchive,static_ListArchive,sizeof(static_ListArchive)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kListHelp					(eListHelp,static_ListHelp,sizeof(static_ListHelp)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kListId					(eListId,static_ListId,sizeof(static_ListId)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kListPost					(eListPost,static_ListPost,sizeof(static_ListPost)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kListSubscribe				(eListSubscribe,static_ListSubscribe,sizeof(static_ListSubscribe)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kListUnsubscribe			(eListUnsubscribe,static_ListUnsubscribe,sizeof(static_ListUnsubscribe)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kMailer					(eMailer,static_Mailer,sizeof(static_Mailer)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kMailerVersion				(eMailerVersion,static_MailerVersion,sizeof(static_MailerVersion)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kMailingList				(eMailingList,static_MailingList,sizeof(static_MailingList)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kMessageId					(eMessageId,static_MessageId,sizeof(static_MessageId)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kMimeOLE					(eMimeOLE,static_MimeOLE,sizeof(static_MimeOLE)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kMimeAutoconverted			(eMimeAutoconverted,static_MimeAutoconverted,sizeof(static_MimeAutoconverted)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kMimeVersion				(eMimeVersion,static_MimeVersion,sizeof(static_MimeVersion)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kMSMailPriority			(eMSMailPriority,static_MSMailPriority,sizeof(static_MSMailPriority)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kMSHasAttach				(eMSHasAttach,static_MSHasAttach,sizeof(static_MSHasAttach)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kMSTnefCorrelator			(eMSTnefCorrelator,static_MSTnefCorrelator,sizeof(static_MSTnefCorrelator)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kOrganization				(eOrganization,static_Organization,sizeof(static_Organization)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kOriginalarrivaltime		(eOriginalarrivaltime,static_Originalarrivaltime,sizeof(static_Originalarrivaltime)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kOriginatingIp				(eOriginatingIp,static_OriginatingIp,sizeof(static_OriginatingIp)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kPrecedence				(ePrecedence,static_Precedence,sizeof(static_Precedence)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kPriority					(ePriority,static_Priority,sizeof(static_Priority)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kReceived					(eReceived,static_Received,sizeof(static_Received)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kReferences				(eReferences,static_References,sizeof(static_References)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kReplyTo					(eReplyTo,static_ReplyTo,sizeof(static_ReplyTo)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kReturnPath				(eReturnPath,static_ReturnPath,sizeof(static_ReturnPath)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kSender					(eSender,static_Sender,sizeof(static_Sender)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kSubject					(eSubject,static_Subject,sizeof(static_Subject)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kThreadIndex				(eThreadIndex,static_ThreadIndex,sizeof(static_ThreadIndex)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kThreadTopic				(eThreadTopic,static_ThreadTopic,sizeof(static_ThreadTopic)-1,Alias);
const CDecAttributeKey CDecAttributeKey::kTo						(eTo,static_To,sizeof(static_To)-1,Alias);

CDecAttributeKey::CDecAttributeKey( Accelerator accelerator, const char* szNamedKey, size_t nNameLength, Ownership storageOwner) 
:	m_eAccelerator(accelerator),
	m_pKey(szNamedKey),
	m_eOwner(storageOwner),
	m_nKeyLength(nNameLength)
{
	if (szNamedKey != NULL && m_eOwner == CopyAndAdopt)
	{
		m_pKey = new char[nNameLength+1];
		if ( m_pKey != NULL )
		{
			memcpy((void*)m_pKey,szNamedKey,nNameLength+1);
			m_eOwner = 	Adopt;
		}
	}
}

CDecAttributeKey::~CDecAttributeKey()
{
	if ( m_eOwner == Adopt )
	{
		delete [] (char*)m_pKey;
	}
}

CDecAttributeKey::CDecAttributeKey()
:	m_eAccelerator(eNoAccelerator),
	m_pKey(NULL),
	m_eOwner(Alias),
	m_nKeyLength(0)
{
}

CDecAttributeKey::CDecAttributeKey( const CDecAttributeKey& rhs)
:	m_eAccelerator(eNoAccelerator),
	m_pKey(NULL),
	m_eOwner(Alias),
	m_nKeyLength(0)
{
	*this = rhs;
}

const CDecAttributeKey& CDecAttributeKey::operator= ( const CDecAttributeKey& rhs)
{
	if ( this != &rhs )
	{
		if (m_eOwner == Adopt)
		{
			delete [] (char*)m_pKey;
		}
		m_pKey = rhs.m_pKey;
		m_eAccelerator = rhs.m_eAccelerator;
		m_eOwner = Alias;
		m_nKeyLength = rhs.m_nKeyLength;
	}

	return *this;
}

CDecAttributeKey::Ownership CDecAttributeKey::GetOwnership() const
{
	return m_eOwner;
}

void CDecAttributeKey::AdoptKey()
{
	if ( m_eOwner == Alias )
	{
		char* ptr = new char[m_nKeyLength+1];
		if ( ptr != NULL )
		{
			memcpy(ptr,m_pKey,m_nKeyLength+1);
			m_pKey = ptr;
			m_eOwner = 	Adopt;
		}
	}
}

CDecAttributeKey::Accelerator	CDecAttributeKey::GetAccelerator() const
{
	return m_eAccelerator;
}


const char*	CDecAttributeKey::GetKey(size_t& keyLengthFillin) const
{
	keyLengthFillin = m_nKeyLength;
	return m_pKey;
}
bool CDecAttributeKey::operator== (const CDecAttributeKey& rhs ) const
{
	return IsEqual(rhs.m_eAccelerator,rhs.m_pKey,rhs.m_nKeyLength);
}

bool CDecAttributeKey::operator== (const CDecAttributeKey* rhs ) const
{
	if ( rhs == NULL )
	{
		return false;
	}
	else
	{
		return IsEqual(rhs->m_eAccelerator,rhs->m_pKey,rhs->m_nKeyLength);
	}
}
bool CDecAttributeKey::IsEqual(Accelerator accelerator, const char* key, size_t keyLength ) const
{
	bool result = false;

	// See if we can compare accelerators
	if ( key != NULL && ( accelerator == eNoAccelerator || m_eAccelerator == eNoAccelerator)  )
	{
		if (  keyLength == m_nKeyLength )
		{
			result = true;	// assume they are the same

			for ( size_t i = 0; i < keyLength; i++ )
			{
				if ( tolower(key[i]) != tolower(m_pKey[i]) )
				{
					result = false;
					break;
				}
			}
		}
	}
	else
	{
		result = ( accelerator == m_eAccelerator );
	}

	return result;
}


IDecAttribute::IDecAttribute() 
{ 
}

IDecAttribute::~IDecAttribute() 
{
}

IDecAttribute& IDecAttribute::operator=(const IDecAttribute& rhs)
{ 
	return *this;
}

IDecAttribute::IDecAttribute(const IDecAttribute& rhs)
{
}

const CDecAttributeKey* CDecTextGeneric::GetKey() const 
{ 
	return m_pKey; 
}

void CDecTextGeneric::SetKey(CDecAttributeKey* pKey,IDecAttribute::Ownership ownership ) 
{
	if ( m_eKeyOwner == IDecAttribute::Adopt && m_pKey != NULL )
	{
		delete (CDecAttributeKey*)m_pKey;
	}	

	m_pKey = pKey;
	m_eKeyOwner = ownership;
}; 

void CDecTextGeneric::SetValue( const char* pValue, size_t nValueLength, IDecAttribute::Ownership ownership)
{
	if ( m_eValueOwner == IDecAttribute::Adopt && m_pValue != NULL )
	{
		delete (char*)m_pValue;
	}	

	m_pValue = pValue;
	m_nValueLength = nValueLength;
	m_eValueOwner = ownership;
}


const char* CDecTextGeneric::GetValueAsText(size_t* pLengthOfValue)  const 
{ 
	if ( pLengthOfValue ) {
		*pLengthOfValue = m_nValueLength;		
	}
	return m_pValue;
}

IDecAttribute::DerivedType CDecTextGeneric::GetType() const 
{ 
	return IDecAttribute::eCDecTextGeneric; 
}

CDecTextGeneric::CDecTextGeneric()
:	m_pKey(0),
	m_eKeyOwner(IDecAttribute::Alias),
	m_pValue(0) ,
	m_nValueLength(0),
	m_eValueOwner(IDecAttribute::Alias)
{
}

CDecTextGeneric::CDecTextGeneric(const CDecAttributeKey& key,
								 const char* pValue, size_t nValueLength,IDecAttribute::Ownership valueOwner)
:	m_pKey(&key),
	m_eKeyOwner(IDecAttribute::Alias),
	m_pValue(pValue) ,
	m_nValueLength(nValueLength),
	m_eValueOwner(valueOwner)
{
	if ( m_eValueOwner == IDecAttribute::CopyAndAdopt)
	{
		m_pValue = new char[m_nValueLength+1];
		if ( m_pValue != NULL )
		{
			((char*)((CDecTextGeneric*)this)->m_pValue)[m_nValueLength] = 0;
			strcpy(((char*)((CDecTextGeneric*)this)->m_pValue),pValue);
			m_eValueOwner = IDecAttribute::Adopt;
		}
	}
}

CDecTextGeneric::CDecTextGeneric(const CDecAttributeKey* pKey, IDecAttribute::Ownership keyOwner,
								 const char* pValue, size_t nValueLength,IDecAttribute::Ownership valueOwner)
:	m_pKey(pKey) ,
	m_eKeyOwner(keyOwner),
	m_pValue(pValue) ,
	m_nValueLength(nValueLength),
	m_eValueOwner(valueOwner)
{
	if ( m_eValueOwner == IDecAttribute::CopyAndAdopt)
	{
		m_pValue = new char[m_nValueLength+1];
		if ( m_pValue != NULL )
		{
			((char*)((CDecTextGeneric*)this)->m_pValue)[m_nValueLength] = 0;
			strcpy(((char*)((CDecTextGeneric*)this)->m_pValue),pValue);
			m_eValueOwner = IDecAttribute::Adopt;
		}
	}
}

CDecTextGeneric::~CDecTextGeneric() 
{
	if (m_eKeyOwner == IDecAttribute::Adopt)
	{
		delete [] ((char*)m_pKey);
	}
	if (m_eValueOwner == IDecAttribute::Adopt) {
		delete [] ((char*)m_pValue);
	}
}


IDecAttributeCollection::IDecAttributeCollection() 
{
}

IDecAttributeCollection::~IDecAttributeCollection() 
{
}

IDecAttributeCollection& IDecAttributeCollection::operator=(const IDecAttributeCollection& rhs) 
{ 
	return *this; 
}

IDecAttributeCollection::IDecAttributeCollection(const IDecAttributeCollection& rhs) 
{
}

unsigned long IDecAttributeCollection::GetId() const
{
	return (unsigned long)this;
}

IDecAttributeCollection::DerivedType CGenericAttributeCollection::GetType() const 
{ 
	return IDecAttributeCollection::eCGenericAttributeCollection; 
}


const CDecAttributeKey* CGenericAttributeCollection::GetKeyAt(size_t index) const 
{ 
	const CDecAttributeKey* result = NULL;
	if ( m_nAttributeIndex > 0 && index < m_nAttributeIndex  ) 
	{
		CGenericAttributeCollection* nonConstThis = (CGenericAttributeCollection*)this;

		IDecAttribute* pAnAttribute = nonConstThis->m_Attributes[index];
		result = pAnAttribute->GetKey();
	}
	return result; 
}

IDecAttribute* CGenericAttributeCollection::GetAttributeAt ( const CDecAttributeKey& key,  size_t index ) const 
{ 
	CGenericAttributeCollection* nonConstThis = (CGenericAttributeCollection*)this;
	IDecAttribute* pResult = NULL;

	size_t foundAt = 0;

	if ( m_nAttributeIndex > 0 )
	{
		for ( size_t i = 0; i < m_nAttributeIndex; i++)
		{
			IDecAttribute* pAnAttribute = nonConstThis->m_Attributes[i];
			
			if ( key == pAnAttribute->GetKey() )
			{
				foundAt++;
			}

			if ( foundAt == index+1 )
			{
				pResult = pAnAttribute;
				break;
			}
		}
	}


	return pResult;
}


size_t CGenericAttributeCollection::GetNumberOfValuesForKey( const CDecAttributeKey& key  ) const 
{ 
	size_t numberOfValues = 0;
	
	if ( m_nAttributeIndex > 0 )
	{

		CGenericAttributeCollection* nonConstThis = (CGenericAttributeCollection*)this;

		for ( size_t i = 0; i < m_nAttributeIndex; i++)
		{
			IDecAttribute* pAnAttribute = nonConstThis->m_Attributes[i];

			if ( key == pAnAttribute->GetKey() )
			{
				numberOfValues++;
			}
		}
	}

	return numberOfValues;
}


CGenericAttributeCollection::CGenericAttributeCollection() 
:	m_nAttributeIndex(0)
{
	m_Attributes.initializeMemory(true);
	m_Attributes.reset();
}


CGenericAttributeCollection::~CGenericAttributeCollection() 
{ 
	for ( size_t i = 0; i < m_nAttributeIndex; i++)
	{
		IDecAttribute* ptr = m_Attributes[i];
		delete ptr;
	}
}

CGenericAttributeCollection& CGenericAttributeCollection::operator=(const CGenericAttributeCollection& rhs) 
{ 
	return *this; 
}

CGenericAttributeCollection::CGenericAttributeCollection(const CGenericAttributeCollection& rhs) 
{
}


void CGenericAttributeCollection::AddAttribute(IDecAttribute* pAttribute)
{
	if ( pAttribute != NULL )
	{
		m_Attributes[m_nAttributeIndex++] = pAttribute;
	}
}
