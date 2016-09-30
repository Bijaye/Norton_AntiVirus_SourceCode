/////////////////////////////////////////////////////////////////////
// NAVRPC.H
//
// This header file defines our RPC stuff.
//

#if !defined (__NAVRPC_H)
#define __NAVRPC_H

/////////////////////////////////////////////////////////////////////
// defines for our protocols
//
// Use these to specify what protocols and endpoints the differnet
// RPC Servers / Clients are going to use.


/////////////////////////////////////////////////////////////////////
// This RPC combination is used for the NAV AP Service.  All requests
// for the kernel mode stuff comes through here.

#define NAVAPSVC_PROTOCOL "ncalrpc"
#define NAVAPSVC_ADDRESS  ""
#define NAVAPSVC_ENDPOINT "Navapsvc"


/////////////////////////////////////////////////////////////////////
// This RPC combination is used for the AlertService.  Here is where
// you request for alerts to be sent.

#define ALERTSVC_PROTOCOL "ncalrpc"
#define ALERTSVC_ADDRESS  ""
#define ALERTSVC_ENDPOINT "Alertsvc"


/////////////////////////////////////////////////////////////////////
// This RPC combination is for the RPC server that is in NAVW

#define NAVW_PROTOCOL "ncalrpc"
#define NAVW_ADDRESS  ""
#define NAVW_ENDPOINT "Navw"


/////////////////////////////////////////////////////////////////////
// This RPC combination is for the Norton Program Scheduler

#define NPS_PROTOCOL "ncalrpc"
#define NPS_ADDRESS  ""
#define NPS_ENDPOINT "NPS"

#endif // !defined (__NAVRPC_H)
