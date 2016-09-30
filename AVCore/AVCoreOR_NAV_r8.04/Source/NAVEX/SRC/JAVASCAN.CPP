//************************************************************************
//
// $Header:   S:/NAVEX/VCS/JAVASCAN.CPv   1.3   26 Dec 1996 15:22:44   AOONWAL  $
//
// Description:
//      Source file for Java scanning engine.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/JAVASCAN.CPv  $
// 
//    Rev 1.3   26 Dec 1996 15:22:44   AOONWAL
// No change.
// 
//    Rev 1.2   02 Dec 1996 14:00:32   AOONWAL
// No change.
// 
//    Rev 1.1   29 Oct 1996 12:58:44   AOONWAL
// No change.
// 
//    Rev 1.0   04 Apr 1996 14:55:54   DCHI
// Initial revision.
//
//************************************************************************

#if 0

#include "javascan.h"
#include "javasig.h"

#include "javashr.h"

#define TRUE    1
#define FALSE	0

// extern unsigned _stklen = 16384U;

// Function prototypes

EXTSTATUS EXTJavaMapNameToConstantIndex
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPWORD          lpwNameToIndexMap,      // Array for mapping
    LPWORD          lpwScanAllFlag          // Scan all flag
);

EXTSTATUS EXTJavaSkipInterfaces
(
    LPCALLBACKREV1 lpCallBack,              // File op callbacks
    HFILE          hFile                    // Handle to file to scan
);

EXTSTATUS EXTJavaSkipSingleAttribute
(
    LPCALLBACKREV1 lpCallBack,              // File op callbacks
    HFILE          hFile                    // Handle to file to scan
);

EXTSTATUS EXTJavaSkipAttributes
(
    LPCALLBACKREV1 lpCallBack,              // File op callbacks
    HFILE          hFile                    // Handle to file to scan
);

EXTSTATUS EXTJavaSkipFields
(
    LPCALLBACKREV1 lpCallBack,              // File op callbacks
    HFILE          hFile                    // Handle to file to scan
);

EXTSTATUS EXTJavaSkipToMethods
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile					// Handle to file to scan
);

EXTSTATUS EXTScanFileJavaMethodAttributes
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    LPWORD         	lpwVID,					// Virus ID storage on hit
    LPWORD          lpwNameToIndexMap,      // Array for mapping
    WORD            wScanAllMethods,        // Scan all
    WORD            wMethodNameIndex        // Index of name
);

EXTSTATUS EXTScanFileJavaMethods
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    LPWORD         	lpwVID,					// Virus ID storage on hit
    LPWORD          lpwNameToIndexMap,      // Array for mapping
    WORD            wScanAllMethods         // Scan all?
);


//********************************************************************
//
// Function:
//  EXTSTATUS EXTScanFileJava()
//
// Description:
//  Scans a file for the presence of Java viruses.
//
// Returns:
//      EXTSTATUS_OK                    If no virus was found
//      EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//      If a virus was found, the function sets *lpwVID to the VID
//      of the virus.
//
//********************************************************************

EXTSTATUS EXTScanFileJava
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    LPWORD         	lpwVID					// Virus ID storage on hit
)
{
    WORD    wReturn = EXTSTATUS_OK;
    WORD    wNameToIndexMap[MAX_JAVA_SIGS];
    DWORD   dwHeaderSig;
    WORD    wScanAllFlag;

    // Verify that it is a Java class file

    if (lpCallBack->FileSeek(hFile,0,SEEK_SET) != 0)
        return EXTSTATUS_FILE_ERROR;

    if (lpCallBack->FileRead(hFile,(LPBYTE)&dwHeaderSig,
		sizeof(DWORD)) != sizeof(DWORD))
        return EXTSTATUS_FILE_ERROR;

    if (dwHeaderSig != 0xBEBAFECAUL)
        return EXTSTATUS_OK;

    // seek to where our constant table count is located...

    if (lpCallBack->FileSeek(hFile,8,SEEK_SET) != 8)
        return EXTSTATUS_FILE_ERROR;

    // Search through constant pool looking for strings matching
    // method names from signatures

    if (EXTJavaMapNameToConstantIndex(lpCallBack,
                                      hFile,
                                      wNameToIndexMap,
                                      &wScanAllFlag) != EXTSTATUS_OK)
        return EXTSTATUS_FILE_ERROR;

    // Read through junk so we can get to methods

    if (EXTJavaSkipToMethods(lpCallBack,
                             hFile) != EXTSTATUS_OK)
        return EXTSTATUS_FILE_ERROR;

    // Search through methods looking for matching method names
    // and scan

	wReturn = EXTScanFileJavaMethods(lpCallBack,
    								 hFile,
                                     lpbyWorkBuffer,
                                     lpwVID,
                                     wNameToIndexMap,
                                     wScanAllFlag);

    return (wReturn);
}


//********************************************************************
//
// Function:
//  EXTSTATUS JavaScanArea()
//
// Description:
//  Scans the specified region for Java signatures.
//
// Returns:
//  EXTSTATUS_OK            If no virus was found
//  EXTSTATUS_FILE_ERROR    If an error occurred
//
//********************************************************************

EXTSTATUS   JavaScanArea
(
    LPCALLBACKREV1  lpCallBack,    // File op callbacks
    HFILE           hFile,         // Handle to file to scan
    LPBYTE          lpbyWorkBuffer,// Work buffer >= 512 bytes
    DWORD           dwOffset,      // File offset to start scan
    DWORD           dwScanDist,    // how far to look?
    WORD            wNumSigs,      // # of sigs to scan
    JAVASIG_T *     pSigs,         // pointer to our sigs
    DWORD           dwSigFlags,    // bitwise & for flags
    LPWORD          lpwVID         // Virus ID storage on hit
)
{
    DWORD       dwCurOffset;
    long        lBytesLeft;
    WORD        wIndex, wScanIndex, wBytesToScan, wTemp;

    // Make sure scan distance <= 128K

    if (dwScanDist > 0x1FFFEUL)
        return EXTSTATUS_FILE_ERROR;

    lBytesLeft = (long)dwScanDist;
    dwCurOffset = dwOffset;

    if (dwOffset & 0x80000000UL)
        return EXTSTATUS_FILE_ERROR;

    while (lBytesLeft > 0)
    {
        if (lpCallBack->FileSeek(hFile,dwCurOffset,SEEK_SET) != dwCurOffset)
            return EXTSTATUS_FILE_ERROR;

		if (lBytesLeft > SCAN_BUFFER_SIZE)
		{
			wBytesToScan = SCAN_BUFFER_SIZE;
	        lBytesLeft -= SCAN_BUFFER_SIZE - SCAN_OVERLAP_SIZE;
		}
		else
		{
            wBytesToScan = (WORD)lBytesLeft;
			lBytesLeft = 0;
		}

        if (lpCallBack->FileRead(hFile,lpbyWorkBuffer,wBytesToScan) !=
            wBytesToScan)
            return EXTSTATUS_FILE_ERROR;

        for (wScanIndex = 0; wScanIndex < wBytesToScan; wScanIndex++)
        {
            for (wIndex = 0; wIndex < wNumSigs; wIndex++)
            {
                if (pSigs[wIndex].wFirstSigWord ==
                    *(LPWORD)(lpbyWorkBuffer + wScanIndex) &&
                    (pSigs[wIndex].dwFlags & dwSigFlags) &&
					(wScanIndex + pSigs[wIndex].wSigLen) <= wBytesToScan)
                {
                    WORD    wLen;
                    WORD    *pSigData;

                    wLen = pSigs[wIndex].wSigLen;
                    pSigData = pSigs[wIndex].pwSig;

                    for (wTemp = 2 ; wTemp < wLen ; wTemp++)
                        if (pSigData[wTemp] !=
                            lpbyWorkBuffer[wScanIndex + wTemp] &&
                            pSigData[wTemp] != WILD_CARD_VALUE)
                            break;

                    if (wTemp == wLen)
                    {
                        *lpwVID = pSigs[wIndex].wVID;

                        return EXTSTATUS_VIRUS_FOUND;
                    }
                }
            }
		}

        dwCurOffset += SCAN_BUFFER_SIZE - SCAN_OVERLAP_SIZE;
    }

    return EXTSTATUS_OK;
}


//********************************************************************
//
// Function:
//  EXTSTATUS EXTJavaMapNameToConstantIndex()
//
// Description:
//  Assumes file pointer points to interfaces_count field.
//  The function parses and skips the following:
//      u2 constant_pool_count;
//      cp_info constant_pool[constant_pool_count - 1];
//
//  During parsing, strings are compared against the method
//  names of the definitions.  The indices of these method
//  names in the constant pool are stored in the wNameToIndexMap[]
//  array.
//
//  If any method name in the definitions has more than one
//  occurrence in the constant name pool, then the scan all
//  flag is set to true.
//
// Returns:
//  EXTSTATUS_OK            If no virus was found
//  EXTSTATUS_FILE_ERROR    If an error occurred
//
//********************************************************************

EXTSTATUS EXTJavaMapNameToConstantIndex
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPWORD          lpwNameToIndexMap,      // Array for mapping
    LPWORD          lpwScanAllFlag          // Scan all flag
)
{
    BYTE byTag, byNameBuf[MAX_JAVA_METHOD_NAME_LEN];
    WORD i, j, k, wConstantPoolCount, wLen;
    CONSTANT_T stConstant;

    // Initialize all mapping entries to zero

    for (i=0;i<gwNumJavaSigs;i++)
        lpwNameToIndexMap[i] = 0;

    // Don't scan all if possible

    *lpwScanAllFlag = FALSE;

    for (i=0;i<gwNumJavaSigs;i++)
        if (gJavaSigs[i].dwFlags & JAVA_SCAN_ALL_METHODS)
        {
            *lpwScanAllFlag = TRUE;
            break;
        }

    // Read constant_pool_count

    if (lpCallBack->FileRead(hFile,(LPBYTE)&wConstantPoolCount,
       sizeof(WORD)) != sizeof(WORD))
       return (EXTSTATUS_FILE_ERROR);

    // don't go 64K times

    if (wConstantPoolCount == 0)
        return EXTSTATUS_FILE_ERROR;

    wConstantPoolCount = WordConvert(wConstantPoolCount) - 1;

    // Parse constant pool and look for names

    for (i=0;i<wConstantPoolCount;i++)
    {
    	if (lpCallBack->FileRead(hFile,&byTag,
			sizeof(BYTE)) != sizeof(BYTE))
			return (EXTSTATUS_FILE_ERROR);

    	switch (byTag)
        {
	        case CONSTANT_Class:
	        case CONSTANT_String:

            	// Class and String are the same size

		    	if (lpCallBack->FileRead(hFile,(LPBYTE)&stConstant,
					sizeof(CONSTANT_Class_T)) != sizeof(CONSTANT_Class_T))
					return (EXTSTATUS_FILE_ERROR);

	        	break;

    	    case CONSTANT_Fieldref:
	        case CONSTANT_Methodref:
        	case CONSTANT_InterfaceMethodref:
        	case CONSTANT_NameAndType:

            	// All three are the same size

		    	if (lpCallBack->FileRead(hFile,(LPBYTE)&stConstant,
					sizeof(CONSTANT_Fieldref_T)) !=
					sizeof(CONSTANT_Fieldref_T))
					return (EXTSTATUS_FILE_ERROR);

        		break;

        	case CONSTANT_Integer:
	        case CONSTANT_Float:

            	// Integer and Float have the same size

		    	if (lpCallBack->FileRead(hFile,(LPBYTE)&stConstant,
					sizeof(CONSTANT_Integer_T)) !=
					sizeof(CONSTANT_Integer_T))
					return (EXTSTATUS_FILE_ERROR);

    	    	break;

        	case CONSTANT_Long:
	        case CONSTANT_Double:

            	// Long and Double have the same size

		    	if (lpCallBack->FileRead(hFile,(LPBYTE)&stConstant,
					sizeof(CONSTANT_Long_T)) !=
					sizeof(CONSTANT_Long_T))
					return (EXTSTATUS_FILE_ERROR);

            	if (++i >= wConstantPoolCount)
                	return (EXTSTATUS_FILE_ERROR);

        		break;

	        case CONSTANT_Utf8:
        	case CONSTANT_Unicode:

            	// Utf8 and Unicode have the same format

				if (lpCallBack->FileRead(hFile,(LPBYTE)&wLen,
                	sizeof(WORD)) != sizeof(WORD))
					return (EXTSTATUS_FILE_ERROR);

                wLen = WordConvert(wLen);

                byNameBuf[0] = 0;

                if (*lpwScanAllFlag == FALSE)
                {
                    // Only try to match method names if flag not set yet
                    // If we know we're going to scan all methods, why bother!

                    for (j=0;j<gwNumJavaSigs;j++)
                    {
                        if ((gJavaSigs[j].dwFlags & JAVA_SCAN_NAMED_METHOD) &&
                            gJavaSigs[j].wMethodNameLen == wLen)
                        {
                            if (byNameBuf[0] == 0)
                            {
                                if (lpCallBack->FileRead(hFile,byNameBuf,
                                    wLen) != wLen)
                                    return (EXTSTATUS_FILE_ERROR);

                                if (byNameBuf[0] == 0)
                                    byNameBuf[0] = TRUE;
                            }

                            // Compare strings

                            for (k=0;k<wLen;k++)
                            {
                               if (gJavaSigs[j].pbyMethodName[k] !=
                                   byNameBuf[k])
                                   break;
                            }

                            // Did the strings match?

                            if (k == wLen)
                            {
                            	// Matched method name

                                if (lpwNameToIndexMap[j])
                                {
                                    // More than one occurrence
                                    //  of this method name in the
                                    //  constant pool

                                    *lpwScanAllFlag = TRUE;
                                }
                                else
                                {
                                    // First occurrence of this
                                    //  method name in the constant
                                    //  pool

                                    lpwNameToIndexMap[j] = i + 1;
                                }
                            }
                        }
                    }
                }

                if (byNameBuf[0] == 0)
                {
                    if (lpCallBack->FileSeek(hFile,wLen,SEEK_CUR) ==
                        (DWORD)-1)
                        return (EXTSTATUS_FILE_ERROR);
                }

        		break;

            default:
            	return (EXTSTATUS_FILE_ERROR);
		}
    }

    return (EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EXTSTATUS EXTJavaSkipInterfaces()
//
// Description:
//  Assumes file pointer points to interfaces_count field.
//  The function skips the following:
//      u2 interfaces_count;
//      u2 interfaces[interfaces_count];
//
// Returns:
//  EXTSTATUS_OK            If no virus was found
//  EXTSTATUS_FILE_ERROR    If an error occurred
//
//********************************************************************

EXTSTATUS EXTJavaSkipInterfaces
(
    LPCALLBACKREV1 lpCallBack,              // File op callbacks
    HFILE          hFile                    // Handle to file to scan
)
{
    WORD wInterfacesCount;

    // Read interfaces_count

	if (lpCallBack->FileRead(hFile,(LPBYTE)&wInterfacesCount,
    	sizeof(WORD)) != sizeof(WORD))
        return (EXTSTATUS_FILE_ERROR);

    wInterfacesCount = WordConvert(wInterfacesCount);

    // Parse skip interfaces

    if (lpCallBack->FileSeek(hFile,
        (DWORD)sizeof(WORD) * (DWORD)wInterfacesCount,SEEK_CUR) ==
        (DWORD)-1)
        return (EXTSTATUS_FILE_ERROR);

    return (EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EXTSTATUS EXTJavaSkipSingleAttribute()
//
// Description:
//  Assumes file pointer points to attribute_name field.
//  The function skips the following:
//      u2 attribute_name;
//      u4 attribute_length;
//      u1 info[attribute_length];
//
// Returns:
//  EXTSTATUS_OK            If no virus was found
//  EXTSTATUS_FILE_ERROR    If an error occurred
//
//********************************************************************

EXTSTATUS EXTJavaSkipSingleAttribute
(
    LPCALLBACKREV1 lpCallBack,              // File op callbacks
    HFILE          hFile                    // Handle to file to scan
)
{
    DWORD dwAttrLen;

    // Skip attribute_name

    if (lpCallBack->FileSeek(hFile,sizeof(WORD),SEEK_CUR) ==
        (DWORD)-1)
        return (EXTSTATUS_FILE_ERROR);

    // Read attribute_length

    if (lpCallBack->FileRead(hFile,(LPBYTE)&dwAttrLen,
		sizeof(DWORD)) != sizeof(DWORD))
        return (EXTSTATUS_FILE_ERROR);

    // Skip attribute_length bytes

    if (lpCallBack->FileSeek(hFile,DWordConvert(dwAttrLen),SEEK_CUR) ==
        (DWORD)-1)
        return (EXTSTATUS_FILE_ERROR);

    return (EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EXTSTATUS EXTJavaSkipAttributes()
//
// Description:
//  Assumes file pointer points to attributes_count field.
//  The function skips the following:
//      u2 attributes_count;
//      attribute_info  attributes[attribute_count];
//
// Returns:
//  EXTSTATUS_OK            If no virus was found
//  EXTSTATUS_FILE_ERROR    If an error occurred
//
//********************************************************************

EXTSTATUS EXTJavaSkipAttributes
(
    LPCALLBACKREV1 lpCallBack,              // File op callbacks
    HFILE          hFile                    // Handle to file to scan
)
{
    WORD wAttributesCount, i;

    // Read attributes_count

    if (lpCallBack->FileRead(hFile,(LPBYTE)&wAttributesCount,
		sizeof(WORD)) != sizeof(WORD))
        return (EXTSTATUS_FILE_ERROR);

    wAttributesCount = WordConvert(wAttributesCount);

    // Skip over all attributes

    for (i=0;i<wAttributesCount;i++)
    {
        if (EXTJavaSkipSingleAttribute(lpCallBack, hFile) != EXTSTATUS_OK)
            return(EXTSTATUS_FILE_ERROR);
    }

    return (EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EXTSTATUS EXTJavaSkipToFields()
//
// Description:
//  Assumes file pointer points to fields_count field.
//  The function skips the following:
//      u2 fields_count;
//      field_info fields[fields_count];
//
// Returns:
//  EXTSTATUS_OK            If no virus was found
//  EXTSTATUS_FILE_ERROR    If an error occurred
//
//********************************************************************

EXTSTATUS EXTJavaSkipFields
(
    LPCALLBACKREV1 lpCallBack,              // File op callbacks
    HFILE          hFile                    // Handle to file to scan
)
{
    WORD i, wFieldsCount;

    // Read fields_count

    if (lpCallBack->FileRead(hFile,(LPBYTE)&wFieldsCount,
		sizeof(WORD)) != sizeof(WORD))
        return (EXTSTATUS_FILE_ERROR);

    wFieldsCount = WordConvert(wFieldsCount);

    // Skip over all fields

    for (i=0;i<wFieldsCount;i++)
    {
        // Skip over:
        //  u2 access_flags;
        //  u2 name_index;
        //  u2 signature_index;

        if (lpCallBack->FileSeek(hFile,6,SEEK_CUR) == (DWORD)-1)
            return (EXTSTATUS_FILE_ERROR);

        // Skip over attributes of field

        if (EXTJavaSkipAttributes(lpCallBack, hFile) != EXTSTATUS_OK)
            return (EXTSTATUS_FILE_ERROR);
    }

    return (EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EXTSTATUS EXTJavaSkipToMethods()
//
// Description:
//  Assumes file pointer points to access_flags field.
//  The function skips the following fields:
//      u2 access_flags;
//      u2 this_class;
//      u2 super_class;
//      u2 interfaces_count;
//      u2 interfaces[interfaces_count];
//      u2 fields_count;
//      field_info fields[fields_count];
//
// Returns:
//  EXTSTATUS_OK            If no virus was found
//  EXTSTATUS_FILE_ERROR    If an error occurred
//
//********************************************************************

EXTSTATUS EXTJavaSkipToMethods
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile					// Handle to file to scan
)
{
    // Skip access_flags, this_class, super_class

    if (lpCallBack->FileSeek(hFile,3*sizeof(WORD),SEEK_CUR) ==
        (DWORD)-1)
		return (EXTSTATUS_FILE_ERROR);

    // Read interfaces_count and parse interfaces

    if (EXTJavaSkipInterfaces(lpCallBack, hFile) != EXTSTATUS_OK)
        return(EXTSTATUS_FILE_ERROR);

    // Read fields_count and parse skip fields

    if (EXTJavaSkipFields(lpCallBack, hFile) != EXTSTATUS_OK)
        return(EXTSTATUS_FILE_ERROR);

    return (EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EXTSTATUS EXTScanFileJavaMethodAttributes()
//
// Description:
//  Parses attributes of a method, scanning for the presence
//  of Java viruses.
//
// Returns:
//  EXTSTATUS_OK            If no virus was found
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//  If a virus was found, the function sets *lpwVID to the VID
//  of the virus.
//
//********************************************************************

EXTSTATUS EXTScanFileJavaMethodAttributes
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    LPWORD         	lpwVID,					// Virus ID storage on hit
    LPWORD          lpwNameToIndexMap,      // Array for mapping
    WORD            wScanAllMethods,        // Scan all
    WORD            wMethodNameIndex        // Index of name
)
{
    WORD i, wAttributesCount, wScanCurMethod;
    DWORD dwAttributeOffset, dwAttributeLength;
    DWORD dwScanDist, dwActualDist;
    EXTSTATUS extResult;

    dwScanDist = 0;

    wScanCurMethod = FALSE;
    if (wScanAllMethods == FALSE)
    {
        for (i=0;i<gwNumJavaSigs;i++)
        {
            // See if method name index matches any definition method name

            if (0 != (gJavaSigs[i].dwFlags & JAVA_SCAN_NAMED_METHOD))
            {
                // Compare index

                if (wMethodNameIndex == lpwNameToIndexMap[i])
                {
                    wScanCurMethod = TRUE;

                    if (gJavaSigs[i].dwDistToSearch > dwScanDist)
                        dwScanDist = gJavaSigs[i].dwDistToSearch;
                }
            }
            else
            if (0 != (gJavaSigs[i].dwFlags & JAVA_SCAN_ALL_METHODS))
            {
                // JAVA_SCAN_ALL_METHODS in at least one definition
                //  means we must scan this method

                wScanCurMethod = TRUE;

                if (gJavaSigs[i].dwDistToSearch > dwScanDist)
                    dwScanDist = gJavaSigs[i].dwDistToSearch;
            }

        }
    }
    else
    {
        // scan all flag set, just update distance to search

        for (i=0;i<gwNumJavaSigs;i++)
            if (gJavaSigs[i].dwDistToSearch > dwScanDist)
                dwScanDist = gJavaSigs[i].dwDistToSearch;

        wScanCurMethod = TRUE;
    }

    // Read attributes_count

    if (lpCallBack->FileRead(hFile,(LPBYTE)&wAttributesCount,
    	sizeof(WORD)) != sizeof(WORD))
        return (EXTSTATUS_FILE_ERROR);

    wAttributesCount = WordConvert(wAttributesCount);

    // Parse through attributes and scan

    for (i=0;i<wAttributesCount;i++)
    {
    	// Skip pass attribute_name

        if (lpCallBack->FileSeek(hFile,sizeof(WORD),SEEK_CUR) == (DWORD)-1)
            return (EXTSTATUS_FILE_ERROR);

        // Read attribute_length

        if (lpCallBack->FileRead(hFile,(LPBYTE)&dwAttributeLength,
            sizeof(DWORD)) != sizeof(DWORD))
            return (EXTSTATUS_FILE_ERROR);

        dwAttributeLength = DWordConvert(dwAttributeLength);

        // Just skip pass attribute if not scanning this method

        if (wScanCurMethod == FALSE)
        {
            if (lpCallBack->FileSeek(hFile,dwAttributeLength,SEEK_CUR) ==
                (DWORD)-1)
                return (EXTSTATUS_FILE_ERROR);
        }
        else
        {
            // We are scanning this method.
            //
            // Remember current file offset for repositioning
            // after scanning the attribute

            dwAttributeOffset = lpCallBack->FileSeek(hFile,0,SEEK_CUR);

            if (dwAttributeOffset == (DWORD)-1)
                return (EXTSTATUS_FILE_ERROR);

            // Compute an actual scan distance

            if (dwScanDist > dwAttributeLength)
                dwActualDist = dwAttributeLength;
            else
                dwActualDist = dwScanDist;

            // scan the attribute... (assume everything is code)

            extResult = JavaScanArea(lpCallBack,
                                     hFile,
                                     lpbyWorkBuffer,
                                     dwAttributeOffset,
                                     dwActualDist,
                                     gwNumJavaSigs,
                                     gJavaSigs,
                                     JAVA_SCAN_NAMED_METHOD |
                                     JAVA_SCAN_ALL_METHODS,
                                     lpwVID);

            if (extResult != EXTSTATUS_OK)
                return(extResult);

            // Position file pointer after this attribute

            if (lpCallBack->FileSeek(hFile,
                                     dwAttributeOffset+dwAttributeLength,
                                     SEEK_SET) ==
                (DWORD)-1)
                return (EXTSTATUS_FILE_ERROR);
    	}
    }

    return (EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EXTSTATUS EXTScanFileJavaMethods()
//
// Description:
//  Parses all methods, scanning for the presence of Java viruses.
//
// Returns:
//  EXTSTATUS_OK            If no virus was found
//  EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//  If a virus was found, the function sets *lpwVID to the VID
//  of the virus.
//
//********************************************************************

EXTSTATUS EXTScanFileJavaMethods
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    LPWORD         	lpwVID,					// Virus ID storage on hit
    LPWORD          lpwNameToIndexMap,      // Array for mapping
    WORD            wScanAllMethods         // Scan all?
)
{
    WORD        i, wMethodsCount, wNameIndex;
    EXTSTATUS   extResult;

    // Read methods_count

	if (lpCallBack->FileRead(hFile,(LPBYTE)&wMethodsCount,
    	sizeof(WORD)) != sizeof(WORD))
        return (EXTSTATUS_FILE_ERROR);

    wMethodsCount = WordConvert(wMethodsCount);

    // Parse through methods and scan for applicable signatures

    for (i=0;i<wMethodsCount;i++)
    {
        // Skip access_flags

        if (lpCallBack->FileSeek(hFile,2,SEEK_CUR) == (DWORD)-1)
            return (EXTSTATUS_FILE_ERROR);

        // Read name_index

        if (lpCallBack->FileRead(hFile,(LPBYTE)&wNameIndex,
            sizeof(WORD)) != sizeof(WORD))
            return (EXTSTATUS_FILE_ERROR);

        wNameIndex = WordConvert(wNameIndex);

        // Skip signature_index

        if (lpCallBack->FileSeek(hFile,2,SEEK_CUR) == (DWORD)-1)
            return (EXTSTATUS_FILE_ERROR);

        // Call auxiliary function to scan the method

        extResult = EXTScanFileJavaMethodAttributes(lpCallBack,
                                                    hFile,
                                                    lpbyWorkBuffer,
                                                    lpwVID,
                                                    lpwNameToIndexMap,
                                                    wScanAllMethods,
                                                    wNameIndex);

        if (extResult != EXTSTATUS_OK)
            return(extResult);
    }

    return (EXTSTATUS_OK);
}

#endif // #if 0
