/*
 * RegistryDefaults.h
 *
 * ----- COPYRIGHT STATEMENT -----
 *
 * Licensed Materials - Property of IBM
 *
 * (c) Copyright IBM Corp. 20000
 *
 * U.S. Government Users Restricted Rights - use,
 * duplication or disclosure restricted by GSA ADP
 * Schedule Contract with IBM Corp.
 *
 * ----- COPYRIGHT STATEMENT -----
 *
 * Contains the default values for the registry variables
 * used by the IcePack and scanExplicit services.  When
 * the value is numeric, this file should also contain
 * the maximum and minimum values.
 *
 * [inw, 2000-02-02]
 * [ejp, 2000-02-08]
 * [inw, 2000-02-08]
 */

/* ----- */

#ifndef __REGISTRYDEFAULTS_DOT_H__
#define __REGISTRYDEFAULTS_DOT_H__

/* webGatewayName */
#define ICEP_DEF_WEBGATEWAYNAME                    "gateways.dis.symantec.com"

/* webGatewayPort */
#define ICEP_DEF_WEBGATEWAYPORT                    "2847"
#define ICEP_MIN_WEBGATEWAYPORT                    "1"
#define ICEP_MAX_WEBGATEWAYPORT                    "65535"

/* webGatewayPortSSL */
#define ICEP_DEF_WEBGATEWAYPORTSSL                 "2848"
#define ICEP_MIN_WEBGATEWAYPORTSSL                 "1"
#define ICEP_MAX_WEBGATEWAYPORTSSL                 "65535"

/* webFirewallName */
#define ICEP_DEF_WEBFIREWALLNAME                   ""

/* webFirewallPort */
#define ICEP_DEF_WEBFIREWALLPORT                   "80"
#define ICEP_MIN_WEBFIREWALLPORT                   "1"
#define ICEP_MAX_WEBFIREWALLPORT                   "65535"

/* webFirewallUser */
#define ICEP_DEF_WEBFIREWALLUSER                   ""

/* webFirewallPassword */
#define ICEP_DEF_WEBFIREWALLPASSWORD               ""

/* webSecureSampleSubmission */
#define ICEP_DEF_WEBSECURESAMPLESUBMISSION         "1"
#define ICEP_MIN_WEBSECURESAMPLESUBMISSION         "0"
#define ICEP_MAX_WEBSECURESAMPLESUBMISSION         "1"

/* webSecureStatusQuery */
#define ICEP_DEF_WEBSECURESTATUSQUERY              "0"
#define ICEP_MIN_WEBSECURESTATUSQUERY              "0"
#define ICEP_MAX_WEBSECURESTATUSQUERY              "1"

/* webSecureSignatureDownload */
#define ICEP_DEF_WEBSECURESIGNATUREDOWNLOAD        "1"
#define ICEP_MIN_WEBSECURESIGNATUREDOWNLOAD        "0"
#define ICEP_MAX_WEBSECURESIGNATUREDOWNLOAD        "1"

/* webSecureIgnoreHostname */
#define ICEP_DEF_WEBSECUREIGNOREHOSTNAME           "0"
#define ICEP_MIN_WEBSECUREIGNOREHOSTNAME           "0"
#define ICEP_MAX_WEBSECUREIGNOREHOSTNAME           "1"

/* webStatusInterval */
#define ICEP_DEF_WEBSTATUSINTERVAL                 "60"
#define ICEP_MIN_WEBSTATUSINTERVAL                 "60"
#define ICEP_MAX_WEBSTATUSINTERVAL                 "240"

/* webBlessedInterval */
#define ICEP_DEF_WEBBLESSEDINTERVAL                "1000"
#define ICEP_MIN_WEBBLESSEDINTERVAL                "1"
#define ICEP_MAX_WEBBLESSEDINTERVAL                "1440"

/* webNeededInterval */
#define ICEP_DEF_WEBNEEDEDINTERVAL                 "15"
#define ICEP_MIN_WEBNEEDEDINTERVAL                 "1"
#define ICEP_MAX_WEBNEEDEDINTERVAL                 "1440"

/* webTimeoutInterval */
#define ICEP_DEF_WEBTIMEOUTINTERVAL                "15"
#define ICEP_MIN_WEBTIMEOUTINTERVAL                "1"
#define ICEP_MAX_WEBTIMEOUTINTERVAL                "1440"

/* webRetryInterval */
#define ICEP_DEF_WEBRETRYINTERVAL                  "15"
#define ICEP_MIN_WEBRETRYINTERVAL                  "1"
#define ICEP_MAX_WEBRETRYINTERVAL                  "1440"

/* webRetryLimit */
#define ICEP_DEF_WEBRETRYLIMIT                     "2"
#define ICEP_MIN_WEBRETRYLIMIT                     "0"
#define ICEP_MAX_WEBRETRYLIMIT                     "100"

/* customerName */
#define ICEP_DEF_CUSTOMERNAME                      ""

/* customerContactName */
#define ICEP_DEF_CUSTOMERCONTACTNAME               ""

/* customerContactTelephone */
#define ICEP_DEF_CUSTOMERCONTACTTELEPHONE          ""

/* customerContactEmail */
#define ICEP_DEF_CUSTOMERCONTACTEMAIL              ""

/* customerIdentifier */
#define ICEP_DEF_CUSTOMERIDENTIFIER                ""

/* sampleQuarantineInterval */
#define ICEP_DEF_SAMPLEQUARANTINEINTERVAL          "1"
#define ICEP_MIN_SAMPLEQUARANTINEINTERVAL          "1"
#define ICEP_MAX_SAMPLEQUARANTINEINTERVAL          "1440"

/* sampleSubmissionPriority */
#define ICEP_DEF_SAMPLESUBMISSIONPRIORITY          "500"
#define ICEP_MIN_SAMPLESUBMISSIONPRIORITY          "0"
#define ICEP_MAX_SAMPLESUBMISSIONPRIORITY          "1000"

/* sampleMaximumPending */
#define ICEP_DEF_SAMPLEMAXIMUMPENDING              "2"
#define ICEP_MIN_SAMPLEMAXIMUMPENDING              "1"
#define ICEP_MAX_SAMPLEMAXIMUMPENDING              "100"

/* sampleStripContent */
#define ICEP_DEF_SAMPLESTRIPCONTENT                "1"
#define ICEP_MIN_SAMPLESTRIPCONTENT                "0"
#define ICEP_MAX_SAMPLESTRIPCONTENT                "1"

/* sampleCompressContent */
#define ICEP_DEF_SAMPLECOMPRESSCONTENT             "1"
#define ICEP_MIN_SAMPLECOMPRESSCONTENT             "0"
#define ICEP_MAX_SAMPLECOMPRESSCONTENT             "1"

/* sampleScrambleContent */
#define ICEP_DEF_SAMPLESCRAMBLECONTENT             "1"
#define ICEP_MIN_SAMPLESCRAMBLECONTENT             "0"
#define ICEP_MAX_SAMPLESCRAMBLECONTENT             "1"

/* definitionBlessedBroadcast */
#define ICEP_DEF_DEFINITIONBLESSEDBROADCAST        "0"
#define ICEP_MIN_DEFINITIONBLESSEDBROADCAST        "0"
#define ICEP_MAX_DEFINITIONBLESSEDBROADCAST        "1"

/* definitionBlessedTargets */
#define ICEP_DEF_DEFINITIONBLESSEDTARGETS          ""

/* definitionDeliveryInterval */
#define ICEP_DEF_DEFINITIONDELIVERYINTERVAL        "15"
#define ICEP_MIN_DEFINITIONDELIVERYINTERVAL        "1"
#define ICEP_MAX_DEFINITIONDELIVERYINTERVAL        "60"

/* definitionDeliveryPriority */
#define ICEP_DEF_DEFINITIONDELIVERYPRIORITY        "500"
#define ICEP_MIN_DEFINITIONDELIVERYPRIORITY        "0"
#define ICEP_MAX_DEFINITIONDELIVERYPRIORITY        "1000"

/* definitionDeliveryTimeout */
#define ICEP_DEF_DEFINITIONDELIVERYTIMEOUT         "15"
#define ICEP_MIN_DEFINITIONDELIVERYTIMEOUT         "1"
#define ICEP_MAX_DEFINITIONDELIVERYTIMEOUT         "1440"

/* definitionLibraryDirectory */
#define ICEP_DEF_DEFINITIONLIBRARYDIRECTORY        "C:\\Program Files\\Symantec\\Quarantine\\Server\\signatures"

/* definitionPrune */
#define ICEP_DEF_DEFINITIONPRUNE                   "1"
#define ICEP_MIN_DEFINITIONPRUNE                   "0"
#define ICEP_MAX_DEFINITIONPRUNE                   "1"

/* definitionUnblessedBroadcast */
#define ICEP_DEF_DEFINITIONUNBLESSEDBROADCAST      "0"
#define ICEP_MIN_DEFINITIONUNBLESSEDBROADCAST      "0"
#define ICEP_MAX_DEFINITIONUNBLESSEDBROADCAST      "1"

/* definitionUnblessedNarrowcast */
#define ICEP_DEF_DEFINITIONUNBLESSEDNARROWCAST     "0"
#define ICEP_MIN_DEFINITIONUNBLESSEDNARROWCAST     "0"
#define ICEP_MAX_DEFINITIONUNBLESSEDNARROWCAST     "1"

/* definitionUnblessedPointcast */
#define ICEP_DEF_DEFINITIONUNBLESSEDPOINTCAST      "1"
#define ICEP_MIN_DEFINITIONUNBLESSEDPOINTCAST      "0"
#define ICEP_MAX_DEFINITIONUNBLESSEDPOINTCAST      "1"

/* definitionUnblessedTargets */
#define ICEP_DEF_DEFINITIONUNBLESSEDTARGETS        ""

/* definitionUnpackTimeout */
#define ICEP_DEF_DEFINITIONUNPACKTIMEOUT           "5"
#define ICEP_MIN_DEFINITIONUNPACKTIMEOUT           "1"
#define ICEP_MAX_DEFINITIONUNPACKTIMEOUT           "1440"

/* definitionHeuristicLevel */
#define ICEP_DEF_DEFINITIONHEURISTICLEVEL          "3"
#define ICEP_MIN_DEFINITIONHEURISTICLEVEL          "0"
#define ICEP_MAX_DEFINITIONHEURISTICLEVEL          "3"

/* definitionChangeTimeout */
#define ICEP_DEF_DEFINITIONCHANGETIMEOUT           "2"
#define ICEP_MIN_DEFINITIONCHANGETIMEOUT           "1"
#define ICEP_MAX_DEFINITIONCHANGETIMEOUT           "5"

/* definitionConcurrencyMinimum */
#define ICEP_DEF_DEFINITIONCONCURRENCYMINIMUM      "2"
#define ICEP_MIN_DEFINITIONCONCURRENCYMINIMUM      "2"
#define ICEP_MAX_DEFINITIONCONCURRENCYMINIMUM      "10"

/* definitionConcurrencyMaximum */
#define ICEP_DEF_DEFINITIONCONCURRENCYMAXIMUM      "10"
#define ICEP_MIN_DEFINITIONCONCURRENCYMAXIMUM      "2"
#define ICEP_MAX_DEFINITIONCONCURRENCYMAXIMUM      "10"

/* configurationChangeCounter */
#define ICEP_DEF_CONFIGURATIONCHANGECOUNTER        "1"
#define ICEP_MIN_CONFIGURATIONCHANGECOUNTER        "1"
#define ICEP_MAX_CONFIGURATIONCHANGECOUNTER        "1000000000"

/* configurationChangeInterval */
#define ICEP_DEF_CONFIGURATIONCHANGEINTERVAL       "1"
#define ICEP_MIN_CONFIGURATIONCHANGEINTERVAL       "1"
#define ICEP_MAX_CONFIGURATIONCHANGEINTERVAL       "1440"

/* scanCompletionTimeout */
#define ICEP_DEF_SCANCOMPLETIONTIMEOUT             "90"
#define ICEP_MIN_SCANCOMPLETIONTIMEOUT             "1"
#define ICEP_MAX_SCANCOMPLETIONTIMEOUT             "3600"

/* scanRetryLimit */
#define ICEP_DEF_SCANRETRYLIMIT                    "5"
#define ICEP_MIN_SCANRETRYLIMIT                    "1"
#define ICEP_MAX_SCANRETRYLIMIT                    "1000"

/* scanRetryInterval */
#define ICEP_DEF_SCANRETRYINTERVAL                 "10"
#define ICEP_MIN_SCANRETRYINTERVAL                 "1"
#define ICEP_MAX_SCANRETRYINTERVAL                 "3600"

/* traceEnable */
#define ICEP_DEF_TRACEENABLE                       "0"
#define ICEP_MIN_TRACEENABLE                       "0"
#define ICEP_MAX_TRACEENABLE                       "1"

/* traceFilename */
#define ICEP_DEF_TRACEFILENAME                     "C:\\Program Files\\Symantec\\Quarantine\\Server\\avistrace.txt"

/* traceSeverity */
#define ICEP_DEF_TRACESEVERITY                     "debug"

/* traceFilter */
#define ICEP_DEF_TRACEFILTER                       "0xFFFFFFFF"
#define ICEP_MIN_TRACEFILTER                       "0"
#define ICEP_MAX_TRACEFILTER                       "0xFFFFFFFF"

/* temporaryDirectory */
#define ICEP_DEF_TEMPORARYDIRECTORY                "c:\\temp"

#endif /* __REGISTRYDEFAULTS_DOT_H__ */

