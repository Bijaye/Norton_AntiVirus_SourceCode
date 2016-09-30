
#ifndef	__PERFSNAP_H_
	#define	__PERFSNAP_H_

#ifndef	_WINPERF_
	#include	<winperf.h>
#endif

class CPerfTitleDatabase;

class CPerfSnapshot
{
private:
	PPERF_DATA_BLOCK		m_pPerfDataHeader;
	CPerfTitleDatabase		*m_pCounterTitles;

	// Private function to convert the ASCII strings passed to TakeSnapshot()
	BOOL	ConvertSnapshotItemName (PTSTR pszIn, PTSTR pszOut, DWORD nSize);

public:
	CPerfSnapshot (CPerfTitleDatabase	*pCounterTitles);
	~CPerfSnapshot (void);

	BOOL	TakeSnapshot (PTSTR pszSnapshotItems);
	void	DisposeSnapshot (void);
	DWORD	GetNumObjectTypes (void);	// # of objects the snapshot includes
	BOOL	GetSystemName (PTSTR pszSystemName, DWORD nSize);
	PVOID	GetPostHeaderPointer (void); // Pointer to data following header
};

#endif