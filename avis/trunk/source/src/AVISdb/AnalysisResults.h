//
//	IBM AntiVirus Immune System
//
//	File Name:	AnalysisResults.h
//
//	Author:		Andy Klapper
//
//	This class represents the AnalysisResults table in the AVIS database.
//
//	$Log: $
//	
//
//	Preliminary
//
//	IBM/Symantec Confidential
//

#if !defined(AFX_ANALYSISRESULTS_H__4342B4E7_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
#define AFX_ANALYSISRESULTS_H__4342B4E7_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISdbDLL.h"
#include "AVIS.h"
#include "AVISDBException.h"
#include "Signature.h"
#include "CheckSum.h"
#include "AnalysisStateInfo.h"

#pragma	warning( disable: 4251 )	// warns that std::string needs to be exported
// to dll users (which would be true if it was
// an internal class)

class AVISDB_API AnalysisResults 
{
public:
  static bool	AddNew(CheckSum& checkSum, uint sig,
                       AnalysisStateInfo& state);
  //  	static bool	AddNew(CheckSum& checkSum, Signature& sig,
  //  						AnalysisStateInfo& state);
  
  AnalysisResults(CheckSum& checkSum);
  virtual ~AnalysisResults();
  
  bool		IsNull(void)		{ return isNull; };
  uint GetSignatureSequenceNum(void) const { return sigSeqNum;};
  Signature	GetSignature(void)	{ Signature	sig(sigSeqNum); return sig; };

  AnalysisStateInfo	SampleState(void){ NullCheck("SampleState"); AnalysisStateInfo asi(state); return asi; };
  CheckSum	GetCheckSum(void)	{ NullCheck("GetCheckSum"); CheckSum cs(checkSum); return cs; };


  void		Refresh(void);

  std::string	ToDebugString(void);	// returns a string representation of the
  // object suitable for debugging messages

private:
  bool			isNull;
  std::string		checkSum;
  uint			sigSeqNum;
  ushort			state;

  void		NullCheck(const char* where)
  {
    if (IsNull())
      {
        std::string	msg(where);
        msg	+= ", AnalysisResults is null";

        throw AVISDBException(exceptType, msg);
      }
  }

  static const AVISDBException::TypeOfException	exceptType;
};
#pragma warning( default: 4251 )	// try to limit the scope of this a little bit

#endif // !defined(AFX_ANALYSISRESULTS_H__4342B4E7_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
