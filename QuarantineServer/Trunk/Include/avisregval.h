// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////////
//
// AVISREGVAL.H - contains registry keys and valus for the AVIS Config server 
//              configuration values.
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef _AVISREGVAL_H_
#define	_AVISREGVAL_H_

#pragma once

#include "qsregval.h"




// THESE ARE CREATED UNDER REGKEY_QSERVER _T( "Software\\Symantec\\Quarantine\\Server" )
// THEY ARE CONCATENATED ON TO THE REGKEY_QSERVER STRING
const TCHAR REGKEY_ASERVER_SUBKEY[]         = _T( "\\Avis" );
const TCHAR REGKEY_ICEPACK_CURRENT_SUBKEY[] = _T( "\\current" );


// CAvisServerConfig2    "Attention"
const TCHAR REGVALUE_GENERAL_ATTENTION_MESSAGE[]    = _T( "attention" );

// CAvisServerConfig2    "Web Communication" 
const TCHAR REGVALUE_STATUS_INTERVAL[]              = _T( "webStatusInterval" );
const TCHAR REGVALUE_GATEWAY_ADDRESS[]              = _T( "webGatewayName" );
const TCHAR REGVALUE_GATEWAY_ADDRESS_PORT[]         = _T( "webGatewayPort" );
const TCHAR REGVALUE_SSL_PORT[]                     = _T( "webGatewayPortSSL" );
const TCHAR REGVALUE_CHECK_SECURE_SUBMISSION[]      = _T( "webSecureSampleSubmission" );
const TCHAR REGVALUE_CHECK_SECURE_STATUS_QUERY[]    = _T( "webSecureStatusQuery" );
const TCHAR REGVALUE_CHECK_SECURE_DEF_DOWNLOAD[]    = _T( "webSecureSignatureDownload" );
const TCHAR REGVALUE_CHECK_SECURE_IGNORE_HOSTNAME[] = _T( "webSecureIgnoreHostname" );
const TCHAR REGVALUE_RETRY_INTERVAL[]               = _T( "webRetryInterval" );
const TCHAR REGVALUE_RETRY_LIMIT[]                  = _T( "webRetryLimit" );
const TCHAR REGVALUE_TRANSACTION_TIMEOUT[]          = _T( "webTimeoutInterval" );

// CAServerFirewallConfig2   "Firewall" 
const TCHAR REGVALUE_PROXY_FIREWALL[]               = _T( "webFirewallName" );
const TCHAR REGVALUE_PROXY_FIREWALL_PORT[]          = _T( "webFirewallPort" );
const TCHAR REGVALUE_FIREWALL_USERNAME[]            = _T( "webFirewallUser" );
const TCHAR REGVALUE_FIREWALL_PASSWORD[]            = _T( "webFirewallPassword" );
                                                    
// CAvisServerConfig         "Sample Policy"        
const TCHAR REGVALUE_STRIP_USER_DATA[]              = _T( "sampleStripContent" );  
const TCHAR REGVALUE_QUEUECHECK_INTERVAL[]          = _T( "sampleQuarantineInterval" );  
const TCHAR REGVALUE_MAX_PENDINGSAMPLES[]           = _T( "sampleMaximumPending" );  
const TCHAR REGVALUE_INITIAL_SUBMISSIONPRIORITY[]   = _T( "sampleSubmissionPriority" );  
const TCHAR REGVALUE_SCRAMBLE_USER_DATA[]           = _T( "sampleScrambleContent" );  
const TCHAR REGVALUE_COMPRESS_USER_DATA[]           = _T( "sampleCompressContent" );  

//  CAvisServerDefPolicyConfig    "Definition Policy"
const TCHAR REGVALUE_DEFLIBRARY_FOLDER[]            = _T( "definitionLibraryDirectory" );
const TCHAR REGVALUE_DEFS_HEURISTIC_LEVEL[]         = _T( "definitionHeuristicLevel" );
const TCHAR REGVALUE_DEFS_UNPACK_TIMEOUT[]          = _T( "definitionUnpackTimeout" );
const TCHAR REGVALUE_DEFS_BLESSED_INTERVAL[]        = _T( "webBlessedInterval" );
const TCHAR REGVALUE_NEEDED_INTERVAL[]              = _T( "webNeededInterval" );
const TCHAR REGVALUE_DEFS_PRUNE[]                   = _T( "definitionPrune" );
const TCHAR REGVALUE_DEFS_ACTIVE_SEQUENCE_NUM[]     = _T( "definitionActiveSequence" );
const TCHAR REGVALUE_DEFS_ACTIVE_BLESSED[]          = _T( "definitionActiveBlessed" );
const TCHAR REGVALUE_DEFS_BLESSED_SEQUENCE_NUM[]    = _T( "definitionBlessedSequence" );


// Definiton Install Page
//const TCHAR REGVALUE_DEFINITIONS_CHECK_INTERVAL[] = _T( "definitionCheckInterval" );
//const TCHAR REGVALUE_AUTODELIVER_DEFINITIONS[]    = _T( "definitionDeliveryPriority" );  // AutoDeliverDefinitions
const TCHAR REGVALUE_DEF_BLESSED_BROADCAST[]        = _T( "definitionBlessedBroadcast" );
const TCHAR REGVALUE_DEF_UNBLESSED_BROADCAST[]      = _T( "definitionUnblessedBroadcast" );
const TCHAR REGVALUE_DEF_UNBLESSED_NARROWCAST[]     = _T( "definitionUnblessedNarrowCast" );
const TCHAR REGVALUE_DEF_UNBLESSED_POINTCAST[]      = _T( "definitionUnblessedPointCast" );
const TCHAR REGVALUE_DEF_DELIVERY_TIMEOUT[]         = _T( "definitionDeliveryTimeout" );
const TCHAR REGVALUE_DEF_DELIVERY_PRIORITY[]        = _T( "definitionDeliveryPriority" );
const TCHAR REGVALUE_DEF_DELIVERY_INTERVAL[]        = _T( "definitionDeliveryInterval" );
const TCHAR REGVALUE_DEF_BLESSED_TARGETS[]          = _T( "definitionBlessedTargets" );
const TCHAR REGVALUE_DEF_UNBLESSED_TARGETS[]        = _T( "definitionUnblessedTargets" );
const TCHAR REGVALUE_DEF_SECURE_USERNAME[]          = _T( "definitionSecureUsername" );
const TCHAR REGVALUE_DEF_SECURE_PASSWORD[]          = _T( "definitionSecurePassword" );



//const TCHAR REGVALUE_[]          = _T( "" );


// AVIS CUSTOMER INFO
const TCHAR REGVALUE_COMPANY_NAME[]               = _T( "customerName" );
const TCHAR REGVALUE_CONTACT_NAME[]               = _T( "customerContactName" );
const TCHAR REGVALUE_CONTACT_PHONE[]              = _T( "customerContactTelephone" );
const TCHAR REGVALUE_CONTACT_EMAIL[]              = _T( "customerContactEmail" );
const TCHAR REGVALUE_CUSTOMER_ACCOUNT[]           = _T( "customerIdentifier" );
 


// CONSOLE ONLY
const TCHAR REGVALUE_CONFIG_CHANGE_COUNTER[]      = _T( "configurationChangeCounter" );
const TCHAR REGVALUE_LAST_CONSOLE_ACCESS[]        = _T( "consoleLastUpdate" );

#endif