#ifndef		__TITLEDB_H_
	#define	__TITLEDB_H_

enum PERFORMANCE_TITLE_TYPE	{ PERF_TITLE_COUNTER, PERF_TITLE_EXPLAIN };

class CPerfTitleDatabase
{
public:
	CPerfTitleDatabase (PERFORMANCE_TITLE_TYPE type);
	~CPerfTitleDatabase (void);

	unsigned	GetLastTitleIndex (void) {return m_nLastIndex; }
	PTSTR		GetTitleStringFromIndex (unsigned Index) ;
	unsigned	GetIndexFromTitleString (PTSTR pszTitle);

private:
	unsigned		m_nLastIndex;
	PTSTR		*m_TitleStrings;
	PTSTR		m_pszRawStrings;
};

#endif
