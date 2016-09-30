////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// AnomalyRisk.cpp
//	Class implementation for determining the risk level of a specified
//		anomaly
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "AnomalyRisk.h"

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
CAnomalyRisk::CAnomalyRisk(ANOMALY_RISK_LEVEL riskLevel)
{
	SetRiskThreshold(riskLevel);
}
//////////////////////////////////////////////////////////////////////
// Destructor
//////////////////////////////////////////////////////////////////////
CAnomalyRisk::~CAnomalyRisk()
{
}
//////////////////////////////////////////////////////////////////////
// Used to set the minimum threshold
//////////////////////////////////////////////////////////////////////
void CAnomalyRisk::SetRiskThreshold(ANOMALY_RISK_LEVEL riskLevel)
{
	if(riskLevel < ANOMALY_RISK_MIN || riskLevel > ANOMALY_RISK_MAX)
		riskLevel = ANOMALY_RISK_MEDIUM;
	m_riskLevel = riskLevel;
}
//////////////////////////////////////////////////////////////////////
// Deteremines if the given anomaly should be remediated based on
//		the fact that
//////////////////////////////////////////////////////////////////////
BOOL CAnomalyRisk::ShouldRemediateAnomaly(ccEraser::IAnomaly *pAnomaly)
{
	BOOL bReturn = FALSE;
	ANOMALY_RISK_LEVEL riskLevel = GetAnomalyRiskLevel(pAnomaly);
	if(IsAcceptableRiskLevel(riskLevel))
		bReturn = TRUE;
	return bReturn;
}
//////////////////////////////////////////////////////////////////////
// Is it a non viral threat
//////////////////////////////////////////////////////////////////////
BOOL CAnomalyRisk::IsNonViral(DWORD dwType)
{
    if(dwType == ccEraser::IAnomaly::SecurityRisk ||
        dwType == ccEraser::IAnomaly::Hacktool ||
        dwType == ccEraser::IAnomaly::SpyWare ||
        dwType == ccEraser::IAnomaly::Trackware ||
        dwType == ccEraser::IAnomaly::Dialer ||
        dwType == ccEraser::IAnomaly::RemoteAccess ||
        dwType == ccEraser::IAnomaly::Adware ||
        dwType == ccEraser::IAnomaly::Joke
		)
	{
		//?? ccEraser::IAnomaly::ClientCompliancy
		//?? ccEraser::IAnomaly::GenericLoadPoint
        return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
// Is it a viral threat
//////////////////////////////////////////////////////////////////////
BOOL CAnomalyRisk::IsViral(DWORD dwType)
{
	// Check this infection item against all viral threat categories
    if(dwType == ccEraser::IAnomaly::Viral ||
        dwType == ccEraser::IAnomaly::Malicious ||
        dwType == ccEraser::IAnomaly::ReservedMalicious ||
        dwType == ccEraser::IAnomaly::Heuristic )
        return TRUE;
    return FALSE;
}
//////////////////////////////////////////////////////////////////////
// Get the risk level for this anomaly
//	Viral infections default to HIGH
//	NonViral defaults to High except Joke = MEDIUM, Adware = ?
//////////////////////////////////////////////////////////////////////
ANOMALY_RISK_LEVEL CAnomalyRisk::GetAnomalyRiskLevel(ccEraser::IAnomaly *pAnomaly)
{
	cc::IKeyValueCollectionPtr pProps = NULL;
	ANOMALY_RISK_LEVEL arlReturn = ANOMALY_RISK_IGNORE;
	BOOL bFound = FALSE;
	ccEraser::eResult res = ccEraser::Success;
	ISymBasePtr pBase = NULL;

	if(pAnomaly)
	{
		res = pAnomaly->GetProperties(pProps);
		if(Failed(res) || !pProps)
		{
			// failed getting risk level
		}
		else
		{
			if(!pProps->GetExists(ccEraser::IAnomaly::Stealth) && !pProps->GetExists(ccEraser::IAnomaly::Removal) &&
				!pProps->GetExists(ccEraser::IAnomaly::Performance) && !pProps->GetExists(ccEraser::IAnomaly::Privacy)
				)
			{	// only set defaults if nothing exists
				if(pProps->GetValue(ccEraser::IAnomaly::Categories, pBase))
				{
					cc::IIndexValueCollectionQIPtr pCategories(pBase);
					for(size_t j = 0; j < pCategories->GetCount() && bFound == FALSE; j++)
					{
						DWORD dwType;
						if(pCategories->GetValue(j, dwType))
						{
							if(IsViral(dwType))
							{
								arlReturn = ANOMALY_RISK_HIGH;
								bFound = TRUE;
							}
							else if(IsNonViral(dwType))
							{
								if(dwType == ccEraser::IAnomaly::Joke)
								{
									if(arlReturn < ANOMALY_RISK_MEDIUM)
										arlReturn = ANOMALY_RISK_MEDIUM;
								}
								else if(dwType == ccEraser::IAnomaly::Adware)
								{
									if(arlReturn < ANOMALY_RISK_HIGH)
										arlReturn = ANOMALY_RISK_HIGH;
								}
								else
								{
									arlReturn = ANOMALY_RISK_HIGH;
									bFound = TRUE;
								}
							}
						}
					}
				}
			}
			else
			{
				arlReturn = CalculateAnomalyRiskLevel(pProps);
			}
		}
	}
	return arlReturn;
}
//////////////////////////////////////////////////////////////////////
// Get all the properties that we need an mathematically calculate
//		the average value for each found property, then determine
//		what level it is, based on NAV implementation
//////////////////////////////////////////////////////////////////////
ANOMALY_RISK_LEVEL CAnomalyRisk::CalculateAnomalyRiskLevel(cc::IKeyValueCollection *pProps)
{
	DWORD dwTemp = 0, dwStealth = 0, dwRemoval = 0, dwPerformance = 0, dwPrivacy = 0;
	double dfAverage = 0.0, dfValues = 0;
	ANOMALY_RISK_LEVEL arlReturn = ANOMALY_RISK_IGNORE;
	if(pProps)
	{
		if(pProps->GetValue(ccEraser::IAnomaly::Stealth, dwTemp))
		{
			dfAverage += dwTemp;
			dfValues += 1.0;
		}
		if(pProps->GetValue(ccEraser::IAnomaly::Removal, dwTemp))
		{
			dfAverage += dwTemp;
			dfValues += 1.0;
		}
		if(pProps->GetValue(ccEraser::IAnomaly::Performance, dwTemp))
		{
			dfAverage += dwTemp;
			dfValues += 1.0;
		}
		if(pProps->GetValue(ccEraser::IAnomaly::Privacy, dwTemp))
		{
			dfAverage += dwTemp;
			dfValues += 1.0;
		}
		if(dfValues > 0)
		{
			dfAverage /= dfValues;
			if(dfAverage >= ANOMALY_RISK_HIGH_FLOOR)
				arlReturn = ANOMALY_RISK_HIGH;
			else if(dfAverage >= ANOMALY_RISK_MEDIUM_FLOOR)
				arlReturn = ANOMALY_RISK_MEDIUM;
			else
				arlReturn = ANOMALY_RISK_LOW;
		}
		else
			arlReturn = ANOMALY_RISK_HIGH;
	}
	return arlReturn;
}
//////////////////////////////////////////////////////////////////////
// Is the risk level found and 
//////////////////////////////////////////////////////////////////////
BOOL CAnomalyRisk::IsAcceptableRiskLevel(ANOMALY_RISK_LEVEL riskLevelFound)
{
	BOOL bAccept = FALSE;
	if(riskLevelFound == ANOMALY_RISK_IGNORE || m_riskLevel <= riskLevelFound)
		bAccept = TRUE;
	return bAccept;
}
