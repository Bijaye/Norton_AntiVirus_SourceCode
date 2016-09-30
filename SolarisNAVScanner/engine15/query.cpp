// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/query.cpv   1.1   04 Sep 1997 19:29:52   DDREW  $
//
// Description:  This file contains the NAVEX QueryInterface function
//               which fills in the provided export table.
//
// Contains: EXTQueryInterface()
//
// See Also: 
//************************************************************************
// $Log:   S:/ENGINE15/VCS/query.cpv  $
// 
//    Rev 1.1   04 Sep 1997 19:29:52   DDREW
// Netware complains about more than one QueryInterface() being exported
// 
//    Rev 1.0   19 May 1997 19:40:54   jsulton
// Initial revision.
//************************************************************************

#include "platform.h"
#include "avtypes.h"
#include "n30type.h"
#include "callbk15.h"
#include "navex15.h"

#ifndef SYM_NLM
EXTSTATUS FAR WINAPI NLOADDS EXTQueryInterface(DWORD dwInterfaceID,
                                               LPLPVOID lplpvExportTable)
#else
EXTSTATUS FAR WINAPI NLOADDS EXTQueryInterfaceEng(DWORD dwInterfaceID,
                                               LPLPVOID lplpvExportTable)
#endif
{
    PEXPORT15_TABLE_TYPE *lplpTable15;

    switch (dwInterfaceID)
    {
        case NAVEX15_INTERFACE:
 
            lplpTable15 = (PEXPORT15_TABLE_TYPE *)lplpvExportTable;

            *lplpTable15 = &export_table;

            return (EXTSTATUS_OK);

        default:
            
            return (EXTSTATUS_INIT_ERROR);
    }
}
