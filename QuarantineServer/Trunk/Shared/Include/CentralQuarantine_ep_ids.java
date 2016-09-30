//////////////////////////////////////////////////////////////////////////
//
// CentralQuarantine_ep_ids.java
// This is a generated Sesa ID interface file created by the AutoHeader program
// (c) 2002 - Symantec Corporation
//
//////////////////////////////////////////////////////////////////////////


package com.symantec.management.sesids


public interface CentralQuarantine_ep_ids
{

    //////////////////////////////////////////////////////////////////////////
    // These IDS were generated from the source file: CentralQuarantine_1_0_ep.xml
    //////////////////////////////////////////////////////////////////////////


    // Event Class IDs
    public static final String SES_CLASS_QUARANTINE                       = "symc_quarantine";
    public static final String SES_CLASS_QUARANTINE_SAMPLE                = "symc_quarantine_sample";

    // Fields for event class symc_quarantine_class
    public static final String SES_FIELD_HOSTNAME                         = "hostname";              // symc_info
    public static final String SES_FIELD_ERROR_MESSAGE                    = "error_message";         // symc_info
    public static final String SES_FIELD_DEFAULT_MESSAGE                  = "default_message";       // symc_info

    // Fields for event class symc_quarantine_sample_class
    public static final String SES_FIELD_PLATFORM                         = "platform";              // symc_info
    public static final String SES_FIELD_FILENAME                         = "filename";              // symc_info
    public static final String SES_FIELD_VIRUS_NAME                       = "virus_name";            // symc_info
    public static final String SES_FIELD_DEFS_NEEDED                      = "defs_needed";           // symc_info
    public static final String SES_FIELD_STATUS                           = "status";                // symc_info
    public static final String SES_FIELD_AGE                              = "age";                   // symc_info
    public static final String SES_FIELD_NOTE                             = "note";                  // symc_info

    // Event IDs
    public static final int SES_EVENT_DISK_FREE_SPACE_LESS_THAN_QUARANTINE_MAX_SIZE = 252000;
    public static final int SES_EVENT_DISK_QUOTA_REMAINING_IS_LOW_FOR_QUARANTINE_DIR = 252001;
    public static final int SES_EVENT_NEW_NON_CERTIFIED_DEFINITIONS_ARRIVED = 252002;
    public static final int SES_EVENT_NEW_CERTIFIED_DEFINITIONS_ARRIVED   = 252003;
    public static final int SES_EVENT_CANNOT_CONNECT_TO_QUARANTINE_SCANNER_SERVICE = 252004;
    public static final int SES_EVENT_UNABLE_TO_ACCESS_DEFINITION_DIRECTORY = 252005;
    public static final int SES_EVENT_CANNOT_INSTALL_DEFINITIONS_ON_TARGET_MACHINES = 252006;
    public static final int SES_EVENT_DEFCAST_ERROR                       = 252007;
    public static final int SES_EVENT_UNABLE_TO_CONNECT_TO_GATEWAY        = 252008;
    public static final int SES_EVENT_THE_QUARANTINE_AGENT_SERVICE_HAS_STOPPED = 252009;
    public static final int SES_EVENT_CENTRAL_QUARANTINE_TEST_ALERT       = 252010;
    public static final int SES_EVENT_SAMPLE_HELD_FOR_MANUAL_SUBMISSION   = 252050;
    public static final int SES_EVENT_SAMPLE_TOO_LONG_WITH_QUARANTINED_STATUS = 252051;
    public static final int SES_EVENT_SAMPLE_TOO_LONG_WITH_SUBMITTED_STATUS = 252052;
    public static final int SES_EVENT_SAMPLE_TOO_LONG_WITH_RELEASED_STATUS = 252053;
    public static final int SES_EVENT_SAMPLE_TOO_LONG_WITH_NEEDED_STATUS  = 252054;
    public static final int SES_EVENT_SAMPLE_TOO_LONG_WITH_DISTRIBUTED_STATUS = 252055;
    public static final int SES_EVENT_SAMPLE_TOO_LONG_WITHOUT_INSTALLING_NEW_DEFS = 252056;
    public static final int SES_EVENT_SAMPLE_WAS_NOT_REPAIRED             = 252057;
    public static final int SES_EVENT_SAMPLE_NEEDS_ATTENTION_FROM_TECH_SUPPORT = 252058;
    public static final int SES_EVENT_INSTALLED                           = 252059;
    public static final int SES_EVENT_ICEPACK_ABANDONED                   = 252060;
    public static final int SES_EVENT_ICEPACK_CONTENT                     = 252061;
    public static final int SES_EVENT_ICEPACK_CRUMBLED                    = 252062;
    public static final int SES_EVENT_ICEPACK_DECLINED                    = 252063;
    public static final int SES_EVENT_ICEPACK_INTERNAL                    = 252064;
    public static final int SES_EVENT_ICEPACK_LOST                        = 252065;
    public static final int SES_EVENT_ICEPACK_MALFORMED                   = 252066;
    public static final int SES_EVENT_ICEPACK_MISSING                     = 252067;
    public static final int SES_EVENT_ICEPACK_OVERRUN                     = 252068;
    public static final int SES_EVENT_ICEPACK_SAMPLE                      = 252069;
    public static final int SES_EVENT_ICEPACK_SUPERCEDED                  = 252070;
    public static final int SES_EVENT_ICEPACK_TYPE                        = 252071;
    public static final int SES_EVENT_ICEPACK_UNAVAILABLE                 = 252072;
    public static final int SES_EVENT_ICEPACK_UNDERRUN                    = 252073;
    public static final int SES_EVENT_ICEPACK_UNPACKAGE                   = 252074;
    public static final int SES_EVENT_ICEPACK_UNPUBLISH                   = 252075;
    public static final int SES_EVENT_REPAIRED                            = 252076;
    public static final int SES_EVENT_QUARANTINE_SERVER_PROCESSED_SAMPLE  = 252077;
    public static final int SES_EVENT_QUARANTINE_SERVER_SAMPLE_WAS_REJECTED = 252078;

    //////////////////////////////////////////////////////////////////////////
    // These IDS were generated from the source file: CentralQuarantine_1_0_pi.xml
    //////////////////////////////////////////////////////////////////////////

    // Product Version
    public static final String PRODUCT_SYMANTEC_CENTRAL_QUARANTINE_VERSION = "1.0";

    // Product IDs
    public static final int SESPRODUCTID_SYMANTEC_CENTRAL_QUARANTINE      = 3013;

    // Software Feature IDs
    public static final int SESSWFID_SYMANTEC_CENTRAL_QUARANTINE___QUARANTINE_SERVER = 30130101;
    public static final int SESSWFID_SYMANTEC_CENTRAL_QUARANTINE___QUARANTINE_AGENT = 30130102;


}
