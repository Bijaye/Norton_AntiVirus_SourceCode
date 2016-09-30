// Filter.h: interface for the Filter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTER_H__02479CB0_67B4_11D2_8927_00A0C9DB9E9C__INCLUDED_)
#define AFX_FILTER_H__02479CB0_67B4_11D2_8927_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <memory>

#include <AVIS.h>
#include <Attributes.h>

#include "AVISFilter.h"
#include "MappedMemory.h"
#include "CompoundDataManipulator.h"
//#include "UnScrambler.h"
//#include "DeCompressor.h"
#include "CRCCalculator.h"
#include "CheckSum.h"
#include "FilterException.h"

#pragma warning(disable: 4251)	// warns that std::string needs to be exported
// to dll users (which would be true if it was
// an internal class)

class AVISFILTER_API Filter  
{
public:
  enum FilterStatus {
    NeedMoreData,			// The filter needs more data to finish
    // filtering i.e. it needs to scan the file.
    // * can appear any time
    // @ keep feeding it data

    Done_Infected,			// The file is infected. (We know this from
    // either a previous request or a scan
    // performed by the filter).
    // * can appear after instantiation or after
    //   full down load (after last data chunk is
    //   processed).
    // @ final state, return answer to client
    //   set imported date to now

    Done_FoundByHeuristic,	// The file is infected (we know this from
    // a scan performed by the filter), but it
    // was found by a heuristic so Symantec wants
    // it uploaded anyway.
    // * can appear after full down load.
    // @ not a final state, pass on to analysis
    //   center
    //   set imported date to now
    //   set analysis state to "Imported"

    Done_BadScan,			// The file may or may not be infected, we
    // do not know because the scanner is unable to
    // scan the file (really clever virus writer or
    // not so clever scanner - you decide).
    // * can appear only after a full down load.
    // @ not a final state, pass on to analysis
    //   center
    //   set imported date to now
    //   set analysis state to "Imported"

    Done_NoRepair,			// The file is infected (we know this from
    Done_UnRepairable,		// a scan performed by the filter), but the
    Done_BadRepair,			// file was not repairable so Symantec wants
    // it uploaded anyway.
    // The three states how the repair operation
    // failed.  In the current filter logic they
    // are handled the same way.
    // * can appear after full down load.
    // @ not a final state, pass on to analysis
    //   center
    //   set imported date to now
    //   set analysis state to "Imported"

    Done_NotInfectable,		// The file cannot be infected.
    // * available after full download
    // @ final state, return answer to client
    //   set imported date to now.

    Done_UnInfected,		// The file is not infected.  (We can only
    // know this from a previous request that has
    // been reviewed by a human).
    // * available at instantiation time
    // @ final state, return answer to client
    //	 set imported date to now
		
    Done_NoDetect,			// The is not infected as far as we can tell
    // from the latest signature files.  Since the
    // sample was submitted in an automated way it
    // is assumed that a false positive has been
    // corrected.
    // * can only appear after full down load.
    // @ final state, return answer to client
    //   set imported date to now.

    Done_UnSubmittable,		// The file is infected, but not by a virus.
    // could be a Trogran Horse.  In any case the
    // analysis of this virus is done.
    // * available at instantiation time
    // @ final state, return answer to client
    //	 set imported date to now

    Done_NAVoverrun,		// The file is infected by a virus and was found
    // not using a heuristic, but while trying to
    // repair the file NAV tried to write more at
    // the end of the file than the scanner service
    // allowed for.  Send on for further analysis.
    // * available after full download
    // @ set imported date to now.


    Done_NeedsHarderLook,	// The file should be passed on to the analysis
    // center for further analysis.  (No prior
    // matching request found and no virus found on
    // local scans)
    // * available after full download
    // @ pass on to analysis center for analysis
    //   set imported date to now
    //	 set analysis state to "Imported"

    Done_Misfired,			// Added by Mike

    //		Done_NeedsQuickLook,	// This flag is no longer used.  It used to
    // represent the case where the customer only
    // gets a free scan with the latest signatures,
    // but does not get to use the analysis center
    // * available after full download
    // @ final state, return answer to client
    //   ???

    Done_Piggybacked,		// The request is already being processed by
    // the analysis center.  From this point on
    // this request is linked to the prior request
    // with the same CRC value.
    // * available after instaniation.
    // @ return current status to client
    //   set imported date to now

    CRCsampleFailure,		// The CRC code failed for some reason, unable
    // to continue processing this (and possibly
    // other) sample.
    // * available after full download
    // @ final state, return error to client
    //   set analysis state to "CRCSampleFailure"

    CRCcontentFailure,		// The CRC code failed for some reason, unable
    // to continue processing this (and possibly
    // other) sample.
    // * available after full download
    // @ final state, return error to client
    //   set analysis state to "CRCContentFailure

    BadSampleCheckSum,		// The sample CRC in the header did not match
    // the CRC generated from the downloaded sample.
    // * available after full download
    // @ final state, return error to client
    //   set analysis state to "BadSampleCheckSum"

    BadContentCheckSum,		// The content CRC in the header did not match
    // the CRC generated from the downloaded
    // content.
    // * available after full download
    // @ final state, return error to client
    //   set analysis state to "BadContentCheckSum"

    MissingAttributes,		// One of the attributes required for the
    // filtering of the sample is missing.
    // Unable to continue processing this sample.
    // * available after full download
    // @ final state, return error to client
    //   set analysis state to "MissingAttributes"

    BadSubmittorID,			// The submittor name given did not exist in
    // the database and could not be added to the
    // database.
    // Unable to continue processing this sample.
    // * available after instaniation.
    // @ final state, return error to client
    //   set analysis state to "BadSubmittorID"

    //		BadHWCorrelator,		// The hw correlator given did not exist in
    // the database and could not be added to the
    // database.
    // Unable to continue processing this sample.
    // * available after instaniation.
    // @ final state, return error to client
    //   set analysis state to "BadHWCorrelator"

    DatabaseError,			// An error occured while trying to access the
    // database.  Unable to continue processing.
    // * available any time.
    // @ put request into pause filter mode waiting
    //   for human intervention.

    DataOverflow,			// More data was passed to the filter than was
    // specified in the header.
    // * available after full download
    // @ final state, return error to client
    //   set analysis state to "DataOverflow"


    //DataUnderflow,		// Less data was passed to the filter than was
    // specified in the header.  NO LONGER IN USE!
    // * available after full download
    // @ final state, return error to client
    //   set analysis state to "DataUnderflow"

    UnableToCreateMappedMem,// The filter was unable to create a memory
    // mapped file to place the content and sample
    // images into.
    // * available after instaniation.
    // @ put request in pause mode waiting for
    //   human intervention.
    //   set analysis state to "PausedFilter"

    ScanError,				// The scanner process either returned an
    // error or was not available to scan the
    // file.
    // * available after instaniation.
    // @ final state, return error to client.
    //   set analysis state to "ScanError"

    UnableToScan,			// The scanner process was unreachable or
    // died while scanning.
    // * available after instaniation.
    // @ put request into pause mode waiting for
    //   human intervention.
    //	 set analysis state to "PausedScanning"

    UnknownSampleType,		// The header specified a sample type that
    // is not handled by the filter (file or
    // sector types are handled).  - NOTE: 
    // sector type is just returned
    // DoneNeedsHarderLook).
    // * available after full download
    // @ final state, return error to client.
    //   set analysis state to "UnknownSampleType"

    UnpackageFailure,		// An error occured while trying to uncompress/
    // unscramble/unencoded the data.  Unable to
    // continue processing.
    // * available while processing data.
    // @ final state, return error to client.
    //   set analysis state to "UnpackageFailure"

    UnknownError			// Something really bad happened durring
    // the intaniation of the filter.  Don't know
    // what, but it's bad.
    // * available after instaniation.
    // @ put request into pause mode waiting for
    //   human intervention.
    //   set analysis state to "PausedFilterUnknown"
  };

  Filter(const std::string& attributes);
  virtual ~Filter();

  FilterStatus	Status(void)		{ return status; };
  uint			LocalCookie(void)	{ return attr.LocalCookie(); };
  FilterStatus	AppendData(const char* buffer, uint length);
  FilterStatus	Done(void);
  bool			VirusName(std::string& vName);
  bool			VirusID(uint& virusID);
  bool			WriteContentsToDisk(std::string& fileName)
  {return contents.WriteToFile(fileName); }
  bool			WriteSampleToDisk(std::string& fileName)
  {return sample.WriteToFile(fileName);}

  std::string		StatusAsString(void) { return std::string(statusAsChar[status]); };
  CheckSum GetSampleCheckSum() const;
  //The following function prototype was added on Mar/16/2000.
  void			SetDone(void);

private:
  bool						done;
  FilterStatus				status;
  Attributes					attr;
  std::string					virusName;
  uint						virusID;
  std::string					contentsFN;
  std::string					sampleFN;
  MappedMemory				contents;
  MappedMemory				sample;
  CompoundDataManipulator		dataManipulator;
  std::auto_ptr<DataManipulator>	unEncoder;
  std::auto_ptr<DataManipulator>	unScrambler;
  std::auto_ptr<DataManipulator>	unCompressor;
  std::auto_ptr<CRCCalculator> crcCalculatorSample;
  std::auto_ptr<CRCCalculator> crcCalculatorContent;
  std::string headerBlock;
  void				AddToResults(std::string& sampleCRC, uint sigSeqNum,
                                             std::string& state);
  static Attributes	CreateRequest(const std::string& attributes, FilterStatus& status);
  std::string	ContentName(Attributes& attr);
  std::string	SampleName(Attributes& attr);
  // Moved these out of line MPW
  static uint			ContentSize(Attributes& attr);
  static uint			SampleSize(Attributes& attr);

  static bool			ParseOutValues(const std::string& attributes, std::string& checkSum,
                                               uint& priority, std::string& submittor,
                                               std::string& minusSpecial);

  static	void		ExceptionHandler(FilterException::TypeOfException type, const char* source);

  static char			*statusAsChar[];
};

#pragma warning(default: 4251)	// try to limit the scope of this a little bit

#endif // !defined(AFX_FILTER_H__02479CB0_67B4_11D2_8927_00A0C9DB9E9C__INCLUDED_)
