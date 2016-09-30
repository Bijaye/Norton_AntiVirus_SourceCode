///////////////////////////////////////////////////
//  This module formats log record data parameters
//

#include "stdafx.h"
#include "rdefs.h"
#include "iacommon.h"
#include "iam.h"
#include "winsock.h"
#include "SymNetDriverAPI.h"
#include "IDSSignatureInfo.h"
#include "ccEventId.h"
#include "ccSerializableEventEx.h"
#include "ccResourceLoader.h"
#include "resource.h"
#include "..\SNLogRes\ResResource.h"

#include "SNLogLogViewerSymNetiEvent.h"
#include "SNLookup.h"

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

//
// NOTE: Please keep the ServiceName's unique
//
static struct _tagServiceEntries {
	PCHAR		pServiceName;		// The actual service name
	int 		portNumber;			// The port number 
} ServiceEntries[] = {
	{	"http", 		80 	},		// HTTP
	{	"www", 			80	},		// ""
	{	"www-http", 	80	},		// ""
	{	"http-alt", 	8001},		// ""
	{	"http-alt-1", 	8008},		// ""
	{	"http-proxy", 	8080},		// Often used as HTTP proxy
	{	"http-proxy-1",	8088},		// Often used as HTTP proxy
	{	"http-mgmt", 	280	},		// HTTP management
	{	"https", 		443	},		// HTTP server
	{	"gss-http", 	488	},		// HTTP misc
	{	"fmpro-http", 	591	},		// ""
	{	"http-rpc-epmap", 593 },	// ""
	{	"bootps",		67 },		// Bootstrap Protocol Server
	{   "bootpc",		68 },		// Bootstrap  Protocol Client
	{	"dcom",			135 },		// Microsoft RPC end point to end point mapping
	{	"imap",			143 },		// Internet Message Access Protocol
	{	"ldap",			389 },		// Lightweight Directory Access Protocol
	{	"video",		458 },		// Connectix and Quick Time Streaming protocols
	{	"video-1",		545 },		// Connectix and Quick Time Streaming protocols
	{	"rtsp",			554 },		// Real Time Stream Protocol
	{	"mountd",		709 },		// NFS mount daemon
	{	"pcnfsd",		721 },		// PC NFS Deamon
	{	"irc",			194 },		// Internet Relay Chat protocol
	{	"irc-serv",		529 },		// ""
	{	"ircs",			994 },		// ""
	{	"ircu",			6665 },		// ""
	{	"ircu-1",		6666 },		// ""
	{	"ircu-2",		6667 },		// ""
	{	"ircu-3",		6668 },		// ""
	{	"ircu-4",		6669 },		// ""
	{	"socks",		1080 },		// Socks
	{	"lotusnotes",	1352 },		// Lotus
	{	"ms-sql-s",		1433 },		// Microsoft misc
	{	"ms_sql-m",		1434 },		// ""
	{	"ms-sna-server",1477 },		// ""
	{	"ms-sna-base",	1478 },		// ""
	{	"orasrv",		1525 },		// Oracle
	{	"tdisrv",		1527 },		// ""
	{	"coauthor",		1529 },		// ""
	{	"nsvt",			1537 },		// HP's NSVT native protocol
	{	"nsvt-stream",	1570 },		// HP's NSVT TCP stream mode
	{	"remote-winsock",1745 },	// Remote Winsock Proxy
	{	"netshow",		1755 },		// Microsoft's NetShow
	{	"msnp",			1863 },		// Microsoft's MSN messaging
	{	"icq",			4000 },		// ICQ chat program
	{	"aol",			5190 },		// America Online
	{	"aol-1",		5191 },		// America Online
	{	"aol-2",		5192 },		// America Online
	{	"aol-3",		5193 },		// America Online
	{	"aol-4",		11523 },	// America Online
	{	"Back-Orifice",	31337 },	// Back Orifice
	{	"Back-Orifice-2000",	54320 },	// Back Orifice 2000
	{	"Back-Orifice-2000-1",	54321 },	// Back Orifice 2000
	{	"NetBus",		12345 },	// Netbus
	{	"NetBus-2",		12346 },	// Netbus2
	{	"NetBus-Pro",	20034 },	// Netbus Pro
	{	"Backdoor-g-1",	1243 },			//* Backdoor-g Trojan
	{	"Backdoor-g-2",	6711 },			//* Backdoor-g Trojan
	{	"Backdoor-g-3",	6776 },			//* Backdoor-g Trojan
	{	"pc-anywhere-data",		5631 },	// pcAnywhere data port
	{	"pc-anywhere-status",	5632 },	// pcAnywhere status port
	{	"xserver",		6000 },		// X Server
	{	"vdolive",		7000 },		// VDOLive Player
	{	"msbd",			7007 },		// Microsoft MSBD (related to NetShow)
	{	"realaudio",	7070 },		// Real Networks Real Audio
	{	"quake",		26000 },	// Quake server game
	{	"quake2",		27910 },	// Quake2 server game
	{	"quake2-2",		27911 },	// Quake2 server game
	{	"ssdp",	1900 },	// simple service discovery protocol
	{	"", 			0	},
};


//---------------------------------------------------------------------------
//
//  int IamGetServByPort( 	PCHAR pServiceName,
//							int	portNumber,
//							PCHAR protocol );
//
// Convert a port number into the string representation of the service name.
// On return, if a successful lookup was performed, return 0, failure returns
// non-zero 
//
//---------------------------------------------------------------------------

int NisEvt::CLogViewerSymNetiEvent::IamGetServByPort( 	PCHAR pServiceName,
						int	portNumber,
						PCHAR protocol ) {

	int returnValue = 0;   // Assume success
	SERVENT	*pServent;
	int		serviceIndex = 0;
	BOOL		bMatchFound = FALSE;

	//
	//  Validate the user supplied service name pointer and port number
	//
	if ( portNumber && pServiceName ) {
		//
		// Call winsock to do the service port lookup
		//
		if (( pServent = getservbyport( portNumber, protocol )) != NULL ) {
			//
			// Copy the result into the user supplied buffer
			//
			strcpy( pServiceName, pServent->s_name );
		} else {
			//
			// Do our own lookup into our internal service entry file
			//
			while ( 	!bMatchFound &&
						ServiceEntries[ serviceIndex ].portNumber ) {
				//
				// Is this service entry the one?
				//
				if ( (int)ServiceEntries[ serviceIndex ].portNumber == (int)htons((short)portNumber) ) {
					//
					// Match found.   Copy the service entry
					//
					strcpy( 	pServiceName, 
								ServiceEntries[ serviceIndex ].pServiceName );
					bMatchFound = TRUE;
				} else {
					//
					// Lookup failed, get the next entry
					//
					serviceIndex++;
				}
			}
			//
			// If we did not find a match, set the
			// error value
			//
			if ( !bMatchFound ) {
				//
				// No match
				//
				returnValue = -1;
			}
		}
	} else {
		//
		// NULL pointer to port number
		//
		returnValue = -1;
	}
	//
	// Return an answer
	//
	return returnValue;
}

//---------------------------------------------------------------------------
// eventLoadString
//---------------------------------------------------------------------------
//
int NisEvt::CLogViewerSymNetiEvent::eventLoadString(
	HINSTANCE h,
	DWORD dwSNid,
	LPSTR buf,
	size_t max,
	LPDWORD lpError
)
{
	int cnt=0;
	DWORD dError;
	DWORD id=-1;

	*lpError = 0;

	// Convert the SymNetDrv string ID into the corresponding NisEvt ID
	VERIFY(LookupStringID (dwSNid, id) == S_OK);
	if (g_ResLoader.LoadString(id, buf, max))
	{
		// buf is an LPSTR so use strlen
		cnt = strlen(buf);
		return(cnt);
	}

	dError = GetLastError();

	if (ERROR_RESOURCE_DATA_NOT_FOUND == dError
	    || ERROR_RESOURCE_NAME_NOT_FOUND == dError
		|| 0 == dError
	 ) {
        if (g_ResLoader.LoadString(IDS_INVALID_STR_ID, buf, max))
		{
			// buf is an LPSTR so use strlen
			cnt = strlen(buf);
			return(cnt);
		}
	}
	*lpError = GetLastError();
	return(0);
}


void NisEvt::CLogViewerSymNetiEvent::EventGetIDSName(HINSTANCE hModule, WORD wSigID, char* pszName, DWORD cbName)
{
	IDSSIGNATUREINFOV5 stIDSSignatureInfo;
	SYMIDS_ERC enErc;

	stIDSSignatureInfo.ui32Version = IDSSIGNATUREINFO_VER_50;
		/* fill in the version we need */

	stIDSSignatureInfo.ui32SignatureID = wSigID;
		/* fill in the signature ID of the signature we want info for */

	DWORD dwHandle;

	m_pSymNetiCallTable->pSNLockIDSConfig(&dwHandle);
	
	m_pSymNetiCallTable->pSNGetIDSSignature (dwHandle, (BYTE *)&stIDSSignatureInfo, &enErc);

	if (enErc == SYMIDS_SUCCESS)
	{
		memcpy (pszName, stIDSSignatureInfo.abySignatureName, IDS_SIGNATURE_NAME_SIZE_MAX + 1);
	}
	else
	{
		_tcscpy(pszName, _T(""));
	}

	m_pSymNetiCallTable->pSNUnlockIDSConfig (dwHandle);
}

void NisEvt::CLogViewerSymNetiEvent::EventGetIDSThreat(HINSTANCE hModule, WORD wSigID, char* pszThreat, DWORD cbThreat)
{
	IDSSIGNATUREINFOV5 stIDSSignatureInfo;
	SYMIDS_ERC enErc;

	stIDSSignatureInfo.ui32Version = IDSSIGNATUREINFO_VER_50;
		/* fill in the version we need */

	stIDSSignatureInfo.ui32SignatureID = wSigID;
		/* fill in the signature ID of the signature we want info for */

	DWORD dwHandle;

	m_pSymNetiCallTable->pSNLockIDSConfig(&dwHandle);

	m_pSymNetiCallTable->pSNGetIDSSignature (dwHandle, (BYTE *)&stIDSSignatureInfo, &enErc);

	DWORD dwErr = 0;

	switch (stIDSSignatureInfo.ui32Priority)
	{
		case 1:
			g_ResLoader.LoadString(IDS_LOW_STRING, pszThreat, cbThreat);
			break;
		case 3:
			g_ResLoader.LoadString(IDS_HIGH_STRING, pszThreat, cbThreat);
			break;
		default:
			g_ResLoader.LoadString(IDS_MEDIUM_STRING, pszThreat, cbThreat);
			break;
	}

	m_pSymNetiCallTable->pSNUnlockIDSConfig (dwHandle);
}

const char *ipFormatStr = "%u.%u.%u.%u";
//---------------------------------------------------------------------------
//
//  int convertIPAddressToString(	IP_ADDRESS ipAddress,
//									PVOID		resultBuffer,
//									DWORD		resultBufferLength,
//									DWORD		resolveUsingTypes,
//									PDWORD		pResolveResult										
//		
// This function converts an IP address into a string by looking up
// the IP address in our DNS cache
//
//---------------------------------------------------------------------------

int NisEvt::CLogViewerSymNetiEvent::convertIPAddressToString(	IP_ADDRESS  ipAddress,
								LPSTR		resultBuffer,
								DWORD		resultBufferLength,
								DWORD		resolveUsing,
								PDWORD		pResolveResult ) 
{									
	int result = LOOKUP_SUCCESS;
	DWORD		filterResult = 0;
	BOOL		bLookupWorked = FALSE;
	//
	// First see if we should try to resolve using
	// DNS cache lookup
	//


	if (( resolveUsing & DNS_CACHE_LOOKUP ) || ( resolveUsing & WINS_CACHE_LOOKUP )) 
	{
		//
		// Make the ioctl call to the Dns filter
		//
		if (m_pSymNetiCallTable->pSNAddressToName(ipAddress, resultBuffer, resultBufferLength) == S_OK) 
		{
			//
			// We resolve a name from the cache
			//
			bLookupWorked = TRUE;
			if ( pResolveResult ) 
			{
				//
				// Default string conversion worked
				//
				*pResolveResult = resolveUsing;
			}

		} 
		else 
		{
			//
			// Iocontrol call failed
			//
//			DEBUG_BREAK;
		}
	}

	//
	// See if the DNS or the WINS cache lookup worked
	//
	if ( !bLookupWorked || ( resolveUsing & DEFAULT_STRING_LOOKUP )) 
	{
		//
		// Make sure the result buffer has at least 16 bytes
		//
		if ( resultBufferLength < 16 ) 
		{
			//
			// The lookup failed
			//
			result = LOOKUP_FAILURE;
		} 
		else 
		{
			PBYTE	pIPAddress = (PBYTE)&ipAddress;
			// Copy a string version of the IP address
			//
			wsprintf(	resultBuffer, 
						ipFormatStr, 
						*pIPAddress, *(pIPAddress+1), *(pIPAddress+2), *(pIPAddress+3) );
			//
			// See if the user wants to know how we converted the IP address into
			// a string
			//
			if ( pResolveResult ) {
				//
				// Default string conversion worked
				//
				*pResolveResult = DEFAULT_STRING_LOOKUP;
			}
		}
	}
	//
	// Return the result
	//
	return result;
} 


#define MAX_ARGS 32
//---------------------------------------------------------------------------
// EventFormatArgumentsEx
//
// Convert the variable arguments of an event to strings. Returns the 
// number of elements in string array
//---------------------------------------------------------------------------
//
DWORD CALLBACK NisEvt::CLogViewerSymNetiEvent::EventFormatArgumentsEx(
	PBYTE data,				//ptr to variable length data (array of SNDATAVALUE-DATA)
	HINSTANCE hModule,		//handle of module to use for reading resource
							// related event types, e.g. EV_T_STR_ID
	LPSTR pBuffer,			//output buffer
	DWORD dMax,				//size of output buffer
	PDWORD pArgsWritten		//output - number of arguments returned in the array
)
{
	LPSTR *array = (LPSTR *)pBuffer;			//ptr to array of argument ptrs
	LPSTR p = pBuffer + ((MAX_ARGS+1) * sizeof(LPSTR *));
												//ptr to arguments
	LPSTR end = pBuffer + dMax;
	WORD dataType;
	WORD portNumber;
	DWORD strIndex = 0;	
	DWORD temp;
	DWORD cnt;
	DWORD error;
	DWORD base;
	DWORD mask;
	DWORD i;
	DWORD secs;
	BOOL bFirst = TRUE;
	BOOL bDone = FALSE;
	DWORD dError = EFA_OK;
	QWORD q;
	DWORD hrs;
	DWORD mins;
	DWORD msecs;
	WORD  ui16SignatureID;
	WORD  ui16Protocol;
	char  name[257];
	char  IPString[20]; 


	ASSERT(m_pSymNetiCallTable);

	//
	// convert the various RNS event data types to strings for compatability
	// with the NT event logging system
	//
	while (bDone == FALSE					
			&& dError == EFA_OK
			&& strIndex < MAX_ARGS
	) {
		//
		// get the event data type
		//
		dataType = ((PSNDATAVALUE)data)->byDataType;
		data += sizeof(SNDATAVALUE);
	
		switch (dataType) {

		case SymNeti::CSNLogEventEx::eEV_T_PORT:
			portNumber = *((PWORD)data);
			data+=sizeof(WORD);
			error = TRUE;
			if ( !IamGetServByPort( name, (int)htons(portNumber), 0) ) {
				//
				// Check to see if there is room and then point 
				// 'array' to the to the service name and then bump
				// internal 'p' pointer
				//
				if ( end - p > (int)(strlen(name)+1)) {
					LPSTR nameend;
					strcat(name,"(");
					nameend = name + strlen(name);
					itoa(portNumber,nameend,10);
					strcat(name,")");
					strcpy( p, name );
					array[strIndex++] = p;
					p += (strlen(name)+1);
					error = FALSE;
				}
			}
			if ( error ) {
				//
				// Unable to convert port number into a service name
				//
				if (end - p > 3 * sizeof(WORD)) {
					array[strIndex++] = p;
					p += wsprintf(p, "%u", portNumber);
					p++;
				} else {
					dError = EFA_ERR_BUFFER_SIZE;
				}
			}
			break;  
		case SymNeti::CSNLogEventEx::eEV_T_IP:
			{


				IP_ADDRESS  ip;
				DWORD resolveResult;
				error = TRUE;
				//
				// Figure out how many more bytes are left in the buffer
				//  + (xxx.xxx.xxx.xxx)
				i = end - p - 18 ;	
				//
				// Convert the IP address to a string
				//

				resolveResult = 0;
				ip = *(IP_ADDRESS *)&data[0];
				if ( convertIPAddressToString(	ip,
							        			 p,		// Buffer to write string into
									 			 i,		// Number of bytes left in the buffer
		                     					 DNS_CACHE_LOOKUP | WINS_CACHE_LOOKUP,
												 &resolveResult) != LOOKUP_FAILURE ) {
					// ignore local host, and IPs that could not be resolved.
					if ((ip != 0x0100007f) && (resolveResult != DEFAULT_STRING_LOOKUP)){
						strcpy(IPString,"(");
						wsprintf(&IPString[1], ipFormatStr, data[0], data[1], data[2], data[3]);
						strcat(IPString,")");
						strcat(p,IPString);
					}


					array[strIndex++] = p;   // Point to new string for the buffer
					i = strlen(p)+1;		 // How many bytes the return result was
					p += i;					 // Accumulate the total number of bytes 
				} else {
					dError = EFA_ERR_BUFFER_SIZE;
				}
				data += sizeof(SNLOG_ADDRESS);

			}
			break;

		case SymNeti::CSNLogEventEx::eEV_T_ASCII:
			//
			// if it's a string don't copy it just set set the array
			// pointer to the data pointer
			//
			array[strIndex++] = (LPSTR)data;
			data += strlen((LPSTR)data) + 1;             		//get size of string
			break;  

		case SymNeti::CSNLogEventEx::eEV_T_STR_ID:
			temp = *((PDWORD)data); 					//get the string id
			data += sizeof(PDWORD);

			if (hModule) {
				cnt = eventLoadString(hModule, temp, p, end - p, &dError);
				if (cnt) {
					array[strIndex++] = p;
					p += cnt + 1;
				}
			} else {
				dError = EFA_ERR_NO_HMODULE;
			}
			break;

		case SymNeti::CSNLogEventEx::eEV_T_WORD:
			if (end - p > 3 * sizeof(WORD)) {
				array[strIndex++] = p;
				p += wsprintf(p, "%u", *((PWORD)data));
				data += sizeof(WORD);
				p++;
			} else {
				dError = EFA_ERR_BUFFER_SIZE;
			}
			break;  

		case SymNeti::CSNLogEventEx::eEV_T_INT:
			if (end - p > 3 * sizeof(INT)) {
				array[strIndex++] = p;
				p += wsprintf(p, "%i", *((PINT)data));
				data += sizeof(INT);
				p++;
			} else {
				dError = EFA_ERR_BUFFER_SIZE;
			}
			break;  


		case SymNeti::CSNLogEventEx::eEV_T_DWORD:
			if (end - p > 3 * sizeof(DWORD)) {
				array[strIndex++] = p;
				p += wsprintf(p, "%lu", *((PDWORD)data));
				data += sizeof(DWORD);
				p++;
			} else {
				dError = EFA_ERR_BUFFER_SIZE;
			}
			break;  

		case SymNeti::CSNLogEventEx::eEV_T_ELAPSE_TIME:
			if (end - p > 3 * sizeof(QWORD)) {
				array[strIndex++] = p;
				q = *((PQWORD)data);
				data += sizeof(QWORD);
				//
				// n.b. 100 ns = 10^-7 secs
				//
				msecs = (DWORD)(q / 10000i64);
				hrs = msecs / (3600UL * 1000UL);
				msecs -= hrs * (3600UL * 1000UL);
				mins = msecs / (60UL * 1000UL);
				msecs -= mins * (60UL * 1000UL);
				secs = msecs / 1000UL;
				msecs -= secs * 1000UL;
				if (hrs) {
					p += wsprintf(p, "%u:%02u:%02u.%03u", hrs, mins, secs, msecs);
				} else if (mins) {
					p += wsprintf(p, "%u:%02u.%03u", mins, secs, msecs);
				} else if (secs) {
					p += wsprintf(p, "%u.%03u", secs, msecs);
				} else {
					p += wsprintf(p, "0.%03u", msecs);
				}
				p++;
			} else {
				dError = EFA_ERR_BUFFER_SIZE;
			}
			break;  

		case SymNeti::CSNLogEventEx::eEV_T_UINT_HEX:
		case SymNeti::CSNLogEventEx::eEV_T_DWORD_HEX:
			if (end - p > 2 * sizeof(DWORD) + 2) {
				array[strIndex++] = p;
				p += wsprintf(p, "0x%08x", *((PDWORD)data));
				data += sizeof(DWORD);
				p++;
			} else {
				dError = EFA_ERR_BUFFER_SIZE;
			}
			break;  
            
		case SymNeti::CSNLogEventEx::eEV_T_LONG:
			if (end - p > 3 * sizeof(LONG)) {
				array[strIndex++] = p;
				p += wsprintf(p, "%li", *((PLONG)data));
				data += sizeof(LONG);
				p++;
			} else {
				dError = EFA_ERR_BUFFER_SIZE;
			}
			break;  


		case SymNeti::CSNLogEventEx::eEV_T_BINARY:

			temp = *((PDWORD)data);				//get the size
			data += sizeof(DWORD);
			if ((UINT)(end - p) > 2 * temp * sizeof(BYTE) + 1) {
				array[strIndex++] = p;
				if (temp) {
					for (i=0; i<temp; i++) {
						p += wsprintf(p, "%02x", *data++);
						*p++ = ' ';
					}
				} else {
					// The CommonClient CResourceLoader LoadString function
					// doesn't return the number of characters loaded so we
					// will use the WinAPI LoadString
					p += LoadString(g_ResLoader.GetResourceInstance(), IDS_NONE, p, end - p);
				}
				*p++ = 0;
			}
			break;  

		case SymNeti::CSNLogEventEx::eEV_T_FLAGS:
			base = *((PDWORD)data);				//get the base string id
			data += sizeof(DWORD);
			mask = *((PDWORD)data); 				//get the mask for flags
			data += sizeof(DWORD);

			if (end - p) { 
				if (mask) {
					for (i=0; i<32; i++) {
					
						if (mask & (1<<i)) {
							//
							// if this isn't the 1st flag add a ", "
							//
							if (bFirst == FALSE) {
								if (end - p > 3) {
									*p++ = ' ';
								} else { 
									break;
								}
							}
							//
							// get the string corresponding to this flag
							//
							if (hModule) {
							 	if ((cnt = eventLoadString(hModule, base + i, p, end - p - 1, &dError)) != 0) {
									if (bFirst == TRUE) {
										array[strIndex++] = p;
										bFirst = FALSE;
									}
									p += cnt;
								} else {
									break;
								}
							} else {
								dError = EFA_ERR_NO_HMODULE;
								break;
							}
						}
					}

				} else {
					array[strIndex++] = p;

					// The CommonClient CResourceLoader LoadString function
					// doesn't return the number of characters loaded so we
					// will use the WinAPI LoadString
					p += LoadString(g_ResLoader.GetResourceInstance(), IDS_NONE, p, end - p);
				}
				//
				// include a NULL
				//
			 	*p++ = 0;

			} else {
				dError = EFA_ERR_BUFFER_SIZE;
			}

			break;

   		case SymNeti::CSNLogEventEx::eEV_T_END:
   			bDone = TRUE;
   			break;

		case SymNeti::CSNLogEventEx::eEV_T_SKIP_BYTE:
			bDone = FALSE;
			dError = EFA_OK;
			break;
		case SymNeti::CSNLogEventEx::eEV_T_IDS_SIG_ID:
			{
				ui16SignatureID = *((PWORD)data);
				data += sizeof(WORD);

				// Lookup the name of this item...
				EventGetIDSName(hModule, ui16SignatureID, (char*)p, 255);
				
				array[strIndex++] = p;
				p += strlen (p) + 1;

				break;
			}
		case SymNeti::CSNLogEventEx::eEV_T_IDS_THREAT:
			{
				ui16SignatureID = *((PWORD)data);
				data += sizeof(WORD);

				// Lookup the threat level of this ids attack...
				EventGetIDSThreat(hModule, ui16SignatureID, (char*)p, 255);
				
				array[strIndex++] = p;
				p += strlen (p) + 1;

				break;
			}
		case SymNeti::CSNLogEventEx::eEV_T_PROTOCOL:
			{
				ui16Protocol = *((PWORD)data);
				data += sizeof(WORD);

				GetProtocolString(ui16Protocol, p, end - p);
				
				array[strIndex++] = p;
				p += strlen (p) + 1;

				break;
			}
		default:
			dError = EFA_ERR_UNKNOWN_TYPE;
			break;  
		}
	}

	array[strIndex] = NULL;
	*pArgsWritten = strIndex;

	return(dError);
}

void NisEvt::CLogViewerSymNetiEvent::GetProtocolString(WORD wProtocol, LPSTR szBuf, DWORD dwSize)
{
	static struct
	{
		WORD  wProto;
		LPSTR szName;
	} arProto[] =
	{
		{ 6,  "TCP"},
		{ 17, "UDP"},
		{ 1,  "ICMP"},
		{ 2,  "IGMP"},
		{ 0,  "IP"}
	};

	if (dwSize < 3)
		return;

	for (int x=0; x < CCDIMOF(arProto); x++)
		if (arProto[x].wProto == wProtocol)
		{
			if (strlen(arProto[x].szName) < dwSize)
			{
				strcpy(szBuf, arProto[x].szName);
				return;
			}

			break;
		}

	ASSERT(FALSE);
	wsprintf (szBuf,"%d", wProtocol);
}

