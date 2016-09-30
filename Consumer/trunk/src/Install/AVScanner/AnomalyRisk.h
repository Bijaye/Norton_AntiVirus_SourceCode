////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// AnomalyRisk.h
//	Class declaration for CAnomalyRisk class with defines
//////////////////////////////////////////////////////////////////////
#pragma once

#include "ccEraserInterface.h"

//////////////////////////////////////////////////////////////////////
// Defines and typedef for class
//////////////////////////////////////////////////////////////////////
#define ANOMALY_RISK_LOW_FLOOR			(double)0.000000
#define ANOMALY_RISK_MEDIUM_FLOOR		(double)2.500000
#define ANOMALY_RISK_HIGH_FLOOR			(double)3.500000

typedef enum
{
	ANOMALY_RISK_MIN = 0x00,
	ANOMALY_RISK_IGNORE = ANOMALY_RISK_MIN,
	ANOMALY_RISK_LOW,
	ANOMALY_RISK_MEDIUM,
	ANOMALY_RISK_HIGH,
	ANOMALY_RISK_MAX = ANOMALY_RISK_HIGH
}ANOMALY_RISK_LEVEL;

//////////////////////////////////////////////////////////////////////
// Class definition
//////////////////////////////////////////////////////////////////////
class CAnomalyRisk
{
public:
	CAnomalyRisk(ANOMALY_RISK_LEVEL riskLevel = ANOMALY_RISK_HIGH);
	~CAnomalyRisk();

	void SetRiskThreshold(ANOMALY_RISK_LEVEL riskLevel);
	BOOL ShouldRemediateAnomaly(ccEraser::IAnomaly *pAnomaly);
	
private:
	ANOMALY_RISK_LEVEL m_riskLevel;

	BOOL IsNonViral(DWORD dwType);
	BOOL IsViral(DWORD dwType);
	BOOL IsAcceptableRiskLevel(ANOMALY_RISK_LEVEL riskLevelFound);
	ANOMALY_RISK_LEVEL CalculateAnomalyRiskLevel(cc::IKeyValueCollection *pProps);
	ANOMALY_RISK_LEVEL GetAnomalyRiskLevel(ccEraser::IAnomaly *pAnomaly);
};

