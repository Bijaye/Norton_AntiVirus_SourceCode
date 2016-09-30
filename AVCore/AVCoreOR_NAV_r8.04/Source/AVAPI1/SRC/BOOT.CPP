#include "platform.h"
#include "disk.h"
#include "avapi.h"

VOID WINAPI VLGetBootSectorLocation (
    HVCONTEXT hContext,
    BYTE      byDriveNumber,
    LPVOID    lpPartitionBuffer,
    UINT      uEntry,
    LPWORD    lpwCylinder,
    LPBYTE    lpbSector,
    LPBYTE    lpbHead,
    LPBYTE    lpbType )
{
    LPPARTENTRYREC      lpLocalPartEntry;
    WORD                wTrack;
    BYTE                bySector;
    BYTE                bySide;

    lpLocalPartEntry = (LPPARTENTRYREC)
                        ((LPSTR)lpPartitionBuffer + PART_TABLE_OFFSET) +
                        uEntry;

    hContext->lpGlobalCallBacks->
              lpGeneralCallBacks->
              lpLinearCallBacks->
              LinearConvertToPhysical((LPVOID)byDriveNumber,
                                      lpLocalPartEntry->relativeSectors,
                                      &bySide,
                                      &wTrack,
                                      &bySector);

    *lpbHead        = bySide;
    *lpwCylinder    = wTrack;
    *lpbSector      = bySector;
    *lpbType        = lpLocalPartEntry->system;
}
