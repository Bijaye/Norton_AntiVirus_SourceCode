// DFResource.h: interface for the CDFResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERFMON_H__9870F913_4BE1_11D2_B96F_0004ACEC31AA__INCLUDED_)
#define AFX_PERFMON_H__9870F913_4BE1_11D2_B96F_0004ACEC31AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include <winperf.h>

#define TOTALBYTES    8192
#define BYTEINCREMENT 1024

class CPerformanceMonitor : public CObject  
{
public:
	LONG m_ErrorCode;
	BOOL m_DataValid;
	void GetPerformanceData();
	CPerformanceMonitor(CString &machineName, CString &ntMachineName, CString &userName);
	virtual ~CPerformanceMonitor();
    DWORD availableMemoryCount;
    DWORD commitedMemoryCount;
    DWORD maxCommitedMemoryCount;
    int processorTime;
    int kernelTime;
    int userTime;
    CString diskFreeString;
    CString systemUpTimeStr;
    BOOL diskSpaceLow;
    BOOL memoryLow;

private:
	LONG GetAIXPerformanceData();
    LONG systemUpTime;
	CString m_UserName;
	CString m_NTMachineName;
    LONG GetNameStrings( char * counterList, char * machine);
    PPERF_COUNTER_DEFINITION NextCounter(PPERF_COUNTER_DEFINITION PerfCntr);
    PPERF_COUNTER_DEFINITION FirstCounter(PPERF_OBJECT_TYPE PerfObj);
    PPERF_INSTANCE_DEFINITION NextInstance(PPERF_INSTANCE_DEFINITION PerfInst);
    PPERF_INSTANCE_DEFINITION FirstInstance( PPERF_OBJECT_TYPE PerfObj );
    PPERF_OBJECT_TYPE NextObject( PPERF_OBJECT_TYPE PerfObj );
    PPERF_OBJECT_TYPE FirstObject( PPERF_DATA_BLOCK PerfData );

    CString m_MachineName;
    HKEY m_hKey;
    LPSTR lpNameStrings;
    LPSTR *lpNamesArray;
    DWORD processorTimeIndex;
    DWORD userTimeIndex;
    DWORD kernelTimeIndex;
    DWORD availableByteIndex;
    DWORD commitedByteIndex;
    DWORD commitLimitIndex;
    char m_CounterString[128];
    BOOL firstTime;
    PPERF_DATA_BLOCK PerfData;
    DWORD BufferSize;
    DWORD newBufferSize;
    LONGLONG oldTime;
    LONGLONG oldProcessorCount;
    LONGLONG oldUserTimeCount;
    LONGLONG oldKernelTimeCount;
    DWORD systemUpTimeIndex;
    LONGLONG rawSystemUpTime;
    LONGLONG systemUpPerfTime;
    LONGLONG systemUpPerfFreq;
    DWORD diskFreeIndex;
    DWORD diskMBFreeIndex;
    DWORD diskFreeData;
    DWORD diskFreeBase;
    DWORD diskMBFree;
};

typedef CTypedPtrArray<CObArray, CPerformanceMonitor*> CPerformanceMonitorList;

#endif // !defined(AFX_PERFMON_H__9870F913_4BE1_11D2_B96F_0004ACEC31AA__INCLUDED_)
