/*******************************************************************
*                                                                  *
* File name:     RCMsgFilter.cpp                                   *
*                                                                  *
* Description:   Implementation of the message filter object       *
*                                                                  *
*                                                                  *
* Statement:     Licensed Materials - Property of IBM              *
*                (c) Copyright IBM Corp. 1999                      *
*                                                                  *
* Author:        Alla Segal                                        *
*                                                                  *
*                U.S. Government Users Restricted Rights - use,    *
*                duplication or disclosure restricted by GSA ADP   *
*                Schedule Contract with IBM Corp.                  *
*                                                                  *
* Note: This code is a modified version of the Koala Client sample *
* from for the Kraig Brockschmidt's Inside OLE, 2nd edition        *
* available through the MSDN                                       *
* Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved*
*                                                                  *
********************************************************************
*                                                                  *
* Function:      Replace the default message filter object in order*
*                to avoid System Busy dialog box                   *
*                                                                  *
*******************************************************************/

#include "stdafx.h"
#include "afxtempl.h"
#include "rcmsgfilter.h"

#include "rcdefs.h"
#include "wb70en32.h"
#include "excel8.h"
#include "msword8.h"
#include "msppt8.h"
#include "msacc8.h"
#include "RCError.h"
#include "RCCommon.h"
#include "RCFile.h"
#include "RCKeys.h"
#include "RCGoatList.h"
#include "RCPilot.h"
#include "RCCmd.h"

extern bool g_application_busy;

RCMessageFilter::RCMessageFilter()
    {
    m_cRef=0;
    //*m_pApp=pApp;
    return;
    }

RCMessageFilter::~RCMessageFilter(void)
    {
    return;
    }



/*
 * RCMessageFilter::QueryInterface
 * RCMessageFilter::AddRef
 * RCMessageFilter::Release
 *
 * Purpose:
 *  Delegating IUnknown members for RCMessageFilter.
 */


STDMETHODIMP RCMessageFilter::QueryInterface(REFIID riid
    , LPVOID *ppv)
    {
    *ppv=NULL;

    if (IID_IUnknown==riid || IID_IMessageFilter==riid)
        *ppv=this;

    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }

STDMETHODIMP_(ULONG) RCMessageFilter::AddRef(void)
    {
    return ++m_cRef;
    }

STDMETHODIMP_(ULONG) RCMessageFilter::Release(void)
    {
    if (0!=--m_cRef)
        return m_cRef;

    delete this;
    return 0;
    }




/*
 * RCMessageFilter::HandleInComingCall
 *
 * Purpose:
 *  Requests that the container call OleSave for the object that
 *  lives here.  Typically this happens on server shutdown.
 *
 * Parameters:
 *  dwCallType      DWORD indicating the type of call received, from
 *                  the CALLTYPE enumeration
 *  hTaskCaller     HTASK of the caller
 *  dwTickCount     DWORD elapsed tick count since the outgoing call
 *                  was made if dwCallType is not CALLTYPE_TOPLEVEL.
 *                  Ignored for other call types.
 *  pInterfaceInfo  LPINTERFACEINFO providing information about the
 *                  call.  Can be NULL.
 *
 * Return Value:
 *  DWORD           One of SERVERCALL_ISHANDLED (if the call might
 *                  be handled), SERVERCALL_REJECTED (call cannot
 *                  be handled), or SERVERCALL_RETRYLATER (try
 *                  again sometime).
 */

STDMETHODIMP_(DWORD) RCMessageFilter::HandleInComingCall
    (DWORD dwCallType, HTASK htaskCaller, DWORD dwTickCount
    , LPINTERFACEINFO pInterfaceInfo)
   {
    /*
     * Because ObjectUser2 doesn't serve any objects itself,
     * this should never occur in this message filter.
     */
    return SERVERCALL_ISHANDLED;
    }





/*
 * RCMessageFilter::RetryRejectedCall
 *
 * Purpose:
 *  Informs the message filter that an call from this process has
 *  been rejected or delayed from a local or remote server, thus
 *  asking the message filter what to do.
 *
 * Parameters:
 *  hTaskCallee     HTASK of the caller
 *  dwTickCount     DWORD elapsed tick count since the call was made
 *  dwRejectType    DWORD either SERVERCALL_REJECTED or
 *                  SERVERCALL_RETRYLATER as returned by
 *                  HandleInComingCall.
 *
 * Return Value:
 *  DWORD           (DWORD)-1 to cancel the call, any number between
 *                  0 and 100 to try the call again immediately, or
 *                  a value over 100 (but not (DWORD)-1) to instruct
 *                  COM to wait that many milliseconds before trying
 *                  again.
 */

STDMETHODIMP_(DWORD) RCMessageFilter::RetryRejectedCall
    (HTASK htaskCallee, DWORD dwTickCount, DWORD dwRejectType)
    {
    UINT    uRet;
    TCHAR   szMsg[256];

    /*
     * A message is pointless as ObjectUserWndProc will
     * output a message as soon as the call returns, overwriting
     * anything we might print here.
     */
    if (SERVERCALL_REJECTED==dwRejectType)
        return (DWORD)-1;

    wsprintf(szMsg, TEXT("RetryRejectedCall waiting %lu")
        , dwTickCount);
    //* m_pApp->Message(szMsg);


    /*
     * If we've waited over 5 seconds, return
     * Otherwise continue waiting.
     */
    if (dwTickCount < 5000)
        return 200;

	RCError rcError;
	rcError.ReportError("OLE command failed: the application is busy");
	g_application_busy = TRUE;
	return (DWORD)-1;
    }



/*
 * RCMessageFilter::MessagePending
 *
 * Purpose:
 *  Gives the caller a chance to process messages while waiting for
 *  a call to an object to complete, to handle things like focus
 *  changes and input.  Usually returning PENDINGMSG_DEFPROCESS
 *  takes care of most things, except that it discards input.  This
 *  function is really useful is you want to process input while
 *  a call is in progress.
 *
 * Parameters:
 *  hTaskCallee     HTASK of the caller
 *  dwTickCount     DWORD elapsed tick count since the call was made
 *  dwPendingType   DWORD with the type of call made from the
 *                  PENDINGTYPE enumeration.
 *
 * Return Value:
 *  DWORD           One of PENDINGMSG_CANCELCALL (cancels the call
 *                  under extreme conditions), PENDINGMSG_WAITNO-
 *                  PROCESS (continue waiting), or PENDINGMSG_WAIT-
 *                  DEFPROCESS (invoke default handling).
 */

STDMETHODIMP_(DWORD) RCMessageFilter::MessagePending
    (HTASK htaskCallee, DWORD dwTickCount, DWORD dwPendingType)
    {
    return PENDINGMSG_WAITDEFPROCESS;
    }







