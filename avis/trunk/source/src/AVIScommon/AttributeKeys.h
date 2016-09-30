// AttributeKeys.h: interface for the AttributeKeys class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATTRIBUTEKEYS_H__89B804DB_C745_11D2_AD5B_00A0C9C71BBC__INCLUDED_)
#define AFX_ATTRIBUTEKEYS_H__89B804DB_C745_11D2_AD5B_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "AVISCommon.h"

class AVISCOMMON_API AttributeKeys  
{
private:
  AttributeKeys();
  virtual ~AttributeKeys();

public:
  static std::string Connetion(void)                { return connection; }
  static std::string ContentLength(void)            { return contentLength; }
  static std::string ContentType(void)              { return contentType; }
  static std::string Date(void)                     { return date; }
  static std::string Pragma(void)                   { return pragma; }
  static std::string Server(void)                   { return server; }
  static std::string UserAgent(void)                { return userAgent; }

  static std::string AlertStatusTimer(void)         { return alertStatusTimer; }
  static std::string AnalysisCookie(void)           { return analysisCookie; }
  static std::string AnalysisGateway(void)          { return analysisGateway; }
  static std::string AnalysisIssue(void)            { return analysisIssue; }
  static std::string AnalysisService(void)          { return analysisService; }
  static std::string AnalysisState(void)            { return analysisState; }
  static std::string AnalysisVirusID(void)          { return analysisVirusID; }
  static std::string AnalysisVirusName(void)        { return analysisVirusName; }

  static std::string Attention(void)                { return attention; }
  static std::string AttentionShort(void)           { return attentionShort; }

  static std::string CheckSumMethod(void)           { return checkSumMethod; }

  static std::string ContentCheckSum(void)          { return contentCheckSum; }
  static std::string ContentCompression(void)       { return contentCompression; }
  static std::string ContentConcealment(void)       { return contentConcealment; }
  static std::string ContentEncoding(void)          { return contentEncoding; }
  static std::string ContentStrip(void)             { return contentStrip; }

  static std::string CustomerContactName(void)      { return customerContactName; }
  static std::string CustomerContactTelephone(void) { return customerContactTelephone; }
  static std::string CustomerContactEmail(void)     { return customerContactEmail; }
  static std::string CustomerCredentials(void)      { return customerCredentials; }
  static std::string CustomerID(void)               { return customerID; }
  static std::string CustomerName(void)             { return customerName; }

  static std::string DateAccessed(void)             { return dateAccessed; }
  static std::string DateAnalyzed(void)             { return dateAnalyzed; }
  static std::string DateBlessed(void)              { return dateBlessed; }
  static std::string DateCompleted(void)            { return dateCompleted; }
  static std::string DateCreated(void)              { return dateCreated; }
  static std::string DateDelivered(void)            { return dateDelivered; }
  static std::string DateDistributed(void)          { return dateDistributed; }
  static std::string DateForwarded(void)            { return dateForwarded; }
  static std::string DateInstalled(void)            { return dateInstalled; }
  static std::string DateModified(void)             { return dateModified; }
  static std::string DateProduced(void)             { return dateProduced; }
  static std::string DateQuarantined(void)          { return dateQuarantined; }
  static std::string DateSampleStatus(void)         { return dateSampleStatus; }
  static std::string DateStatus(void)               { return dateStatus; }
  static std::string DateSubmitted(void)            { return dateSubmitted; }

  static std::string Error(void)                    { return error; }
  static std::string ErrorImport(void)              { return errorImport; }

  static std::string PlatformAddress(void)          { return platformAddress; }
  static std::string PlatformDeliver(void)          { return platformDeliver; }
  static std::string PlatformCorrelator(void)       { return platformCorrelator; }
  static std::string PlatformComputer(void)         { return platformComputer; }
  static std::string PlatformDistributor(void)      { return platformDistributor; }
  static std::string PlatformDomain(void)           { return platformDomain; }
  static std::string PlatformGUID(void)             { return platformGUID; }
  static std::string PlatformHost(void)             { return platformHost; }
  static std::string PlatformLangauge(void)         { return platformLangauge; }
  static std::string PlatformOwner(void)            { return platformOwner; }
  static std::string PlatformProcessor(void)        { return platformProcessor; }
  static std::string PlatformScanner(void)          { return platformScanner; }
  static std::string PlatformSupport(void)          { return platformSupport; }
  static std::string PlatformSystem(void)           { return platformSystem; }
  static std::string PlatformUser(void)             { return platformUser; }

  static std::string SampleCategory(void)           { return sampleCategory; }
  static std::string SampleChanges(void)            { return sampleChanges; }
  static std::string SampleCheckSum(void)           { return sampleCheckSum; }
  static std::string SampleChecksumSubmitted(void)  { return sampleChecksumSubmitted; }
  static std::string SampleExtension(void)          { return sampleExtension; }
  static std::string SampleFileSize(void)           { return sampleFileSize; }
  static std::string SampleFile(void)               { return sampleFile; }
  static std::string SampleFileID(void)             { return sampleFileID; }
  static std::string SampleGeometry(void)           { return sampleGeometry; }
  static std::string SamplePriority(void)           { return samplePriority; }
  static std::string SampleReason(void)             { return sampleReason; }
  static std::string SampleSector(void)             { return sampleSector; }
  static std::string SampleStatus(void)             { return sampleStatus; }
  static std::string SampleStatusReadable(void)     { return sampleStatusReadable; }
  static std::string SampleStrip(void)              { return sampleStrip; }
  static std::string SampleSubmissionRoute(void)    { return sampleSubmissionRoute; }
  static std::string SampleType(void)               { return sampleType; }
  static std::string SampleUUID(void)               { return sampleUUID; }

  static std::string ScanResult(void)               { return scanResult; }
  static std::string ScanSignaturesName(void)       { return scanSignaturesName; }
  static std::string ScanSignaturesSequence(void)   { return scanSignaturesSequence; }
  static std::string ScanSignaturesVersion(void)    { return scanSignaturesVersion; }
  static std::string ScanVirusID(void)              { return scanVirusID; }
  static std::string ScanVirusName(void)            { return scanVirusName; }

  static std::string ClientScanResult(void)         { return clientScanResult; }
  static std::string ClientScanSignaturesName(void) { return clientScanSignaturesName; }
  static std::string ClientScanSignaturesSequence(void) { return clientScanSignaturesSequence; }
  static std::string ClientScanSignaturesVersion(void)  { return clientScanSignaturesVersion; }
  static std::string ClientScanVirusID(void)        { return clientScanVirusID; }
  static std::string ClientScanVirusName(void)      { return clientScanVirusName; }

  static std::string SignatureName(void)            { return signaturesName; }
  static std::string SignaturePriority(void)        { return signaturesPriority; }
  static std::string SignatureSequence(void)        { return signaturesSequence; }
  static std::string SignatureSequenceMinimum(void) { return signaturesSequenceMinimum; }
  static std::string SignaturesVersion(void)        { return signaturesVersion; }

  static std::string SubmissionCount(void)          { return submissionCount; }

  static std::string IcePackInitialized(void)       { return icePackInitialized; }

private:
  static const std::string connection;
  static const std::string contentLength;
  static const std::string contentType;
  static const std::string date;
  static const std::string pragma;
  static const std::string server;
  static const std::string userAgent;

  static const std::string alertStatusTimer;
  static const std::string analysisCookie;
  static const std::string analysisGateway;
  static const std::string analysisIssue;
  static const std::string analysisService;
  static const std::string analysisState;
  static const std::string analysisVirusID;
  static const std::string analysisVirusName;

  static const std::string attention;
  static const std::string attentionShort;

  static const std::string checkSumMethod;

  static const std::string contentCheckSum;
  static const std::string contentCompression;
  static const std::string contentConcealment;
  static const std::string contentEncoding;
  static const std::string contentStrip;

  static const std::string customerContactName;
  static const std::string customerContactTelephone;
  static const std::string customerContactEmail;
  static const std::string customerCredentials;
  static const std::string customerID;
  static const std::string customerName;

  static const std::string dateAccessed;
  static const std::string dateAnalyzed;
  static const std::string dateBlessed;
  static const std::string dateCompleted;
  static const std::string dateCreated;
  static const std::string dateDelivered;
  static const std::string dateDistributed;
  static const std::string dateForwarded;
  static const std::string dateInstalled;
  static const std::string dateModified;
  static const std::string dateProduced;
  static const std::string dateQuarantined;
  static const std::string dateSampleStatus;
  static const std::string dateStatus;
  static const std::string dateSubmitted;

  static const std::string error;
  static const std::string errorImport;

  static const std::string platformAddress;
  static const std::string platformComputer;
  static const std::string platformCorrelator;
  static const std::string platformDeliver;
  static const std::string platformDistributor;
  static const std::string platformDomain;
  static const std::string platformGUID;
  static const std::string platformHost;
  static const std::string platformLangauge;
  static const std::string platformOwner;
  static const std::string platformProcessor;
  static const std::string platformScanner;
  static const std::string platformSupport;
  static const std::string platformSystem;
  static const std::string platformUser;

  static const std::string sampleCategory;
  static const std::string sampleChanges;
  static const std::string sampleCheckSum;
  static const std::string sampleChecksumSubmitted;
  static const std::string sampleExtension;
  static const std::string sampleFileSize;
  static const std::string sampleFile;
  static const std::string sampleFileID;
  static const std::string sampleGeometry;
  static const std::string samplePriority;
  static const std::string sampleReason;
  static const std::string sampleSector;
  static const std::string sampleStatus;
  static const std::string sampleStatusReadable;
  static const std::string sampleStrip;
  static const std::string sampleSubmissionRoute;
  static const std::string sampleType;
  static const std::string sampleUUID;

  static const std::string scanResult;
  static const std::string scanSignaturesName;
  static const std::string scanSignaturesSequence;
  static const std::string scanSignaturesVersion;
  static const std::string scanVirusID;
  static const std::string scanVirusName;

  static const std::string clientScanResult;
  static const std::string clientScanSignaturesName;
  static const std::string clientScanSignaturesSequence;
  static const std::string clientScanSignaturesVersion;
  static const std::string clientScanVirusID;
  static const std::string clientScanVirusName;

  static const std::string signaturesName;
  static const std::string signaturesPriority;
  static const std::string signaturesSequence;
  static const std::string signaturesSequenceMinimum;
  static const std::string signaturesVersion;

  static const std::string submissionCount;

  static const std::string icePackInitialized;
};

#endif // !defined(AFX_ATTRIBUTEKEYS_H__89B804DB_C745_11D2_AD5B_00A0C9C71BBC__INCLUDED_)
