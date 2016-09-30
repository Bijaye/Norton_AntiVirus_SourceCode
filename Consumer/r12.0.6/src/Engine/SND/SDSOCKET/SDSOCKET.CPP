// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SDSOCKET/VCS/sdsocket.cpv   1.6   13 Aug 1998 10:36:38   DCHI  $
//
// Description:
//
//  Contains scan and deliver socket client implementation
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/SDSOCKET/VCS/sdsocket.cpv  $
// 
//    Rev 1.6   13 Aug 1998 10:36:38   DCHI
// Changed SendLine() to not call the first send if the text is empty.
// Using Trumpet Winsock, the call returned SOCKET_ERROR.
// 
//    Rev 1.5   08 Jun 1998 11:07:54   CNACHEN
// Added support for either numeric or symbolic internet addresses.
// 
//    Rev 1.4   27 May 1998 13:19:58   ECHIEN
// Added HTTP_VERSION to Get Request string
// 
//    Rev 1.3   20 May 1998 16:19:54   CNACHEN
// Added error check for NULL after gethostbyname call.
// 
//    Rev 1.2   14 May 1998 15:20:16   CNACHEN
// Added BACK_END_ALL_CLEAN status
// 
//    Rev 1.1   07 May 1998 16:48:04   DCHI
// Changed to GetLine() to not return until seeing '\n'.
// 
//    Rev 1.0   16 Mar 1998 18:01:26   CNACHEN
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "stdio.h"

#define _SDSOCKET_CPP_

#include "sdsocket.h"
#include "sdsocki.h"

//************************************************************************
// int IsNumericAddress(lpszServerName)
//
// Purpose
//
//  This function determines if an address is a numeric IP address or a
//  standard string address: 111.33.44.11 vs www.symantec.com
//
// Parameters:
//
//  lpszServerName: www.sarcbackstatus.com or ###.###.###.###
//
// Returns:
//
//  TRUE if the address is numeric
//  FALSE otherwise
//
//************************************************************************
// 6/8/98 Carey created.
//************************************************************************

BOOL SocketClientImp::IsNumericAddress
(
    LPTSTR      lpszServerName
)
{
    int         i;

    // determine whether or not an address is a numeric one, ie "123.33.51.255"
    // or a symbolic one, ie "www.symantec.com"

    for (i=0;lpszServerName[i];i++)
        if (!isdigit(lpszServerName[i]) && lpszServerName[i] != '.')
            return(FALSE);

    return(TRUE);
}


//************************************************************************
// int GetBackEndStatus(lpszServerName, nPriorityCode, lpbyPreSubmissionData)
//
// Purpose
//
//  This function connects to the SARC HTTP server to query the back end's
//  status.
//
// Parameters:
//
//  lpszServerName: www.sarcbackstatus.com or similar
//  nPriorityCode: PLATINUM_PRIORITY, CORPORATE_PRIORITY or CUSTOMER_PRIORITY
//  lpbyResSubmissionData: Provided by the packaging API
//
// Returns:
//
//  BACK_END_CONNECTION_ERROR if there is an error connecting to the back end
//  BACK_END_DATA_ERROR       if there is a data error during xmission
//  BACK_END_SEND_PACKAGE     if the back end is up and is accepting packages
//  BACK_END_FP_USE_LU        if the back end has determined that the
//                            submission is definitely an FP; client app should
//                            tell the user to use LiveUpdate within the next
//                            week.
//  BACK_END_BUSY_TRY_LATER   if the back end needs to throttle submissions
//  BACK_END_ALL_CLEAN        all files are definitely clean; client app
//                            should tell the user not to worry.
//
//************************************************************************
// 3/16/98 Carey created.
//************************************************************************

int SocketClientImp::GetBackEndStatus
(
    LPTSTR              lpszServerName,
    int                 nPriorityCode,
    LPBYTE              lpbyPreSubmissionData
)
{
    char                szEncoded[MAX_HTTP_GET_ENCODE_LENGTH],
                        szGetRequest[MAX_HTTP_GET_LENGTH];
    char                *aszPriority[NUM_PRIORITIES] = {HIGH_PRIORITY,
                                                        MEDIUM_PRIORITY,
                                                        LOW_PRIORITY};
    int                 nResult;

    // connect to the socket server
    // create a "GET ..." request for the server
    // send the get request
    // get the response
    // close the connection - shut down sockets
    // return the proper result

    if (Connect(lpszServerName) != TRUE)
    {
        return(BACK_END_CONNECTION_ERROR);
    }

    // construct our sample info buffer to send for our get request

    if (ConvertDataToText(lpbyPreSubmissionData,
                          szEncoded,
                          MAX_HTTP_GET_ENCODE_LENGTH) != TRUE)
    {
        Disconnect();

        // this should never really happen!

        return(BACK_END_DATA_ERROR);
    }

    // construct the get request!

    sprintf(szGetRequest,
            "GET /%s?CRCDATA=%s&PRIORITY=%s %s",
            GET_PATH,
            szEncoded,
            aszPriority[nPriorityCode],
            HTTP_VERSION);

    // send it out! follow with a blank line

    if (SendLine(szGetRequest) != TRUE ||
        SendLine("") != TRUE)
    {
        Disconnect();

        return(BACK_END_DATA_ERROR);
    }

    // now get the response from the server

    if (GetResponse(&nResult) != TRUE)
    {
        Disconnect();

        return(BACK_END_DATA_ERROR);
    }

    // disconnect and return our result

    Disconnect();

    return(nResult);
}

//************************************************************************
// void MIMEEncode64TriToQuad(lpbyTri, lpszQuad)
//
// Purpose
//
//  This function encodes 3 8-bit bytes into 4 valid BASE64 bytes.
//
// Parameters:
//
//  lpbyTri: IN=Array of 3, 8-bit bytes to be encoded
//  lpszQuad: OUT=Array of 4, valid BASE64 characters to be outputted
//
// Returns:
//
//  nothing
//
//************************************************************************
// 8/19/96 Carey created.
//************************************************************************

void SocketClientImp::MIMEEncode64TriToQuad
(
    LPBYTE              lpbyTri,
    LPSTR               lpszQuad
)
{
    BYTE        byQuad[4];
    int         i;

    byQuad[0] = lpbyTri[0] >> 2;
    byQuad[1] = ((lpbyTri[0] & 0x3) << 4) | (lpbyTri[1] >> 4);
    byQuad[2] = ((lpbyTri[1] & 0xf) << 2) | (lpbyTri[2] >> 6);
    byQuad[3] = lpbyTri[2] & 0x3F;

    for (i=0;i<4;i++)
    {
        if (byQuad[i] <= 25)
            lpszQuad[i] = byQuad[i] + 'A';
        else if (byQuad[i] >= 26 && byQuad[i] <= 51)
            lpszQuad[i] = byQuad[i] + 'a' - 26;
        else if (byQuad[i] >= 52 && byQuad[i] <= 61)
            lpszQuad[i] = byQuad[i] + '0' - 52;
        else if (byQuad[i] == 62)
            lpszQuad[i] = '+';
        else if (byQuad[i] == 63)
            lpszQuad[i] = '/';
    }
}

//************************************************************************
// BOOL ConvertDataToText(lpbyData,lpszText,nMaxLength)
//
// Purpose
//
//  This function converts an arbitrary binary data buffer into a base-64
//  text message.  The binary data must have the following header:
//  [DWORD type][DWORD version][DWORD dwLength][dwLength bytes follow]
//
//  This will produce a text buffer containing a-z, A-Z, + and / and will
//  be terminated by a 0 character.
//
// Parameters:
//
//  lpbyData: Data provided by the packaging API
//  lpszText: Output string created by this function.
//  nMaxLength: Maximum length of data that can be placed in lpszText buffer
//
// Returns:
//
//  TRUE on success
//  FALSE on failure (buffer too small)
//
//************************************************************************
// 3/16/98 Carey created.
//************************************************************************

BOOL SocketClientImp::ConvertDataToText
(
    LPBYTE              lpbyData,
    LPTSTR              lpszText,
    int                 nMaxLength
)
{
    LPSUBMISSION_HEADER     lpstHeader = (LPSUBMISSION_HEADER)lpbyData;
    int                     i, nCount = 0, nStringIndex = 0;
    BYTE                    byTri[3] = {0};

    for (i=0;i<lpstHeader->dwSize + sizeof(SUBMISSION_HEADER_T);i++)
    {
        // create sets of 3 characters

        byTri[nCount++] = lpbyData[i];

        if (nCount == 3)
        {
            // encode the 3 into 4 valid BASE64 characters and add them
            // to our output line

            if (nStringIndex + 4 > nMaxLength - 1)
                return(FALSE);

            MIMEEncode64TriToQuad(byTri,lpszText+nStringIndex);

            nCount = 0;
            nStringIndex += 4;
            byTri[0] = byTri[1] = byTri[2] = 0;
        }
    }

    // see if we have a partially full byTri (< 3 bytes).  add it to the
    // current line.

    if (nCount != 0)
    {
        // this test may fail some buffer fills that would be OK, but the
        // client should really provide a much bigger buffer anyway.

        if (nStringIndex + 3 > nMaxLength - 1)
                return(FALSE);

        MIMEEncode64TriToQuad(byTri,lpszText+nStringIndex);

        if (nCount == 1)
            nStringIndex += 2;
        else if (nCount == 2)
            nStringIndex += 3;
    }

    lpszText[nStringIndex] = 0;

    return(TRUE);
}

//************************************************************************
// BOOL SendLine(lpszText)
//
// Purpose
//
//  This function sends a line of text followed by a CR/LF to the server
//
// Parameters:
//
//  lpszText: String to be sent.
//
// Returns:
//
//  TRUE on success
//  FALSE on failure
//
//************************************************************************
// 3/16/98 Carey created.
//************************************************************************

BOOL SocketClientImp::SendLine
(
    LPTSTR              lpszText
)
{
    if (lpszText[0])
    {
        if (send(m_socket,
                 (const char *)lpszText,
                 strlen(lpszText),
                 0) == SOCKET_ERROR)
            return(FALSE);
    }

    if (send(m_socket,
             "\r\n",
             sizeof(char)*2,
             0) == SOCKET_ERROR)
        return(FALSE);

    return(TRUE);
}

//************************************************************************
// BOOL GetLine(lpszText,nMaxLength)
//
// Purpose
//
//  This function retrieves a line of text from the socket, ending in either
//  CR or LF.  If a CR/LF ends the line, this is treated as 2 separate lines.
//
// Parameters:
//
//  lpszText: Pointer to a string where this function will place the server's
//            response (0 terminated)
//  nMaxLength: Maximum length of buffer to accept text.
//
// Returns:
//
//  TRUE on success
//  FALSE on failure
//
//************************************************************************
// 3/16/98 Carey created.
//************************************************************************

BOOL SocketClientImp::GetLine
(
    LPTSTR              lpszResult,
    int                 nMaxLength
)
{
    int                 nErr, nIndex = 0;
    char                chNext;

    if (nMaxLength == 0)
        return(FALSE);

    while (1)
    {
        nErr = recv (m_socket,&chNext,sizeof(char),0);
    
        switch (nErr)
        {
            case 0:
                // socket closed by host!
                return(FALSE);

            case SOCKET_ERROR:
                // error!
                return(FALSE);

            default:

                // got our data - 0 terminate it

                if (chNext != '\r' && chNext != '\n')
                {
                    if (nIndex < nMaxLength-1)
                        lpszResult[nIndex++] = chNext;
                    else
                    {
                        if (nIndex == nMaxLength-1)
                        {
                            lpszResult[nIndex] = 0;
                            return(TRUE);
                        }
                        else
                        {
                            // out of bounds!

                            return(FALSE);
                        }
                    }
                }
                else
                if (chNext == '\n')
                {
                    lpszResult[nIndex] = 0;
                    return(TRUE);
                }
                break;
        }
    }
}

//************************************************************************
// BOOL Connect(lpszServerName)
//
// Purpose
//
//  This function connects to the specified server and prepares for socket
//  operations.
//
// Parameters:
//
//  lpszServerName: Name of server to connect to.
//
// Returns:
//
//  TRUE on success
//  FALSE on failure
//
//************************************************************************
// 3/16/98 Carey created.
//************************************************************************

BOOL SocketClientImp::Connect
(
    LPTSTR              lpszServerName
)
{
    WORD                    wVersionRequested;
	WSADATA					wsaData; 
	int						err; 
	DWORD					dwAddr;
	SOCKET					sock;
	struct sockaddr_in		stSIN = {0};
    struct hostent FAR *    lpstHostEnt;

    // zero our our socket first

    m_socket = INVALID_SOCKET;

    // now request a version of sockets
	
    wVersionRequested = MAKEWORD(1, 1);
 
	err = WSAStartup(wVersionRequested, &wsaData); 

	if (err != 0)
	{
        // error initializing socket library

        return(FALSE);
	}

    // see if we have a numeric or standard textual address

    if (IsNumericAddress(lpszServerName))
    {
        dwAddr = inet_addr (lpszServerName);        // convert numeric addr to
                                                    // a DWORD form
    }
    else
    {
        // convert www.symantec.com to an internet address

        lpstHostEnt = gethostbyname(lpszServerName);

        if (lpstHostEnt == NULL)
        {
            WSACleanup();

            // error: obtaining INET addr!

            return(FALSE);
        }

        dwAddr = *(LPDWORD)lpstHostEnt->h_addr_list[0];
    }

	if (dwAddr == INADDR_NONE)
	{
        WSACleanup();

        // error: obtaining INET addr!

        return(FALSE);
	}

	// now create a new socket

	sock = socket ( AF_INET, SOCK_STREAM, 0);
    
	if (sock == INVALID_SOCKET)
	{
		WSACleanup();

        // error: obtaining new socket!

        return(FALSE);
	}

    //  connect the socket

	stSIN.sin_family = AF_INET;
    stSIN.sin_port = htons(HTTP_PORT_NUM);
	stSIN.sin_addr.S_un.S_addr = dwAddr;

	err = connect (sock,(sockaddr *)&stSIN,sizeof(stSIN));

	if (err != 0)
	{
        // close our socket

        closesocket (sock);

		WSACleanup();

        // Error: connecting to dest!

        return(FALSE);
    }

    m_socket = sock;

    return(TRUE);
}

//************************************************************************
// BOOL Disconnect(void)
//
// Purpose
//
//  Disconnect from the socket server, close down the socket library, etc.
//
// Parameters:
//
//  None
//
// Returns:
//
//  TRUE on success
//  FALSE on failure
//
//************************************************************************
// 3/16/98 Carey created.
//************************************************************************


BOOL SocketClientImp::Disconnect
(
    void
)
{
    int                 nErr;

    if (m_socket != INVALID_SOCKET)
    {
        nErr = closesocket (m_socket);

        WSACleanup();

        if (nErr != 0)
            return(FALSE);

        return(TRUE);
    }

    return(TRUE);
}

//************************************************************************
// BOOL GetResponse(lpnResponse)
//
// Purpose
//
//  Get the entire response from the server and look for a response code
//  on the last line:
//
// figure out what our response says.  Expected response:
//
// Line 1:  HTTP/1.0 200 OK                 [crlf]
// Line 2:  Content-type: text/html         [crlf]
// Line 3-N: More types:
// [blank line]
// result code such as 1, 2, 3 or 4
//
// Parameters:
//
//  lpnResponse: This is going to be set to a value indicating the status of
//               the back end.  This will only be valid if the return value is
//               TRUE (successful)
//
// Returns:
//
//  TRUE on success
//  FALSE on failure
//
//************************************************************************
// 3/16/98 Carey created.
//************************************************************************

BOOL SocketClientImp::GetResponse
(
    LPINT                       lpnResponse
)
{
    char                        szLine[MAX_HTTP_RESULT_LENGTH];
    int                         i, nMaxLines = 0;

    // figure out what our response says.  Expected response:
    //
    // Line 1:  HTTP/1.0 200 OK                 [crlf]
    // Line 2:  Content-type: text/html         [crlf]
    // Line 3-N: More types:
    // [blank line]
    // result code

    while (nMaxLines < MAX_RESPONSE_LINES)
    {
        // skip over all the header lines first!

        if (GetLine(szLine,MAX_HTTP_RESULT_LENGTH) != TRUE)
        {
            return(FALSE);
        }

        if (szLine[0] == 0)
        {
            // got our CR and we're looking for our result code now!

            for (i=0;i<2;i++)
            {
                // make sure to skip over an additional LF that might be
                // following the CR

                if (GetLine(szLine,MAX_HTTP_RESULT_LENGTH) != TRUE)
                {
                    return(FALSE);
                }

                switch (atoi(szLine))
                {
                    case BACK_END_DATA_ERROR:
                        *lpnResponse = BACK_END_DATA_ERROR;
                        return(TRUE);
                    case BACK_END_SEND_PACKAGE:
                        *lpnResponse = BACK_END_SEND_PACKAGE;
                        return(TRUE);
                    case BACK_END_FP_USE_LU:
                        *lpnResponse = BACK_END_FP_USE_LU;
                        return(TRUE);
                    case BACK_END_BUSY_TRY_LATER:
                        *lpnResponse = BACK_END_BUSY_TRY_LATER;
                        return(TRUE);
                    case BACK_END_ALL_CLEAN:
                        *lpnResponse = BACK_END_ALL_CLEAN;
                        return(TRUE);
                    case 0:
                        // empty line - keep looking!
                        break;
                    default:
                        return(FALSE);
                }
            }

            return(FALSE);
        }

        nMaxLines++;
    }

    return(FALSE);
}

//************************************************************************
// void Release(void)
//
// Purpose
//
//  Does a delete this;
//
// Parameters:
//
//  none
//
// Returns:
//
//  nothing
//
//************************************************************************
// 3/16/98 Carey created.
//************************************************************************

void SocketClientImp::Release
(
    void
)
{
    delete this;
}

//************************************************************************
// SocketClient *CreateInstance(void)
//
// Purpose
//
//  Returns a new instance of the socket client class.
//
// Parameters:
//
//  none
//
// Returns:
//
//  A pointer to a socket client or NULL.
//
//************************************************************************

SocketClient *SocketClientFactory::CreateInstance
(
    void
)
{
    return (new SocketClientImp);
}
