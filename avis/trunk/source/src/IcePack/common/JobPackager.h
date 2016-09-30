// JobPackager.h: interface for the JobPackager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JOBPACKAGER_H__5B816B6E_CB2B_11D2_AD61_00A0C9C71BBC__INCLUDED_)
#define AFX_JOBPACKAGER_H__5B816B6E_CB2B_11D2_AD61_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <CMclThread.h>
#include <list>

#include "DefTarget.h"

#include <algorithm>

// new types
typedef std::list<CDefTarget> CDistributionList;

class JobPackager  : public CMclThreadHandler
{
private:
	CDistributionList m_listDist;

public:
	JobPackager();
	virtual ~JobPackager();

	unsigned ThreadHandlerProc();

private:
	bool IsDistributionListEmpty();
	bool PushTargetToFrontOfDistributionList(CDefTarget& dt);
	bool PushTargetToBackOfDistributionList(CDefTarget& dt);
	CDefTarget GetTargetAtBackOfDistributionList();
	CDefTarget GetTargetFromFrontOfDistributionList();
	CDefTarget PopFirstTargetOffDistributionList();
	void PushBroadcastTargetsOntoDistributionList(
		const std::string& strListOfTargets,
		CDefTarget::TargetType targetType);
	void PushBlessedBroadcastTargetsOntoDistributionList();
	void PushUnblessedBroadcastTargetsOntoDistributionList();
	void PushUnblessedTargetsOntoDistributionList(bool boInitializing = false);
	void ProcessDuplicates(CDefTarget& dtOriginal, bool boMoveToBackOfList);
	void RemoveAllDuplicatesFromList(CDefTarget& dtListElement);
	void MoveAllDuplicatesToBackOfList(CDefTarget& dtListElement);
};

#endif // !defined(AFX_JOBPACKAGER_H__5B816B6E_CB2B_11D2_AD61_00A0C9C71BBC__INCLUDED_)
