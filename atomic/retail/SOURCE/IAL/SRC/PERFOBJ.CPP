// Copyright 1991-1997 Symantec Corporation, Peter Norton Product Group
//************************************************************************
//  $Header:   S:/IAL/VCS/perfobj.cpv   1.1   25 Jun 1997 17:24:18   DSACKIN  $
//
//  Description:
//      This file contains performance database object manipulation functions
//
//************************************************************************
//
//  $Log:   S:/IAL/VCS/perfobj.cpv  $
// 
//    Rev 1.1   25 Jun 1997 17:24:18   DSACKIN
// Ported from Marconi
// 
//    Rev 1.1   24 Apr 1997 11:01:54   pvenkat
// No Difference.
// 
//
//************************************************************************
// NOTE:    This is from Matt Pietrek's Under the Hood column of MSJ
//************************************************************************

#ifndef	WIN32_LEAN_AND_MEAN
	#define	WIN32_LEAN_AND_MEAN
#endif

#include	<windows.h>
#include	<winperf.h>
#include	<stdlib.h>

#pragma		hdrstop

#include	"titledb.h"
#include	"perfobj.h"
#include	"objinst.h"
#include	"makeptr.h"


CPerfObject::CPerfObject (PPERF_OBJECT_TYPE const pObjectList, CPerfTitleDatabase *const pPerfCounterTitles)
{
	m_pObjectList			= pObjectList;
	m_pPerfCounterTitles	= pPerfCounterTitles;
}

CPerfObjectInstance *
CPerfObject::GetFirstObjectInstance (void)
{
	m_currentObjectInstance = 0;
	if (m_currentObjectInstance >= GetObjectInstanceCount () )
		return NULL;

	// Point at the first PERF_INSTANCE_DEFINTION
	m_pCurrentObjectInstanceDefinition = MakePtr (PPERF_INSTANCE_DEFINITION,
													m_pObjectList,
													m_pObjectList->DefinitionLength );
	return new CPerfObjectInstance (m_pCurrentObjectInstanceDefinition,
									MakePtr (PPERF_COUNTER_DEFINITION,
											m_pObjectList,
											m_pObjectList->HeaderLength),
									m_pObjectList->NumCounters,
									m_pPerfCounterTitles,
									(m_pObjectList->NumInstances == PERF_NO_INSTANCES) ? 
											TRUE : FALSE);
}


CPerfObjectInstance *
CPerfObject::GetNextObjectInstance (void)
{
	if (m_pObjectList->NumInstances == PERF_NO_INSTANCES)
		return NULL;

	if (++m_currentObjectInstance >= GetObjectInstanceCount ())
		return NULL;

	// Advance to the next PERF_INSTANCE_DEFINITION in the list.  However,
	// following the current PERF_INSTANCE_DEFINITION is the counter data,
	// which is also of variable length.  So, we gotta take that into 
	// account when finding the next PERF_INSTANCE_DEFINITION

	// First, get a pointer to the counter data size field.
	PDWORD pCounterDataSize = MakePtr (PDWORD, 
										m_pCurrentObjectInstanceDefinition,
										m_pCurrentObjectInstanceDefinition->ByteLength);
	
	// Now we can point at the next PPERF_INSTANCE_DEFINITION
	m_pCurrentObjectInstanceDefinition = MakePtr (PPERF_INSTANCE_DEFINITION,
													m_pCurrentObjectInstanceDefinition,
						m_pCurrentObjectInstanceDefinition->ByteLength + *pCounterDataSize);

	// Create a CPerfObjectInstance base around the PPERF_INSTANCE_DEFINITION
	return new CPerfObjectInstance (m_pCurrentObjectInstanceDefinition,
										MakePtr (PPERF_COUNTER_DEFINITION,
												m_pObjectList,
												m_pObjectList->HeaderLength),
										m_pObjectList->NumCounters,
										m_pPerfCounterTitles,
										FALSE);
}

BOOL
CPerfObject::GetObjectTypeName (PTSTR pszObjectTypeName, DWORD dwSize)
{
	PTSTR pszName = m_pPerfCounterTitles->GetTitleStringFromIndex (m_pObjectList->ObjectNameTitleIndex);

	if (!pszName)
		return FALSE;

	lstrcpyn (pszObjectTypeName, pszName, dwSize);
	return TRUE;
}
