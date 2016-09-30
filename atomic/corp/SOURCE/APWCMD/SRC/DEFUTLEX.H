class  CDefUtilsEx : public CDefUtils
{
public:
    BOOL StartNewDefsDetection(void);
    BOOL IsNewDefsAvailable(void);
    void StopNewDefsDetection(void);

private:
    BOOL DirectoryExists(LPCSTR pszDir);
    BOOL GetVirusDefsDir(LPSTR pszDefsDir, int nMaxCount);
    char StringGetLastChar(LPCSTR lpszString);
    void RemoveLastBackslash(LPSTR pszDir);

    HANDLE m_hDefsUpdated;    // handle for directory change notification
};
