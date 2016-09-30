
#ifndef EXCLUSIONS_H
#define EXCLUSIONS_H

#pragma pack(push)
#pragma pack(1)
#include "Platform.h"
#include "Exclude.h"
#pragma pack(pop)
#include "ExclusionManagerLoader.h"
#include "ExclusionInterface.h"
#include "N32Exclu.h"

class CExclusions
{
public:
	// Constructor.
	CExclusions();

	// Destructor.
	~CExclusions();

	// Initializes the class.
	bool Initialize();

    // Check a path against the exclusions
    NavExclusions::IExclusion::ExclusionState IsPathMatch(
                                                      const char* pszPath,
                                                      bool bIsOem,
                       NavExclusions::IExclusion::ExclusionState requiredState);

    // Check an IScanInfection against the exclusions
    bool IsMatch(IScanInfection* pInfection);

    // Check an IAnomaly against the exclusions
    bool IsMatch(ccEraser::IAnomaly* pAnomaly);

    // Add an anomaly to the exclusions
    bool AddAnomaly(ccEraser::IAnomaly* pAnomaly);

private:
    void Uninitialize();    // Release resources
    NavExclusions::IExclusionManagerPtr m_spExclusions;
    NavExclusions::N32Exclu_IExclusionManagerFactory m_ExclusionManagerLoader;

	// Are we ready?
	bool m_bReady;

    // Did we perform an add?
    bool m_bDirty;

    bool IsNetworkDrive(const char* szItem);

    // Perform nasty FS stuff for an ISI.
    bool HandleISI(IScanInfection* pInfection, 
                   NavExclusions::IExclusion::ExclusionState requiredState,
                   NavExclusions::IExclusion::ExclusionState &eState);

    // Perform nasty FS stuff for an IA.
    bool HandleAnomalyFS(ccEraser::IAnomaly* pAnomaly, 
                         NavExclusions::IExclusion::ExclusionState requiredState);
};

#endif

