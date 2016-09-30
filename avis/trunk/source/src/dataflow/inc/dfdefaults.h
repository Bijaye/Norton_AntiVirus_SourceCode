/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFDEFAULTS.H                                                    */
/* FUNCTION:  To define defaults                                              */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/* HISTORY:  02/25 1998  - SNK                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef DFDEFAULTS_H
#define DFDEFAULTS_H


#define	 DEFAULT_SAMPLE_SUBMISSION_INTERVAL      1     // sec
#define	 DEFAULT_SAMPLE_SUBMISSION_INTERVAL_MAX 30     // sec 
#define  DEFAULT_SAMPLE_ARRIVAL_INTERVAL        30     // sec  
#define  DEFAULT_SAMPLE_ARRIVAL_INTERVAL_MAX    60     // sec  
#define  DEFAULT_DIR_CHECK_INTERVAL          10800     // sec
#define	 DEFAULT_CACHE_CHECK_INTERVAL          600     // sec
#define	 CACHE_CHECK_INTERVAL_MIN               10     // sec
#define  DIR_CHECK_INTERVAL_MIN                 30     // sec
#define  DEFAULT_DATABASE_RESCAN_INTERVAL       30     // sec  
#define  DEFAULT_DATABASE_RESCAN_INTERVAL_MAX   60     // sec  
#define  DEFAULT_DEF_IMPORT_INTERVAL             0     // min  
#define  DEFAULT_DEF_IMPORT_INTERVAL_MAX        60     // min  
#define  DEFAULT_STATUS_UPDATE_INTERVAL          0     // min  
#define  DEFAULT_STATUS_UPDATE_INTERVAL_MAX     60     // min  
#define  DEFAULT_UNDEFERRER_INTERVAL             0     // min  
#define  DEFAULT_UNDEFERRER_INTERVAL_MAX        60     // min  
#define  DEFAULT_ATTRIBUTE_INTERVAL              0     // min  
#define  DEFAULT_ATTRIBUTE_INTERVAL_MAX         60     // min  


    
#define  DEFAULT_SAMPLES_MAX                  1000  
#define  DEFAULT_SAMPLES                        30      // Jalan Was 30


// DataFlow Resources Defaults

// Configurable parameters using AVISDFRL.cfg file

#define DFR_DEFAULT_MAX_LOG_FILE_SIZE               1000    // KB
#define DFR_DEFAULT_LOG_BUFFER_SIZE                 0       // KB
#define DFR_DEFAULT_LOG_FILE_NAME                   _T("DFResourceActivity.Log")
#define DFR_DEFAULT_SINGLE_JOB_PER_MACHINE          FALSE
#define DFR_DEFAULT_AUTO_DISABLE_COUNT              200
#define DFR_DEFAULT_SHOW_ANALYSIS_APP_WINDOW        FALSE

/* Sample AVISDFRL.cfg File

Max Log File Size (KB) : 1000
Log Buffer Size (KB) : 16
Log File Name : DFResourceActivity.Log
Single Job Per Machine : FALSE
Auto Disable Error Count : 200
Show Resource Window : FALSE

*/

// Non configurable parameters
#define DFR_DEFAULT_VIEW_REFRESH_INTERVAL           1       // Seconds
#define DFR_DEFAULT_STATISTICS_WRITE_INTERVAL       30       // Minutes

// End DataFlow Resources Defaults

#endif
