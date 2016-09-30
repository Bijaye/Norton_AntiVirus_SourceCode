// Functions that translate a date and a duration to and from the 
// SmellyCat II authorization string

// for this function, szKey must be 12 characters long, minimum
BOOL ConvertKeyDWORDToString(DWORD dwKey, LPTSTR szKey);

// for this function, string must be upper case
BOOL ConvertKeyStringToDWORD(LPCTSTR szKey, DWORD *pKey);

BOOL EncodeDate(int iDay, int iMonth, int iYear, int iDuration, DWORD *pData);
BOOL DecodeDate(DWORD Data, int *pDay, int *pMonth, int *pYear, int *pDuration);
BOOL SCShouldExtend(LPCTSTR szKey);

