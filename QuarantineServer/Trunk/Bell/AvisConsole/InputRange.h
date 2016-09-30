/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

/*----------------------------------------------------------------------------
  InputRange.h

  Defines for Max/Min settings for Spin controls
  Defines for string sizes and default settings
 ----------------------------------------------------------------------------*/

#ifndef _INPUTRANGE_H_DEFINED
   #define  _INPUTRANGE_H_DEFINED


#include "RegistryDefaults.h"


#define  MAX_LENGTH_HEX_DISPLAY_STTRING               8


// AvisServerConfig "Sample Policy"
#define  POLICY_DEFAULT_INITIAL_SUBMISSION_PRIORITY   500
#define  POLICY_MIN_SPIN_INITIAL_SUBMISSION_PRIORITY  0
#define  POLICY_MAX_SPIN_INITIAL_SUBMISSION_PRIORITY  1000

#define  POLICY_DEFAULT_MAX_PENDING_SAMPLES           2
#define  POLICY_MIN_SPIN_PENDING_SAMPLES			  1
#define  POLICY_MAX_SPIN_PENDING_SAMPLES			  100

#define  POLICY_DEFAULT_DEFINITION_CHECK_INTERVAL     10
#define  POLICY_MIN_SPIN_QUEUE_CHECK_INTERVAL 		  1
#define  POLICY_MAX_SPIN_QUEUE_CHECK_INTERVAL		  60

#define  POLICY_DEFAULT_QUEUE_CHECK_INTERVAL          1
#define  POLICY_MIN_SPIN_DEFINITION_CHECK_INTERVAL 	  1
#define  POLICY_MAX_SPIN_DEFINITION_CHECK_INTERVAL	  60

#define  POLICY_DEFAULT_DEFLIBRARY_PATH_MAXLEN        259

#define  WEB_MIN_SPIN_STATUS_INTERVAL	              60
#define  WEB_MAX_SPIN_STATUS_INTERVAL                 240
#define  WEB_DEFAULT_STATUS_INTERVAL                  15

#define  POLICY_DEFAULT_STRIP_USER_DATA               1
#define  POLICY_DEFAULT_COMPRESS_USER_DATA            1
#define  POLICY_DEFAULT_SCRAMBLE_USER_DATA            1



// AvisServerConfig2   WEB COMMUNICATIONS
#define  WEB_DEFAULT_GATEWAY_ADDRESS_MAXLEN           259
#define  WEB_DEFAULT_GATEWAY_ADDRESS                  _T("pilot.gateways.dis.symantec.com")

#define  WEB_DEFAULT_GATEWAY_PORT                     2847
#define  WEB_MIN_SPIN_GATEWAY_PORT	                  1
#define  WEB_MAX_SPIN_GATEWAY_PORT                    65535

#define  WEB_DEFAULT_SSL_PORT                         2848
#define  WEB_MIN_SPIN_SSL_PORT                        1    
#define  WEB_MAX_SPIN_SSL_PORT                        65535

#define  WEB_DEFAULT_RETRY_INTERVAL                   10
#define  WEB_MIN_SPIN_RETRY_INTERVAL	              1
#define  WEB_MAX_SPIN_RETRY_INTERVAL                  60

#define  WEB_DEFAULT_RETRY_LIMIT                      2
#define  WEB_MIN_SPIN_RETRY_LIMIT                     1
#define  WEB_MAX_SPIN_RETRY_LIMIT                     100

#define  WEB_DEFAULT_TRANSACTION_TIMEOUT              5
#define  WEB_MIN_SPIN_TRANSACTION_TIMEOUT             1
#define  WEB_MAX_SPIN_TRANSACTION_TIMEOUT             60

#define  WEB_DEFAULT_CHECK_SECURE_IGNORE_HOSTNAME     1
#define  WEB_DEFAULT_CHECK_SECURE_DEF_DOWNLOAD        1
#define  WEB_DEFAULT_CHECK_SECURE_STATUS_QUERY        1
#define  WEB_DEFAULT_CHECK_SECURE_SAMPLE_SUBMISSION   1



// DEFINITIONS PAGE

#define  DEF_DEFAULT_DEFS_HEURISTIC_LEVEL             2
#define  DEF_MIN_SPIN_DEFS_HEURISTIC_LEVEL            0
#define  DEF_MAX_SPIN_DEFS_HEURISTIC_LEVEL            3

#define  DEF_DEFAULT_UNPACK_TIMEOUT                   5
#define  DEF_MIN_SPIN_UNPACK_TIMEOUT                  1
#define  DEF_MAX_SPIN_UNPACK_TIMEOUT                  60

#define  DEF_DEFAULT_DEFS_BLESSED_INTERVAL            720
#define  DEF_MIN_SPIN_DEFS_BLESSED_INTERVAL           1
#define  DEF_MAX_SPIN_DEFS_BLESSED_INTERVAL           1440

#define  WEB_DEFAULT_DEFS_NEEDED_INTERVAL             15
#define  WEB_MIN_SPIN_DEFS_NEEDED_INTERVAL	          1
#define  WEB_MAX_SPIN_DEFS_NEEDED_INTERVAL            60

#define  DEF_DEFAULT_DEFS_PRUNE                       0
//#define  DEF_MIN_SPIN_DEFS_PRUNE
//#define  DEF_MAX_SPIN_DEFS_PRUNE




//#define  DEF_DEFAULT_
//#define  DEF_MIN_SPIN_
//#define  DEF_MAX_SPIN_

// DEFINITIONS INSTALL PAGE
#define  DEF_DEFAULT_DELIVERY_TIMEOUT                 30
#define  DEF_MIN_SPIN_DELIVERY_TIMEOUT                1
#define  DEF_MAX_SPIN_DELIVERY_TIMEOUT                60

#define  DEF_DEFAULT_DELIVERY_PRIORITY                500
#define  DEF_MIN_SPIN_DELIVERY_PRIORITY               0
#define  DEF_MAX_SPIN_DELIVERY_PRIORITY               1000

#define  DEF_DEFAULT_DELIVERY_INTERVAL                15
#define  DEF_MIN_SPIN_DELIVERY_INTERVAL               1
#define  DEF_MAX_SPIN_DELIVERY_INTERVAL               60

#define  DEF_DEFAULT_CHECK_BLESSED_BROADCAST          0
#define  DEF_DEFAULT_CHECK_UNBLESSED_BROADCAST        0
#define  DEF_DEFAULT_CHECK_UNBLESSED_NARROWCAST       1
#define  DEF_DEFAULT_CHECK_UNBLESSED_POINTCAST        1

// AServerFirewallConfig2  AVIS FIREWALL
#define  WEB_DEFAULT_PROXY_FIREWALL_MAXLEN            259
#define  WEB_DEFAULT_FIREWALL_PORT	                  80
#define  WEB_MIN_SPIN_FIREWALL_PORT	                  1
#define  WEB_MAX_SPIN_FIREWALL_PORT                   65535
#define  FIREWALL_USERNAME_MAXLEN                     259
#define  FIREWALL_PASSWORD_MAXLEN                     259


// CCustomerInfo
#define  CUST_EMAIL_MIN_LENGTH                        1
#define  CUST_EMAIL_MAX_LENGTH                        255

// ALERTING PAGE
#define ALERTING_DEFAULT_ALERT_INTERVAL               15
#define ALERTING_MIN_SPIN_ALERT_INTERVAL              1
#define ALERTING_MAX_SPIN_ALERT_INTERVAL              1440
#define ALERTING_DEFAULT_ENABLE                       FALSE
#define ALERTING_DEFAULT_NT_EVENT_LOG                 TRUE



// SampleActionsPage    ACTIONS
#define  ACTIONS_MIN_SPIN_SUBMISSION_PRIORITY	      0
#define  ACTIONS_MAX_SPIN_SUBMISSION_PRIORITY	      1000


// SERVER DEFAULTS
//#define  CONFIG_DEFAULT_STATUS_INTERVAL               15
//#define  CONFIG_DEFAULT_DEFLIBRARY_FOLDER             _T("c:\\signatures")
#define  CONFIG_DEFAULT_QUEUE_CHECK_INTERVAL          1
#define  CONFIG_DEFAULT_MAX_PENDING_SAMPLES           2
#define  CONFIG_DEFAULT_DEFINITION_CHECK_INTERVAL     10



#endif
