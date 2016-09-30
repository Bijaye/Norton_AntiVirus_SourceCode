
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ICEPACKTOKENS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ICEPACKTOKENS_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifdef ICEPACKTOKENS_EXPORTS
#define ICEPACKTOKENS_API __declspec(dllexport)
#else
#define ICEPACKTOKENS_API __declspec(dllimport)
#endif

//--------------------------------------------------------------------
// Declare the tables of IcePack tokens as either exported or imported
// variables for the IcePackTokens.DLL file, depending upon whether
// we are building the IcePackTokens.DLL file itself, or another
// program that is linked with it.
//--------------------------------------------------------------------

extern ICEPACKTOKENS_API const wchar_t* const IcePackAttentionTable[];
extern ICEPACKTOKENS_API const wchar_t* const IcePackErrorTable[];
extern ICEPACKTOKENS_API const wchar_t* const IcePackStateTable[];
extern ICEPACKTOKENS_API const wchar_t* const IcePackStatusTable[];
extern ICEPACKTOKENS_API const wchar_t* const IcePackResultTable[];

// LOOKUP ICEPACK STRINGS FROM TOKEN
#define ICEPACK_TOKEN_ATTENTION_TABLE    1
#define ICEPACK_TOKEN_ERROR_TABLE        2
#define ICEPACK_TOKEN_STATE_TABLE        3
#define ICEPACK_TOKEN_STATUS_TABLE       4
#define ICEPACK_TOKEN_RESULT_TABLE       5
//TCHAR *LookUpIcePackTokenString(TCHAR *lpszToken, DWORD dwTableID);
