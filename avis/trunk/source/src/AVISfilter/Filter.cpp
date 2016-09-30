// Filter.cpp: implementation of the Filter class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"


#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4251 )

#include <string>

#include "AVISFilter.h"
#include "Filter.h"

#include <Logger.h>

#include <XAnalysisState.h>

#include <Attributes.h>
#include <AnalysisResults.h>
#include <AnalysisRequest.h>
#include <AttributesParser.h>

#include <MapView.h>
#include <NAVScan.h>

#include "ToolFactory.h"
#include "deflate\Deflator.h"
#include "deflate\Inflator.h"
#include "FilterException.h"

// This constant specifies the width of the SubmittorID fields in the
// Submittors and AnalysisRequest tables in the database.  The value
// of the X-Customer-Identifier attribute will be truncated if it is
// wider than this constant.  (EJP 7/29/00)

#define MaximumSubmittorLength 64

char *Filter::statusAsChar[] = {
  "NeedMoreData",
  "Done_Infected",
  "Done_FoundByHeuristic",
  "Done_BadScan",
  "Done_NoRepair",
  "Done_UnRepairable",
  "Done_BadRepair",
  "Done_NotInfectable",
  "Done_UnInfected",
  "Done_NoDetect",
  "Done_UnSubmittable",
  "Done_NAVoverrun",
  "Done_NeedsHarderLook",
  "Done_Misfired",
  //		"Done_ForwardAnyway",
  //		"Done_NeedsHarderLook",
  //		"Done_NeedsQuickLook",
  "Done_Piggybacked",
  "CRCsampleFailure",
  "CRCcontentFailure",
  "BadSampleCheckSum",
  "BadContentCheckSum",
  "MissingAttributes",
  "BadSubmittorID",
  "DatabaseError",
  //		"DataOverflow",
  //	"DataUnderflow",
  "UnableToCreateMappedMem",
  "ScanError",
  "UnableToScan",
  "UnknownSampleType",
  "UnpackageFailure",
  "UnknownError"
};

using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static	const string	sampleCheckSumTag(AttributeKeys::SampleCheckSum());
static	const string	priorityTag(AttributeKeys::SamplePriority());
static	const string	submittorTag(AttributeKeys::CustomerID());
static	const string	stateTag(AttributeKeys::AnalysisState());
static	const string	checkSumMethodTag(AttributeKeys::CheckSumMethod());
static	const string	encodingMethodTag(AttributeKeys::ContentEncoding());
static	const string	concealmentMethodTag(AttributeKeys::ContentConcealment());
static	const string	compressionMethodTag(AttributeKeys::ContentCompression());
static	const string	sampleTypeTag(AttributeKeys::SampleType());
static	const string	sampleFileTag(AttributeKeys::SampleFile());
static	const string	sampleFileExtensionTag(AttributeKeys::SampleExtension());
static	const string	contentCheckSumTag(AttributeKeys::ContentCheckSum());

#pragma warning ( disable : 4101 )

void Filter::ExceptionHandler(FilterException::TypeOfException type, const char* source)
{
  try
    {
      throw;
    }
  catch (FilterException& e)
    {
      throw;
    }
  catch (AVISException& e)
    {
      string	errMsg("AVISException thrown in ");
      errMsg	+= source;
      errMsg	+= " [";
      errMsg	+= e.FullString();
      errMsg	+= "]";

      Logger::Log(Logger::LogError, Logger::LogAvisFilter, errMsg.c_str());

      throw FilterException(type, errMsg);
    }
  catch (exception& e)
    {
      string	errMsg("std:: exception thrown in ");
      errMsg	+= source;
      errMsg	+= " [";
      errMsg	+= e.what();
      errMsg	+= "]";

      Logger::Log(Logger::LogError, Logger::LogAvisFilter, errMsg.c_str());

      throw FilterException(type, errMsg);
    }
  catch (...)
    {
      string	errMsg("Exception of unknown type caught in ");
      errMsg	+= source;

      throw FilterException(type, errMsg);
    }

}
#pragma warning ( default : 4101 )

bool Filter::ParseOutValues(const string& attributes, string& checkSum,
                            uint& priority, string& submittor,
                            string& minusSpecial)
{
  EntryExit	entryExit(Logger::LogAvisFilter, "ParseOutValues");

  try
    {
      string		key, value;
      int			pos = 0;

      checkSum = submittor = "";
      priority = 0;

      while (string::npos !=
             (pos = AttributesParser::GetNextKeyValuePair(attributes, pos, key, value)))
        {
          AttributesParser::RightCase(key);

          if (sampleCheckSumTag == key)
            {
              checkSum = value;
            }
          else if (priorityTag == key)
            {
              priority = atoi(value.c_str());
            }
          else if (submittorTag == key)
            {
              submittor = value;
            }
          else if (stateTag != key)
            {
              minusSpecial	+= key;
              minusSpecial	+= ": ";
              minusSpecial	+= value;
              minusSpecial	+= "\n";
            }
        }

      // If no X-Customer-Identifier attribute was included by the
      // submittor, use a dummy value.  If an attribute was included
      // by the submittor and the value exceeds the maximum length of
      // the "submittorID" fields in the database, then truncate the
      // value.  (EJP 7/29/00)
      
      if (submittor.size()==0) submittor = "unknown customer";
      if (submittor.size()>MaximumSubmittorLength) submittor = submittor.substr(0,MaximumSubmittorLength);
    }
  catch (...)
    {
      ExceptionHandler(FilterException::ParseOutValues, "Filter's ParseOutValues function");
    }

  return checkSum != "" && submittor != "";
}


Filter::Filter(const string& attributes) :
  done(true),
  status(Filter::UnknownError),
  attr(CreateRequest(attributes, status)),
  virusName(""),
  virusID(0),
  contentsFN(ContentName(attr)),
  sampleFN(SampleName(attr)),
  contents(contentsFN, ContentSize(attr)),
  sample(sampleFN, SampleSize(attr)),
  unEncoder(NULL), unScrambler(NULL), unCompressor(NULL),
  headerBlock(attributes)
  // ,
  //   crcCalculatorSample(),
  //   crcCalculatorContent()
  //   crcCalculatorSample(ToolFactory::GetCRCCalculator(attr[checkSumMethodTag])),
  //   crcCalculatorContent(ToolFactory::GetCRCCalculator(attr[checkSumMethodTag]))
{

  EntryExit	entryExit(Logger::LogAvisFilter, "Filter constructor");
  try
    {
      //-------------------------------------------------------------------
      // If we've determined that we actually need to process the content
      //-------------------------------------------------------------------
      if (Filter::NeedMoreData == status)
        {
          // only NOW can we instantiate the crcCalculator* objects
          // But because there's no assignment operator for auto_ptr
          // which takes the contained template type, I'm doing this.
          std::auto_ptr<CRCCalculator> _crc_sample(ToolFactory::GetCRCCalculator(attr[checkSumMethodTag]));
          std::auto_ptr<CRCCalculator> _crc_content(ToolFactory::GetCRCCalculator(attr[checkSumMethodTag]));
          // There IS a simple same-type assignment operator
          crcCalculatorSample = _crc_sample;
          crcCalculatorContent = _crc_content;
          //---------------------------------------------------------------
          // But we've botched up the "mapped memory"
          //---------------------------------------------------------------
          if (0 == contents.LengthLeft() ||
              0 == sample.LengthLeft()	)// unable to create one of the maps
            {
              //-----------------------------------------------------------
              // Bail
              //-----------------------------------------------------------
              status = Filter::UnableToCreateMappedMem;
              done = true;
            }
          else
            {
              //---------------------------------------------------------------
              // Otherwise continue just fine.
              //---------------------------------------------------------------
              
              done = false;
            }
        }
      else
        {
          //---------------------------------------------------------------
          // On the other hand, if we don't need to touch the
          // content....
          // Just set the flag
          //---------------------------------------------------------------
          done = true;
        }

      //---------------------------------------------------------------
      // But if we really DO need to muck with the content.
      // Note: This means that AnalysisRequest::AddNew actually did
      // have to add a new record
      //---------------------------------------------------------------
      if (!done)
        {
          Logger::Log(Logger::LogDebug, Logger::LogAvisFilter,
                      "Filter constructor building sample unpackaging tools");

          string	method = attr[encodingMethodTag];

          if (!method.empty() && method != "")
            {
              if ("base64" == method)
                method = "base64decode";
              auto_ptr<DataManipulator>	tmp(ToolFactory::GetDataUnManipulator(method));
              unEncoder	= tmp;
              dataManipulator.Add(*unEncoder);
            }

          method = attr[concealmentMethodTag];
          if (!method.empty() && "" != method)
            {
              auto_ptr<DataManipulator>	tmp(ToolFactory::GetDataUnManipulator(method));
              unScrambler	= tmp;
              dataManipulator.Add(*unScrambler);
            }

          method = attr[compressionMethodTag];
          if (!method.empty() && method == Deflator::TypeName())
            {
              method = Inflator::TypeName();
              auto_ptr<DataManipulator>		tmp(ToolFactory::GetDataUnManipulator(method));
              unCompressor = tmp;
              dataManipulator.Add(*unCompressor);
            }
          else if ("" != method)
            {
              string	msg("Filter constructor, ");
              msg	+= method;
              msg += " is not a valid compression method";
              Logger::Log(Logger::LogError, Logger::LogAvisFilter, msg.c_str());
              throw FilterException(FilterException::Constructor, msg);
            }
        }
    }
  catch (...)
    {
      ExceptionHandler(FilterException::Constructor, "Filter constructor");
    }
}

Filter::~Filter()
{
  EntryExit	entryExit(Logger::LogAvisFilter, "Filter destructor");

  try
    {
      if (!done)
        attr[stateTag]	= string(XAnalysisState::Lost());
    }
  catch (...)
    {
      Logger::Log(Logger::LogError, Logger::LogAvisFilter,
                  "exception thrown from inside of the filter destructor");
    }
}

Filter::FilterStatus Filter::AppendData(const char* buffer, uint length)
{

  // This function is called too frequently while sample data is being
  // received to log each entry and exit.  The caller will log any
  // error code returned from this function.  (EJP 6/25/00)

#define DATA_RECEIVE_DETAILS 0
#if DATA_RECEIVE_DETAILS
  EntryExit	entryExit(Logger::LogAvisFilter, "Filter::AppendData");
#endif 

  try
    {
      if (Filter::NeedMoreData == status)
        {
          if (contents.LengthLeft() >= length)
            {
              uint	inLen	= length;
              uint	outLen	= sample.LengthLeft();

              memcpy(contents.CurrEnd(), buffer, length);
              crcCalculatorContent->CRCCalculate(contents.CurrEnd(), length);
              contents.ExtendEnd(length);

              while (inLen > 0 &&
                     dataManipulator.Process((const uchar *) buffer, inLen,
                                             sample.CurrEnd(),
                                             outLen = sample.LengthLeft()) &&
                     outLen > 0)
                {
                  crcCalculatorSample->CRCCalculate(sample.CurrEnd(), outLen);
                  sample.ExtendEnd(outLen);
                }

              if (DataManipulator::NoError != dataManipulator.Error())
                {
                  done	= true;
                  status	= Filter::UnpackageFailure;

                  Logger::Log(Logger::LogWarning, Logger::LogAvisFilter,
                              "Unpackage failure in Filter::AppendData()");
                }
              //Modified the comparison below from an || to && on Mar/28/2000.
              //We are done appending data only when we have no more data left 
              //in both the contents and the sample buffers.
              else if (0 == contents.LengthLeft() && 0 == sample.LengthLeft())
                {
                  Done();
                }
            }
          else
            {
              Logger::Log(Logger::LogWarning, Logger::LogAvisFilter,
                          "DataOverflow failure in Filter::AppendData()");
              status = Filter::DataOverflow;
            }
        }
    }

  catch (...)
    {
      ExceptionHandler(FilterException::AppendData, "Filter::AppendData");
    }

  return status;
}

//
//	So we think we are done...
Filter::FilterStatus Filter::Done()
{
  EntryExit	entryExit(Logger::LogAvisFilter, "Filter::Done");

  if (done)
    return status;

  done = true;

  try
    {
      uint	outLen(sample.LengthLeft());
      while (dataManipulator.Done(sample.CurrEnd(), outLen))
        {
          crcCalculatorSample->CRCCalculate(sample.CurrEnd(), outLen);
          sample.ExtendEnd(outLen);
          outLen = sample.LengthLeft();
        }

      if (DataManipulator::NoError != dataManipulator.Error())
        return status = Filter::UnpackageFailure;


      string	contentCRC;
      if (!crcCalculatorContent->Done(contentCRC))
        {
          return status = Filter::CRCcontentFailure;
        }
      else if (0 != stricmp(contentCRC.c_str(),
                            ((string) attr[contentCheckSumTag]).c_str()))
        {
          return status = Filter::BadContentCheckSum;
        }

      string	sampleCRC;
		
      if (crcCalculatorSample->Done(sampleCRC))
        {
          if (0 == stricmp(sampleCRC.c_str(),
                           ((string) attr[sampleCheckSumTag]).c_str()))
            {
              string	sampleType	= attr[sampleTypeTag];
              if ("sector" == sampleType)
                {
                  status = Filter::Done_NeedsHarderLook;
                }
              else if ("file"	== sampleType)
                {
                  string	sampleReason;
                  string	navVersion;
                  bool		infectable			= true;
                  bool		wasRepaired			= false;
                  uint		sigSeqNum	 = 0;
                  string	mappedName("\\\\.\\mapped\\");	// this is my convention!
                  mappedName	+= sampleFN;

                  NAVScan::ScanRC	scanRC(NAVScan::ScanBlessed(mappedName,
                                                                    attr.LocalCookie(),
                                                                    true, virusName, sigSeqNum,
                                                                    navVersion, virusID));

                  std::string _message("NAVScan::ScanBlessed returned: ");
                  _message.append(NAVScan::RCtoChar(scanRC));
                  Logger::Log(Logger::LogInfo, Logger::LogAvisFilter,
                              _message.c_str());

                  uint _latest_seq_num = 0;
                  NAVScan::LatestSeqNum(_latest_seq_num);
                  
                  if ( (!(NAVScan::NotInfectable	== scanRC ||
                          NAVScan::UnSubmittable	== scanRC ||
                          NAVScan::Repaired             == scanRC)
                        ) &&
                       ( _latest_seq_num > sigSeqNum )
                       )
                    {
                      Logger::Log(Logger::LogInfo, Logger::LogAvisFilter,
                                  "Filter scanning with blessed did not finish the sample, now trying latest instead");

                      sigSeqNum	= 0;
                      scanRC = NAVScan::ScanNewest(mappedName, attr.LocalCookie(),
                                                   true, virusName, sigSeqNum, navVersion,
                                                   virusID);
                      _message = "NAVScan::ScanNewest returned: ";
                      _message.append(NAVScan::RCtoChar(scanRC));
                      Logger::Log(Logger::LogInfo, Logger::LogAvisFilter,
                                  _message.c_str());
                    }

                  switch (scanRC)
                    {
                    case NAVScan::Infected:	// this should never happen, programming error!
                      throw FilterException(FilterException::Done, "scan returned Infected state (this state should never leave the scanner!)");

                    case NAVScan::NotInfectable:				// file cannot be infected
                      status	= Filter::Done_NotInfectable;
                      AddToResults(sampleCRC, sigSeqNum, string(XAnalysisState::UnInfected()));
                      break;

                    case NAVScan::NotInfected:
                      sampleReason = attr[AttributeKeys::SampleReason()];
                      if (string("manual")	== sampleReason ||
                          string("")			== sampleReason)
                        status = Filter::Done_NeedsHarderLook;
                      else
                        {
                          status        = Filter::Done_Misfired;
                          AddToResults(sampleCRC,sigSeqNum,string(XAnalysisState::Misfired()));
//                           status	= Filter::Done_NoDetect;	// false positive fixed
                        }
                      break;

                    case NAVScan::UnSubmittable:				// Found something, Trogan Horse?,
                      status = Filter::Done_UnSubmittable;	// it ends here.
                      AddToResults(sampleCRC, sigSeqNum, string(XAnalysisState::Infected()));
                      break;

                      //					case NAVScan::NAVrepairOverrun:
                      //						status = Filter::Done_NAVoverrun;
                      //						break;

                    case NAVScan::NoRepairInstructions:			// Found a virus, but repair
                      status = Filter::Done_NoRepair;			// failed
                      break;
                    case NAVScan::RepairTriedAndError:
                      status = Filter::Done_BadRepair;
                      break;
                    case NAVScan::RepairTriedAndFailed:
                      status = Filter::Done_UnRepairable;
                      break;

                    case NAVScan::Heuristic:
                      status = Filter::Done_FoundByHeuristic;
                      break;

                    case NAVScan::Repaired:
                      status = Filter::Done_Infected;
                      AddToResults(sampleCRC, sigSeqNum, string(XAnalysisState::Infected()));
                      break;

                    case NAVScan::BadScan:
                    case NAVScan::SigDefMissing:
                    case NAVScan::NavLoadDefFileError:
                    case NAVScan::NavVcdInitError:
                    case NAVScan::NavDefinitionFileError:
                    case NAVScan::NavCrashed:
                      status	= Filter::Done_BadScan;
                      break;

                    default:
                      status = Filter::ScanError;
                    }
                }
              else
                status = Filter::UnknownSampleType;
            }
          else
            status = Filter::BadSampleCheckSum;
        }
      else
        status = Filter::CRCsampleFailure;
    }

  catch (...)
    {
      ExceptionHandler(FilterException::Done, "Filter::Done");
    }

  if (Logger::WillLog(Logger::LogDebug, Logger::LogAvisFilter))
    {
      string	msg("At end of Filter::Done method filter status is set to");
      msg	+= StatusAsString();

      Logger::Log(Logger::LogDebug, Logger::LogAvisFilter, msg.c_str());
    }

  return status;
}


bool Filter::VirusName(string& vName)
{
  EntryExit	entryExit(Logger::LogAvisFilter, "FilterVirusName");

  bool	rc = false;

  if (Filter::Done_Infected == status)
    {
      vName	= virusName;
      rc		= true;
    }
  else
    vName	= "";

  return rc;
}

bool Filter::VirusID(uint& vID)
{
  EntryExit	entryExit(Logger::LogAvisFilter, "Filter::VirusID");

  bool	rc = false;

  if (Filter::Done_Infected == status)
    {
      vID		= virusID;
      rc		= true;
    }
  else
    vID		= 0;

  return rc;
}


Attributes Filter::CreateRequest(const string& attributes,
                                 FilterStatus& status)
{
  EntryExit	entryExit(Logger::LogAvisFilter, "Filter::CreateRequest");
  Attributes    attr(attributes);
  try
    {
      string		minusSpecial;		// attributes minus ones stored in request
      string		checkSumStr;		// checkSum from attributes
      uint		priority;			// priority from attributes
      string		submittorStr;		// submittorID from attributes
      DateTime	now;
      bool		piggyBacked;

      AnalysisStateInfo	state(string(XAnalysisState::Receiving()));

      status	= Filter::NeedMoreData;

      if (ParseOutValues(attributes, checkSumStr, priority, submittorStr, minusSpecial))
        {
          uint			localCookie;
          CheckSum		checkSum(checkSumStr);
          Submittor		submittor(submittorStr);

          if (submittor.IsNull())
            {
              if (Submittor::AddNew(submittorStr))
                submittor.Refresh();
              else
                {
                  status = Filter::BadSubmittorID;
                  return attr;
                }
            }

          if (AnalysisRequest::AddNew(checkSum, priority,
                                      state, submittor, piggyBacked,
                                      localCookie))
            {
              char _other_debug_string[1024];
              memset(_other_debug_string,0,1024);
              sprintf(_other_debug_string,"AnalysisRequest::AddNew returned with: state:[%s],cookie[%d],piggyBacked[%s]",
                      ((std::string)state).c_str(),
                      localCookie,
                      (piggyBacked ? "true" : "false"));
              
              Logger::Log(Logger::LogDebug,
                          Logger::LogAvisFilter,
                          _other_debug_string);
  
              // We have a new case here.  It is possible that the
              // AnalysisRequest::AddNew method may return a
              // localCookie of zero which will be valid and will
              // indicate that Final Status has been found for a
              // pre-existing sample with this checksum.
              
              if (localCookie)
                {
                  attr.LocalCookie(localCookie);
                  if (piggyBacked == true)
                    {
                      // Found checksum in AnalysisRequest table
                      // Populate AnalysisRequest object
                      AnalysisRequest	request(localCookie);
                      state = request.State();
                      // Populate Attributes Object
                      //                      attr	= Attributes(localCookie);
                      //Added on Mar/23/2000. We should not be doing the
                      //following 'minusSpecial' thing. That is the primary reason
                      //for all the mysterious GetCRCCalculator bug where the "md5"
                      //string used to be always "". This is because, the minusSpecial
                      //deletes all the attributes from the Attributes table first for the
                      //existing cookie and then it adds those attributes again. For 
                      //piggybacked samples, we don't have to delete and add the attributes
                      //again. We can just live with the attributes of the similar sample
                      //which arrived first. The following statement will cause errors
                      //while the other servlet thread attempts to send sample attributes 
                      //to the analysis center on a 'getSuspectSample' request at this time.
                      //attr	= (string) minusSpecial;
                      // Set Filter::Status to Done_Piggybacked
                      status = Filter::Done_Piggybacked;
                    }
                  else
                    {
                      // New sample.  
                      // Populate AnalysisRequest object
                      AnalysisRequest	request(localCookie);
                      state = request.State();
                      // Populate Attributes Object
                      //attr	= Attributes(localCookie);
                      //attr	= (string) minusSpecial;
                      // Set Filter::Status to NeedMoreData
                      status = Filter::NeedMoreData;
                    }
                }
              else
                {
                  if (piggyBacked == true)
                    {
                      // Exception case
                      // Throw an exception.
                      throw FilterException(FilterException::CreateRequest,
                                            "AnalysisRequest returned Cookie=0 and piggyBacked = true");
                    }
                  else
                    {
                      // Found checksum in AnalysisResults table
                      // Do NOT populate AnalysisRequest record
                      // Do NOT populate Attributes record
                      // Set status to "appropriate" Done_* code
                      if (state == string(XAnalysisState::Infected()))
                        {
                          status	= Filter::Done_Infected;
                        }
                      else if (state == string(XAnalysisState::UnInfected()))
                        {
                          status	= Filter::Done_UnInfected;
                        }
                      else if (state == string(XAnalysisState::Misfired()))
                        {
                          status = Filter::Done_Misfired;
                        }
                      else if (state == string(XAnalysisState::UnInfectable()))
                        {
                          status = Filter::Done_NotInfectable;
                        }
                      else if (state == string(XAnalysisState::UnSubmitable()))
                        {
                          status = Filter::Done_UnSubmittable;
                        }
                      else
                        {
                          // This is an exception condition.
                          throw FilterException(FilterException::CreateRequest,
                                                "Invalid final state from checksum match");
                        }
                    }
                }
            }
          else
            status = Filter::DatabaseError;
        }
      else
        status = Filter::MissingAttributes;

      return attr;
      // Which may very well be a "null object" (gasp)
    }

  catch (...)
    {
      ExceptionHandler(FilterException::CreateRequest, "Filter::CreateRequest");
    }

  return attr;
}

string	Filter::ContentName(Attributes& attr)
{
  EntryExit	entryExit(Logger::LogAvisFilter, "Filter::ContentName");
  string		name("Content");

  try
    {
      name.append(attr["X-Sample-Checksum"]);

      //       if (0 != attr.LocalCookie())
      //         {
      //           char buffer[1024];
      //           memset(buffer,0,1024);
      
      //           sprintf(buffer, "Content%d", attr.LocalCookie());
      //           name = buffer;
      //     }
    }
  catch (...)
    {
      ExceptionHandler(FilterException::ContentName, "Filter::ContentName");
    }
  
  return name;
}

string Filter::SampleName(Attributes& attr)
{
  EntryExit	entryExit(Logger::LogAvisFilter, "Filter::SampleName");
  
  string	name("Sample");
  
  try
    {
      name.append(attr["X-Sample-Checksum"]);
      name.append(".");
      name.append(attr[sampleFileExtensionTag]);
      //       if (0 != attr.LocalCookie())
      //         {
      //           char buffer[1024];
      //           memset(buffer,0,1024);
      //           string	extension(attr[sampleFileExtensionTag]);
      //           sprintf(buffer, "Sample%d.%s", attr.LocalCookie(), extension.c_str());
      //           name = buffer;
      //         }
    }
  catch (...)
    {
      ExceptionHandler(FilterException::SampleName, "Filter::SampleName");
    }

  return name;
}


void Filter::AddToResults(string& sampleCRC, uint sigSeqNum, string& state)
{
  EntryExit	entryExit(Logger::LogAvisFilter, "Filter::AddToResults");

  try
    {
      //		Signature	signature(sigSeqNum);
      //		if (signature.IsNull())
      //			throw FilterException(FilterException::AddToResults, "signature is null");

      CheckSum			checkSum(sampleCRC);
      AnalysisStateInfo	analysisState(state);
      AnalysisResults::AddNew(checkSum, sigSeqNum, analysisState);

      if (Done_Infected == status)
        {
          char	buffer[1024];
          memset(buffer,0,1024);
          sprintf(buffer, "%d", virusID);

          try
            {
              attr[AttributeKeys::ScanVirusID()]	= string(buffer);
              attr[AttributeKeys::ScanVirusName()]= virusName;
            }
          catch (...)
            {
              Logger::Log(Logger::LogWarning, Logger::LogAvisFilter,
                          "Exception thrown while trying to set scanVirusID and scanVirusName attributes in Filter::AddToResults");
            }
        }
    }

  catch (...)
    {
      ExceptionHandler(FilterException::AddToResults, "Filter::AddToResults");
    }
}

uint Filter::ContentSize(Attributes& attr)
{
  uint _rval = 0;
  if (attr.LocalCookie() != 0)
    {
      _rval = atoi(((std::string)
                    attr[AttributeKeys::ContentLength()]).c_str()); 
    }
  return _rval;
}

uint Filter::SampleSize(Attributes& attr)
{
  uint _rval = 0;
  if (attr.LocalCookie() != 0)
    {
      _rval =  atoi(((std::string)
                     attr[AttributeKeys::SampleFileSize()]).c_str()); 
    }
  return _rval;
}

CheckSum Filter::GetSampleCheckSum() const
{
  // Parse the thing out manually.
  // It is amazingly ridiculous that I have to do this.
  std::string _checksum_string;
  AttributesParser::ParseOneAttribute(headerBlock,"X-Sample-Checksum",_checksum_string);
  CheckSum _checksum(_checksum_string);
  return _checksum;
  
}

//This function was added on Mar/16/2000. If we receive a duplicate sample,
//the sample receiver client need not receive the sample contents at all.
//Because, we are now returning the cookie number of an earlier sample that
//matches the checksum of the current sample OR we will return a zero cookieid
//when there is a final result avaialble for the sample's checksum. In these
//cases, it is not necessary to feed the filter with contents. Because of that
//we have to explicitly set the "done" flag to true so that the filter destructor
//will not set the sample's analysis state to "lost". The filter member function
//can't be called because it does so many other integrity checks. Since we have not
//fed the filter, calling that function will throw exception. Hence the following
//simple function is required for handling the duplicate samples.
void Filter::SetDone(void)
{
  done = true;
}
