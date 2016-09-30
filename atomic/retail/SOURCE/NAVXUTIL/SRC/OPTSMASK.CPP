// Copyright 1997 Symantec Corporation, Peter Norton Group
//***************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/optsmask.cpv   1.2   10 Apr 1998 14:36:58   MKEATIN  $
//
// Description:
//   Provides routines and data structure to mask out and override options
//   read from the options data file into a NAVOPTION data structure. In
//   addition, you can specify if you want the mask to write out the new
//   setting (SAVE) or keep that option the unchanged on disk (NOSAVE).
//
// Contains:
//
//***************************************************************************
// $Log:   S:/NAVXUTIL/VCS/optsmask.cpv  $
// 
//    Rev 1.2   10 Apr 1998 14:36:58   MKEATIN
// Added AtomicMasks[] force off Immediate Notify, Virus Sensor, and
// File Inocculation.
// 
//    Rev 1.1   18 Feb 1998 17:25:04   mdunn
// Ported NEC stuff from ROSNEC.
// 
//    Rev 1.0   06 Feb 1997 21:05:40   RFULLER
// Initial revision
// 
//    Rev 1.1   24 Jan 1997 15:28:00   JBRENNA
// Separate out the processing logic into local routines. This will make it
// very easy to add more tables like NecMasks[].
// 
//    Rev 1.0   24 Jan 1997 15:06:38   JBRENNA
// Initial revision.
// 
//***************************************************************************


#include "platform.h"
#include "xapi.h"
#include "options.h"

#include "OptsMask.h"

// Jim Brennan: I have only confirmed that this functionality works correctly
// for Win32 platforms. I am not familiar enough with the other platforms to
// ensure that it will work under them too. It should be easy to make this 
// available for other platforms. Areas of concern when porting to another
// platform:
//  1. CMaskDesc::CalcFieldPointer(): Does the pointer arithmetic work
//     correctly on the new platform?
//  2. CMaskDesc::WriteToMemory() and CMaskDesc::ReadFromMemory(): Does the
//     memcpy calls work correctly on the new platform?
//  3. Declaration of NecMasks: Does the new platform support "offsetof"?
//  4. Does "pCurMask - Masks" yield the number of masks processed?

#ifndef SYM_WIN32
#error This file may have problems on a non-Win32 platform.
#endif

//***************************************************************************
// Defines ******************************************************************
//***************************************************************************

#define NUM_ARRAY_ELEMS(arr)     (sizeof (arr) / sizeof (arr[0]))

//***************************************************************************
// Data Structures **********************************************************
//***************************************************************************

// This class is used to describe each NavOption field that needs to be
// overwritten.
class CMaskDesc {
public:
    typedef enum { NO_TYPE, BYTE_TYPE, DW_TYPE, W_TYPE } VALUE_TYPE;
    typedef enum { NOSAVE, SAVE } SAVE_TYPE;

    CMaskDesc (const size_t Offset = 0)
    {
        Init (Offset);
    }

    CMaskDesc (const size_t    Offset,
               const BYTE      byteValue,
               const SAVE_TYPE eSaveValue = NOSAVE)
    {
        Init (Offset, BYTE_TYPE, eSaveValue);
        m_uValue.byte = byteValue;
    }

    CMaskDesc (const size_t    Offset,
               const DWORD     dwValue,
               const SAVE_TYPE eSaveValue = NOSAVE)
    {
        Init (Offset, DW_TYPE, eSaveValue);
        m_uValue.dw = dwValue;
    }

    CMaskDesc (const size_t    Offset,
               const WORD      wValue,
               const SAVE_TYPE eSaveValue = NOSAVE)
    {
        Init (Offset, W_TYPE, eSaveValue);
        m_uValue.w = wValue;
    }

    // Update our the original value for this field. This saved value is used
    // to restore the value for NOSAVE fields.
    void UpdateOrigValue (LPNAVOPTIONS lpNavOpts)
    {
        m_uOrigValue = ReadFromMemory (CalcFieldPointer(lpNavOpts));
    }

    // Update our memorized m_uValue to the current setting in the options
    // structure.
    void UpdateMemValue (LPNAVOPTIONS lpNavOpts)
    {
        m_uValue = ReadFromMemory (CalcFieldPointer(lpNavOpts));
    }

    // Put the current m_uValue into the options structure.
    void LoadMemValue (LPNAVOPTIONS lpNavOpts)
    {
        WriteToMemory (CalcFieldPointer(lpNavOpts), m_uValue);
    }

    // Put the current m_uOrigValue into the options structure.
    void LoadOrigValue (LPNAVOPTIONS lpNavOpts)
    {
        WriteToMemory (CalcFieldPointer(lpNavOpts), m_uOrigValue);
    }

    BOOL IsSaveNewValue (void) const
    {
        return (m_eSaveValue == SAVE);
    }

private:    // Data Types

    // These are the values that are copied into the specified structure
    // location. In other words, this becomes the new value of that field.
    typedef union {
        BYTE            byte;
        DWORD           dw;
        WORD            w;
    } VALUE;
    
private:    // Functions

    void Init (const size_t     Offset,
               const VALUE_TYPE eValueType = NO_TYPE,
               const SAVE_TYPE  eSaveValue = NOSAVE)
    {
        m_Offset      = Offset;
        m_eValueType  = eValueType;
        m_eSaveValue  = eSaveValue;

        memset (&m_uValue,     0, sizeof (VALUE));
        memset (&m_uOrigValue, 0, sizeof (VALUE));
    }

    BYTE *CalcFieldPointer (LPNAVOPTIONS lpPointer) const
    {
        return ((BYTE*)lpPointer) + m_Offset;
    }

    // Read the information from lpPointer into the returned VALUE structure.
    // The current VALUE type determines the size of the information copied.
    VALUE ReadFromMemory (const void FAR *lpPointer) const
    {
        VALUE TmpValue;

        switch (m_eValueType)
        {
            case BYTE_TYPE:
                memcpy (&TmpValue.byte, lpPointer, sizeof (BYTE));
                break;
            case DW_TYPE:
                memcpy (&TmpValue.dw,   lpPointer, sizeof (DWORD));
                break;
            case W_TYPE:
                memcpy (&TmpValue.w,    lpPointer, sizeof (WORD));
                break;

            case NO_TYPE:
            default:
                // should never be called with no data type.
                SYM_ASSERT (FALSE);

                // bogus information.
                memset (&TmpValue, 0xFE, sizeof (TmpValue));
                break;
        }
        return TmpValue;
    }

    // Write the specified "Value" to the specified memory location. The 
    // current value type determines the size of the information copied.
    void WriteToMemory (void FAR     *lpPointer,
                        const VALUE  Value)
    {
        switch (m_eValueType)
        {
            case BYTE_TYPE:
                memcpy (lpPointer, &Value.byte, sizeof (BYTE));
                break;
            case DW_TYPE:
                memcpy (lpPointer, &Value.dw,   sizeof (DWORD));
                break;
            case W_TYPE:
                memcpy (lpPointer, &Value.w,    sizeof (WORD));
                break;

            case NO_TYPE:
            default:
                // should never be called with no data type.
                SYM_ASSERT (FALSE);
                break;
        }
    }


private:    // Data

    // # bytes into NAVOPTIONS structure for this field.
    size_t      m_Offset;
    
    // When saving options, write out m_uValue as the new setting for
    // that option.
    SAVE_TYPE   m_eSaveValue;

    // Type of the new value. NO_TYPE means no new value.
    VALUE_TYPE  m_eValueType;

    VALUE   m_uValue;       // New field value
    VALUE   m_uOrigValue;   // Original field value.
};

//***************************************************************************
// Local Data ***************************************************************
//***************************************************************************

// This structure is used to determine how to override the current
// NAVOPTIONS settings. The first parameter provides the offset into the 
// NAVOPTIONS structure for the desired field to override. The second
// parameter provides the new setting for field. The third field determines
// if the new setting should be saved to disk. If this is NOSAVE, the field
// is only changed in memory. The field remains unchanged on disk.

static CMaskDesc NecMasks[] = {
    CMaskDesc (offsetof (NAVOPTIONS, scanner.bMasterBootRec),
               (BYTE)FALSE, CMaskDesc::NOSAVE),

    CMaskDesc (offsetof (NAVOPTIONS, tsr.bScanMBoot),
               (BYTE)FALSE, CMaskDesc::NOSAVE),

    CMaskDesc (offsetof (NAVOPTIONS, tsr.bScanBootRecs),
               (BYTE)FALSE, CMaskDesc::NOSAVE),

    CMaskDesc (offsetof (NAVOPTIONS, inoc.bUseSystemInoc),
               (BYTE)FALSE, CMaskDesc::NOSAVE),
};

// in Atomic, we removed file inoc, virus sensor and immedate notify

static CMaskDesc AtomicMasks[] = {
    CMaskDesc (offsetof (NAVOPTIONS, inoc.bUseInoc),
               (BYTE)FALSE, CMaskDesc::NOSAVE),

    CMaskDesc (offsetof (NAVOPTIONS, inoc.bInocFloppies),
               (BYTE)FALSE, CMaskDesc::NOSAVE),

    CMaskDesc (offsetof (NAVOPTIONS, inoc.bInocFast),
               (BYTE)FALSE, CMaskDesc::NOSAVE),

    CMaskDesc (offsetof (NAVOPTIONS, inoc.bDispDelete),
               (BYTE)FALSE, CMaskDesc::NOSAVE),

    CMaskDesc (offsetof (NAVOPTIONS, inoc.bDispExcl),
               (BYTE)FALSE, CMaskDesc::NOSAVE),

    CMaskDesc (offsetof (NAVOPTIONS, scanner.bPrestoNotify),
               (BYTE)FALSE, CMaskDesc::NOSAVE),

    CMaskDesc (offsetof (NAVOPTIONS, tsr.bScanUnknown),
               (BYTE)FALSE, CMaskDesc::NOSAVE),
};



//***************************************************************************
// Static Functions *********************************************************
//***************************************************************************

//---------------------------------------------------------------------------
// Description:
//   Walk through each individual mask description (a NavOpts Field). For
//   each description, memorize the original options setting. This is used
//   when NOSAVE was specified on the mask description. In addition, this
//   routine loads the new value into the options structure.
//
//   This is generally called from the OptsMaskReadPostHook function.
//
// Parameters:
//   lpNavOpts     [in out] Options structure to modify.
//   Masks         [in out] Mask description to apply on lpNavOpts.
//   iNumMasks     [in]     Number of "Masks"
//---------------------------------------------------------------------------
static void ReadPostHook (LPNAVOPTIONS   lpNavOpts, 
                          CMaskDesc FAR  *Masks,
                          int            iNumMasks)
{
    auto   CMaskDesc *pCurMask;

    for (pCurMask = Masks; pCurMask - Masks < iNumMasks; ++pCurMask)
    {
        pCurMask->UpdateOrigValue (lpNavOpts);
        pCurMask->LoadMemValue    (lpNavOpts);
    }
}

//---------------------------------------------------------------------------
// Description:
//   Walk through each individual mask description (a NavOpts Field). For
//   each description that is marked as NOSAVE, restore the original value
//   back into the options structure. That way, the write routine will
//   actually save the original value rather than our new value. Note that
//   we also update the "in memory" value in case it has changed. That way
//   when we restore it in the PostHook routine it goes back to the same
//   value.
//
//   This is generally called from the OptsMaskWritePreHook function.
//
// Parameters:
//   lpNavOpts     [in out] Options structure to modify.
//   Masks         [in out] Mask description to apply on lpNavOpts.
//   iNumMasks     [in]     Number of "Masks"
//---------------------------------------------------------------------------
static void WritePreHook (LPNAVOPTIONS   lpNavOpts, 
                          CMaskDesc FAR  *Masks,
                          int            iNumMasks)
{
    auto   CMaskDesc *pCurMask;

    for (pCurMask = Masks; pCurMask - Masks < iNumMasks; ++pCurMask)
    {
        if (!pCurMask->IsSaveNewValue())
        {
            pCurMask->UpdateMemValue (lpNavOpts);
            pCurMask->LoadOrigValue  (lpNavOpts);
        }
    }
}

//---------------------------------------------------------------------------
// Description:
//   Walk through each individual mask description (a NavOpts Field). For
//   each description that is marked as NOSAVE, restore the "in memory"
//   value into the options structure. It currently has the "original"
//   value loaded from the PreHook function.
//
//   This is generally called from the OptsMaskWritePostHook function.
//
// Parameters:
//   lpNavOpts     [in out] Options structure to modify.
//   Masks         [in out] Mask description to apply on lpNavOpts.
//   iNumMasks     [in]     Number of "Masks"
//---------------------------------------------------------------------------
static void WritePostHook (LPNAVOPTIONS   lpNavOpts, 
                           CMaskDesc FAR  *Masks,
                           int            iNumMasks)
{
    auto   CMaskDesc *pCurMask;

    for (pCurMask = Masks; pCurMask - Masks < iNumMasks; ++pCurMask)
    {
        if (!pCurMask->IsSaveNewValue())
        {
            pCurMask->LoadMemValue (lpNavOpts);
        }
    }
}

//***************************************************************************
// Local Functions **********************************************************
//***************************************************************************

void OptsMaskReadPostHook (LPNAVOPTIONS lpNavOpts)
{
    if (HWIsNEC())
        ReadPostHook (lpNavOpts, NecMasks, NUM_ARRAY_ELEMS (NecMasks));

    ReadPostHook (lpNavOpts, AtomicMasks, NUM_ARRAY_ELEMS (AtomicMasks));
}

void OptsMaskWritePreHook (LPNAVOPTIONS lpNavOpts)
{
    if (HWIsNEC())
        WritePreHook (lpNavOpts, NecMasks, NUM_ARRAY_ELEMS (NecMasks));

    WritePreHook (lpNavOpts, AtomicMasks, NUM_ARRAY_ELEMS (AtomicMasks));
}

void OptsMaskWritePostHook (LPNAVOPTIONS lpNavOpts)
{
    if (HWIsNEC())
        WritePostHook (lpNavOpts, NecMasks, NUM_ARRAY_ELEMS (NecMasks));

    WritePostHook (lpNavOpts, AtomicMasks, NUM_ARRAY_ELEMS (AtomicMasks));
}

//***************************************************************************
// Exported Functions *******************************************************
//***************************************************************************
