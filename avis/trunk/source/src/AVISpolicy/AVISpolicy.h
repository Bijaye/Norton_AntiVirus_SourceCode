//
// IBM AntiVirus Immune System
//
// File Name: SamplePolicy.h
// Author:    Milosz Muszynski
//
// Sample Acceptability Policy Function
//
// $Log: $
//

#ifndef _SAMPLEPOLICY_H_
#define _SAMPLEPOLICY_H_

#ifdef AVIS_POLICY_EXPORTS
#define AVIS_POLICY_API __declspec(dllexport)
#else
#define AVIS_POLICY_API __declspec(dllimport)
#endif


struct usageLimits		{	unsigned int dailyUsageLimit	;
							unsigned int weeklyUsageLimit	;
							unsigned int monthlyUsageLimit	;
							unsigned int yearlyUsageLimit	;	};



enum policyReturnValue	{	acceptable				= 0,
							missingHeaders			,
							missingCredentials		,
							invalidCredentials		,
							expiredCredentials		,
							missingCorrelator		,
							invalidCorrelator		,
							undersizeSample			,
							oversizeSample			,
							undersizeContent		,
							oversizeContent			,
							undersizeBuffer			};



extern "C" AVIS_POLICY_API 
policyReturnValue samplePolicy(		const char *			sampleAttributes		,
									const int				newchangedAttributesSize,
									char *					newchangedAttributes	,
									struct usageLimits *	customerUsageLimits		,
									struct usageLimits *	platformUsageLimits		);

// sampleAttributes			  -	(input only)
//								null-terminated text string with all of the attributes of the sample
//								received; attributes are specified in arbitrary order, separated by
//								a newline character ("\n"), the last attribute is terminated by two
//								newline characters and a null character ("\n\n\0"), attribute names
//								are case-insensitive and unique, attribute values are case-sensitive
//
// newchangedAttributesSize	  -	(input only)
//								integer with the size in bytes of the buffer referenced by the 
//								newchangedAttributesBuffer argument; should be large enough to hold the 
//								maximum possible new and changed attributes; if the buffer is not large 
//								enough then return value undersizeBuffer will be returned
//
// newchangedAttributesBuffer -	(output only)
//								both new and changed attributes can be returned in this buffer, attributes
//								may be returned in arbitrary order, attribute names are case-insensitive
//
// customerUsageLimits		  - (output only)
//								usage limits for a particular customer; if this parameter is returned,
//								the X-Customer-Identifier must also be returned with the newchangedAttributes
//								argument; at the time the policy function receives control this argument 
//								has allocated memory and all the values are set to zero - the policy function
//								must not allocate memory for this argument
//								this parameter is currently not used and may be set to NULL (6/21/1999)
//
// platformUsageLimits		  - (output only)
//								usage limits for a particular platform; if this parameter is returned,
//								the X-Platform-Correlator must also be returned with the newchangedAttributes
//								argument; at the time the policy function receives control this argument 
//								has allocated memory and all the values are set to zero - the policy function
//								must not allocate memory for this argument
//								this parameter is currently not used and may be set to NULL (6/21/1999)


#endif _SAMPLEPOLICY_H_
