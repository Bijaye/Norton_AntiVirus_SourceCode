// Copyright 2004 Symantec Corporation. All rights reserved.

#ifndef _ndkNDSServices_h_
#define _ndkNDSServices_h_

#include "ndkDefinitions.h"

#ifdef __cplusplus
extern "C" {
#endif


/*NDSServices*/
//NWDSAddObject
//NWDSAllocBuf
//NWDSAuthenticate
//NWDSCreateContextHandle
//NWDSFreeBuf
//NWDSFreeContext
//NWDSGenerateObjectKeyPair
//NWDSGetContext
//NWDSGetServerDN
//NWDSInitBuf
//NWDSLogin
//NWDSLogout
//NWDSMapNameToID
//NWDSModifyObject
//NWDSOpenStream
//NWDSPutAttrName
//NWDSPutAttrVal
//NWDSPutChange
//NWDSRemoveObject
//NWDSSetContext
/*following APIs are only used in vpstart.nlm which we have already ported without the platform related calls. stubs not needed*/
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSAddObject(NWDSContextHandle context, pnstr8 objectName, pnint32 iterationHandle, nbool8 more, pBuf_T objectInfo);  
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSAllocBuf(size_t size, ppBuf_T buf);  
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSAuthenticate(NWCONN_HANDLE conn, nflag32 optionsFlag, pNWDS_Session_Key_T sessionKey);  
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSCreateContextHandle(NWDSContextHandle N_FAR  *newHandle);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSFreeBuf(pBuf_T buf);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSFreeContext(NWDSContextHandle   context);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSGenerateObjectKeyPair(NWDSContextHandle contextHandle, pnstr8 objectName, pnstr8 objectPassword, nflag32 pwdOption);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSGetContext(NWDSContextHandle context, nint key, nptr value);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSGetServerDN(NWDSContextHandle context, NWCONN_HANDLE connHandle, pnstr8 serverDN);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSInitBuf(NWDSContextHandle context, nuint32 operation, pBuf_T buf);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSLogin(NWDSContextHandle context, nflag32 optionsFlag, pnstr8 objectName, pnstr8 password, nuint32 validityPeriod);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSLogout(NWDSContextHandle context);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSMapNameToID(NWDSContextHandle context, NWCONN_HANDLE connHandle, pnstr8 object, pnuint32 objectID);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSModifyObject(NWDSContextHandle context, pnstr8 objectName, pnint32 iterationHandle, nbool8 more, pBuf_T changes);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSOpenStream(NWDSContextHandle context, pnstr8 objectName, pnstr8 attrName, nflag32 flags, NWFILE_HANDLE N_FAR *fileHandle);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSPutAttrName(NWDSContextHandle context, pBuf_T buf, pnstr8 attrName);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSPutAttrVal(NWDSContextHandle context, pBuf_T buf, nuint32 syntaxID, nptr attrVal);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSPutChange(NWDSContextHandle context, pBuf_T buf, nuint32 changeType, pnstr8 attrName);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSRemoveObject(NWDSContextHandle context, pnstr8 object);
//N_EXTERN_LIBRARY (NWDSCCODE) NWDSSetContext(NWDSContextHandle context, nint key, nptr value);

#ifdef __cplusplus
}
#endif
  
#endif // _ndkNDSServices_h_
