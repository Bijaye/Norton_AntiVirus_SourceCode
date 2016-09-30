////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

//
// CC team no longer maintains the event IDs for individual groups.
// they do reserve event id ranges for each group
// the BB team's IDs are reserved between 6000-6899 for BB and
// 6900-6999 for Shields
//
// !!! ALL EVENT IDs RELATED TO ccEvtMgr SHOULD BE DEFINED HERE !!!
//
#define BB_EVENT_ID_BASE                            6000
#define SHIELD_EVENT_ID_BASE                        6900

// The SP 1.0 (Pavlov) events
#define CC_SYMPROTECT_POLICY_EVENT                  (BB_EVENT_ID_BASE + 0)
#define CC_SYMPROTECT_START_EVENT                   (BB_EVENT_ID_BASE + 1)
#define CC_SYMPROTECT_STOP_EVENT                    (BB_EVENT_ID_BASE + 2)
#define CC_SYMPROTECT_QUERY_EVENT                   (BB_EVENT_ID_BASE + 3)
#define CC_SYMPROTECT_RELOAD_EVENT                  (BB_EVENT_ID_BASE + 4)
#define CC_SYMPROTECT_MANIFEST_EVENT                (BB_EVENT_ID_BASE + 5)

// The BB 1.5 (Skinner) events
#define CC_BB_SETTINGS_BASE_EVENT                   (BB_EVENT_ID_BASE + 6)
#define CC_BB_SETTINGS_VALIDATION_EVENT             (BB_EVENT_ID_BASE + 7)
#define CC_BB_CONFIGURATION_UPDATE_EVENT            (BB_EVENT_ID_BASE + 8)
#define CC_BB_SETTINGS_FORCE_REFRESH_EVENT          (BB_EVENT_ID_BASE + 9)
#define CC_BB_SYSTEM_NOTIFY_EVENT                   (BB_EVENT_ID_BASE + 10)
#define CC_SYMPROTECT_SET_CONFIG_EVENT              (BB_EVENT_ID_BASE + 11)
#define CC_SYMPROTECT_QUERY_CONFIG_EVENT            (BB_EVENT_ID_BASE + 12)
#define CC_BB_AUTHORIZED_USER_QUERY_EVENT           (BB_EVENT_ID_BASE + 13)
#define CC_BB_AUTHORIZED_USER_SET_EVENT             (BB_EVENT_ID_BASE + 14)
#define CC_BB_AUTHORIZED_HASH_QUERY_EVENT           (BB_EVENT_ID_BASE + 15)
#define CC_BB_AUTHORIZED_HASH_SET_EVENT             (BB_EVENT_ID_BASE + 16)
#define CC_BB_AUTHORIZED_LOCATION_QUERY_EVENT       (BB_EVENT_ID_BASE + 17)
#define CC_BB_AUTHORIZED_LOCATION_SET_EVENT         (BB_EVENT_ID_BASE + 18)
#define CC_BB_AUTHORIZED_DEVICE_QUERY_EVENT         (BB_EVENT_ID_BASE + 19)
#define CC_BB_AUTHORIZED_DEVICE_SET_EVENT           (BB_EVENT_ID_BASE + 20)
#define CC_BB_BEHAVIORBLOCKING_DETECTION_EVENT      (BB_EVENT_ID_BASE + 21)
#define CC_BB_BEHAVIORBLOCKING_DEBUG_EVENT          (BB_EVENT_ID_BASE + 22)
#define CC_BB_AUTHORIZED_MSI_SET_EVENT              (BB_EVENT_ID_BASE + 23)
#define CC_BB_AUTHORIZED_MSI_QUERY_EVENT            (BB_EVENT_ID_BASE + 24)
#define CC_BB_BEHAVIORBLOCKING_STATUS_EVENT         (BB_EVENT_ID_BASE + 25)
#define CC_BB_BBFRAMEWORK_CONFIG_EVENT              (BB_EVENT_ID_BASE + 26)

// The BB 2005-1 (Freud) event
#define CC_SM_CLIENT_REGISTRATION_EVENT             (BB_EVENT_ID_BASE + 27)
#define CC_SM_NOTIFICATION_EVENT                    (BB_EVENT_ID_BASE + 28)
#define CC_SM_RULESET_QUERY_EVENT                   (BB_EVENT_ID_BASE + 29)
#define CC_SM_RULESET_SET_EVENT                     (BB_EVENT_ID_BASE + 30)
#define CC_SYMPROTECT_SET_CONFIG_EVENT2             (BB_EVENT_ID_BASE + 31)
#define CC_SYMPROTECT_QUERY_CONFIG_EVENT2           (BB_EVENT_ID_BASE + 32)

// BB 2006-2 event
#define CC_BB_STATE_QUERY_EVENT                     (BB_EVENT_ID_BASE + 33)
#define CC_BB_STATE_SET_EVENT                       (BB_EVENT_ID_BASE + 34)

// Shield Events
#define CC_SHIELDS_SETTINGS_QUERY_EVENT             (SHIELD_EVENT_ID_BASE + 0)
#define CC_SHIELDS_EXCLUSIONS_QUERY_EVENT           (SHIELD_EVENT_ID_BASE + 1)
#define CC_SHIELDS_SETTINGS_SET_EVENT               (SHIELD_EVENT_ID_BASE + 2)
#define CC_SHIELDS_EXCLUSIONS_SET_EVENT             (SHIELD_EVENT_ID_BASE + 3)
