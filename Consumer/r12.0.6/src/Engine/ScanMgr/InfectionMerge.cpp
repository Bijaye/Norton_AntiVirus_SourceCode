#include "Stdafx.h"
#include "Infectionmerge.h"

CInfectionMerge::CInfectionMerge(void) : m_bOEMMode(true)
{
}

CInfectionMerge::~CInfectionMerge(void)
{
    empty();
}

// If the item already exists in our map then we return false, otherwise
// we add the item and return true
bool CInfectionMerge::MergeInfection(IScanInfection* pInfection)
{
    bool bAdded = false;
    std::string sFile;

    // Get the virus ID.
    unsigned long ulVirusID = pInfection->GetVirusID();

    // For non file infections, return that the index is added.
    if( buildIndexString( pInfection, sFile ) == false )
        return true;

    // Convert it to lower case
    ConvertToLower(sFile);

    // Search for this element in the list.
    INDEX_MAP::iterator index = m_Index.find( sFile );
    if( index == m_Index.end() )
    {
        // No such element.  Go ahead and add it.
        CCTRACEI(_T("CInfectionMerge::MergeInfection() - Adding item %s to our list."), sFile.c_str());

        INDEX_VALUE* pValue = new INDEX_VALUE;
        pValue->insert(ulVirusID);

        // Insert into map.
        m_Index[sFile] = pValue;
        bAdded = true;
    }
    else
    {
        // We found the element.  Test to see if the virus ID is already present.  
        if( (*index).second->find( ulVirusID ) == (*index).second->end() )
        {
            // This value does not exist.  add it.
            CCTRACEI(_T("CInfectionMerge::MergeInfection() - Found item %s already in list but the VID is different. Adding to list."), sFile.c_str());
            (*index).second->insert( ulVirusID );
            bAdded = true;
        }
        else
        {
            CCTRACEI(_T("CInfectionMerge::MergeInfection() - Found item %s already in list with the same VID."), sFile.c_str());
        }
    }

    return bAdded;
}

// Just check if the item already exists in our map
bool CInfectionMerge::CheckInfection(IScanInfection* pInfection)
{
    std::string sFile;

    // Get the virus ID.
    unsigned long ulVirusID = pInfection->GetVirusID();

    // For non file infections, return that the index is added.
    if( buildIndexString( pInfection, sFile ) == false )
        return false;

    // Convert it to lower case
    ConvertToLower(sFile);

    // Search for this element in the list.
    INDEX_MAP::iterator index = m_Index.find( sFile );
    if( index == m_Index.end() )
    {
        // No such element.
        CCTRACEI(_T("CInfectionMerge::CheckInfection() - Item %s is not in our list."), sFile.c_str());
        return false;
    }
    else
    {
        // We found the element.  Now make sure the virus ID is also matched for this file.  
        if( (*index).second->find( ulVirusID ) == (*index).second->end() )
        {
            // This value does not exist.
            CCTRACEI(_T("CInfectionMerge::CheckInfection() - Found item %s already in list but the VID is different."), sFile.c_str());
            return false;
        }
        else
        {
            CCTRACEI(_T("CInfectionMerge::CheckInfection() - Found item %s already in list with the same VID."), sFile.c_str());
            return true;
        }
    }
}

bool CInfectionMerge::CheckFileVidItem(LPCSTR pcszLongFileName, unsigned long ulVID)
{
    // If we're in OEM mode we need to convert this path
    std::string sFile;
    if( m_bOEMMode )
    {
        ConvertToProperCharSet(pcszLongFileName, sFile);
    }
    else
        sFile = pcszLongFileName;

    // Convert it to lower case
    ConvertToLower(sFile);

    // Now sFile is our index string. Search for this element in the list.
    INDEX_MAP::iterator index = m_Index.find( sFile );
    if( index == m_Index.end() )
    {
        // No such element.
        CCTRACEI(_T("CInfectionMerge::CheckInfection(file-vid) - Item %s is not in our list."), sFile.c_str());
        return false;
    }
    else
    {
        // We found the element.  Now make sure the virus ID is also matched for this file.  
        if( (*index).second->find( ulVID ) == (*index).second->end() )
        {
            // This value does not exist.
            CCTRACEI(_T("CInfectionMerge::CheckInfection(file-vid) - Found item %s already in list but the VID is different."), sFile.c_str());
            return false;
        }
        else
        {
            CCTRACEI(_T("CInfectionMerge::CheckInfection(file-vid) - Found item %s already in list with the same VID."), sFile.c_str());
            return true;
        }
    }
}

bool CInfectionMerge::CheckContainer(IScanCompressedFileInfection2* pCompressed)
{
    std::string sFile;
    int iCount = pCompressed->GetComponentCount();
    int iOuterIndex = iCount-1;

    // Start at 1 to skip the item we are checking and only check for the
    // levels above it
    for( int i = 1; i < iOuterIndex; i++ )
        sFile += pCompressed->GetComponent( i );

    // The outer component possibly needs to be converted to the proper character set
    if( m_bOEMMode  )
    {
        // Make sure the container path is in OEM as well
        if( !pCompressed->AreOEMFileApis() )
        {
            std::string strTemp;
            ConvertToProperCharSet(pCompressed->GetComponent( iOuterIndex ), strTemp);
            sFile += strTemp;
        }
        else
            sFile += pCompressed->GetComponent( iOuterIndex );

    }
    else
    {
        // Make sure the container path is in ANSI as well
        if( pCompressed->AreOEMFileApis() )
        {
            std::string strTemp;
            ConvertToProperCharSet(pCompressed->GetComponent( iOuterIndex ), strTemp);
            sFile += strTemp;
        }
        else
            sFile += pCompressed->GetComponent( iOuterIndex );
    }

    // Convert it to lower case
    ConvertToLower(sFile);

    // Now sFile is our index string. Search for this element in the list.
    INDEX_MAP::iterator index = m_Index.find( sFile );
    if( index == m_Index.end() )
    {
        // No such element.
        CCTRACEI(_T("CInfectionMerge::CheckContainer() - Item %s is not in our list."), sFile.c_str());
        return false;
    }
    else
    {
        // We found the element.  Now make sure the virus ID is also matched for this file.  
        if( (*index).second->find( pCompressed->GetVirusID() ) == (*index).second->end() )
        {
            // This value does not exist.
            CCTRACEI(_T("CInfectionMerge::CheckContainer() - Found item %s already in list although the VID is different..."), sFile.c_str());
            return true;
        }
        else
        {
            CCTRACEI(_T("CInfectionMerge::CheckContainer() - Found item %s already in list with the same VID."), sFile.c_str());
            return true;
        }
    }
}

void CInfectionMerge::empty()
{
    // Free all the index allocated memory
    for( INDEX_MAP::iterator it = m_Index.begin(); it != m_Index.end(); ++it )
        delete (*it).second;

    m_Index.clear();
}

void CInfectionMerge::SetToANSIMode()
{
    m_bOEMMode = false;
}

bool CInfectionMerge::buildIndexString( IScanInfection* pInfection, std::string& sFile )
{
    IScanFileInfection2Ptr pFileInfection;
    IScanCompressedFileInfection2Ptr pCompressedFileInfection;
    sFile.clear();

    // Test for file infection.
    if( pInfection->QueryInterface( IID_ScanFileInfection, (void**) &pFileInfection ) == SYM_OK )
    {
        // The file name is the index string

        // Are the file API's for this file infection the same as this class is expecting?
        if( m_bOEMMode )
        {
            // Make sure the file paths are in OEM as well
            if( !pFileInfection->AreOEMFileApis() )
                ConvertToProperCharSet(pFileInfection->GetLongFileName(), sFile);
            else
                sFile = pFileInfection->GetLongFileName();
        }
        else
        {
            // Make sure the file paths are in ANSI as well
            if( !pFileInfection->AreOEMFileApis() )
                ConvertToProperCharSet(pFileInfection->GetLongFileName(), sFile);
            else
                sFile = pFileInfection->GetLongFileName();
        }
    }
    // Test for compressed infection.
    else if( pInfection->QueryInterface( IID_ScanCompressedFileInfection2, (void**) &pCompressedFileInfection ) == SYM_OK )
    {
        // Combine all components except the outer most component which may need
        // some character converting
        int iCount = pCompressedFileInfection->GetComponentCount();
        int iOuterIndex = iCount-1;
        for( int i = 0; i < iOuterIndex; i++ )
            sFile += pCompressedFileInfection->GetComponent( i );

        // The outer component possibly needs to be converted to OEM
        if( m_bOEMMode  )
        {
            // Make sure the container path is in OEM as well
            if( !pCompressedFileInfection->AreOEMFileApis() )
            {
                std::string strTemp;
                ConvertToProperCharSet(pCompressedFileInfection->GetComponent( iOuterIndex ), strTemp);
                sFile += strTemp;
            }
            else
                sFile += pCompressedFileInfection->GetComponent( iOuterIndex );
            
        }
        else
        {
            // Make sure the container path is in ANSI as well
            if( pCompressedFileInfection->AreOEMFileApis() )
            {
                std::string strTemp;
                ConvertToProperCharSet(pCompressedFileInfection->GetComponent( iOuterIndex ), strTemp);
                sFile += strTemp;
            }
            else
                sFile += pCompressedFileInfection->GetComponent( iOuterIndex );
        }
    }
    else
        // Non-file based infection.
    {
        return false;
    }

    return true;
}

void CInfectionMerge::ConvertToProperCharSet(LPCSTR pcszOrig, std::string& strOut)
{
    std::vector<char> vNew;

    int iBufferLen = _tcslen(pcszOrig);
    vNew.reserve(iBufferLen + 1);

    if( m_bOEMMode )
        CharToOem(pcszOrig, &vNew[0]);
    else
        OemToChar(pcszOrig, &vNew[0]);

    strOut = &vNew[0];
}

void CInfectionMerge::ConvertToLower(std::string& str)
{
    CAtlString strTemp = str.c_str();
    _tcslwr(strTemp.GetBuffer(strTemp.GetLength()+1));
    strTemp.ReleaseBuffer();
    str = (LPCTSTR)strTemp;
}