#include "platform.h"
#include "hugeoem.h"

#define SIZE_TEMPOEM    0xFFFE

// ************************************************************************ //
//                      Local function prototypes                           //
// ************************************************************************ //

VOID LOCAL HugeMemCopy (HPSTR hpTo, HPSTR hpFrom, DWORD dwNumBytes);

// ************************************************************************ //
// ************************************************************************ //


DWORD SYM_EXPORT WINAPI HugeOemToAnsiBuff (HPSTR hpszOemString,
                                      HPSTR hpszAnsiString,
                                      DWORD dwOemSize)
{
    auto    DWORD   dwBytesConverted = 0;
    auto    LPSTR   lpMem;
    auto    UINT    uSizeToProcess;

                                        // --------------------------------
                                        // This is to be consistent with
                                        // the Windows API that allows zero
                                        // --------------------------------

    if (dwOemSize == 0 || dwOemSize > 0xFFFF)
        {
                                        // --------------------------------
                                        // Allocate a block of mem < 64K
                                        // --------------------------------

        if ((lpMem = MemAllocPtr (GHND, SIZE_TEMPOEM)) != NULL)
            {
            dwBytesConverted = dwOemSize;

                                        // --------------------------------
                                        // While there are bytes to be
                                        // processed stay in this loop.
                                        // --------------------------------
            while ( dwOemSize )
                {
                                        // --------------------------------
                                        // If bytes remaining is larger than
                                        // SIZE_TEMPOEM, just process
                                        // SIZE_TEMPOEM bytes.
                                        // --------------------------------

                if (dwOemSize > SIZE_TEMPOEM)
                    uSizeToProcess = SIZE_TEMPOEM;
                else
                    uSizeToProcess = (UINT)dwOemSize;

                                        // --------------------------------
                                        // Copy Oem bytes to convert into
                                        // our temporary buffer.
                                        // --------------------------------

                HugeMemCopy (lpMem, hpszOemString, uSizeToProcess);

                                        // --------------------------------
                                        // Convert these bytes from Oem to
                                        // Ansi.
                                        // --------------------------------

                OemToAnsiBuff (lpMem, lpMem, uSizeToProcess);

                                        // --------------------------------
                                        // Save the converted Ansi bytes
                                        // into the provided Ansi buffer.
                                        // --------------------------------

                HugeMemCopy (hpszAnsiString, lpMem, uSizeToProcess);

                                        // --------------------------------
                                        // Increment buffer with the number
                                        // of bytes processed.
                                        // --------------------------------

                hpszOemString  += uSizeToProcess;
                hpszAnsiString += uSizeToProcess;
                dwOemSize      -= uSizeToProcess;
                }

            MemFreePtr (lpMem);
            }
        }
    else
        {
        dwBytesConverted = dwOemSize;
        OemToAnsiBuff (hpszOemString, hpszAnsiString, (UINT)dwOemSize);
        }

    return ( dwBytesConverted );
}

// ************************************************************************ //
// ************************************************************************ //

DWORD SYM_EXPORT WINAPI HugeAnsiToOemBuff (HPSTR hpszAnsiString,
                                       HPSTR hpszOemString,
                                       DWORD dwAnsiSize)
{
    auto    DWORD   dwBytesConverted = 0;
    auto    LPSTR   lpMem;
    auto    UINT    uSizeToProcess;

                                        // --------------------------------
                                        // This is to be consistent with
                                        // the Windows API that allows zero
                                        // --------------------------------

    if (dwAnsiSize == 0 || dwAnsiSize > 0xFFFF)
        {
                                        // --------------------------------
                                        // Allocate a block of mem < 64K
                                        // --------------------------------

        if ((lpMem = MemAllocPtr (GHND, SIZE_TEMPOEM)) != NULL)
            {
            dwBytesConverted = dwAnsiSize;

                                        // --------------------------------
                                        // While there are bytes to be
                                        // processed stay in this loop.
                                        // --------------------------------
            while ( dwAnsiSize )
                {
                                        // --------------------------------
                                        // If bytes remaining is larger than
                                        // SIZE_TEMPOEM, just process
                                        // SIZE_TEMPOEM bytes.
                                        // --------------------------------

                if (dwAnsiSize > SIZE_TEMPOEM)
                    uSizeToProcess = SIZE_TEMPOEM;
                else
                    uSizeToProcess = (UINT)dwAnsiSize;

                                        // --------------------------------
                                        // Copy Oem bytes to convert into
                                        // our temporary buffer.
                                        // --------------------------------

                HugeMemCopy (lpMem, hpszAnsiString, uSizeToProcess);

                                        // --------------------------------
                                        // Convert these bytes from Oem to
                                        // Ansi.
                                        // --------------------------------

                AnsiToOemBuff (lpMem, lpMem, uSizeToProcess);

                                        // --------------------------------
                                        // Save the converted Ansi bytes
                                        // into the provided Ansi buffer.
                                        // --------------------------------

                HugeMemCopy (hpszOemString, lpMem, uSizeToProcess);

                                        // --------------------------------
                                        // Increment buffer with the number
                                        // of bytes processed.
                                        // --------------------------------

                hpszOemString  += uSizeToProcess;
                hpszAnsiString += uSizeToProcess;
                dwAnsiSize     -= uSizeToProcess;
                }

            MemFreePtr (lpMem);
            }
        }
    else
        {
        dwBytesConverted = dwAnsiSize;
        AnsiToOemBuff (hpszAnsiString, hpszOemString, (UINT)dwAnsiSize);
        }

    return ( dwBytesConverted );
}

// ************************************************************************ //
// ************************************************************************ //

VOID LOCAL HugeMemCopy (HPSTR hpTo, HPSTR hpFrom, DWORD dwNumBytes)
{
    for (; dwNumBytes; *hpTo++ = *hpFrom++, dwNumBytes--)
        ;
}
