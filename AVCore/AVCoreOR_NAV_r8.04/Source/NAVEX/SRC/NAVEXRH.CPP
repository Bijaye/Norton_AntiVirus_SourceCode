//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexrh.cpv   1.6   26 Dec 1996 15:22:04   AOONWAL  $
//
// Description:
//      Contains NAVEX One Half repair code...
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexrh.cpv  $
// 
//    Rev 1.6   26 Dec 1996 15:22:04   AOONWAL
// No change.
// 
//    Rev 1.5   02 Dec 1996 14:01:50   AOONWAL
// No change.
// 
//    Rev 1.4   29 Oct 1996 12:58:42   AOONWAL
// No change.
// 
//    Rev 1.3   17 Jun 1996 17:32:54   CNACHEN
// Zero sector function was not properly setting the # sector count to 1 for the
// disk write physical function.  It is now.
// 
//    Rev 1.2   17 Jun 1996 10:17:44   CNACHEN
// Added One half, all variants, DOS/DX support, and boy is it ugly.
// 
//    Rev 1.1   06 Jun 1996 16:54:18   CNACHEN
// Added support for the other two variants as well as sector-level
// resumption of decryption...
// 
//    Rev 1.0   04 Jun 1996 12:19:34   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef SYM_NLM

#include "platform.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
#include "ctsn.h"
#include "callback.h"
#endif

#include "navex.h"

// Declare shared routines

#include "navexshr.h"

#ifdef SYM_DOSX
#undef SYM_DOS
#endif

// *****************************************
//
// Prototypes for local MBR repair functions
//
// *****************************************

WORD EXTRepairPartOneHalf(LPCALLBACKREV1 lpCallBack,
                          LPN30          lpsN30,
                          BYTE           byDrive,
                          LPBYTE         lpbyWorkBuffer);


// *********************
//
// Data structures
//
// *********************

#define ONE_HALF_XOR_ENCRYPTION         0
#define ONE_HALF_XOR_ROL_ENCRYPTION     1

#define NAV_MID_WAY_STRUCT_OFFSET       0x110   // zeroed area of virus
#define MBR_OFFSET                      0x400   // in our work buffer

typedef struct
{
    BYTE        byRepairSig[3];     // for "NAV"
    BYTE        byCurHead;          // last head decrypted
    BYTE        byCurSec;           // last sector decrypted
} MIDWAY_T, FAR *LPMIDWAY;

typedef struct
{
    WORD        wStartCyl;
    BYTE        byStartHead;        // for mid-way stopping
    BYTE        byStartSec;         // for mid-way stopping
    WORD        wMaxCyl;
    BYTE        byMaxHead;
    BYTE        byMaxSec;
    WORD        wKey;
    WORD        wMBROffset;
    WORD        wEncryptionType;
    WORD        wOffsetOfCylCount;  // in the virus
} RESTORE_INFO_T, FAR *LPRESTORE_INFO;

#if defined(SYM_DOS) || defined(SYM_DOSX)

// prototypes so we can call our physical callbacks

typedef UINT (WINAPI FAR *SDiskReadPhysicalType)(LPVOID lpBuffer,
                                                 BYTE bCount,
                                                 WORD wCylinder,
                                                 BYTE bSector,
                                                 BYTE bHead,
                                                 BYTE bDrive);

typedef UINT (WINAPI FAR *SDiskWritePhysicalType)(LPVOID lpBuffer,
                                                  BYTE bCount,
                                                  WORD wCylinder,
                                                  BYTE bSector,
                                                  BYTE bHead,
                                                  BYTE bDrive);


WORD    OneHalfGetDiskGeometry
(
    BYTE                        byDrive,
    LPBYTE                      lpbyWorkBuffer,
    LPRESTORE_INFO              lpRestoreInfo,
    SDiskReadPhysicalType       DRPCallBack
)
{
    WORD            wMaxCyl, wSuccess;
    BYTE            byMaxHead, byMaxSec;

    _asm {

        push    es
        pusha

;        int     3h
;        mov     ax, 0001h

        // prepare to read in the MBR

        les     di, lpbyWorkBuffer

        mov     ax, 0201h
        mov     bx, di
        mov     cx, 0001h
        xor     dx, dx
        mov     dl, byDrive

        push    offset read_1_done
        jmp     disk_read_1

read_1_done:

        jc      get_disk_geometry_error

        add     di, 01EEh               ; point to end of partition table
        mov     cx,4

get_active:

        mov     bl, byte ptr es:[di+4] ; get system type

        cmp     bl, 1                  ; 12-bit fat
        je      found_it

        cmp     bl, 4                  ; skip 2 and 3
        jb      get_continue

        cmp     bl, 6                  ; 4=16-bit fat, 5=extended DOS part
        jbe     found_it               ; 6=32-bit fat

get_continue:
        sub     di, 10h
        loop    get_active

found_it:

        mov     ax, es:[DI+ 06h]        ; end cyl/sec
        mov     dh, es:[DI+ 01h]        ;

        xchg    ah, al                  ; bits 00 through 07
        mov     cl, 6                   ;
        shr     ah, cl                  ; get bits 08 and 09
        and     dh, 0C0h                ; get bits 10 and 11
        mov     cl, 4
        shr     dh, cl
        or      ah, dh                  ; add bits 10 and 11

        mov     wMaxCyl,  ax

        mov     ah, 8h
        mov     dl, byDrive
        int     13h                     ; this int 13 call works in DX
        jc      get_disk_geometry_error


        and     cl, 03fh
        and     dh, 03fh

        mov     byMaxHead, dh
        mov     byMaxSec,  cl

        mov     wSuccess, TRUE
        jmp     get_disk_geometry_done


get_disk_geometry_error:
        mov     wSuccess, FALSE

get_disk_geometry_done:

        popa
        pop     es

        jmp     get_disk_geometry_really_done

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

disk_read_1:

        pusha
        push        ds
        push        es

        push        es                      ; SEG:OFF
        push        bx

        xor         ah, ah                  ; clear AH
        push        ax                      ; AL=count

        mov         al, ch                  ; AL = lower 8 bits of track
        mov         ch, cl                  ; store sector number in CH temporarily
        mov         ah, cl                  ; AH = upper 2 bits of track (soon)
        mov         cl, 6
        shr         ah, cl                  ; AH = AH >> 6
        push        ax                      ; push track (cylinder) on stack

        mov         al, ch
        and         ax, 03fh                ; isolate sector number
        push        ax                      ; push sector on stack

        xor         ah, ah                  ; clear upper part of AX

        mov         al, dh
        push        ax                      ; push head on stack

        mov         al, dl
        push        ax                      ; push drive # on stack

        call        DRPCallBack             ; perform the disk read

        ; if AX is 0, success, !0 error

        or          ax, ax
        jz          read_success_1
        stc
        jmp         read_return_1

read_success_1:
        clc

read_return_1:

        pop         es
        pop         ds

        popa
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



get_disk_geometry_really_done:

    }   // _asm

    if (wSuccess == FALSE)
        return(EXTSTATUS_DISK_READ_ERROR);

    lpRestoreInfo->wMaxCyl      = wMaxCyl;
    lpRestoreInfo->byMaxHead    = byMaxHead;
    lpRestoreInfo->byMaxSec     = byMaxSec;



    return(EXTSTATUS_OK);
}

WORD    OneHalfGetDecryptionDataAux
(
    BYTE                        byDrive,
    LPBYTE                      lpbyWorkBuffer,
    LPN30                       lpsN30,
    LPRESTORE_INFO              lpRestoreInfo,
    WORD                        wStartCylOffset,
    WORD                        wStartCylBookOffset,
    WORD                        wStartCylBookValue,
    WORD                        wKeyOffset,
    WORD                        wKeyBookOffset,
    WORD                        wKeyBookValue,
    SDiskReadPhysicalType       DRPCallBack
)
{
    WORD        wKey, wResult, wStartCyl;
    BYTE        byMaxSec;
    LPMIDWAY        lpstMidway;

    byMaxSec = lpRestoreInfo->byMaxSec;

    _asm
    {
        push    es
        pusha

;        int     3h
;        mov     ax, 0002h

// read in the infected MBR and store this MBR at 400h from the top of
// the work buffer.  This will be used so we can write out our updated
// MBR as we work...

        les     di, lpbyWorkBuffer
        add     di, MBR_OFFSET

        mov     bx, di

        mov     ax, 0201h
        mov     cx, 0001h
        xor     dx, dx
        mov     dl, byDrive

        push    offset read_2_done
        jmp     disk_read_2

read_2_done:

        jc      bad_disk_get_decrypt

        push    bx
        add     bx, wStartCylBookOffset
        mov     ax, wStartCylBookValue
        cmp     es:[bx], ax
        pop     bx
        jnz     no_bookmark_found

        push    bx
        add     bx, wStartCylOffset
        mov     cx, word ptr es:[bx]
        mov     wStartCyl, cx      ; this is starting cylinder #
        pop     bx

// read in the virus body sector (3 from the end of the first track)

        les     di, lpbyWorkBuffer

        xor     cx, cx
        mov     cl, byMaxSec
        sub     cl, 3              ; virus body stored here - contains the
                                   ; key used for decryption

        mov     ax, 0201h
        mov     bx, di
        xor     dx, dx
        mov     dl, byDrive

        push    offset read_2a_done
        jmp     disk_read_2

read_2a_done:

        jc      bad_disk_get_decrypt

        push    bx
        add     bx, wKeyBookOffset
        mov     ax, wKeyBookValue
        cmp     es:[bx], ax
        pop     bx
        jnz     no_bookmark_found

        add     bx, wKeyOffset
        mov     cx, word ptr es:[bx]
        mov     wKey, cx                ; this is starting cylinder #
        jmp     book_found

no_bookmark_found:
bad_disk_get_decrypt:

        mov     wResult, 0
        jmp     get_decrypt_done

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

disk_read_2:

        pusha
        push        ds
        push        es

        push        es                      ; SEG:OFF
        push        bx

        xor         ah, ah                  ; clear AH
        push        ax                      ; AL=count

        mov         al, ch                  ; AL = lower 8 bits of track
        mov         ch, cl                  ; store sector number in CH temporarily
        mov         ah, cl                  ; AH = upper 2 bits of track (soon)
        mov         cl, 6
        shr         ah, cl                  ; AH = AH >> 6
        push        ax                      ; push track (cylinder) on stack

        mov         al, ch
        and         ax, 03fh                ; isolate sector number
        push        ax                      ; push sector on stack

        xor         ah, ah                  ; clear upper part of AX

        mov         al, dh
        push        ax                      ; push head on stack

        mov         al, dl
        push        ax                      ; push drive # on stack

        call        DRPCallBack             ; perform the disk read

        ; if AX is 0, success, !0 error

        or          ax, ax
        jz          read_success_2
        stc
        jmp         read_return_2

read_success_2:
        clc

read_return_2:

        pop         es
        pop         ds
        popa
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

book_found:

        mov     wResult, 1              ; success

get_decrypt_done:


        popa
        pop     es

    }

    if (wResult == FALSE)
        return(EXTSTATUS_NO_REPAIR);

    lpRestoreInfo->wKey = wKey;
    lpRestoreInfo->wStartCyl = wStartCyl;

// remember where the virus keeps its cylinder count so we can update it
// as we decrypt the encrypted sectors

    lpRestoreInfo->wOffsetOfCylCount = wStartCylOffset;

// now see if we need to resume our repair or whether we're starting from
// scratch.  If we find our "NAV" signature then we're mid-way.  continue
// from where we left off

    lpstMidway = (LPMIDWAY)
                    (lpbyWorkBuffer + NAV_MID_WAY_STRUCT_OFFSET + MBR_OFFSET);

    if (lpstMidway->byRepairSig[0] == 'N' &&
        lpstMidway->byRepairSig[1] == 'A' &&
        lpstMidway->byRepairSig[2] == 'V')
    {
        lpRestoreInfo->byStartHead = lpstMidway->byCurHead;
        lpRestoreInfo->byStartSec = lpstMidway->byCurSec;
    }
    else
    {
        // just starting to repair...

        lpRestoreInfo->byStartHead = 0;
        lpRestoreInfo->byStartSec = 1;
    }

    return(EXTSTATUS_OK);
}



WORD    OneHalfGetDecryptionData
(
    BYTE                        byDrive,
    LPBYTE                      lpbyWorkBuffer,
    LPN30                       lpsN30,
    LPRESTORE_INFO              lpRestoreInfo,
    SDiskReadPhysicalType       DRPCallBack
)
{
    EXTSTATUS           extStatus;

    // check for new 3577 variant first

    extStatus = OneHalfGetDecryptionDataAux(byDrive,
                                    lpbyWorkBuffer,
                                    lpsN30,
                                    lpRestoreInfo,
                                    0x0029,             // offset of CYL
                                    0x0027,             // bookmark off for CYL
                                    0xCB50,             // bookmark value
                                    0x01BE,             // offset of KEY
                                    0x01BC,             // bookmark offset for KEY
                                    0x3781,             // bookmark value
                                    DRPCallBack);

    if (extStatus == EXTSTATUS_OK)
    {
        lpRestoreInfo->wEncryptionType = ONE_HALF_XOR_ENCRYPTION;

        return(EXTSTATUS_OK);
    }

    // check for 3544 variant next

    extStatus = OneHalfGetDecryptionDataAux(byDrive,
                                    lpbyWorkBuffer,
                                    lpsN30,
                                    lpRestoreInfo,
                                    0x0029,             // offset of CYL
                                    0x0027,             // bookmark off for CYL
                                    0xCB50,             // bookmark value
                                    0x01D1,             // offset of KEY
                                    0x01CF,             // bookmark offset for KEY
                                    0x3781,             // bookmark value
                                    DRPCallBack);

    if (extStatus == EXTSTATUS_OK)
    {
        lpRestoreInfo->wEncryptionType = ONE_HALF_XOR_ENCRYPTION;

        return(EXTSTATUS_OK);
    }

    // check for 3570 variant next

    extStatus = OneHalfGetDecryptionDataAux(byDrive,
                                    lpbyWorkBuffer,
                                    lpsN30,
                                    lpRestoreInfo,
                                    0x002F,             // offset of CYL
                                    0x002D,             // bookmark off for CYL
                                    0xCB50,             // bookmark value
                                    0x01E1,             // offset of KEY
                                    0x01DF,             // bookmark offset for KEY
                                    0xB850,             // bookmark value
                                    DRPCallBack);

    if (extStatus == EXTSTATUS_OK)
    {
        lpRestoreInfo->wEncryptionType = ONE_HALF_XOR_ROL_ENCRYPTION;

        // 50           PUSH AX
        // B8 ?? ??     MOV AX, WKEY
        // 26           ES:
        // 31 07        XOR [BX], AX
        // D1 C8        ROR AX, 1
        // 43           INC BX
        // E2 F8        LOOP (to ES:)

        return(EXTSTATUS_OK);
    }

    return(extStatus);
}


WORD OneHalfProcessDisk
(
    BYTE                        byDrive,
    LPBYTE                      lpbyWorkBuffer,
    LPRESTORE_INFO              lpRestoreInfo,
    SDiskReadPhysicalType       DRPCallBack,
    SDiskWritePhysicalType      DWPCallBack
)
{
    WORD                wCylCount, wKey, wStatus, wEncryptionType;
    WORD                wMaxCyl, wStartCyl, wOffsetOfCylCount;
    BYTE                byMaxHead, byMaxSec;
    BYTE                byHeadCount, bySecCount, byStartHead, byStartSec;
    DWORD               dwCounter;
    BYTE                byRow, byCol, byVPage, bySMode;

    wMaxCyl = lpRestoreInfo->wMaxCyl;
    wStartCyl = lpRestoreInfo->wStartCyl;
    byMaxHead = lpRestoreInfo->byMaxHead;
    byMaxSec = lpRestoreInfo->byMaxSec;
    wKey = lpRestoreInfo->wKey;
    wEncryptionType = lpRestoreInfo->wEncryptionType;
    wOffsetOfCylCount = lpRestoreInfo->wOffsetOfCylCount;

    // obtain our where-to-decrypt-info

    byStartHead = lpRestoreInfo->byStartHead;
    byStartSec = lpRestoreInfo->byStartSec;

    _asm
    {
        push    es
        pusha

;        int     3h
;        mov     ax, 0003h

        les     di, lpbyWorkBuffer
                                          ; calculate counter
        xor     dx, dx
        xor     cx, cx

        mov     ax, wMaxCyl
        sub     ax, wStartCyl

        mov     cl, byMaxHead
        inc     cl
        mul     cx

        mov     cl, byMaxSec
        mul     cx

        mov     word ptr [dwCounter], ax
        mov     word ptr [dwCounter+2], dx

        mov     al, byStartHead
        mov     cl, byMaxSec
        mul     cl

        xor     cx, cx
        mov     cl, byStartSec
        add     ax, cx
        dec     ax

        sub     word ptr [dwCounter], ax
        sbb     word ptr [dwCounter+2], 0

// setup for display

        pusha

        mov     ah,0Fh                  ; Get video mode
        int     10h                     ;
        mov     bySMode,al              ; Save mode
        mov     byVPage,bh              ; Save page for use

        mov     ah, 03h
        int     10h

        mov     byRow, dh
        mov     byCol, dl

        popa

// setup for display

        mov     ax, wStartCyl
        mov     wCylCount, ax

        mov     bx, di
        mov     dl, byDrive

cyl_loop:
        mov     al, byStartHead
        mov     byHeadCount, al

head_loop:
        mov     al, byStartSec
        mov     bySecCount, al

sec_loop:
        mov     ax, wCylCount
        xchg    ah, al
        mov     cl, 6
        shl     al, cl
        or      al, bySecCount
        mov     cx, ax
        mov     dh, byHeadCount
        mov     dl, byDrive
        mov     ax, 0201h

        push    offset read_3_done
        jmp     disk_read_3

read_3_done:

// decrypt that sector

        push    cx
        push    dx
        push    bx

        cmp     wEncryptionType, ONE_HALF_XOR_ROL_ENCRYPTION
        je      do_xor_rol_decryption

// do standard decryption

        mov     cx, 100h
        mov     dx, wKey

dec_loop:

        xor     es:[bx], dx
        add     bx, 2

        loop    dec_loop
        jmp     done_decryption

do_xor_rol_decryption:

// do funky 3570 variant decryption

        mov     cx, 200h                ; 512 bytes to decrypt
        mov     dx, wKey

dec_loop2:

        xor     es:[bx], dx
        ror     dx, 1
        inc     bx
        loop    dec_loop2

done_decryption:

        pop     bx
        pop     dx
        pop     cx

// done decrypting that sector, write it out

        push    offset write_3_done
        jmp     disk_write_3

write_3_done:

// now update the MBR counts so we know where we were if the system is reset

        push    offset dec_counter
        jmp     update_mbr

// update our decryption status? perhaps write out to slack sector?

dec_counter:

        sub     word ptr [dwCounter], 1
        sbb     word ptr [dwCounter+2], 0

// display the counter value...

        pusha

// locate the cursor

        mov     ah, 2
        mov     bh, byVPage
        mov     dh, byRow
        mov     dl, byCol
        int     10h

// clear those registers

        xor     ax, ax
        xor     cx, cx
        xor     dx, dx

// print those values

        mov     cx, word ptr [dwCounter+2]
        push    offset print_second
        jmp     print_word                          ; call is horked by C8

print_second:
        mov     cx, word ptr [dwCounter]
        push    offset print_done
        jmp     print_word                          ; call is horked by C8

print_done:

        popa

// end display the counter value...

        inc     bySecCount
        mov     al, bySecCount
        cmp     al, byMaxSec

        jg      done_sec_loop                       ;;>> sec loop
        jmp     sec_loop
done_sec_loop:

        mov     byStartSec, 1                       ;; init start sector

        inc     byHeadCount                         ;;
        mov     al, byHeadCount                     ;;>> head loop
        cmp     al, byMaxHead

        jg      head_loop_e
        jmp     head_loop
head_loop_e:

        mov     byStartHead, 0                      ;; init start head
        inc     wCylCount
        mov     ax, wCylCount
        cmp     ax, wMaxCyl
                                                    ; jl: Last cylinder is NOT
                                                    ; encrypted!!

        jge     done_looping
        jmp     cyl_loop

done_looping:

// now repair the MBR...

        les     di, lpbyWorkBuffer

        xor     cx, cx
        mov     cl, byMaxSec
        sub     cx, 7                               ; 7 sectors from the end
                                                    ; of the first cylinder
        mov     ax, 0201h
        mov     bx, di
        xor     dx, dx
        mov     dl, byDrive

        push    offset read_3a_done
        jmp     disk_read_3

read_3a_done:

        cmp     word ptr es:[bx+01feh], 0AA55h
        jne     process_cyl_error

        mov     ax, 0301h
        mov     cx, 0001h

        push    offset write_3a_done
        jmp     disk_write_3

write_3a_done:

        jmp     process_cyl_done

process_cyl_error:

        mov     wStatus, 0
        jmp     process_cyl_all_done


print_word:

        push    cx
        mov     al, ch
        push    offset print_word2
        jmp     printnum

print_word2:
        pop     ax
        push    offset print_wordr
        jmp     printnum

print_wordr:
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

disk_read_3:

        pusha
        push        ds
        push        es


        push        es                      ; SEG:OFF
        push        bx

        xor         ah, ah                  ; clear AH
        push        ax                      ; AL=count

        mov         al, ch                  ; AL = lower 8 bits of track
        mov         ch, cl                  ; store sector number in CH temporarily
        mov         ah, cl                  ; AH = upper 2 bits of track (soon)
        mov         cl, 6
        shr         ah, cl                  ; AH = AH >> 6
        push        ax                      ; push track (cylinder) on stack

        mov         al, ch
        and         ax, 03fh                ; isolate sector number
        push        ax                      ; push sector on stack

        xor         ah, ah                  ; clear upper part of AX

        mov         al, dh
        push        ax                      ; push head on stack

        mov         al, dl
        push        ax                      ; push drive # on stack

        call        DRPCallBack             ; perform the disk read

        ; if AX is 0, success, !0 error

        or          ax, ax
        jz          read_success_3
        stc
        jmp         read_return_3

read_success_3:
        clc

read_return_3:

        pop         es
        pop         ds

        popa
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

disk_write_3:

        pusha
        push        ds
        push        es


        push        es                      ; SEG:OFF
        push        bx

        xor         ah, ah                  ; clear AH
        push        ax                      ; AL=count

        mov         al, ch                  ; AL = lower 8 bits of track
        mov         ch, cl                  ; store sector number in CH temporarily
        mov         ah, cl                  ; AH = upper 2 bits of track (soon)
        mov         cl, 6
        shr         ah, cl                  ; AH = AH >> 6
        push        ax                      ; push track (cylinder) on stack

        mov         al, ch
        and         ax, 03fh                ; isolate sector number
        push        ax                      ; push sector on stack

        xor         ah, ah                  ; clear upper part of AX

        mov         al, dh
        push        ax                      ; push head on stack

        mov         al, dl
        push        ax                      ; push drive # on stack

        call        DWPCallBack             ; perform the disk write

        ; if AX is 0, success, !0 error

        or          ax, ax
        jz          write_success_3
        stc
        jmp         write_return_3

write_success_3:
        clc

write_return_3:

        pop         es
        pop         ds
        popa
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



printnum:                               ; al has hex byte

        push    ax
        push    bx
        push    cx
        push    dx

        mov     ah, al                  ; save for later

        mov     cl, 4
        shr     al, cl
        cmp     al, 9
        ja      printletter
        add     al, '0'
        jmp     printchar
printletter:
        add     al, 'A'-0ah
printchar:
        push    ax
        mov     ah, 0eh
        mov     bh, byVPage
        int     10h
        pop     ax

        mov     al, ah
        and     al, 0fh
        cmp     al, 9
        ja      printletter2
        add     al, '0'
        jmp     printchar2
printletter2:
        add     al, 'A'-0ah
printchar2:

        mov     ah, 0eh
        mov     bh, byVPage
        int     10h

        pop     dx
        pop     cx
        pop     bx
        pop     ax

        ret

update_mbr:

        pusha
        les     bx, lpbyWorkBuffer
        add     bx, MBR_OFFSET      ; point to our saved MBR

// store our cylCount wherever the virus keeps its count, so it will use
// our updated count if it gets control again

        mov     ax, wCylCount
        mov     si, wOffsetOfCylCount
        mov     es:[bx+si], ax

// store our NAV signature and our head and sector counts in the
// slack space of the MBR

        mov     byte ptr es:[bx+NAV_MID_WAY_STRUCT_OFFSET], 'N'
        mov     byte ptr es:[bx+NAV_MID_WAY_STRUCT_OFFSET+1], 'A'
        mov     byte ptr es:[bx+NAV_MID_WAY_STRUCT_OFFSET+2], 'V'

        mov     al, byHeadCount
        mov     byte ptr es:[bx+NAV_MID_WAY_STRUCT_OFFSET+3], al

// make sure when we save the sector number we inc it, so if the user
// reboots, we start decrypting the sector *following* the last one that
// was decrypted

        mov     al, bySecCount
        inc     al
        mov     byte ptr es:[bx+NAV_MID_WAY_STRUCT_OFFSET+4], al

// write the modified MBR sector out and return

        mov     ax, 0301h
        mov     cx, 0001h
        xor     dx, dx
        mov     dl, byDrive

        push    offset write_3b_done
        jmp     disk_write_3

write_3b_done:

        popa
        ret

process_cyl_done:

        mov     wStatus, 1

process_cyl_all_done:

        popa
        pop     es
    }   // _asm

    if (wStatus == FALSE)
        return(EXTSTATUS_NO_REPAIR);

    return(EXTSTATUS_OK);
}

WORD OneHalfZeroVirusBody
(
    BYTE                        byDrive,
    LPBYTE                      lpbyWorkBuffer,
    LPRESTORE_INFO              lpRestoreInfo,
    SDiskWritePhysicalType      DWPCallBack
)
{
    BYTE                byMaxSec;

    byMaxSec = lpRestoreInfo->byMaxSec;

    _asm
    {
        pusha
        push    es

        mov     ax, 0deadh
        int     3h

// zero our buffer to save over the virus infection

        les     di, lpbyWorkBuffer
        mov     cx, 100h
        xor     ax, ax
        repnz   stosw

// overwrite the last 7 sectors of the first track (used by the virus)

        les     bx, lpbyWorkBuffer

        xor     dx, dx
        mov     dl, byDrive
        xor     cx, cx
        mov     cl, byMaxSec
        sub     cx, 7                       ; zero last 7 sectors of track 0
        mov     al, 1                       ; # of sectors to write...

zero_loop:

        push    offset write_4_done
        jmp     disk_write_4

write_4_done:

        inc     cl
        cmp     cl, byMaxSec
        jle     zero_loop

        pop     es
        popa

        jmp     zero_return

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

disk_write_4:

        pusha
        push        ds
        push        es
        

        push        es                      ; SEG:OFF
        push        bx

        xor         ah, ah                  ; clear AH
        push        ax                      ; AL=count

        mov         al, ch                  ; AL = lower 8 bits of track
        mov         ch, cl                  ; store sector number in CH temporarily
        mov         ah, cl                  ; AH = upper 2 bits of track (soon)
        mov         cl, 6
        shr         ah, cl                  ; AH = AH >> 6
        push        ax                      ; push track (cylinder) on stack

        mov         al, ch
        and         ax, 03fh                ; isolate sector number
        push        ax                      ; push sector on stack

        xor         ah, ah                  ; clear upper part of AX

        mov         al, dh
        push        ax                      ; push head on stack

        mov         al, dl
        push        ax                      ; push drive # on stack

        call        DWPCallBack             ; perform the disk write

        ; if AX is 0, success, !0 error

        or          ax, ax
        jz          write_success_4
        stc
        jmp         write_return_4

write_success_4:
        clc

write_return_4:

        pop         es
        pop         ds
        popa
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

zero_return:

    }

// always OK since we're just zeroing stuff out, which isn't really necessary

    return(EXTSTATUS_OK);

}


#endif      // #if defined(SYM_DOS) || defined(SYM_DOSX)

// EXTRepairPartOneHalf
// Repair for OneHalf
//
// Arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

WORD EXTRepairPartOneHalf
(
    LPCALLBACKREV1      lpCallBack,
    LPN30               lpsN30,
    BYTE                byDrive,
    LPBYTE              lpbyWorkBuffer
)
{
    WORD                wReturn = EXTSTATUS_NO_REPAIR;

#if defined(SYM_DOS) || defined(SYM_DOSX)
    RESTORE_INFO_T      stRestoreInfo;

    // obtain our disk geometry so we can do our repair

    wReturn = OneHalfGetDiskGeometry(byDrive,
                                     lpbyWorkBuffer,
                                     &stRestoreInfo,
                                     lpCallBack->SDiskReadPhysical);


    if (wReturn != EXTSTATUS_OK)
        return(wReturn);

    wReturn = OneHalfGetDecryptionData(byDrive,
                                       lpbyWorkBuffer,
                                       lpsN30,
                                       &stRestoreInfo,
                                       lpCallBack->SDiskReadPhysical);

    if (wReturn != EXTSTATUS_OK)
        return(wReturn);

    wReturn = OneHalfProcessDisk(byDrive,
                                 lpbyWorkBuffer,
                                 &stRestoreInfo,
                                 lpCallBack->SDiskReadPhysical,
                                 lpCallBack->SDiskWritePhysical);

    if (wReturn != EXTSTATUS_OK)
        return(wReturn);

    wReturn = OneHalfZeroVirusBody(byDrive,
                                   lpbyWorkBuffer,
                                   &stRestoreInfo,
                                   lpCallBack->SDiskWritePhysical);

#endif

    return(wReturn);
}

#endif  // #ifndef SYM_NLM


