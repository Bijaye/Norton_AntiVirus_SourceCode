/////////////////////////////////////////////////////////////////////
// RPCPROT.H - Created - 02/05/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/RPCPROT.H_v   1.0   10 May 1997 01:52:14   KSACKIN  $
//
// Description:
//    This header file will contain defines for all of the protocols
//    that we are going to support in our application.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/RPCPROT.H_v  $
// 
//    Rev 1.0   10 May 1997 01:52:14   KSACKIN
// Initial revision.
// 
//    Rev 1.0   24 Apr 1997 20:17:12   JBRENNA
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:10:46   RFULLER
// Initial revision
// 
//    Rev 1.2   05 Feb 1997 15:35:34   DSACKIN
// Fixed a typo on ncacn_at_dsp
//
//    Rev 1.1   05 Feb 1997 15:17:00   DSACKIN
// Added the ncacn_at_dsp protocol
//
//    Rev 1.0   05 Feb 1997 15:15:34   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__RPCPROT_H)
#define __RPCPROT_H

#define NUMBER_OF_PROTOCOLS 12

#define  PROTOCOL_NCACN_NB_TCP    "ncacn_nb_tcp"
#define  PROTOCOL_NCACN_NB_IPX    "ncacn_nb_ipx"
#define  PROTOCOL_NCACN_NB_NB     "ncacn_nb_nb"
#define  PROTOCOL_NCACN_IP_TCP    "ncacn_ip_tcp"
#define  PROTOCOL_NCACN_NP        "ncacn_np"
#define  PROTOCOL_NCACN_SPX       "ncacn_spx"
#define  PROTOCOL_NCACN_DNET_NSP  "ncacn_dnet_nsp"
#define  PROTOCOL_NCACN_AT_DSP    "ncacn_at_dsp"
#define  PROTOCOL_NCACN_VNS_SPP   "ncacn_vns_spp"
#define  PROTOCOL_NCADG_IP_UDP    "ncadg_ip_udp"
#define  PROTOCOL_NCADG_IPX       "ncadg_ipx"
#define  PROTOCOL_NCALRPC         "ncalrpc"

#endif // if !defined(__RPCPROT_H)
