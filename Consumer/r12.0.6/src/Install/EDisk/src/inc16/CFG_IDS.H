/*----------------------------------------------------------------------*

		THIS FILE MUST BE KEPT IN SYNC WITH THE
		CFG_IDS.H IN THE QUAKE LIBARY.  THE ONE
		IN QUAKE IS CONSIDERED THE "MASTER" COPY.

		WE'VE ALREADY HAD A COLLISION BETWEEN NDIAGS
		AND NDD BECAUSE PEOPLE USED THE SAME ID NUMBER
		DIFFERENTLY UNDER NORTON LIBRARY AND QUAKE!!!

 *----------------------------------------------------------------------*/

/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   V:/INCLUDE/VCS/CFG_IDS.H_V   1.4   09 Dec 1993 19:46:12   BASIL  $ *
 *                                                                      *
 * Description:                                                         *
 *      Configuration Manager Block IDs.  Please add any new IDs only   *
 *      in the defined ID ranges for your product.  Otherwise, you      *
 *      risk walking on somebody else's stuff.                          *
 *                                                                      *
 *      Coordinate any product additions with Ed Carlin in Santa        *
 *      Monica (x4259).                                                 *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************ 
 * $Log:   V:/INCLUDE/VCS/CFG_IDS.H_V  $ *
 * 
 *    Rev 1.4   09 Dec 1993 19:46:12   BASIL
 * Added Ndiags_2 and NDDW_MSG.
 * 
 *    Rev 1.3   16 Sep 1993 18:22:52   JOHN
 * Added new NDD id's.
 * 
 *    Rev 1.2   03 Dec 1992 11:47:42   BASIL
 * Added CFGID_NU_PRINTCFGID (1011)
 * 
 *    Rev 1.1   01 Dec 1992 15:16:24   JOEP
 * Added an ID for NDIAGS it is 1010
 * 
 * 
 *    Rev 1.0   01 Dec 1992 10:47:20   CRAIG
 * Back-ported from Quake.
// 
//    Rev 1.3   08 Oct 1992 07:33:48   ED
// Added Norton Mail
// 
//    Rev 1.2   05 Oct 1992 16:05:38   ED
// Added MCI Mail ID
// 
//    Rev 1.1   01 Oct 1992 14:53:32   ED
// Added Norton Library section
// 
//    Rev 1.0   01 Oct 1992 12:29:38   ED
// Initial revision.
 ************************************************************************/

#ifndef _INCL_CFGIDS_
#define _INCL_CFGIDS_


/************************************************************************
 * Norton Library IDs - 1 - 999                                         *
 ************************************************************************/

#define CFGID_NLIB_START        1

#define CFGID_NLIB_LIBRARY      1               // basic configuration stuff
#define CFGID_NLIB_TEMPDIRS     2               // Temporary Directories

#define CFGID_NLIB_END          999

/************************************************************************
 * Norton Utilities IDs - 1000 - 2999                                   *
 ************************************************************************/

#define CFGID_NU_START          1000

        // Former records in NUConfigRec
#define CFGID_NU_DRIVES         1000            // drivesType
#define CFGID_NU_DISKEDIT       1001            // DE
#define CFGID_NU_DISKDOCTOR_1   1002            // NDD original options
#define CFGID_NU_SAFEFORMAT     1003            // SF
#define CFGID_NU_WIPEINFO       1004            // WipeInfo
#define CFGID_NU_DISKREET       1005            // NCRYPT
#define CFGID_NU_INTEGRATOR     1006            // NI
#define CFGID_NU_FILEFIND       1007            // FileFind
#define CFGID_NU_NCD            1008            // NCD
#define CFGID_NU_SPEEDISK       1009            // SD
#define CFGID_NU_NDIAGS         1010            // NDIAGS
#define CFGID_NU_PRINTCFGID     1011            // Printer config ID
#define CFGID_NU_NDD_CUSTOM_MSG 1012		// NDD Custom Message
#define CFGID_NU_DISKDOCTOR_2   1013            // NDD additional options
#define CFGID_NU_NDDW_MSG       1014            // NDD Win Custom Message
#define CFGID_NU_NDIAGS_2       1015            // NDiags additional options

#define CFGID_NU_DISKDOCTOR     CFGID_NU_DISKDOCTOR_1 // For compatibility

#define CFGID_NU_END            2999

/************************************************************************
 * Norton Desktop for Windows IDs - 3000 - 4999                         *
 ************************************************************************/

#define CFGID_NDW_START         3000

#define CFGID_NDW_MCIMAIL       3000            // MCI Mail Driver
#define CFGID_NDW_NORTONMAIL    3001            // Norton Mail Program

#define CFGID_NDW_END           4999


#endif  // _INCL_CFGIDS_
