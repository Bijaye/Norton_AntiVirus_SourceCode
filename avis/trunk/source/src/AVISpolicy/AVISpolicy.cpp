//
// IBM AntiVirus Immune System
//
// File Name: SamplePolicy.cpp
// Author:    Milosz Muszynski
//
// Sample Acceptability Policy Function
//
// $Log: $
//

//------ defines ------------------------------------------------------

#define AVIS_POLICY_EXPORTS

//------ includes -----------------------------------------------------

#include "Base.h"
#include "AVISpolicy.h"


//------ local data ---------------------------------------------------

// profile file definition

// the following array defines data items to be stored in the profile file

ProfileItemData profileItems [] =
{
	{	"requiredAttributes",												// attribute name tag
		"", 																// default value
		"", 																// placeholder for the actual value
		"list of required headers to be verified by the policy function" 	// description
	},
	{	"checkContentLengthLimits"	,	"no"	   	,	"",	"if 'yes' content length limits will be checked"	},
	{	"maxContentLength"			,	"5000000"	,	"", "maximum content length"							},
	{	"minContentLength"			,	"10"		,	"",	"minimum content length"							},	

	{	"checkSampleSizeLimits"		,	"no"	   	,	"",	"if 'yes' sample size limits will be checked"		},
	{	"maxSampleSize"				,	"10000000"	,	"",	"maximum sample size"								},	
	{	"minSampleSize"				,	"10"		,	"",	"minimum sample size"								},	

	{	"verifySamplePriority"		,	"no"		,	"", "if 'yes' sample priority will be verified"			},
	{	"maxSamplePriority"			,	"1000"		,	"",	"maximum sample priority allowed"					},

	{	"setCustomerUsageLimits"	,	"no"	   	,	"",	"if 'yes' customer limits will be set"				},
	{	"customerDailyUsageLimit"	,	"0"			,	"", "customer daily usage limit allowed"   				},
	{	"customerWeeklyUsageLimit"	,	"0"			,	"", "customer weekly usage limit allowed"  				},
	{	"customerMonthlyUsageLimit"	,	"0"			,	"", "customer monthly usage limit allowed" 				},
	{	"customerYearlyUsageLimit"	,	"0"			,	"", "customer yearly usage limit allowed"  				},

	{	"setPlatformUsageLimits"	,	"no"	   	,	"",	"if 'yes' platform limits be set"					},
	{	"platformDailyUsageLimit"	,	"0"			,	"", "platform daily usage limit allowed"   				},
	{	"platformWeeklyUsageLimit"	,	"0"			,	"", "platform weekly usage limit allowed"				},
	{	"platformMonthlyUsageLimit"	,	"0"			,	"", "platform monthly usage limit allowed" 				},
	{	"platformYearlyUsageLimit"	,	"0"			,	"", "platform yearly usage limit allowed"  				},

	{	"addDatePolicyAttribute"	,	"no"	   	,	"",	"if 'yes' date policy attribute will be added"		},
	{	"addPolicyVersionAttribute"	,	"no"	   	,	"",	"if 'yes' policy version attribute will be added"	},
};

const int  profileCount = sizeof( profileItems )/sizeof( profileItems[ 0 ] ); // number of profile attributes

const char profileFileName [] = "AVISpolicy.prf"; // profile file name and path

const char policyVersion [] = "IBM AVIS Sample Policy Function v. 1.0.0";





//------ public functions ---------------------------------------------

// currently the sample acceptability policy function is performing only 
// one type of check - attributes included with the sample are matched
// against a list of required attributes stored in the profile file

policyReturnValue samplePolicy(		const char *			sampleAttributes			,
 									const int				newchangedAttributesSize	,
 									char *					newchangedAttributes		,
 									struct usageLimits *	customerUsageLimits			,
 									struct usageLimits *	platformUsageLimits			)
{
	Profile profile( profileItems, profileCount, profileFileName );

	ConstStrStrMap _sampleAttributes    ( sampleAttributes	   );		// move sampleAttributes into a hash table for easy access
	StrStrMap      _newchangedAttributes( newchangedAttributes );		// move newchangedAttributes into a hash table for easy access

	policyReturnValue returnValue = acceptable;		// set the default return value 


	// check required attributes


	StrVector requiredAttributes( profile.getString( "requiredAttributes" ) ); // list of required attributes is taken from the profile

	// for each required attribute find a match in a sampleAttribute

	for ( StrVector::iterator iter = requiredAttributes.begin(); iter != requiredAttributes.end(); iter++ )
	{
		if ( !_sampleAttributes.exists( *iter ) )
		{
			returnValue = missingHeaders; // attribute was missing or its value was a white space string
		}
	}


	// check the content length limits

	if ( profile.getBool( "checkContentLengthLimits" ) )
	{
		if ( _sampleAttributes.getLong( "Content-Length" ) > profile.getLong( "maxContentLength" ) )
			returnValue = oversizeContent;
		else if ( _sampleAttributes.getLong( "Content-Length" ) < profile.getLong( "minContentLength" ) )
			returnValue = undersizeContent;
	}


	// check the sample size limits

	if ( profile.getBool( "checkSampleSizeLimits" ) )
	{
		if ( _sampleAttributes.getLong( "X-Sample-Size" ) > profile.getLong( "maxSampleSize" ) )
			returnValue = oversizeSample;
		else if ( _sampleAttributes.getLong( "X-Sample-Size" ) < profile.getLong( "minSampleSize" ) )
			returnValue = undersizeSample;
	}


	// add policy attributes

	if ( profile.getBool( "addDatePolicyAttribute" ) )
		_newchangedAttributes.set( "X-Date-Policy", TimeUtil::gmt() );

	if ( profile.getBool( "addPolicyVersionAttribute" ) )
		_newchangedAttributes.set( "X-Policy-Version", policyVersion );


	// verify and correct the priority attribute

	if ( profile.getBool( "verifySamplePriority" ) )
	{
		if ( _sampleAttributes.getLong( "X-Sample-Priority" ) > profile.getLong( "maxSamplePriority" ) )
		{
			_newchangedAttributes.set( "X-Sample-Priority", profile.getString( "maxSamplePriority" ) );
		}
	}


	// set the usage limits to configurable values

	// we do not implement usage limits in this release (6/21/1999)
	//
	//
	//if ( profile.getBool( "setCustomerUsageLimits" ) )
	//{
	//	if ( customerUsageLimits != NULL )
	//	{
	//	customerUsageLimits->dailyUsageLimit	= profile.getInt( "customerDailyUsageLimit" );
	//	customerUsageLimits->weeklyUsageLimit	= profile.getInt( "customerWeeklyUsageLimit" );
	//	customerUsageLimits->monthlyUsageLimit	= profile.getInt( "customerMonthlyUsageLimit" );
	//	customerUsageLimits->yearlyUsageLimit	= profile.getInt( "customerYearlyUsageLimit" );
	//	}
	//}
	//
	//if ( profile.getBool( "setPlatformUsageLimits" ) )
	//{
	//	if ( platformUsageLimits != NULL )
	//	{
	//	platformUsageLimits->dailyUsageLimit	= profile.getInt( "platformDailyUsageLimit" );
	//	platformUsageLimits->weeklyUsageLimit	= profile.getInt( "platformWeeklyUsageLimit" );
	//	platformUsageLimits->monthlyUsageLimit	= profile.getInt( "platformMonthlyUsageLimit" );
	//	platformUsageLimits->yearlyUsageLimit	= profile.getInt( "platformYearlyUsageLimit" );
	//	}
	//}


	// convert the new and changed attributes to the string format

	if ( _newchangedAttributes.length() < newchangedAttributesSize )
	{
		_newchangedAttributes.copyTo( newchangedAttributes );
	}
	else
	{
		returnValue = undersizeBuffer;
	}

	return returnValue;
}


