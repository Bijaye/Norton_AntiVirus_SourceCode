// EXEFile.cpp : Support functions for computing EXE size.
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2000, 2005 by Symantec Corporation.  All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// Headers

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "DecPlat.h"
#include "Dec2.h"
#include "Dec2EX.h"
#include "Dec2LIB.h"
#include "Decstdio.h"

#define ASC_STR_PKSFX_for_Windows "\x50\x4b\x53\x46\x58\xae\x20\x66\x6f\x72\x20\x57\x69\x6e\x64\x6f\x77\x73"
#define ASC_STR__winzip_ "\x5f\x77\x69\x6e\x7a\x69\x70\x5f"  // "_winzip_"
#define ASC_STR_WinZip	"\x57\x69\x6e\x5a\x69\x70"          // "WinZip"

DECRESULT GetNextExeSignature(FILE *fp, IMAGE_DOS_HEADER *pDosExeHeader, BYTE *pSig);
DECRESULT GetDosExeSize(IMAGE_DOS_HEADER *pDosExeHeader, DWORD *pdwSize);
DECRESULT GetPEExeSize(FILE *fp, DWORD *pdwSize, DWORD dwActualFileSize, bool *pbIsZipExtractor);
DECRESULT CheckForZipNEExe(FILE *fp, bool *pbIsZipExtractor);


/////////////////////////////////////////////////////////////////////////
// GetExeSize()
//
// Get the size of the EXE portion of a self-extracting archive file.
// The size includes the EXE headers and all of the EXE sections (code,
// data, resources, debug directories, etc.).
//
// Special consideration is given to WinZip 32-bit self-extracting archives.
// Most self-extracting acrhives consist of a standard EXE file with a
// PKZip file tacked onto the end.  When using PE EXE self-extractors,
// WinZip archives sometimes keep the PKZip file within an EXE section
// named _winzip_, with additional header information.  For these files,
// the size is the offset of the _winzip_ raw data.
//
// Special consideration is given to 16-bit NE EXE headers.  Since we
// don't have code to correctly determine the full size of an NE EXE program,
// NE Zip self-extractors are detected by checking the program description
// field for the extractor name.  We test for both WinZip and EasyZip signatures.
//
// Special consideration is also given to DOS (non-PE) EXE files.
// If certain bytes are set in the header, the size is rounded up by one.
// This adjustment duplicates the method used by the Beaverton Zip library
// and is designed to accommodate certain Zip extractors.

DECRESULT GetExeSize(FILE *fp, DWORD *pdwSize, bool *pbIsZipExtractor)
{
	DECRESULT			 hr;
	IMAGE_DOS_HEADER	*pDosExeHeader;

	pDosExeHeader = new IMAGE_DOS_HEADER;
	if (!pDosExeHeader)
		return E_FAIL;

	hr = E_FAIL;
	*pdwSize = 0;
	memset(pDosExeHeader, 0, sizeof(IMAGE_DOS_HEADER));

	DWORD dwCurrent = (DWORD)dec_ftell(fp);
	DWORD dwActualFileSize = 0;
	if (!dec_fseek(fp, 0, SEEK_END))
	{
		dwActualFileSize = (DWORD)dec_ftell(fp);
		dec_fseek(fp, dwCurrent, SEEK_SET);
	}
	
	if (dec_fread(pDosExeHeader, sizeof(IMAGE_DOS_HEADER), 1, fp) == 1)
	{
		// We need to convert from little to big endian.
		// Note that we are converting all of the fields here so
		// that any code that looks at these fields later on does not
		// need to.
		pDosExeHeader->e_magic	= WENDIAN(pDosExeHeader->e_magic);
		pDosExeHeader->e_cblp	= WENDIAN(pDosExeHeader->e_cblp);
		pDosExeHeader->e_cp		= WENDIAN(pDosExeHeader->e_cp);
		pDosExeHeader->e_lfanew = DWENDIAN(pDosExeHeader->e_lfanew);

		if (pDosExeHeader->e_magic == IMAGE_DOS_SIGNATURE)	// MZ
		{
			BYTE  sig[4];
			DWORD dwSig;
			WORD  wSig;

			if (SUCCEEDED(GetNextExeSignature(fp, pDosExeHeader, sig)))
			{
				dwSig = *(DWORD *)sig;
				wSig = *(WORD *)sig;

				dwSig = DWENDIAN(dwSig);
				wSig  = WENDIAN(wSig);

				if (dwSig == IMAGE_NT_SIGNATURE)  // PE00
				{
					DWORD dwSize;
					if (SUCCEEDED(GetPEExeSize(fp, &dwSize, dwActualFileSize, pbIsZipExtractor)))
					{
						*pdwSize = dwSize;
						hr = DEC_OK;
					}
				}
				else if (wSig == IMAGE_OS2_SIGNATURE)  // NE
				{
					// Check for specific NE Zip extractor programs.
					if (SUCCEEDED(CheckForZipNEExe(fp, pbIsZipExtractor)))
					{
						*pdwSize = 0;
						hr = DEC_OK;
					}
				}
			}

			// If no second EXE header, or unknown type, use DOS header only.
			if (hr == E_FAIL)
			{
				DWORD dwSize;
				if (SUCCEEDED(GetDosExeSize(pDosExeHeader, &dwSize)))
				{
					*pdwSize = dwSize;
					hr = DEC_OK;
				}
			}
		}
	}

	delete pDosExeHeader;
	return hr;
}


/////////////////////////////////////////////////////////////////////////
// GetNextExeSignature()
//
// Given a file pointer that has just been used to read an MZ EXE header,
// seek to the start of the second EXE header (if any) and return the
// first four bytes.

DECRESULT GetNextExeSignature(FILE* fp, IMAGE_DOS_HEADER* pDosExeHeader, BYTE* pSig)
{
	DWORD dwOffset;

	dwOffset = pDosExeHeader->e_lfanew;

	// If the MZ header has an offset for an additional header...
	if (dwOffset != 0)
	{
		// Seek to the second header.
		if (dec_fseek(fp, dwOffset, SEEK_SET) == 0)
		{
			// Read the first four bytes of the second header.
			if (dec_fread(pSig, 4, 1, fp) == 1)
			{
				// Seek back to the start of the second header.
				dec_fseek(fp, -4, SEEK_CUR);
				return DEC_OK;
			}
		}
	}
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////
// GetDosExeSize()
//
// Given an MZ EXE header, and assuming it is for a simple MZ exe
// (not a PE or other type of EXE), calculate the size of the EXE file.

DECRESULT GetDosExeSize(IMAGE_DOS_HEADER* pDosExeHeader, DWORD* pdwSize)
{
	WORD wRemainder;
	WORD wNumSectors;

	*pdwSize = 0;

	wRemainder	= pDosExeHeader->e_cblp;
	wNumSectors = pDosExeHeader->e_cp;
	if (wRemainder != 0)
	{
		// There is a remainder so reduce the "rounded up" sector count.
		wNumSectors--;
	}

	// Multiply the number of sectors by 512 and add the remainder.
	*pdwSize = ((DWORD)wNumSectors << 9) | wRemainder;

	// Make an adjustment for non-standard ZIP self-extract headers.
	// TODO: Seems a bit unreliable in the event the file is not a
	//       self-extracting ZIP.
	LPBYTE p = (LPBYTE)pDosExeHeader;
	if (p[7] != 26 && p[29] == 1)
		*pdwSize += 1;

	return DEC_OK;
}


/////////////////////////////////////////////////////////////////////////
// GetPEExeSize()
//
// Given a file pointer positioned at a PE EXE header, calculate the
// size of the EXE file.  Give special consideration to WinZip extractors.

DECRESULT GetPEExeSize(FILE *fp, DWORD *pdwSize, DWORD dwActualFileSize, bool *pbIsZipExtractor)
{
	*pdwSize = 0;
	*pbIsZipExtractor = false;
	const int nDebugEntry = 6;

	
	// If we can read the basic PE header and it has the right signature...
	IMAGE_NT_HEADERS NTHeader;
	if (dec_fread(&NTHeader, sizeof(NTHeader), 1, fp) == 1)
	{
		NTHeader.Signature = DWENDIAN(NTHeader.Signature);
		NTHeader.OptionalHeader.SizeOfHeaders = DWENDIAN(NTHeader.OptionalHeader.SizeOfHeaders);
		NTHeader.FileHeader.NumberOfSections = WENDIAN(NTHeader.FileHeader.NumberOfSections);
		NTHeader.OptionalHeader.DataDirectory[nDebugEntry].VirtualAddress = DWENDIAN(NTHeader.OptionalHeader.DataDirectory[nDebugEntry].VirtualAddress);
		NTHeader.OptionalHeader.DataDirectory[nDebugEntry].Size = DWENDIAN(NTHeader.OptionalHeader.DataDirectory[nDebugEntry].Size);
		NTHeader.OptionalHeader.FileAlignment = DWENDIAN(NTHeader.OptionalHeader.FileAlignment);

		if (NTHeader.Signature == IMAGE_NT_SIGNATURE)  // PE00
		{
			// Start with the main header size.
			*pdwSize = NTHeader.OptionalHeader.SizeOfHeaders;

			// Loop through the section headers.
			DWORD dwDebugDirectoryOffset = 0;
			for (DWORD dwCount = 0; dwCount < NTHeader.FileHeader.NumberOfSections; dwCount++)
			{
				IMAGE_SECTION_HEADER SectionHeader;
				if (dec_fread(&SectionHeader, sizeof(SectionHeader), 1, fp) == 1)
				{
					SectionHeader.PointerToRawData = DWENDIAN(SectionHeader.PointerToRawData);
					SectionHeader.SizeOfRawData	  = DWENDIAN(SectionHeader.SizeOfRawData);
					SectionHeader.VirtualAddress   = DWENDIAN(SectionHeader.VirtualAddress);

					// Make sure these are valid values...
					if ((NTHeader.OptionalHeader.FileAlignment != 0) && 
						((SectionHeader.PointerToRawData % NTHeader.OptionalHeader.FileAlignment != 0) || 
						 (SectionHeader.SizeOfRawData    % NTHeader.OptionalHeader.FileAlignment != 0)	  ) )
					{
						continue;
					} 


					// If we found a section named _winzip_,
					// just return the file offset of the section data.
#if defined(UNIX)
					if (strcmp((const char*)(SectionHeader.Name), ASC_STR__winzip_) == 0)
#else
					if (strcmp(reinterpret_cast<const char*>(SectionHeader.Name), "_winzip_") == 0)
#endif
					{
						*pdwSize = SectionHeader.PointerToRawData;
						*pbIsZipExtractor = true;
						return DEC_OK;
					}

					// Make sure that we haven't gone off the deep end with bad data...
					DWORD dwEndOfDataOffset = SectionHeader.PointerToRawData + SectionHeader.SizeOfRawData;
					if ((dwActualFileSize == 0) || ((SectionHeader.PointerToRawData < dwActualFileSize) && (dwEndOfDataOffset < dwActualFileSize)))
					{
						if (dwEndOfDataOffset > *pdwSize)
							*pdwSize = dwEndOfDataOffset;
					}

					// If we found a debug section, remember the debug directory location.
					if ((SectionHeader.VirtualAddress == NTHeader.OptionalHeader.DataDirectory[nDebugEntry].VirtualAddress)
							&& SectionHeader.PointerToRawData != 0
							&& SectionHeader.SizeOfRawData != 0)
						dwDebugDirectoryOffset = SectionHeader.PointerToRawData;
				}
				else
					return E_FAIL;
			}

			// If there is debug information...
			DWORD dwTotalDebugDirs = NTHeader.OptionalHeader.DataDirectory[nDebugEntry].Size / sizeof(IMAGE_DEBUG_DIRECTORY);
			if (dwTotalDebugDirs != 0 && dwDebugDirectoryOffset != 0)
			{
				// Read the last debug directory.
				DWORD dwLastDebugDirectoryOffset = dwDebugDirectoryOffset + (dwTotalDebugDirs - 1) * sizeof(IMAGE_DEBUG_DIRECTORY);
				if (dec_fseek(fp, dwLastDebugDirectoryOffset, SEEK_SET) != 0)
					return E_FAIL;
				IMAGE_DEBUG_DIRECTORY DebugDirectory;
				if (dec_fread(&DebugDirectory, sizeof(DebugDirectory), 1, fp) != 1)
					return E_FAIL;

				DebugDirectory.PointerToRawData = DWENDIAN(DebugDirectory.PointerToRawData);
				DebugDirectory.SizeOfData		= DWENDIAN(DebugDirectory.SizeOfData);

				// Round up the last debug size to be file aligned.
				DWORD dwAlignSize = 0;
				DWORD dwEndOfDebug = DebugDirectory.PointerToRawData + DebugDirectory.SizeOfData;

				if ((dwActualFileSize == 0) || (dwEndOfDebug < dwActualFileSize))
				{
					if (NTHeader.OptionalHeader.FileAlignment != 0 &&
						dwEndOfDebug < (0xffffffff ^ NTHeader.OptionalHeader.FileAlignment))
					{
						dwAlignSize = (dwEndOfDebug / NTHeader.OptionalHeader.FileAlignment) + 1;
						dwAlignSize = dwAlignSize * NTHeader.OptionalHeader.FileAlignment;
					}

					// Calculate the size based on the last debug entry.
					// This overrides any previously calculated size.
					*pdwSize = dwAlignSize;
				}
			}

			return DEC_OK;
		}
	}
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////
// CheckForZipNEExe()
//
// Given a file pointer positioned at a NE EXE header, determine if the
// EXE header is that of a WinZip or EasyZIP self-extracting Zip file.

DECRESULT CheckForZipNEExe(FILE *fp, bool *pbIsZipExtractor)
{
	*pbIsZipExtractor = false;

	// If we can read the basic NE header and it has the right signature...
	_IMAGE_OS2_HEADER NEHeader;
	if (dec_fread(&NEHeader, sizeof(NEHeader), 1, fp) == 1)
	{
		NEHeader.ne_nrestab = DWENDIAN(NEHeader.ne_nrestab);
		NEHeader.ne_magic	= WENDIAN(NEHeader.ne_magic);

		if (NEHeader.ne_magic == IMAGE_OS2_SIGNATURE)  // NE
		{
			// If there's a non-resident name table...
			if (NEHeader.ne_nrestab > 0)
			{
				// If we can seek to the start of the non-resident name table...
				if (dec_fseek(fp, NEHeader.ne_nrestab, SEEK_SET) == 0)
				{
					// Read the first part of the table, enough for the longest string.
					BYTE data[256];
					int nBytes = dec_fread(data, 1, sizeof(data), fp);
					// If the table contains an initial string...
					if (nBytes > 0 && data[0] > 0)
					{
						// Null-terminate the string.
						// This is the program description, as shown by EXEHDR.
						char* pString = (char*) &data[1];
						pString[data[0]] = '\0';

						// If the string begins with "WinZip"...
						// TODO: add check for "EasyZIP" once 16-bit extractor bug is fixed
						if (strncmp(pString, ASC_STR_WinZip, strlen(ASC_STR_WinZip)) == 0 ||
							strncmp(pString, ASC_STR_PKSFX_for_Windows, strlen(ASC_STR_PKSFX_for_Windows)) == 0)
						{
							*pbIsZipExtractor = true;
						}
					}
				}
			}
		}
	}
	return DEC_OK;
}

