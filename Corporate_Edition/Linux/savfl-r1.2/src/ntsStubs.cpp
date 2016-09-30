#include <PSCAN.H>
#include <ndkDefinitions.h>

CBA_Addr ipxaddress,ipaddress;
/**************************************************************/
DWORD SendRequestTo(unsigned short                   usPortFrom,
		    const char                      *szComName,
		    ScsSecureComms::IAuth           *pMyAuthCredentials,
		    const char                      *szComputerName,
		    SENDCOM_HINTS                    hints,
		    ScsSecureComms::IMessageBuffer  *pMsgOut,
		    ScsSecureComms::IMessageBuffer  *pReply)
{
    DWORD       cc = MINUS_ONE;
    return cc;
}


#ifdef SCALABILITY_TESTING
#error "Not implemented"
//     DWORD CheckInWithMommyAsFakeClient( bool reuse_conn, bool forced, unsigned long& fake_client_idx );
//     DWORD CheckInWithMommy( bool reuse_conn = true, bool forced = true, unsigned short source_port = 0 );
DWORD CheckInWithMommy( bool reuse_conn, bool forced, unsigned short source_port )
{
    DWORD       cc = MINUS_ONE;
    return cc;
}
#else
DWORD CheckInWithMommy( bool reuse_conn )
{
    DWORD       cc = MINUS_ONE;
    return cc;
}
#endif // def SCALABILITY_TESTING

BOOL IsMyParentGood(void)
{
    BOOL bParentGood = FALSE;
    return bParentGood;
}

DWORD GetSupportedProtocols( DWORD *count, DWORD *array, CBA_Addr **addresses, DWORD array_size )
{
    *count = 0;
    return ERROR_SUCCESS;
}

DWORD AddPacketToProcessQueue( PSEMS sem )
{
    DWORD cc = ERROR_SUCCESS;
    return( cc );
}

DWORD StopProcessing(void)
{
    return( ERROR_SUCCESS );
}

DWORD StopNTS(void)
{
    return 0;
}

DWORD InitNTS(void)
{
    return 0;
}

DWORD   CvtCBA2GRC(DWORD t)
{
    return( ERROR_SUCCESS );
}

void DeInitNTS(void)
{
}

BOOL isNetwork(void)
{
    return GetVal(hMainKey,szReg_Val_ConnectedToNetwork,1);
}

DWORD ReStartPongEngine(bool bNotifyParent)
{
        return COM_ERROR_TRANSPORT_NOT_OK;
}

DWORD RawSendRequest(CBA_Addr *address,BYTE *To,int tSize,BYTE *From,int *fSize)
{
    return ERROR_SEND_FAILED;
}

void SystemProcess(void)
{
}



