//////////////////////////////////////////////////////////////////////
// 
// This file contains three tables that can be used to map text 
// tokens used by the IcePack agent into display strings:
// 
//    The IcePackAttentionTable table contains tokens used in the
//    "attention" registry variable.
// 
//    The IcePackErrorTable table contains tokens used in the 
//    X-Error attribute of samples in quarantine.
//
//    The IcePackStateTable table contains tokens used in the
//    X-Analsysis-State attributes of samples in quarantine.
// 
// Each entry in each table has this format:
// 
//    L"token=national language message."
// 
// The "token" values and the "equals" characters are not displayed 
// by Quarantine Console.  They must not be changed when this file
// is translated into other national languages.
//
// The "national language message" values are displayed by Quarantine
// Console.  They should be changed when this file is translated
// into other national languages.
// 
//                             -- Edward Pring <pring@watson.ibm.com>
// 
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
// This include file contains all of the POSIX and Windows include 
// files required by this program.
//--------------------------------------------------------------------

#include "StdAfx.h"

//--------------------------------------------------------------------
// This include file declares the IcePack token tables defined below
// as exported variables for the IcePackTokens.DLL file we are
// building.
//--------------------------------------------------------------------

#define ICEPACKTOKENS_EXPORTS
#include "IcePackTokens.h"

//--------------------------------------------------------------------
// This table contains all of the tokens that may appear in the
// "attention" registry variable.
//--------------------------------------------------------------------

extern ICEPACKTOKENS_API const wchar_t* const IcePackAttentionTable[] = 
{
  L"defcast=defcast: IcePack cannot connect to the DefCast component.",
  L"disk=disk: IcePack cannot access the definition or temporary directory.",
  L"download=download: IcePack cannot connect to the gateway to download definitions.",
  L"quarantine=quarantine: IcePack cannot connect to the Quarantine Service.",
  L"query=query: IcePack cannot connect to the gateway to query sample status.",
  L"scanexplicit=scanexplicit: IcePack cannot connect to the ScanExplicit Service.",
  L"submit=submit: IcePack cannot connect to the gateway to submit samples.",
  L"target=target: IcePack cannot install definitions on one or more machines.",
  NULL   
};

extern ICEPACKTOKENS_API const wchar_t* const IcePackErrorTable[] = 
{
  L"abandoned=abandoned: This signature sequence number has been abandoned." ,
  L"content=content: This sample's content checksum does not match its content.",
  L"crumbled=crumbled: This sample's cookie has not been assigned by the gateway.",
  L"declined=declined:  This sample has been declined by the gateway.",
  L"internal=internal: An internal failure occurred while processing this sample.",
  L"lost=lost: This sample was not completely received due to a network failure.",
  L"malformed=malformed: An essential attribute of this sample was malformed.",
  L"missing=missing: An essential attribute of this sample was missing.",
  L"overrun=overrun: The content of this sample exceeds its length.",
  L"sample=sample: This sample's sample checksum does not match its content.",
  L"superceded=superceded: This signature sequence number has been superceded.",
  L"type=type: This sample's type is not supported.",
  L"unavailable=unavailable: This signature sequence number has not been published.",
  L"underrun=underrun: The length of this sample exceeds its content.",
  L"unpackage=unpackage: The sample or signature could not be unpacked.",
  L"unpublished=unpublished: The signature set could not be published.",
  NULL
};

extern ICEPACKTOKENS_API const wchar_t* const IcePackStateTable[] = 
{
  // These are tranport states:
  L"captured=captured: This sample has been captured and stored in quarantine.",
  L"submitting=submitting: This sample is being submitted to the gateway.",
  L"receiving=receiving: This sample is being received by the gateway.",
  L"accepted=accepted: This sample has been accepted by the gateway.",
  L"importing=importing: This sample is being imported into the analysis center.",
  L"imported=imported: This sample has been imported into the analysis center.",

  // These are dataflow states:
  L"classifying=classifying: This sample is being classified to determine its datatype.",
  L"rescan=rescan: This sample will be re-scanned with newer virus definition files.",

  // These are binary analysis states:
  L"binary=binary: This sample has been classified as a binary program.",
  L"binaryControlling=binaryControlling: This sample's replication environments are being set.",
  L"binaryWait=binaryWait: This sample is waiting for replication engines.",
  L"binaryReplicating=binaryReplicating: This sample is being executed by a replication engine.",
  L"binaryScoring=binaryScoring: Signatures for a new virus are being scored.",

  // These are macro analysis states:
  L"macro=macro: This sample has been classified as a binary program.",
  L"macroControlling=macroControlling: This sample's replication environments are being set.",
  L"macroWait=macroWait: This sample is waiting for replication engines.",
  L"macroReplicating=macroReplicating: This sample is being executed by a replication engine.",
  L"macroScoring=macroScoring: Signatures for a new virus are being scored.",

  // These are build states:
  L"signatures=signatures: Signatures for a new virus have been selected.",
  L"locking=locking: The definition generation service is being locked.",
  L"incrBuilding=incrBuilding:  Virus definition files are being built.",
  L"incrUnitTesting=incrUnitTesting: Virus definition files are being tested.",
  L"fullBuilding=fullBuilding: Virus definition files are being built.",
  L"fullUnitTesting=fullUnitTesting: Virus definition files are being tested.",
  L"unlocking=unlocking: The definition generation service is being unlocked.",

  // These are deferral states:
  L"defer=defer: This sample will be deferred to experts for manual analysis.",
  L"deferred=deferred: This sample has been deferred to experts for manual analysis.",
  L"deferring=deferring: This sample is being deferred to experts for manual analysis.",

  // These are archive states:
  L"archive=archive: This sample's automated analysis files will be archived.",
  L"archiving=archiving: This sample's automated analysis files are being archived.",

  // These are final states:
  L"attention=attention: User intervention is required.",
  L"empty=empty: This sample is empty.",
  L"error=error: An error occurred while processing the sample.",
  L"uninfectable=uninfectable: This sample contains no executable code.",
  L"unsubmitable=unsubmitable: This sample cannot be submitted for analysis.",
  L"nodetect=nodetect: No suspicious code was detected in this sample.",
  L"infected=infected: This sample is infected with a virus that can be repaired.",
  L"uninfected=uninfected: This sample is not infected with a virus.",
  L"misfired=misfired: This sample is not infected, but did cause a false positive.",

  NULL
};


extern ICEPACKTOKENS_API const wchar_t* const IcePackStatusTable[] =
{
  L"quarantined=quarantined: This sample has been received by Central Quarantine.",
  L"submitted=submitted: This sample has been submitted for analysis.",
  L"held=held: This sample is held for submission to the analysis center.",
  L"released=released: This sample will be submitted to the analysis center.",
  L"unneeded=unneeded: New definitions are not needed on this sample's originator.",
  L"needed=needed: New definitions are needed for this sample.",
  L"available=available: New definitions are held for delivery to this sample's originator.",
  L"distributed=distributed: New definitions have been delivered to this sample's originator.",
  L"installed=installed: New definitions have been installed on this sample's originator.",
  L"attention=attention: Contact Symantec technical support for assistance with this sample.",
  L"error=error: An error occurred while processing this sample.",
  L"notinstalled=notinstalled: Definitions could not be delivered to this sample's originator.",
  L"restart=restart: This sample will be restarted.",
//L"legacy=legacy: This sample must be submitted manually via Scan&Deliver.",
  L"distribute=distribute: New definitions will be delivered to this sample's originator",
  NULL
};


extern ICEPACKTOKENS_API const wchar_t* const IcePackResultTable[] =
{
  L"badrepair=badrepair: Repair engine failed while disinfecting this sample.",
  L"badscan=badscan: Scan engine failed while disinfecting this sample.",
  L"heuristic=heuristic=heuristic: This sample may be infected with an unknown virus.",
  L"nodetect=nodetect: No known viruses or suspicious code were found in this sample.",
  L"norepair=norepair: There are no repair instructions for the virus infecting this sample.",
  L"overrun=overrun: Repair engine outgrew buffer space for this sample.",
  L"repaired=repaired: This sample has been successfully disinfected.",
  L"underrun=underrun: Repair engine stepped outside buffer space for this sample.",
  L"unrepairable=unrepairable: This sample was not successfully disinfected.",
  L"uninfectable=uninfectable: This sample contains no code.",
  L"unsubmittable=unsubmittable: This sample may not be submitted for analysis.",
  NULL
};

