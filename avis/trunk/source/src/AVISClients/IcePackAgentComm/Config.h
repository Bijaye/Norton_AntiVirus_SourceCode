#ifndef ___CONFIG85FCDED0_4C2C_11d3_8AAA_FC9B63000000__H__
#define ___CONFIG85FCDED0_4C2C_11d3_8AAA_FC9B63000000__H__

/*
 * I think that there is a special place reserved in hell for
 * people who use enums like this.  And if there isn't, there
 * certainly should be.
 *
 * inw 2000-05-31.
 */
enum {  CFG_DEFAULT_connection_timeout              = 15};
enum {  CFG_DEFAULT_connection_retries              =  3};
enum {  CFG_DEFAULT_send_request_timeout            = 15};
enum {  CFG_DEFAULT_send_request_data_timeout       = 15};
enum {  CFG_DEFAULT_receive_response_timeout        = 15};
enum {  CFG_DEFAULT_receive_response_data_timeout   = 15};
enum {  CFG_DEFAULT_unpacking_timeout               = 90};
#define CFG_DEFAULT_firewall_URL                      ""
#define CFG_DEFAULT_proxy_user_name                   ""
#define CFG_DEFAULT_proxy_password                    ""
#define CFG_DEFAULT_enable_WinINet_debugging          false
#define CFG_DEFAULT_unpacking_temp_dir                "\\temp"

/*
 * The log file name has been changed to an empty string, and a check for
 * "" has been added to CommClient::ThreadProc() in a desperate attempt to
 * get rid of the bloody IcePackAgentComm.log files that keep popping up
 * all over the place.
 *
 * Unfortunately, the logging code is so unmanageably buggered beyond all
 * hope of recovery that I refuse to attempt to look at it any more, chiefly
 * because every time I do so, a mist of red rage descends over my vision
 * and I can do no more work for several hours.
 *
 * inw 2000-05-31.
 */
#define CFG_DEFAULT_log_file_name               ""
#define CFG_DEFAULT_trace_file_name             "IcePackAgentComm.trc"
#define CFG_DEFAULT_agent_name                  "IcePackAgentComm:"

/*
 * Do _NOT_ ask why one of these includes the "//" and the other
 * does not.  It's stupid, but it's there.
 *
 * inw 2000-05-31.
 */
#define CFG_secure_http_URL_prefix              "https:"
#define CFG_default_URL_prefix                  "http://"

#define CFG_sample_uploader_servlet             "/AVIS/postSuspectSample"
#define CFG_sample_uploader_content_type        "application/x-avis-sample"
#define CFG_import_signature_required_headers   "Accept: application/x-NAV-VDB"
#define CFG_name_for_latest_signature           "newest"
#define CFG_name_for_latest_blessed_signature   "blessed"
#define CFG_signature_inquirer_servlet_name     "/AVIS/headSignatureSet"
#define CFG_signature_importer_servlet_name     "/AVIS/getSignatureSet"
#define CFG_signature_file_postfix              ".exe"
#define CFG_signature_file_vdb_postfix          ".vdb"
#define CFG_status_receiver_servlet_name        "/AVIS/getSampleStatus"

#endif /* ___CONFIG85FCDED0_4C2C_11d3_8AAA_FC9B63000000__H__ */
