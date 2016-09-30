#ifndef _RCMSGFILTER_H
#define _RCMSGFILTER_H
/*******************************************************************
*                                                                  *
* File name:     RCMsgFilter.cpp                                   *
*                                                                  *
* Description:   The message filter object                         *
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
*******************************************************************/


class RCMessageFilter : public IMessageFilter
    {
    protected:
        ULONG           m_cRef;      //Object reference count
        HWND            m_hDlg;      //Dialog handle


    public:
        RCMessageFilter();
        ~RCMessageFilter(void);

        STDMETHODIMP QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP_(DWORD) HandleInComingCall(DWORD, HTASK
            , DWORD, LPINTERFACEINFO);
        STDMETHODIMP_(DWORD) RetryRejectedCall(HTASK, DWORD, DWORD);
        STDMETHODIMP_(DWORD) MessagePending(HTASK, DWORD, DWORD);
    };

typedef RCMessageFilter *PRCMessageFilter;


#endif //_MSGFILT_H
