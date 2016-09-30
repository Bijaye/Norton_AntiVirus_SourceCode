#pragma once

class CInfectionMerge
{
public:
    CInfectionMerge(void);
    ~CInfectionMerge(void);

    // Returns false if an identical infection item is already in our list
    // the item will be added to our list
    bool MergeInfection(IScanInfection* pInfection);

    // Returns true if an identical infection item is already in our list
    bool CheckInfection(IScanInfection* pInfection);
    bool CheckFileVidItem(LPCSTR pcszLongFileName, unsigned long ulVID);
    bool CheckContainer(IScanCompressedFileInfection2* pCompressed);

    void SetToANSIMode();
    void empty();

    bool buildIndexString( IScanInfection* pInfection, std::string& sFile );

protected:
    void ConvertToProperCharSet(LPCSTR pszOrig, std::string& strOut);
    void ConvertToLower(std::string& str);

    // Index for merging.
    typedef std::set<unsigned long> INDEX_VALUE;
    typedef std::map<std::string, INDEX_VALUE*> INDEX_MAP;
    INDEX_MAP m_Index;

    // Are we storing the file names in ANSI mode?
    bool m_bOEMMode;
};
