/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      dfmsg.h                                                         */
/* FUNCTION:  To list user's messages                                         */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/*----------------------------------------------------------------------------*/
/* HISTORY:  September 1, 1998 - SNK                                          */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef DFMSG_H
#define DFMSG_H

#define LAUNCHER  
//#define LAUNCHER_VIEW  


#define ALM_REQ_START_JOB           (WM_USER + 101)
#define ALM_REQ_IGNORE_JOB          (WM_USER + 102)
#define ALM_REQ_RELOAD_RESOURCES    (WM_USER + 103)
#define ALM_ON_DELETE               (WM_USER + 104)
  
#define DFLAUNCHER_POST_JOB_STATUS  (WM_USER + 201)

#define DFLAUNCHER_JOB_COMPLETE     (WM_USER + 301)
#define DFLAUNCHER_REFRESH_SUMMARY  (WM_USER + 302)
#define DFMANAGER_BLOCK_PROCESSING  (WM_USER + 303)
#define DFMANAGER_SEND_MAIL         (WM_USER + 304)
#define DFLAUNCHER_DISABLE_MACHINE  (WM_USER + 305)


//Tesing messages. 
#define ON_JOBACCEPT     WM_USER + 10
#define ON_JOBEND        WM_USER + 11 
#define ON_JOBSUBMIT     WM_USER + 12 
#define ON_JOBPOSTPONE   WM_USER + 14 
#define ON_JOBTIMEOUT    WM_USER + 15 
#define ON_JOBIGNORE     WM_USER + 16 


#endif

