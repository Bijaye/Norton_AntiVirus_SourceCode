#ifdef CREATE_AVISTRANSACTIONS_DLL
#define IMPEXP __declspec( dllexport )
#else
#define IMPEXP __declspec( dllimport )
#endif
