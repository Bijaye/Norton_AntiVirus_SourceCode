#include "platform.h"
#include "disk.h"

/*----------------------------------------------------------------------*/
/* UnscrambleBIOSCoding ()                                              */
/*      Takes an encoded BYTE-pair representing the cylinder, sector on */
/*      a disk, with the lower 8-bits of the cylinder, in the 2nd BYTE, */
/*      but the top 2-bits of the cylinder are in the first BYTE.       */
/*                                                                      */
/*      Returns the unencoded values for the cylinder and sector.       */
/*----------------------------------------------------------------------*/

//VOID WINAPI DiskUnscrambleBIOSCoding (WORD encodedValue, LPWORD cylinderPtr, LPBYTE sectorPtr)
//{
//                                        /* Compute cylinder             */
//    *cylinderPtr = HIBYTE(encodedValue) + ((encodedValue << 2) & 0x0300);
//
//                                        /* Remove 2 top bits            */
//    *sectorPtr = (((BYTE) encodedValue) & (BYTE) 0x3F);
//}


VOID WINAPI VLGetBootSectorLocation (
    LPVOID  lpPartitionBuffer,
    UINT    uEntry,
    LPWORD  lpwCylinder,
    LPBYTE  lpbSector,
    LPBYTE  lpbHead,
    LPBYTE  lpbType )
{
    LPPARTENTRYREC      lpLocalPartEntry;
    WORD                wTrack;
    BYTE                bySector;

    lpLocalPartEntry = (LPPARTENTRYREC)
                          ((LPSTR)lpPartitionBuffer + PART_TABLE_OFFSET) +
                            uEntry;

                                        /* Unscramble 10-bit cylinder   */

    DiskUnscrambleBIOSCoding (*((LPWORD) &(lpLocalPartEntry->startSector)), &wTrack, &bySector);

    *lpbHead        = lpLocalPartEntry->startHead;
    *lpwCylinder    = wTrack;
    *lpbSector      = bySector;
    *lpbType        = lpLocalPartEntry->system;
}
