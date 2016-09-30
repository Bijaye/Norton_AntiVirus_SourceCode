// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/query.cpv   1.2   06 Aug 1998 13:10:58   MKEATIN  $
//
// Description:  This file contains the NAVEX QueryInterface function
//               which fills in the provided export table.
//
// Contains: EXTQueryInterface()
//
// See Also: 
//************************************************************************
// $Log:   S:/NAVEX/VCS/query.cpv  $
// 
//    Rev 1.2   06 Aug 1998 13:10:58   MKEATIN
// Added the FAR keyword to a couple of pointers.
// 
//    Rev 1.1   08 May 1997 12:50:52   MKEATIN
// Now we set the value of lplpvExportTable to the internal export_table. 
// So, apps no longer need to allocated memory for an export table since 
// they'll all point to this internal export_table (see navex15.cpp).
// 
//    Rev 1.0   06 May 1997 16:17:24   MKEATIN
// Initial revision.
//************************************************************************

#include "platform.h"
#include "avtypes.h"
#include "n30type.h"
#include "callbk15.h"
#include "navex15.h"

EXTSTATUS FAR WINAPI NLOADDS EXTQueryInterface(DWORD dwInterfaceID,
                                               LPLPVOID lplpvExportTable)
{
    PEXPORT15_TABLE_TYPE FAR *lplpTable15;

    switch (dwInterfaceID)
    {
        case NAVEX15_INTERFACE:
 
            lplpTable15 = (PEXPORT15_TABLE_TYPE FAR *)lplpvExportTable;

            *lplpTable15 = &export_table;

            return (EXTSTATUS_OK);

        default:
            
            return (EXTSTATUS_INIT_ERROR);
    }
}
