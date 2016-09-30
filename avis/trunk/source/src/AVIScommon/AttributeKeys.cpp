// AttributeKeys.cpp: implementation of the AttributeKeys class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AttributeKeys.h"


using namespace std;

const string AttributeKeys::connection                = "Connection";
const string AttributeKeys::contentLength             = "Content-Length";
const string AttributeKeys::contentType               = "Content-Type";

const string AttributeKeys::date                      = "Date";
const string AttributeKeys::pragma                    = "Pragma";
const string AttributeKeys::server                    = "Server";
const string AttributeKeys::userAgent                 = "User-Agent";

const string AttributeKeys::alertStatusTimer          = "X-Alert-Status-Timer";
const string AttributeKeys::analysisCookie            = "X-Analysis-Cookie";
const string AttributeKeys::analysisGateway           = "X-Analysis-Gateway";
const string AttributeKeys::analysisIssue             = "X-Analysis-Issue";
const string AttributeKeys::analysisService           = "X-Analysis-Service";
const string AttributeKeys::analysisState             = "X-Analysis-State";
const string AttributeKeys::analysisVirusID           = "X-Analysis-Virus-Identifier";
const string AttributeKeys::analysisVirusName         = "X-Analysis-Virus-Name";

const string AttributeKeys::attention                 = "X-Attention";
const string AttributeKeys::attentionShort            = "X-Attention-Short";

const string AttributeKeys::checkSumMethod            = "X-Checksum-Method";

const string AttributeKeys::contentCheckSum           = "X-Content-Checksum";
const string AttributeKeys::contentCompression        = "X-Content-Compression";
const string AttributeKeys::contentEncoding           = "X-Content-Encoding";
const string AttributeKeys::contentConcealment        = "X-Content-Scrambling";
const string AttributeKeys::contentStrip              = "X-Content-Strip";

const string AttributeKeys::customerContactName       = "X-Customer-Contact-Name";
const string AttributeKeys::customerContactTelephone  = "X-Customer-Contact-Telephone";
const string AttributeKeys::customerContactEmail      = "X-Customer-Contact-Email";
const string AttributeKeys::customerCredentials       = "X-Customer-Credentials";
const string AttributeKeys::customerID                = "X-Customer-Identifier";
const string AttributeKeys::customerName              = "X-Customer-Name";

const string AttributeKeys::dateAccessed              = "X-Date-Accessed";
const string AttributeKeys::dateAnalyzed              = "X-Date-Analyzed";
const string AttributeKeys::dateBlessed               = "X-Date-Blessed";
const string AttributeKeys::dateCompleted             = "X-Date-Completed";
const string AttributeKeys::dateCreated               = "X-Date-Created";
const string AttributeKeys::dateDelivered             = "X-Date-Delivered";
const string AttributeKeys::dateDistributed           = "X-Date-Distributed";
const string AttributeKeys::dateForwarded             = "X-Date-Forwarded";
const string AttributeKeys::dateInstalled             = "X-Date-Installed";
const string AttributeKeys::dateModified              = "X-Date-Modified";
const string AttributeKeys::dateProduced              = "X-Date-Produced";
const string AttributeKeys::dateQuarantined           = "X-Date-Quarantined";
const string AttributeKeys::dateSampleStatus          = "X-Date-Sample-Status";
const string AttributeKeys::dateStatus                = "X-Date-Status";
const string AttributeKeys::dateSubmitted             = "X-Date-Submitted";

const string AttributeKeys::error                     = "X-Error";
const string AttributeKeys::errorImport               = "X-Error-Import";

const string AttributeKeys::platformAddress           = "X-Platform-Address";
const string AttributeKeys::platformComputer          = "X-Platform-Computer";
const string AttributeKeys::platformCorrelator        = "X-Platform-Correlator";
const string AttributeKeys::platformDeliver           = "X-Platform-Deliver";
const string AttributeKeys::platformDistributor       = "X-Platform-Distributor";
const string AttributeKeys::platformDomain            = "X-Platform-Domain";
const string AttributeKeys::platformGUID              = "X-Platform-GUID";
const string AttributeKeys::platformHost              = "X-Platform-Host";
const string AttributeKeys::platformLangauge          = "X-Platform-Langauge";
const string AttributeKeys::platformOwner             = "X-Platform-Owner";
const string AttributeKeys::platformProcessor         = "X-Platform-Processor";
const string AttributeKeys::platformScanner           = "X-Platform-Scanner";
const string AttributeKeys::platformSupport           = "X-Platform-Support";
const string AttributeKeys::platformSystem            = "X-Platform-System";
const string AttributeKeys::platformUser              = "X-Platform-User";

const string AttributeKeys::sampleCategory            = "X-Sample-Category";
const string AttributeKeys::sampleChanges             = "X-Sample-Changes";
const string AttributeKeys::sampleCheckSum            = "X-Sample-Checksum";
const string AttributeKeys::sampleChecksumSubmitted   = "X-Sample-Checksum-Submitted";
const string AttributeKeys::sampleExtension           = "X-Sample-Extension";
const string AttributeKeys::sampleFileSize            = "X-Sample-Size";
const string AttributeKeys::sampleFile                = "X-Sample-File";
const string AttributeKeys::sampleFileID              = "X-Sample-FileID";
const string AttributeKeys::sampleGeometry            = "X-Sample-Geometry";
const string AttributeKeys::samplePriority            = "X-Sample-Priority";
const string AttributeKeys::sampleReason              = "X-Sample-Reason";
const string AttributeKeys::sampleSector              = "X-Sample-Sector";
const string AttributeKeys::sampleStatus              = "X-Sample-Status";
const string AttributeKeys::sampleStatusReadable      = "X-Sample-Status-Readable";
const string AttributeKeys::sampleStrip               = "X-Sample-Strip";
const string AttributeKeys::sampleSubmissionRoute     = "X-Sample-Submission-Route";
const string AttributeKeys::sampleType                = "X-Sample-Type";
const string AttributeKeys::sampleUUID                = "X-Sample-UUID";

const string AttributeKeys::scanResult                = "X-Scan-Result";
const string AttributeKeys::scanSignaturesName        = "X-Scan-Signatures-Name";
const string AttributeKeys::scanSignaturesSequence    = "X-Scan-Signatures-Sequence";
const string AttributeKeys::scanSignaturesVersion     = "X-Scan-Signatures-Version";
const string AttributeKeys::scanVirusID               = "X-Scan-Virus-Identifier";
const string AttributeKeys::scanVirusName             = "X-Scan-Virus-Name";

/*
 * These attributes are used by IcePack.  They are initialised when
 * IcePack first sees a sample, and are set to the values of the
 * corresponding X-Scan... attributes.  This is in order to have
 * this information available both in IcePack, and as the sample
 * goes up the chain.
 */
const string AttributeKeys::clientScanResult          = "X-Client-Scan-Result";
const string AttributeKeys::clientScanSignaturesName  = "X-Client-Scan-Signatures-Name";
const string AttributeKeys::clientScanSignaturesSequence = "X-Client-Scan-Signatures-Sequence";
const string AttributeKeys::clientScanSignaturesVersion  = "X-Client-Scan-Signatures-Version";
const string AttributeKeys::clientScanVirusID         = "X-Client-Scan-Virus-Identifier";
const string AttributeKeys::clientScanVirusName       = "X-Client-Scan-Virus-Name";

const string AttributeKeys::signaturesName            = "X-Signatures-Name";
const string AttributeKeys::signaturesPriority        = "X-Signatures-Priority";
const string AttributeKeys::signaturesSequence        = "X-Signatures-Sequence";
const string AttributeKeys::signaturesSequenceMinimum = "X-Signatures-Sequence-Minimum";
const string AttributeKeys::signaturesVersion         = "X-Signatures-Version";

const string AttributeKeys::submissionCount           = "X-Submission-Count";

const string AttributeKeys::icePackInitialized        = "X-IcePack-Initialized";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AttributeKeys::AttributeKeys()
{
}

AttributeKeys::~AttributeKeys()
{
}
