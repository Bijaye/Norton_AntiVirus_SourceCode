/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFError.h                                                       */
/* FUNCTION:  error file                                                      */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/*----------------------------------------------------------------------------*/
/* HISTORY: November 1998 - SNK                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef DFERROR_H
#define DFERROR_H

#define DATABASE_INTERFACE


#define MAX_SAMPLES            31
#define MAX_LENGTH             1024

#define DF_FILE_OPEN_ERROR     10

#define PATH_ERROR             11 
#define INPUTQUEUE_PATH_ERROR  12 

#define THREAD_ID	           WM_USER+520

#define CHECK_INTERVAL         60     //sec (1 min)
#define CHECK_DIR_INTERVAL     300    //sec (5 min)

#define UNDEFSEQ              555555    
#define STUPDATESEQ           666666   

#define SCANSEQ               777777    
#define DEFIMPSEQ             888888    

#define ATTRIBUTECOLLECTSEQ	  444444  


#define DF_SUCCESS                        0
#define DF_SAMPLE_ARRIVED                77  
#define DF_RESULT_FILE_ERROR             20 
#define DF_STOP_FILE_ERROR               21 
#define DF_EVALUATOR_FAILED              22 
#define DF_OBJECT_ERROR                  23 
#define DF_JOB_STATUS_ERROR              24 
#define DF_COOKIE_ERROR                  25 
#define DF_CONFIG_FILE_ERROR             26 
#define DF_IMPORT_SAMPLE_ERROR           27 
#define DF_CONFIG_FILE_CREATE_ERROR      28 
#define DF_CONFIG_FILE_HAS_BEEN_CREATED  29        
#define DF_UNC_EMPTY                     30        
#define DF_GET_SAMPLE_ERROR              34        
#define DF_BUILD_DEF_FILENAME_EMPTY      35        
#define DF_BASE_DEF_DIR_EMPTY            36        
#define DF_PACKAGE_ERROR                 37        
#define DF_LATEST_SIGNATURE_ERROR        38        
#define DF_DEF_IMPORTER_DIR_EMPTY        39        
#define DF_CRITICAL_ERROR                40        




#define DF_DABASE_OPEN_ERROR             200        
#define DF_DABASE_CLOSE_ERROR            201        
#define DF_DABASE_ADD_NEW_STATE_ERROR    202 
#define DF_DATABASE_UPDATE_RECORD_ERROR  203        
#define DF_DATABASE_GET_RECORD_ERROR     204        
#endif 
