//
//	IBM AntiVirus Immune System
//
//	File Name:	Attributes.h
//
//	Author:		Andy Klapper
//
//	This class represents the Attributes and AttributeKeys tables in the
//	AVIS database.
//
//
//	NOTE:	This class will modify all key values to conform to the http
//			standard style.  This style capitizies the initial character and
//			any character that follows a dash(-).  All other characters are
//			forced to be lower case.  This is being done to ensure no duplicates
//			due to capitialization differences.  (DB2 does not appear to have
//			a case insentive UNIQUE constraint).
//
//	$Log:  $
//	
//
//	Preliminary
//
//	IBM/Symantec Confidential
//

#if !defined(AFX_ATTRIBUTES_H__4C842321_52EA_11D2_88F2_00A0C9DB9E9C__INCLUDED_)
#define AFX_ATTRIBUTES_H__4C842321_52EA_11D2_88F2_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISdbDLL.h"
#include <AVIS.h>
#include "attributekeys.h"
#include "AVISDBException.h"
#include "log/types.h"

#pragma warning(disable: 4251)	// warns that std::string needs to be exported
// to dll users (which would be true if it was
// an internal class)

class Filter;
class AVISDB_API Attributes  
{
  friend Filter;
public:
  Attributes(const Attributes& copy_from_);
  virtual ~Attributes();
  
  Attributes& operator =(const Attributes& assign_from_);

  //	Return all of the attributes for this localCookie as one long string,
  //	in http format (key: value\nkey: value\n ...)
  operator std::string() const;

  //	Add some number of attributes to the database
  //	(input is in http attributes format)
  Attributes& operator+=(const std::string& addAttributes);

  //	Remove some number of attributes from the database
  //	(input is in http attributes format (we'll be a little more
  //	forgiving, you don't have to include bogus values)
  Attributes& operator-=(const std::string& subAttributes);

  //	Replace the current attributes with these attributes
  //	(input is in http attributes format)
  Attributes& operator= (const std::string& replaceAttributes);


  //
  //	Compare attributes to a string
  bool	operator==(const std::string& attrs) const;

  //
  //	Compare attributes to a string
  bool	operator!=(const std::string& attrs) const;

  std::string& operator[](const std::string& key_);

  uint		LocalCookie(void);
  uint          LocalCookie(uint cookie_) { return localCookie = cookie_;};

  std::string	ToDebugString(void);	// returns a string representation of the
  // object suitable for debugging messages

  //
  // I had removed these, putting them into file local scope, but
  // they're used by AnalysisRequest and god knows what else
  //
  static ushort	CheckSumID();
  static ushort	PriorityID();
  static ushort	StateID();
  static ushort	SubmittorID();
  static ushort	AnalyzedID();
  static ushort SignatureSequenceID();
  //

private:
  Attributes(uint lc);
  Attributes(const std::string& http_headers_);
  Attributes(uint lc,const std::string& http_headers_);
  
  void load_attributes_block();
  void save_attributes_block();

  MStringString _attribute_block;
  bool _dirty;
  uint		      localCookie;
};

#pragma warning(default: 4251)	// try to limit the scope of this a little bit

#endif // !defined(AFX_ATTRIBUTES_H__4C842321_52EA_11D2_88F2_00A0C9DB9E9C__INCLUDED_)
