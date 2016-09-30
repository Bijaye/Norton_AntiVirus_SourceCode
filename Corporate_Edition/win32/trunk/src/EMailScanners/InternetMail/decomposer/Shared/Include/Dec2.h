// Dec2.h : Decomposer 2 Interface
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.


// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#if !defined(DEC2_H)
#define DEC2_H

#include "DecDefs.h"
#include "FastArray.h"

/////////////////////////////////////////////////////////////////////////////
// Forward Declarations

class IDecomposer;
class IDecEngine;
class IDecObject;
class IDecContainerObject;
class IDecEventSink;
class IDecIOCB;
class CMIMEParser;
class CDecObj;

/////////////////////////////////////////////////////////////////////////////
// Interface IDecomposer

class IDecomposer
{
public:
	IDecomposer() {}
	virtual ~IDecomposer() {}

	// IDecomposer methods:
	virtual DECRESULT AddEngine(IDecEngine *pEngine) = 0;
	virtual DECRESULT RemoveEngines() = 0;
	virtual DECRESULT SetTempDir(const char *szDir) = 0;
	virtual DECRESULT SetMaxExtractSize(DWORD dwMaxSize) = 0;
	virtual DECRESULT AbortProcess(bool bAbort) = 0;
	virtual DECRESULT SetOption(DWORD dwOptionID, DWORD dwValue) = 0;
	virtual DECRESULT GetOption(DWORD dwOptionID, DWORD *pdwValue) = 0;
	virtual DECRESULT Process(IDecObject *pObject,
							 IDecEventSink *pSink,
							 IDecIOCB *pIOCallback,
							 WORD *pwResult,
							 char *pszNewDataFile) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecEngine

class IDecEngine
{
public:
	IDecEngine() {}
	virtual ~IDecEngine() {}

	// IDecEngine methods:
	virtual DECRESULT AbortProcess(bool bAbort) = 0;
	virtual DECRESULT SetTempDir(const char *szDir) = 0;
	virtual DECRESULT SetMaxExtractSize(DWORD dwMaxSize) = 0;
	virtual DECRESULT SetIOCallback(IDecIOCB *pIOCallback) = 0;
};

// *** AS400 Specific Notes ***
// The AS/400 currently does not support the Decomposer Attribute API. All
// Attribute API code is compiled out for the AS/400. Blocks of code that
// are compiled out for this reason are tagged: *** Start/End AS400 Attribute API Removal ***.
#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
/**
 *
 *	\mainpage Decomposer Attribute API Reference
 *
 * This set of documentation covers a subset of the Decomposer API.  The long term
 * goal is to provide a complete set of API documentation that is available in HTML
 * format.  This documentation was generated from the Decomposer source using Doxygen.  
 * 
 * The primary classes documented in this release are those classes related to
 * attributes.  The attribute classes are those called out by the box labeled
 * 'attributes' in the diagram below. Here is the list of attribute classes:
 *
 *    - CDecAttributeKey
 *         -	This class provides simple key support for the key value pair
 *				used by Decomposer Attributes.
 *    - IDecAttribute 
 *         -	This is the interface that all attributes support.  The base interface
 *				allows easy access to the attribute key and a textual representation of
 *				attribute value.  This class also provides access to subclass typing
 *				information.  This is useful for accessing enhanced API's that may be
 *				available on classes that implement this interface.
 *         -	CDecTextGeneric
 *              - This class implements IDecAttribute and provides support for simple
 *                text based key value pairs.
 *    - IDecAttributeCollection
 *         -	This is the interface class for, as the name implies, a collection of 
 *				attributes.  This interface provides an easy way to retrieve keys and the
 *				attributes associated with those keys.  This interface, like attributes,
 *				provides a way to determine the subclass that implements the interface.  
 *				This is useful for accessing enhanced API's that may be available on classes 
 *				that implement this interface.
 *		   -	AttributeCollections are retrieved from IDecObject (See class for sample code).
 *         -	CGenericAttributeCollection
 *              - This class provides an implmentation of IDecAttributeCollection.  The
 *				  class does not provide any more features then those expressed in IDecAttributeCollection.
 *         -	CMIMEAttributeCollection
 *              - This class provides an implmentation of IDecAttributeCollection.  This
 *				  class supports the base interface as well as providing direct methods to
 *				  to access common MIME header fields. (See class for sample code).
 *				  
 * 
 * <img src="Images/ClassDiagram.gif">
 */

/////////////////////////////////////////////////////////////////////////////
// CDecAttributeKey

/**
 * CDecAttributeKey is the class that holds the keys assoiciated with 
 * Decomposer attributes.  The class provides support for a 'C' string
 * key as well as an accelerator.  The accelerator is used by the class
 * comparison routines.  Each well-known key has an associated accelerator
 * that is useful for accelerating comparisons.
 *
 * An attribute key can be constructed without an accelerator.  To create
 * a key without an accelerator use the value 
 * CDecAttributeKey::Accelerator::eNoAccelerator.  The key will work like 
 * properly but comparisons will be slower.
 *
 * CDecAttributeKey supports multiple ownership symantecs.  The desired 
 * ownership is expressed during construction.  The options are:
 * 
 *    - Adopt
 *         -#	The storage associated with the key will delete upon distruction.
 *				The class manages the lifetime of the string.
 *    - CopyAndAdopt
 *         -#	The 'C' string will be copied and owned by the class. The class
 *				manages the lifetime of the string.
 *    - Alias
 *         -#	The 'C' string key will be referenced by the class but not deleted
 *				The caller is responsible for managing the lifetime of the string.
 *
 *
 * CDecAttributeKey provides a set of convience static data members that provided pre-built
 * keys for the well known keys described by the accelerator enum list.  These static
 * keys can be used by the caller but are not required to be.
 *
 */
class CDecAttributeKey
{
public:

	/**
	 * Accelerator provides a list of well known keys.  An accelerated key
	 * provides for faster comparison.  If a desired key is not in the list
	 * the caller must specify eNoAccelerator during construction of this class.
	 */
	enum Accelerator
	{
		eNoAccelerator,			/**< Special value indicating that no accelerator is available. */
		
		// General accelerator name
		eName,					/**< General name key. */
		ePathname,				/**< General pathname key. */
		eParentAttributeCollectionId,	/**< key used to locate parent attribute collection if available */

		// MIME specific accelerators

		eAcceptLanguage,			/**< Well-known MIME key. */
		eApparentlyTo,				/**< Well-known MIME key. */
		eAuthor,					/**< Well-known MIME key. */
		eAutoSubmitted,				/**< Well-known MIME key. */
		eBcc,						/**< Well-known MIME key. */
		eCc,						/**< Well-known MIME key. */
		eComments,					/**< Well-known MIME key. */
		eContentClass,				/**< Well-known MIME key. */
		eContentDescription,		/**< Well-known MIME key. */
		eContentDisposition,		/**< Well-known MIME key. */
		eContentId,					/**< Well-known MIME key. */
		eContentLength,				/**< Well-known MIME key. */
		eContentLocation,			/**< Well-known MIME key. */
		eContentTransferEncoding,	/**< Well-known MIME key. */
		eContentType,				/**< Well-known MIME key. */
		eDate,						/**< Well-known MIME key. */
		eDeliveredTo,				/**< Well-known MIME key. */
		eEgroupsReturn,				/**< Well-known MIME key. */
		eEncrypted,					/**< Well-known MIME key. */
		eErrorsTo,					/**< Well-known MIME key. */
		eFrom,						/**< Well-known MIME key. */
		eId,						/**< Well-known MIME key. */
		eImportance,				/**< Well-known MIME key. */
		eInReplyTo,					/**< Well-known MIME key. */
		eKeywords,					/**< Well-known MIME key. */
		eListArchive,				/**< Well-known MIME key. */
		eListHelp,					/**< Well-known MIME key. */
		eListId,					/**< Well-known MIME key. */
		eListPost,					/**< Well-known MIME key. */
		eListSubscribe,				/**< Well-known MIME key. */
		eListUnsubscribe,			/**< Well-known MIME key. */
		eMailer,					/**< Well-known MIME key. */
		eMailerVersion,				/**< Well-known MIME key. */
		eMailingList,				/**< Well-known MIME key. */
		eMessageId,					/**< Well-known MIME key. */
		eMimeOLE,					/**< Well-known MIME key. */
		eMimeAutoconverted,			/**< Well-known MIME key. */
		eMimeVersion,				/**< Well-known MIME key. */
		eMSMailPriority,			/**< Well-known MIME key. */
		eMSHasAttach,				/**< Well-known MIME key. */
		eMSTnefCorrelator,			/**< Well-known MIME key. */
		eOrganization,				/**< Well-known MIME key. */
		eOriginalarrivaltime,		/**< Well-known MIME key. */
		eOriginatingIp,				/**< Well-known MIME key. */
		ePrecedence,				/**< Well-known MIME key. */
		ePriority,					/**< Well-known MIME key. */
		eReceived,					/**< Well-known MIME key. */
		eReferences,				/**< Well-known MIME key. */
		eReplyTo,					/**< Well-known MIME key. */
		eReturnPath,				/**< Well-known MIME key. */
		eSender,					/**< Well-known MIME key. */
		eSubject,					/**< Well-known MIME key. */
		eThreadIndex,				/**< Well-known MIME key. */
		eThreadTopic,				/**< Well-known MIME key. */
		eTo							/**< Well-known MIME key. */
	};


	static const CDecAttributeKey kName;
	static const CDecAttributeKey kPathname;
	static const CDecAttributeKey kParentAttributeCollectionId;

	static const CDecAttributeKey kAcceptLanguage;
	static const CDecAttributeKey kApparentlyTo;
	static const CDecAttributeKey kAuthor;
	static const CDecAttributeKey kAutoSubmitted;
	static const CDecAttributeKey kBcc;
	static const CDecAttributeKey kCc;
	static const CDecAttributeKey kComments;
	static const CDecAttributeKey kContentClass;
	static const CDecAttributeKey kContentDescription;
	static const CDecAttributeKey kContentDisposition;
	static const CDecAttributeKey kContentId;
	static const CDecAttributeKey kContentLength;
	static const CDecAttributeKey kContentLocation;
	static const CDecAttributeKey kContentTransferEncoding;
	static const CDecAttributeKey kContentType;
	static const CDecAttributeKey kDate;
	static const CDecAttributeKey kDeliveredTo;
	static const CDecAttributeKey kEgroupsReturn;
	static const CDecAttributeKey kEncrypted;
	static const CDecAttributeKey kErrorsTo;
	static const CDecAttributeKey kFrom;
	static const CDecAttributeKey kId;
	static const CDecAttributeKey kImportance;
	static const CDecAttributeKey kInReplyTo;
	static const CDecAttributeKey kKeywords;
	static const CDecAttributeKey kListArchive;
	static const CDecAttributeKey kListHelp;
	static const CDecAttributeKey kListId;
	static const CDecAttributeKey kListPost;
	static const CDecAttributeKey kListSubscribe;
	static const CDecAttributeKey kListUnsubscribe;
	static const CDecAttributeKey kMailer;
	static const CDecAttributeKey kMailerVersion;
	static const CDecAttributeKey kMailingList;
	static const CDecAttributeKey kMessageId;
	static const CDecAttributeKey kMimeOLE;
	static const CDecAttributeKey kMimeAutoconverted;
	static const CDecAttributeKey kMimeVersion;
	static const CDecAttributeKey kMSMailPriority;
	static const CDecAttributeKey kMSHasAttach;
	static const CDecAttributeKey kMSTnefCorrelator;
	static const CDecAttributeKey kOrganization;
	static const CDecAttributeKey kOriginalarrivaltime;
	static const CDecAttributeKey kOriginatingIp;
	static const CDecAttributeKey kPrecedence;
	static const CDecAttributeKey kPriority;
	static const CDecAttributeKey kReceived;
	static const CDecAttributeKey kReferences;
	static const CDecAttributeKey kReplyTo;
	static const CDecAttributeKey kReturnPath;
	static const CDecAttributeKey kSender;
	static const CDecAttributeKey kSubject;
	static const CDecAttributeKey kThreadIndex;
	static const CDecAttributeKey kThreadTopic;
	static const CDecAttributeKey kTo;


	/**
	 * Owneship expresses the desired memory management behavior of the key string.
	 */
	enum Ownership {
		Adopt,			/**< Indicates that the class ownes the storage and is responsible for managing the storage. */
		Alias,			/**< Indicates that the class has an alias to storage managed external to the attribute. */
		CopyAndAdopt	/**< Indicates that the class should make a copy of the data being provided. */
	};

	/**
	 * Default constructor.
	 */

	CDecAttributeKey();

	/**
	 * Primary constructor.	Given an accelerator, keyname, length and ownership flag this constructor will 
	 *						create a fully functional key.
	 *
	 * @param accelerator		eNoAccelerator for unknown keys and the appropriate value if known.
	 * @param szNamedKey		pointer to a NON-NULL 'C' string containing the ASCII key.
	 * @param nNameLength		length of the string excluding the NULL terminator.
	 * @param storageOwnership	Adopt,CopyAndAdopt, or Alias.
	 *
	 */
	CDecAttributeKey( Accelerator accelerator, const char* szNamedKey, size_t nNameLength, Ownership storageOwner);

	/**
	 *
	 * Destructor.	Deletes the storage associated with the key if the ownership is adopt.  Otherwise,
	 *				no storage is deleted.
	 */
	virtual ~CDecAttributeKey();

	/**
	 *
	 * Copy Constructor.	The resulting CDecAttribute instance will have an ownership of 'Alias'
	 *						The caller is responsible for managing storage lifetime.  For example,
	 *						'this' instance must no longer be used if the 'rhs' is deleted and it's
	 *						ownership was adopt.
	 *
	 * @param rhs CDecAttribute to copy
	 *
	 */
	CDecAttributeKey( const CDecAttributeKey& rhs);

	/**
	 * Assignment Operator.	The resulting CDecAttribute instance will have an ownership of 'Alias'
	 *						The caller is responsible for managing storage lifetime.  For example,
	 *						'this' instance must no longer be used if the 'rhs' is deleted and it's
	 *						ownership was adopt. Reference to any previous 'C' string pointed to by
	 *						this class is lost.  If the current'C' string was owned, it will be
	 *						deleted before this operation completes.
	 * @param rhs
	 *
	 * @returns const CDecAttributeKey&  Reference to 'this' instance.
	 */
	const CDecAttributeKey&	operator= ( const CDecAttributeKey& rhs);

	/**
	 * operator==	Case insensitive comparison operator.
	 *
	 * @param	rhs	Instance to compare against.  The reference cannot be NULL.
	 *				If NULL is passed this routine will crash.
	 *
	 * @returns	bool	true if the rhs instance is equal to 'this' instance.
	 */
	virtual	bool operator== (const CDecAttributeKey& rhs) const;

	/**
	 * operator==	Case insensitive comparison operator that takes a pointer.
	 *
	 * @param	rhs		Pointer to instance to compare against. 
	 *
	 * @returns	bool	True if the rhs instance is equal to 'this' instance.
	 *					False if the instance is not equal or the passed pointer is NULL.
	 */
	virtual	bool operator== (const CDecAttributeKey* rhs) const;

	/**
	 * IsEqual	Given the appropriate paramters this routine returns true if the instances
	 *			are equal.  If either accelerator equals eNoAccelerator then a string comparison
	 *			is performed.  The actual characters will only be compared if the string
	 *			lengths match.  The characters are converted to lowercase before comparison.
	 *
	 * @param	accelerator		Accelerator to compare against this instance.
	 * @param	key				Pointer to the 'C' string.
	 * @param	keyLength		Length of 'C' string not including terminating NULL.
	 *
	 * @returns	bool		True if equal, otherwise false.  False is returned if passed
	 *						key == NULL.
	 */
	virtual	bool IsEqual(Accelerator accelerator, const char* key, size_t keyLength ) const;

	/**
	 * GetOwnership returns current ownership state
	 * 
	 * @returns Ownership	Adopted or Alias.  CopyAndAdopt converts to Adopt during construction.
	 */
	virtual	Ownership	GetOwnership() const;

	/**
	 * AdoptKey converts an aliased key to an adopted key.  The storage for the 'C' string
	 * will be deleted during destruction if AdoptKey is called.
	 *
	 */
	virtual	void		AdoptKey();

	/**
	 * GetAccelerator
	 *
	 * @returns
	 */
	virtual	Accelerator	GetAccelerator() const;

	/**
	 * GetKey returns the 'C' string key associated with this instance.
	 *
	 * @param	keyLengthFillin	The length of the key returned is stored in this parameter.
	 *							The value of keyLengthFillin is undefined if the funtion returns
	 *							NULL.
	 *
	 * @returns	const char*		Pointer to 'C' string containing the string or NULL if no key available.
	 */
	virtual	const char*	GetKey(size_t& keyLengthFillin) const;

	/**
	 *
	 * GetKey returns the 'C' string key associated with this instance.  This is a convenience
	 *			functions that allows the caller to specify a pointer to the size_t that will hold
	 *			the size of the returned key.
	 *
	 * @param	pnKeyLengthFillin	Pointer to size_t to hold the key length or NULL if size is not desired.
	 *								This paramter defaults to NULL and therefore clients can use the form
	 *								ptr->GetKey();
	 *
	 * @returns	const char*		Pointer to 'C' string containing the string or NULL if no key available.
	 */
	inline	const char*	GetKey(size_t* pnKeyLengthFillin = NULL) const
	{
		size_t drop;
		return GetKey( (pnKeyLengthFillin ? *pnKeyLengthFillin : drop) );
	}

private:
	
	Accelerator		m_eAccelerator;
	const char*		m_pKey;
	Ownership		m_eOwner;
	size_t			m_nKeyLength;
};


/**
 *	IDecAttribute represents an attribute associated with data.  An
 *	attribute can be considered meta-data associated with actual data.
 *	For example, a filename, modifcation date, are examples of attributes
 *	associated with a file. The attributes are not part of the file rather
 *	they describe it.  A single attribute is made up of a key and value.  
 *	This interface provides a simple way to retrieve the key and a representation
 *	of the value as a string.
 *
 *	It is possible, and hopefully, quite common, to have values that are more complex
 *	than a single text string.  For example, one could provide an attribute
 *	class to provide detailed information about a filename.

 *
 * <B>IMPORTANT NOTES:</B>	IDecAttribute is <B><I>not thread-safe</I></B> and all pointers
 *							returned by the class are owned the class.  Therefore,
 *							callers should <B><I>never delete</I></B> any data returned by this class.
 *							Futhermore, the values returned by this class are not
 *							guranteed to have a lifetime beyond the instance itself.
 *							Therefore, callers <B><I>must not retain pointers</I></B> beyond
 *							the lifetime of this class.
 *
 *
 */

class IDecAttribute {

public:

	/**
	 *	DerivedType indicates the actual derived type on an instance
	 *	that implements the IDecAttribute interface.  This is useful
	 *	if the caller wishes to cast an instance to a derived class in
	 *	order to use an extended API.   By convention the enum name is
	 *	the same as the derived class name.
	 */

	enum DerivedType {
			eCDecTextGeneric,		/**< Indicates the instance of IDecAttribute is an instance of CDecTextGeneric.  */  
	};

	/**
	 *	GetType returns the declared type of a class implementing IDecAttribute
	 *
	 *	@returns DerivedType which can be used to cast an instance to a derived class.
	 */

	virtual DerivedType GetType() const  = 0;

	/**
	 *	GetKey returns a reference to the internal key for this attribute.
	 *
	 *	@returns CDecAttributeKey* pointer to key object or NULL if no key set.
	 */

	virtual const CDecAttributeKey* GetKey() const = 0;

	/**
	 *	GetValueAsText Returns a text representation for the value if possible.
	 *
	 *	@param pLengthOfValue	If the pointer is not NULL the length of the value
	 *							is stored in the parameter.  If there is no key
	 *							the length will be set to zero.
	 *
	 *	@returns const char*	The text representation of the value or NULL if
	 *							no value is available.
	 */

	virtual const char* GetValueAsText(size_t* pLengthOfValue) const = 0;

public:

	/**
	 * <B><I>(For Use By Decomposer Only)</I></B>
	 *
	 * The Ownership enum is used to express who owns the data associated with
	 * this attribute and thus who is reponsible for deleteing the data.
	 * 
	 * This enum is intended to be used by the attribute producer.  Attribute
	 * consumers do not need to use this enum.
	 *
	 */

	enum Ownership {
		Adopt,			/**< Indicates that the class ownes the storage and is responsible for managing the storage. */
		Alias,			/**< Indicates that the class has an alias to storage managed external to the attribute. */
		CopyAndAdopt	/**< Indicates that the class should make a copy of the data being provided. */
	};


			IDecAttribute();
	virtual ~IDecAttribute();

private:

	// Disallowed functions...

			IDecAttribute& operator=(const IDecAttribute& rhs);
			IDecAttribute(const IDecAttribute& rhs);

};

/**
 *	CDecTextGeneric implements IDecAttribute and provides access to simple text attributes.
 */

class CDecTextGeneric: public IDecAttribute
{

public:

	// Constructors and Destructors

	CDecTextGeneric();

	/**
	 * Constructor.	This constructor takes a reference to a key and pointer to data.  The main
	 *				main purpose of this constructor is to allow ease of use with CDecAttribute's
	 *				static well-known keys.  The ownership of the referenced key is set internally
	 *				to 'Alias'.  This means the client must ensure that the key last longer than
	 *				this instance of CDecTextGeneric.  This is not an issue when using the statics
	 *				from CDecAttributeKey.
	 *
	 * @param	key				Reference to a key.  Ownership is set to alias and a pointer to the key is 
	 *												 held internally.
	 * @param	pValue			Pointer to a value.  NULL is not allowed.
	 * @param	nValueLength	Length of the 'C' string excluding NULL terminator.
	 * @param	valueOwner		Specifies who owns the value.
	 *
	 */
	CDecTextGeneric(const CDecAttributeKey& key,
					const char* pValue, size_t nValueLength,IDecAttribute::Ownership valueOwner);

	CDecTextGeneric(const CDecAttributeKey* pKey, IDecAttribute::Ownership keyOwner,
					const char* pValue, size_t nValueLength,IDecAttribute::Ownership valueOwner);

	virtual ~CDecTextGeneric();

	// Methods inherited from IDecAttribute...

	virtual const CDecAttributeKey*		GetKey() const;
	virtual const char*					GetValueAsText(size_t* pLengthOfValue)  const;
	virtual IDecAttribute::DerivedType	GetType() const ;

	// CDecTextGeneric specific APIs

	virtual void SetKey(CDecAttributeKey* pKey,IDecAttribute::Ownership keyOwner );
	virtual void SetValue( const char* pValue, size_t nValueLengthLength,IDecAttribute::Ownership keyOwner);
private:

	// Disallowed functions...

			CDecTextGeneric& operator=(const CDecTextGeneric& ) { return *this; };
			CDecTextGeneric(const CDecTextGeneric& ) {};

	// Key

	const CDecAttributeKey*		m_pKey;	
	IDecAttribute::Ownership	m_eKeyOwner;

	// Value

	const char*					m_pValue;
	size_t						m_nValueLength;
	IDecAttribute::Ownership	m_eValueOwner;
};


/** 
 *	IDecAttributeCollection is a collection of attributes.  The class proves a means of accessing all 
 *	attributes in a generic way.  The class also provides direct methods to retrieve common
 *	attributes such as name.
 *
 *	Attributes are accessed via a key name.  An attribute key may have multiple values.  To access
 *	a particular attribute the caller specifies the key and index.
 *
 * <B>IMPORTANT NOTES:</B>	IDecAttributeCollection is <B><I>not thread-safe</I></B> and all pointers
 *							returned by the class are owned the class.  Therefore,
 *							callers should <B><I>never delete</I></B> any data returned by this class.
 *							Futhermore, the values returned by this class are not
 *							guranteed to have a lifetime beyond the instance itself.
 *							Therefore, callers <B><I>must not retain pointers</I></B> beyond
 *							the lifetime of this class.
 *
 */

class IDecAttributeCollection {

public:

	/**
	 *	DerivedType indicates the actual derived type on an instance
	 *	that implements the IDecAttributeCollection interface.  This is useful
	 *	if the caller wishes to cast an instance to a derived class in
	 *	order to use an extended API.   By convention the enum name is
	 *	the same as the derived class name.
	 */

	enum DerivedType {
		eCGenericAttributeCollection,
		eCMIMEAttributeCollection	/**< Indicates that the derived type is CMIMEAttributeCollection */
	};

	/**
	 * GetId returns the ID for this instance of Atttribute collections
	 *
	 * @returns unsigned long unique ID for this instance.
	 */

	virtual unsigned long GetId() const;

	/**
	 *	GetType returns the type of collection represented by this instance.
	 *
	 *	@returns IDecAttributeCollection::DerivedType
	 */

	virtual IDecAttributeCollection::DerivedType GetType() const = 0;

	/**
	 * Given an index GetKeyAt will return a key or a NULL pointer.  GetKeyAt
	 * can optionally return the length of the key.  There are no holes in the index
	 * therefore callers can stop retrieving keys when they encounter the first
	 * NULL entry.
	 *
	 * @param index The Index of the key to retrieve.
	 *
	 * @returns const char* pointer to key or NULL if no key available
	 */

	virtual	const CDecAttributeKey*	GetKeyAt(size_t index ) const = 0;

	/**
	 * Given a key and index this method returns the associated attribute.  If an attribute
	 * is not available NULL is returned.
	 *
	 * @param accelerator	can be used to speed up the lookup time for a given key.  The
	 *						value NoKeyAvailable can be used with any named string key. The
	 *						results are undefined if the accelerator does not not match the
	 *						the named key (except NoKeyAvailable which indicates no accelerator).
	 * @param szNamedKey	pointer to a key name.  Null is NOT allowed.
	 * @param pKeyLenth		pointer to key length if it's known.
	 * @param index			Index of the attribute desired.
	 *
	 * @returns IDecAttribute pointer or NULL if not available
	 */

	virtual	IDecAttribute*	GetAttributeAt (const CDecAttributeKey& key,  size_t index ) const = 0;

	inline	IDecAttribute*	GetAttributeAt (const CDecAttributeKey* key,  size_t index ) const 
	{
		if ( key != NULL )
		{
			return (GetAttributeAt(*key,index));
		}
		else 
		{
			return NULL;
		}
	};

	/**
	 * GetNumberOfValuesForKey returns the count of values associated with the key.
	 *
	 * @param key	Reference to the CDecAttributeKey to find.
	 *
	 * @returns size_t number of valus associate with the key.
	 */

	virtual size_t GetNumberOfValuesForKey( const CDecAttributeKey& key ) const = 0;

	/**
	 * GetNumberOfValuesForKey returns the count of values associated with the key.
	 *
	 * @param key	Pointer to the CDecAttributeKey. If NULL is passed, zero will be returned.
	 *
	 * @returns size_t number of valus associate with the key.
	 */

	inline  size_t GetNumberOfValuesForKey( const CDecAttributeKey* key ) const
	{
		if ( key != NULL )
		{
			return GetNumberOfValuesForKey(*key);
		}
		else 
		{
			return 0;
		}
	};



protected:
	friend class CDecObj;

	virtual	~IDecAttributeCollection();
			IDecAttributeCollection();

private:
	// Disallowed
			IDecAttributeCollection& operator=(const IDecAttributeCollection& rhs);
			IDecAttributeCollection(const IDecAttributeCollection& rhs);
};


/**
 *	CGenericAttributeCollection is a general class that implements IDecAttributeCollection.  There
 *  is no additional functionality beyond that provided for in the IDecAttributeCollection interface class.
 *
*/
class CGenericAttributeCollection: public IDecAttributeCollection {

public:

	// Constructors and Destructor

			CGenericAttributeCollection();
	virtual	~CGenericAttributeCollection();

	// Methods inherited from IDecAttributeCollection...

	virtual DerivedType		GetType() const;
	virtual	const CDecAttributeKey*	GetKeyAt(size_t index) const;
	virtual	IDecAttribute*	GetAttributeAt ( const CDecAttributeKey& key,  size_t index ) const;
	virtual size_t			GetNumberOfValuesForKey( const CDecAttributeKey& key ) const;

public:

	void	AddAttribute(IDecAttribute* pAttribute);

private:

#define DEFAULT_GENERIC_KEY_VALUE_ARRAY_SIZE 20


	// Disallowed
	
			CGenericAttributeCollection& operator=(const CGenericAttributeCollection& rhs);
			CGenericAttributeCollection(const CGenericAttributeCollection& rhs);


	// Data Members
	
	FastArray<IDecAttribute*,DEFAULT_GENERIC_KEY_VALUE_ARRAY_SIZE> m_Attributes;
	size_t	m_nAttributeIndex;
};


/**
 *	CMIMEAttributeCollection is a collection of MIME attributes.  The class proves a means of accessing 
 *	MIME attributes directly through specific method calls.  The data provided is also available
 *	through the more generic API provided by the base class IDecAttributeCollection.
 *
 *	<pre>
 *
 *  DECRESULT CMyEventSink::OnContainerEnd( IDecContainerObject *pObject, void **ppInsertObjects)
 *  {
 *      // Attributes are saved in collections.  For container types there may be
 *      // attributes describing entries within the container.  For non-container types
 *      // there will be only one attribute collection.
 *
 *      size_t numberOfCollections = pObject->GetNumberOfAttributeCollections();
 *
 *      for ( size_t i = 0; i < numberOfCollections; i++ )
 *      {
 *          IDecAttributeCollection* ptr = pObject->GetAttributeCollectionAt(i);
 *
 *          // Handle MIME subclasses.
 *
 *          switch ( ptr->GetType() )
 *          {
 *              case IDecAttributeCollection::DerivedType::CMIMEAttributeCollection:
 *              {
 *                  CMIMEAttributeCollection* mimePtr = (CMIMEAttributeCollection*)ptr;
 *                  size_t length = 0;
 *                  const char* pCharPtr;
 *
 *                  pCharPtr = mimePtr->GetContentType(length);
 *                  if ( pCharPtr != NULL && length != 0 )
 *                  {
 *                      printf("ContentType = %s\\n",pCharPtr);
 *                  }
 *
 *                  pCharPtr = mimePtr->GetContentTypeBoundary(length);
 *                  if ( pCharPtr != NULL && length != 0 )
 *                  {
 *                      printf("ContentType Boundary = %s\\n",pCharPtr);
 *                  }
 *              }
 *          }
 *      }
 *      if (!m_options.Quiet() && !m_options.OneLine())
 *      {
 *          MyPrintf("Callback:           OnContainerEnd\\n");
 *          PrintObjectName(pObject);
 *      }
 *      return DEC_OK;
 *  }
 *	</pre>
*/
class CMIMEAttributeCollection: public IDecAttributeCollection {

public:

	// Constructors and Destructor

			CMIMEAttributeCollection();
	virtual	~CMIMEAttributeCollection();

	// Methods inherited from IDecAttributeCollection...

	virtual DerivedType		GetType() const;
	virtual	const CDecAttributeKey*		
							GetKeyAt(size_t index) const;
	virtual	IDecAttribute*	GetAttributeAt ( const CDecAttributeKey& key,  size_t index ) const;
	virtual size_t			GetNumberOfValuesForKey( const CDecAttributeKey& key ) const;

	// CMIMEAttributeCollection specific APIs

	/**
	 * GetRawHeaderBuffer returns a pointer to a section of memory that contains
	 * the raw MIME header.
	 *
	 * @param nbufferSizeFillin	A fillin paramater that will contain the size of the buffer.
	 *	
	 * @returns const char* Pointer to the memory that holds the header or NULL if no header specified
	 */
	virtual	const char*		GetRawHeaderBuffer(size_t& nbufferSizeFillin) const;

	/**
	 * GetContentType returns the content-type pair 'major/minor'.  For example,
	 * the values 'text/html' and 'text/plain' are common return values from this
	 * call.  <B>NOTE:</B> It's possible to have a content-type returned via 
	 * GetAttributeAt call and not have a value returned from this call.  This 
	 * occurs whenever a content-type field does not contain a a string in the
	 * form 'major/minor'.
	 *
	 * @param nLengthFillin Contains the length of the content-type.
	 *						nLengthFillin will be 0 if no content type is available
	 *
	 * @returns const char* Pointer to C string containing the content type data.
	 *						NULL is returned if no content type is available.
	 */
	virtual	const char*		GetContentType(size_t& nLengthFillin) const;

	/**
	 * GetContentTypeCharset returns the charset modifier for the content type.
	 *
	 * @param nLengthFillin Contains the length of the charset for the content-type.
	 *						nLengthFillin will be 0 if no content type is available
	 *
	 * @returns const char* Pointer to 'C' string containing the charset value
	 *						NULL is returned if no charset is available.
	 */
	virtual const char*		GetContentTypeCharset(size_t& nLengthFillin) const;

	/**
	 * GetContentTypeBoundary returns the boundary modifier for the content type.
	 *
	 * @param nLengthFillin Contains the length of the boundary of the content-type.
	 *						nLengthFillin will be 0 if no content type is available
	 *
	 * @returns const char* Pointer to 'C' string containing the boundary value
	 *						NULL is returned if no boundary is available.
	 */
	virtual const char*		GetContentTypeBoundary(size_t& nLengthFillin) const;

	/**
	 * GetStartOfHeaderOffset returns the offset from the beginning of the file
	 * to the start of the MIME header.  The starting position is the position that
	 * Decomposer discovered the first valid MIME header line.
	 *
	 * @param nOffsetFillin The offset is stored in this field.  This is an output
	 *						parameter only. The value is undefined if the function
	 *						returns false.
	 *
	 * @returns bool		True if the offset is known, otherwise false.
	 */
	virtual bool			GetStartOfHeaderOffset(size_t& nOffsetFillin) const;

private:

#define DEFAULT_MIME_KEY_VALUE_ARRAY_SIZE 20

	bool	ValidIndex(size_t index) const;

	friend class CMIMEParser;

	class CKeyValue {
	public:
		CKeyValue();
		CKeyValue(int nKeyID,size_t nKeyOffset,size_t nKeyLength, size_t nValueOffset,size_t nValueLength,CDecTextGeneric* pAttribute);
		virtual ~CKeyValue();

		int						GetKeyID () const;
		size_t					GetKeyOffset() const;
		size_t					GetKeyLength() const;
		size_t					GetValueOffset() const;
		size_t					GetValueLength() const;
		CDecTextGeneric*		GetAttribute() const;
		const CDecAttributeKey*	GetKey() const;

		void	SetKeyOffset(size_t nValue);
		void	SetKeyLength(size_t nValue);
		void	SetValueOffset(size_t nValue);
		void	SetValueLength(size_t nValue);
		void	SetAttribute(CDecTextGeneric* pAttribute);
		void	SetBaseAddress(void* baseAddress);

	private:
			int		m_nKeyID;
			size_t	m_nKeyOffset;
			size_t	m_nKeyLength;
			size_t	m_nValueOffset;
			size_t	m_nValueLength;
			void*	m_pBaseAddress;

			CDecTextGeneric*		m_pAttribute;
			const CDecAttributeKey*	m_pKey;
			bool					m_bDeleteKey;
	};

	void	AddKeyValue(int nKeyID,size_t nKeyOffset,size_t nValueOffset,size_t valueSize);

	// The header base offset is the offset within the file to where the
	// MIME header starts.

	void	SetHeaderBaseOffset(size_t nBaseOffset);
	
	// AdoptHeaderBuffer takes ownership of a chunk of memory that
	// contains a MIME header.

	void	AdoptHeaderBuffer(char* pBuffer, size_t nBufferSize);

	void	AdjustKeyValues();

	bool	HasWhiteSpace(char* pString,size_t nStringLength, size_t& whiteSpaceInFront, size_t& whiteSpaceAtRear) const;

	void	SetContentTypeSlashSeparatorOffset(size_t nOffset);

	void	SetContentTypeBoundaryOffset(size_t nOffset);

	void	SetContentTypeCharset(size_t nOffset,const char* pCharset,size_t nLength);

	unsigned char	ConvertToLowerCase(unsigned char aChar) const;

	// Disallowed
	
			CMIMEAttributeCollection& operator=(const CMIMEAttributeCollection& rhs);
			CMIMEAttributeCollection(const CMIMEAttributeCollection& rhs);


	// Data Members

	char*	m_pHeaderBuffer;
	size_t	m_nBufferSize;
	size_t	m_nHeaderBaseOffset;
	// Key Value Pair Array
	FastArray<CKeyValue*,DEFAULT_MIME_KEY_VALUE_ARRAY_SIZE> m_keyValuePairs;
	size_t	m_nValuePairIndex;

	// Values associated with well known MIME header keys.
	int		m_nContentTypeIndex;			// -1 == unknown
	int		m_nContentTypeSlashOffset;		// -1 == unknown
	int		m_nContentTypeBoundaryOffset;	// -1 == unknown
	int		m_nContentTypeCharsetOffset;	// -1 == unknown

	size_t	m_nContentTypeLength;			// Init to 0
	char*	m_nContentTypePtr;				// Init to NULL
	size_t	m_nContentTypeBoundaryLength;	// Init to 0
	char*	m_nContentTypeBoundaryPtr;		// Init to NULL
	size_t	m_nContentTypeCharsetLength;	// Init to 0
	char*	m_nContentTypeCharsetPtr;		// Init to NULL

};
#endif //*** End AS400 Attribute API Removal *** 


/////////////////////////////////////////////////////////////////////////////
// Interface IDecObject

/**
 *	IDecObject. The end-all and be-all of Decomposer classes!  This interface
 *	provides the primary means of interacting with client code during all Decomposer
 *	callbacks. The class however has started to suffer from object bloat or 
 *	<A HREF="http://pcroot.cern.ch/TaligentDocs/TaligentOnline/DocumentRoot/1.0/Docs/books/WM/WM_44.html#0">"Hardening of the architecture".</A>
 *	This issue will not be addressed immediately but some methods
 *	may be depricated in future releases.
 *
 *	This documentation is going to cover the use of Attributes when using IDecObject.
 *	Other API calls are documented elsewhere and are not currently covered by this
 *	document.
 *
 *	Attributes are a generic way to retrieve information about a file and perhaps it's
 *	content.  Each file should have at least one attribute collection which will contain
 *	at a minimum a pathname attribute that describes the location of the file.  It's
 *	<B><I>important</I></B> to note that the current implementation of Attributes is
 *	primarely centered around MIME.  The first implementation favors MIME in terms of the amount
 *	attributes that are available but the API is written in a generic way.  Having a
 *	generic API allows the Decomposer team to introduce similar functionality with 
 *	other engines in the future.
 *
 *	More documentation will be provided in the near future but for now a short example
 *	will serve to aid the developer.
 *
 *	<pre>
 *
 *  void DumpAttributes(IDecObject* pObject)
 *  {
 *      size_t numberOfCollections = pObject->GetNumberOfAttributeCollections();
 *
 *      char    state0[] = "UNKNOWN STATE!";
 *      char    state1[] = "eUninitialized";
 *      char    state2[] = "eInitialized";
 *      char    state3[] = "ePartiallyInitialzed";
 *
 *      char*   pState = &state0[0];
 *
 *      switch ( pObject->GetAttributeState() )
 *      {
 *          case IDecObject::AttributeState::eInitialized :{
 *                  pState = &state2[0];
 *                  break;
 *                }
 *          case IDecObject::AttributeState::eUninitialized :{
 *                  pState = &state1[0];
 *                  break;
 *                }
 *          case IDecObject::AttributeState::ePartiallyInitialzed :{
 *                  pState = &state3[0];
 *                  break;
 *                }
 *      }
 *
 *      for ( size_t i = 0; i < numberOfCollections; i++ )
 *      {
 *          IDecAttributeCollection* ptr = pObject->GetAttributeCollectionAt(i);
 *
 *
 *          printf( "\\n\\n===================  Support = %s, State = %s ==========================\\n\\n", 
 *              (pObject->GetAttributeSupport() != IDecObject::AttributeSupport::eEngineSupport) ? "Base Only" : "Full Engine", pState );
 *
 *          // Use the base class API to get the keys....
 *
 *
 *          size_t keyLength;
 *          const CDecAttributeKey* keyPtr;
 *          int index = 0;
 *          while  ( (keyPtr = ptr->GetKeyAt(index++)) != NULL )
 *          {
 *              size_t keyValues = ptr->GetNumberOfValuesForKey(keyPtr);
 *              while ( keyValues > 0 ) 
 *              {
 *                  size_t attributeLength;
 *                  IDecAttribute* anAttribute = ptr->GetAttributeAt(keyPtr,keyValues-1);
 *                  const char* attrValue = anAttribute->GetValueAsText(&attributeLength);
 *                  printf("Key = %s, value #%d = %s\\n",keyPtr->GetKey(&keyLength),keyValues,attrValue);
 *                  printf("Key From Attribute = %s\\n",(anAttribute->GetKey())->GetKey(&keyLength));
 *                  keyValues--;
 *              }
 *          }   
 *  }
 *	</pre>
 *
 */
class IDecObject
{
public:
			IDecObject() {}
	virtual ~IDecObject() {}

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
	// -- Start Attribute APIs

	/**
	 *	AttributeState indicates the initialized state of this instance.  An instance
	 *	may be initialized, uninitialized, or partially initialized.  The primary
	 *	purpose of this enum is to allow the caller to determine if this instance
	 *	is ready for use.  This is important since an instance of IDecAttributeCollection
	 *	can be retrieved on different Decomposer callbacks.
	 */

	enum AttributeState {
		eUninitialized,			/**< The instance has not yet been initialized and should not be used */
		eInitialized,			/**< The instance is fully initialized and ready for use */
		ePartiallyInitialzed	/**< The object is partially initialized but all attributes may not be available */
	};

	/**
	 *	AttributeSupport indicates the level of support for the attributes contained 
	 *	in an instance of IDecObject.  There are two levels of support. The first level
	 *	eBaseSupport indicates that the attributes associated with 'this' instance have
	 *	been provided by the Decomposer framework, and the actual engine processing the
	 *	file is not attribute aware or enabled.  The second eEngineSupport indicates that
	 *	the engine providing 'this' instance is attribute aware and enabled.
	 */

	enum AttributeSupport {
		eBaseSupport,		/**< The instance supports base attributes only (i.e. engine is not attribute aware). */
		eEngineSupport		/**< The engine providing this instance fully supports attributes. */
	};


	/**
	 * GetAttributeState returns the initialized state of the attributes.
	 *
	 *	@returns IDecObject::AttributeState
	 */
	
	virtual AttributeState GetAttributeState() const = 0;

	/**
	 * GetAttributeSupport returns the level of support for attributes for 'this' instance.
	 *
	 *	@returns IDecObject::AttributeSupport
	 */

	virtual AttributeSupport GetAttributeSupport() const = 0;

	// Index of 0 returns attributes for this object.
	// If this is a container increasing indexes return
	// attributes for iteams within the container.

	/**
	 * Given a valid index GetAttributeCollectionAt returns a IDecAttrributeCollection pointer.
	 *
	 * @param index to the desired attribute collection
	 *
	 * @returns IDecAttributeCollection* pointer to collection or NULL if non are available
	 */
	virtual IDecAttributeCollection* GetAttributeCollectionAt(size_t index) const = 0;
	
	/**
	 * Returns the current number of Attribute collections associated with this instance
	 * of IDecObject.
	 *
	 * @returns size_t number of current attribute collections
	 */
	virtual size_t GetNumberOfAttributeCollections() const = 0;

	
	// -- End Attribute APIs
#endif //*** End AS400 Attribute API Removal *** 

	// IDecObject methods:
	virtual DECRESULT GetName(char *pszName, DWORD *pdwNameSize, DWORD *pdwCharset) = 0;
	virtual DECRESULT GetSecondaryName(char *pszName, DWORD *pdwNameSize, DWORD *pdwCharset) = 0;
	virtual DECRESULT GetNameType(DWORD *pdwNameType) = 0;
	virtual DECRESULT GetParent(IDecContainerObject **ppParentObject) = 0;
	virtual DECRESULT GetDepth(DWORD *pdwDepth) = 0;
	virtual DECRESULT GetDataFile(char *pszDataFile, DWORD *pdwNameSize) = 0;
	virtual DECRESULT GetCanDelete(bool *pbCanDelete) = 0;
	virtual DECRESULT GetCanDeleteAncestor(bool *pbCanDelete, DWORD *pdwDepth) = 0;
	virtual DECRESULT GetCanModify(bool *pbCanModify) = 0;
	virtual DECRESULT GetCanReplace(bool *pbCanReplace) = 0;
	virtual DECRESULT GetCanRename(bool *pbCanRename) = 0;
	virtual DECRESULT GetProperty(const char *szName, DWORD &value) = 0;
	virtual DECRESULT GetProperty(const char *szName, DWORD *pValue) = 0;
	virtual DECRESULT SetProperty(const char *szName, DWORD value) = 0;
	virtual DECRESULT GetProperty(const char *szName, const char *&value) = 0;
	virtual DECRESULT SetProperty(const char *szName, const char *value) = 0;
	virtual DECRESULT RemoveProperty(const char *szName) = 0;
	virtual DECRESULT GetNormalizedName(char *pszInput, char *pszOutput, DWORD dwOutputSize, bool bANSI) = 0;
	virtual DECRESULT GetRenameItemName(char **ppszName) = 0;
	virtual DECRESULT SetRenameItemName(const char *pszName, bool bSetParent) = 0;
	virtual DECRESULT GetMIMEItemContentType(char **ppszType) = 0;
	virtual DECRESULT SetMIMEItemContentType(const char *pszType, bool bSetParent) = 0;
	virtual DECRESULT GetAttributes(DWORD *pdwAttrs, DWORD *pdwAttrType) = 0;
	virtual DECRESULT SetAttributes(DWORD dwAttrs, DWORD dwAttrType) = 0;
	virtual DECRESULT GetTime(DWORD *pdwHour, DWORD *pdwMinute, DWORD *pdwSecond, DWORD *pdwMillisecond) = 0;
	virtual DECRESULT SetTime(DWORD dwHour, DWORD dwMinute, DWORD dwSecond, DWORD dwMillisecond) = 0;
	virtual DECRESULT GetDate(DWORD *pdwYear, DWORD *pdwMonth, DWORD *pdwDay) = 0;
	virtual DECRESULT SetDate(DWORD dwYear, DWORD dwMonth, DWORD dwDay) = 0;
	virtual DECRESULT GetItemType(DWORD *pdwType) = 0;
	virtual DECRESULT SetItemType(DWORD dwType) = 0;
	virtual DECRESULT GetMalformed(DWORD *pdwType) = 0;
	virtual DECRESULT SetMalformed(DWORD dwType) = 0;
	virtual DECRESULT GetEngineInterface(void **ppInterface) = 0;
	virtual DECRESULT SetEngineInterface(void *pInterface) = 0;
	virtual const char* ToString(unsigned int ilevel) = 0;

};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecContainerObject

class IDecContainerObject :
	public IDecObject
{
public:
	IDecContainerObject() {}
	virtual ~IDecContainerObject() {}

	// IDecContainerObject methods:
	virtual DECRESULT GetContainerType(DWORD *pdwType) = 0;
	virtual DECRESULT GetUncompressedSize(DWORDLONG *pdwlSize) = 0;
	virtual DECRESULT GetCompressedSize(DWORDLONG *pdwlSize) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecEventSink

class IDecEventSink
{
public:
	IDecEventSink() {}
	virtual ~IDecEventSink() {}

	//
	// Methods supported when DEC_OPTION_EVENTSINK_VERSION == EVENT_SINK_VERSION_1
	//
	virtual DECRESULT OnObjectBegin(
		IDecObject *pObject,
		WORD *pwResult,
		char *pszNewDataFile,
		bool *pbContinue) = 0;

	virtual DECRESULT OnObjectEnd(
		IDecContainerObject *pObject,
		DECRESULT hr) = 0;

	virtual DECRESULT OnDiskFileNeeded(
		IDecContainerObject *pObject,
		char *pszdiskfile,
		int iBufferSize) = 0;

	virtual DECRESULT OnObjectNotIdentified(
		IDecObject *pObject,
		WORD *pwResult,
		char *pszNewDataFile) = 0;

	virtual DECRESULT OnContainerIdentified(
		IDecContainerObject *pObject,
		bool *pbContinue) = 0;

	virtual DECRESULT OnContainerEnd(
		IDecContainerObject *pObject,
		void **ppInsertObjects) = 0;

	virtual DECRESULT OnChildFound(
		IDecContainerObject *pObject,
		DWORD dwType,
		const char *pszChildName,
		DWORD dwCharset,
		bool *pbContinue) = 0;

	virtual DECRESULT OnChildBad(
		IDecContainerObject *pObject,
		DECRESULT hr,
		const char *pszChildName,
		bool *pbContinue) = 0;

	virtual DECRESULT OnBusy() = 0;

	virtual DECRESULT OnEngineError(
		IDecContainerObject *pObject,
		DECRESULT hr,
		WORD *pwResult,
		char *pszNewDataFile) = 0;

	virtual DECRESULT OnKeyNeeded(
		IDecContainerObject *pObject,
		unsigned char *pbyKey,
		int nKeyBufferLen,
		int *piKeyLen,
		bool *pbContinue,
		DWORD *lpdwParam) = 0;
	
	//
	// Methods supported when DEC_OPTION_EVENTSINK_VERSION == EVENT_SINK_VERSION_2
	//
	virtual DECRESULT OnChildFound2(
		IDecContainerObject *pObject,
		DWORD dwType,
		const char *pszChildName,
		DecNameType ChildNameType,
		DWORD dwCharset,
		bool *pbContinue) = 0;

	virtual DECRESULT OnChildBad2(
		IDecContainerObject *pObject,
		DECRESULT hr,
		const char *pszChildName,
		DecNameType ChildNameType,
		bool *pbContinue) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Class CDecEventSink
//
// Standard implementation of a Decomposer event sink.  All clients should
// derive from this class rather than IDecEventSink because if we add new
// methods to that interface later, a default implementation will be provided
// in CDecEventSink.

class CDecEventSink : 
	public IDecEventSink
{
public:
	//
	// Methods supported when DEC_OPTION_EVENTSINK_VERSION == EVENT_SINK_VERSION_1
	//
	virtual DECRESULT OnObjectBegin(
		IDecObject * /*pObject*/,
		WORD *pwResult,
		char *pszNewDataFile,
		bool *pbContinue)
	{
		*pwResult = DECRESULT_UNCHANGED;
		*pszNewDataFile = '\0';
		*pbContinue = true;
		return DEC_OK;
	}

	virtual DECRESULT OnObjectEnd(
		IDecContainerObject * /*pObject*/,
		DECRESULT hr)
	{
		return hr;
	}

	virtual DECRESULT OnDiskFileNeeded(
		IDecContainerObject * /*pObject*/,
		char * /*pszdiskfile*/,
		int /*iBufferSize*/)
	{
		return DEC_OK;
	}

	virtual DECRESULT OnContainerIdentified(
		IDecContainerObject * /*pObject*/,
		bool *pbContinue)
	{
		*pbContinue = true;
		return DEC_OK;
	}

	virtual DECRESULT OnContainerEnd(
		IDecContainerObject * /*pObject*/,
		void ** /*ppInsertObjects*/)
	{
		return DEC_OK;
	}

	virtual DECRESULT OnChildFound(
		IDecContainerObject * /*pObject*/,
		DWORD /*dwType*/,
		const char * /*pszChildName*/,
		DWORD /*dwCharset*/,
		bool *pbContinue)
	{ 
		*pbContinue = true;
		return DEC_OK;
	}

	virtual DECRESULT OnChildBad(
		IDecContainerObject * /*pObject*/,
		DECRESULT hr,
		const char * /*pszChildName*/,
		bool *pbContinue)
	{
		*pbContinue = false;
		return hr;
	}

	virtual DECRESULT OnObjectNotIdentified(
		IDecObject * /*pObject*/,
		WORD *pwResult,
		char *pszNewDataFile)
	{
		*pwResult = DECRESULT_UNCHANGED;
		*pszNewDataFile = '\0';
		return DEC_OK;
	} 

	virtual DECRESULT OnBusy() 
	{ 
		return DEC_OK; 
	}

	virtual DECRESULT OnEngineError(
		IDecContainerObject * /*pObject*/,
		DECRESULT hr,
		WORD *pwResult,
		char *pszNewDataFile)
	{
		*pwResult = DECRESULT_UNCHANGED;
		*pszNewDataFile = '\0';
		return hr;
	} 

	virtual DECRESULT OnKeyNeeded(
		IDecContainerObject * /*pObject*/,
		unsigned char * /*pbyKey*/,
		int /*nKeyBufferLen*/,
		int * /*piKeyLen*/,
		bool *pbContinue,
		DWORD * /*lpdwParam*/)
	{
		*pbContinue = false;
		return DECERR_CHILD_DECRYPT;
	}

	//
	// Methods supported when DEC_OPTION_EVENTSINK_VERSION == EVENT_SINK_VERSION_2
	//
	virtual DECRESULT OnChildFound2(
		IDecContainerObject * /*pObject*/,
		DWORD /*dwType*/,
		const char * /*pszChildName*/,
		DecNameType /*ChildNameType*/,
		DWORD /*dwCharset*/,
		bool *pbContinue)
	{ 
		*pbContinue = true;
		return DEC_OK;
	}

	virtual DECRESULT OnChildBad2(
		IDecContainerObject * /*pObject*/,
		DECRESULT hr,
		const char * /*pszChildName*/,
		DecNameType /*ChildNameType*/,
		bool *pbContinue)
	{
		*pbContinue = false;
		return hr;
	}
};


/////////////////////////////////////////////////////////////////////////////
// Interface IDecIOCB
//
// This is the base interface for IDecIO.  This is necessary because
// DecIO.h must include some header files that are specific to providing
// stream I/O support.  Since we do not want to have to force Decompsoer
// clients into using DecIO.h unless they want to, this interface allows
// us to strongly type the pIOCallback parameter in the Process function.

class IDecIOCB
{
public:
	IDecIOCB() { }
	virtual ~IDecIOCB() { }
};


/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2_CPP
	#define DECLINKAGE __declspec(dllexport) 
  #else
	#define DECLINKAGE __declspec(dllimport) 
  #endif
#else
  #ifdef __cplusplus
	#define DECLINKAGE extern "C"
  #else
	#define DECLINKAGE
  #endif
#endif


DECLINKAGE DECRESULT DecNewDecomposer(IDecomposer **ppDecomposer);
DECLINKAGE DECRESULT DecDeleteDecomposer(IDecomposer *pDecomposer);
DECLINKAGE DECRESULT DecNewFileObject(const char *szName, DWORD dwCharset, const char *szDataFile, IDecObject **ppObject);
DECLINKAGE DECRESULT DecDeleteFileObject(IDecObject *pObject);
DECLINKAGE void		DecGetVersion(const char **ppszVersion);

#endif	// DEC2_H
