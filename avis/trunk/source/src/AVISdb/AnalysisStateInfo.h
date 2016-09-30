//
//	IBM AntiVirus Immune System
//
//	File Name:	AnalysisStateInfo.h
//
//	Author:		Andy Klapper
//
//	This class represents the AnalysisStateInfo table in the AVIS database.
//
//	$Log:  $
//	
//
//	Preliminary
//
//	IBM/Symantec Confidential
//

#if !defined(AFX_ANALYSISSTATEINFO_H__4342B4EA_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
#define AFX_ANALYSISSTATEINFO_H__4342B4EA_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVISdbDLL.h"
#include "AVIS.h"
#include "AVISDBException.h"

#pragma warning(disable: 4251)	// warns that std::string needs to be exported
								// to dll users (which would be true if it was
								// an internal class)

class AVISDB_API AnalysisStateInfo  
{
public:
	static bool	AddNew(std::string& name);

	explicit AnalysisStateInfo(std::string& findName);
	explicit AnalysisStateInfo(uint id);

	virtual ~AnalysisStateInfo();

	bool	IsNull()		{ return isNull; };
	ushort	ID()			{ NullCheck("ID"); return id;	};

	operator std::string()	{ NullCheck("operator std::string"); return name; };

	bool operator==(AnalysisStateInfo& rhs)
							{ NullCheck("operator==(AnalysisStateInfo&)");
							  if (rhs.isNull)
								  throw AVISDBException(exceptType, "operator==(AnalysisStateInfo&), rhs.isNull");
							  return id == rhs.id; }
	bool operator!=(AnalysisStateInfo& rhs) { return !(*this == rhs); };

	bool operator==(std::string& stateName)
							{ NullCheck("operator==(string)"); return stateName == name; };
	bool operator!=(std::string& stateName)
							{ return !(*this == stateName); };

	//
	//	Allow a state to be changed
	AnalysisStateInfo& operator=(std::string& value);

	static std::string UnknownSampleState() { return std::string("unknown_sample_state"); };
	static std::string SampleCheckError()	{ return std::string("sample_check_error"); };
	static std::string Accepted()			{ return std::string("accepted"); };
	static std::string Building()			{ return std::string("building"); };
	static std::string Built()				{ return std::string("built"); };
	static std::string Captured()			{ return std::string("captured"); };
	static std::string Declined()			{ return std::string("declined"); };
	static std::string Deferred()			{ return std::string("deferred"); };
	static std::string Exectuting()			{ return std::string("exectuting"); };
	static std::string Extracted()			{ return std::string("extracted"); };
	static std::string Extracting()			{ return std::string("extracting"); };
	static std::string Imported()			{ return std::string("imported"); };
	static std::string Importing()			{ return std::string("importing"); };
	static std::string Infected()			{ return std::string("infected"); };
	static std::string Pong()				{ return std::string("pong"); };
	static std::string Released()			{ return std::string("released"); };
	static std::string Replicated()			{ return std::string("replicated"); };
	static std::string Scored()				{ return std::string("scored"); };
	static std::string Scoring()			{ return std::string("scoring"); };
	static std::string Segragated()			{ return std::string("segragated"); };
	static std::string Segregating()		{ return std::string("segregating"); };
	static std::string Submitting()			{ return std::string("submitting"); };
	static std::string Testing()			{ return std::string("testing"); };
	static std::string UnInfected()			{ return std::string("uninfected"); };
	static std::string Corrupted()			{ return std::string("corrupted"); };
	static std::string Malformed()			{ return std::string("malformed"); };
	static std::string Lost()				{ return std::string("lost"); };
  static std::string Misfired() { return std::string("misfired");};
	static std::string Receiving()			{ return std::string("receiving"); };
	static std::string CRCSampleFailure()	{ return std::string("crc_sample_failure"); };
	static std::string CRCContentFailure()	{ return std::string("crc_content_failure"); };
	static std::string BadSampleCheckSum()	{ return std::string("bad_sample_checksum"); };
	static std::string BadContentCheckSum()	{ return std::string("bad_content_checksum"); };
	static std::string UnScrambleFailure()	{ return std::string("unscramble_failure"); };
	static std::string UncompressFailure()	{ return std::string("uncompress_failure"); };
	static std::string MissingAttributes()	{ return std::string("missing_attributes"); };
	static std::string BadSubmittorID()		{ return std::string("bad_submittor_id"); };
	static std::string BadHWCorrelator()	{ return std::string("bad_hw_correlator"); };
	static std::string DataOverflow()		{ return std::string("data_overflow"); };
	static std::string PausedFilter()		{ return std::string("paused_filter"); };
	static std::string ScanError()			{ return std::string("scan_error"); };
	static std::string PausedScanning()		{ return std::string("paused_scanning"); };
	static std::string UnknownSampleType()	{ return std::string("unknown_sample_type"); };
	static std::string UnpackageFailure()	{ return std::string("unpackage_failure"); };
	static std::string PausedFilterUnknown(){ return std::string("paused_filter_unknown"); };
	static std::string ExportForAnalysis()	{ return std::string("export_for_analysis"); };
	static std::string UnSubmitable()		{ return std::string("unsubmitable"); };
	static std::string UnInfectible()		{ return std::string("uninfectible"); };
	static std::string BufferError()		{ return std::string("buffer_error"); };
	static std::string InternalError()		{ return std::string("internal_error"); };
	static std::string NoRepair()			{ return std::string("norepair"); };
	static std::string UnRepairable()		{ return std::string("unrepairable"); };
	static std::string BadRepair()			{ return std::string("badrepair"); };
	static std::string NoDetect()			{ return std::string("nodetect"); };
	static std::string Defer()				{ return std::string("defer"); };
  static std::string Deferring()			{ return std::string("deferring"); };
  static std::string Error()                            { return std::string("error"); };


	static uint			NoDetectID();

	static bool	Test(void);

	std::string	ToDebugString(void);	// returns a string representation of the
										// object suitable for debugging messages

private:
	bool		isNull;
 	ushort		id;
	std::string	name;

	static const ushort	NameLength;



	void		NullCheck(const char* where)
	{
		if (IsNull())
		{
			std::string	msg(where);
			msg	+= ", AnalysisStateInfo is null";
			throw AVISDBException(exceptType, msg);
		}
	}

	static const AVISDBException::TypeOfException	exceptType;
};

#pragma warning(default: 4251)	// try to limit the scope of this a little bit

#endif // !defined(AFX_ANALYSISSTATEINFO_H__4342B4EA_562F_11D2_8905_00A0C9DB9E9C__INCLUDED_)
