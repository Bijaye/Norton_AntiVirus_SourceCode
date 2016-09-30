// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef __REALTIME_H__
#define __REALTIME_H__

BOOL StartSERT(void);
void EndSERT(void);
DWORD MEC_BeginRTSWatch(PROCESSRTSNODE pfnProcessRTSNode, PSNODE pSNode, void *pContext);
DWORD MEC_StopRTSWatches(void);
DWORD MEC_ReloadRTSConfig(void);

#endif //__REALTIME_H__