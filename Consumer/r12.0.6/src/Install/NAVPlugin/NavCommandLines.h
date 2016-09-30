// Copyright 2000 Symantec, Peter Norton Computing Group
// -------------------------------------------------------------------------
// NavCommandLines.h
//		This file contains the class which handles registering and manipulating the
// NAV command lines with LiveUpdate.  This class is also responsible for removing
// virus definitions' command lines, it told to do so.
//
// $Header:   
// -------------------------------------------------------------------------
// $Log:   
// 
/////////////////////////////////////////////////////////////////////////////
#ifndef __NAV_COMMAND_LINES_H__
#define __NAV_COMMAND_LINES_H__

//#include "NavLiveUpdate.h"			// LiveUpdate management class
#include "StString.h"				// String management class

// ---------------------------------------------------------------------------
// CNavCommandLines -
class CNavCommandLines
{
	public:
		CNavCommandLines();			// Standard Constructor
		~CNavCommandLines();		// Standard Destructor

		// This function will register all of the command lines with LiveUpdate
		// The command lines are added to the registery under 
		// HKLM\software\Symantec\Norton AntiVirus\LiveUpdate\CmdLines key during
		// installation.  This function will add these command lines to LiveUpdate
		// during installation and will modify them after installation.
		HRESULT RegisterCommandLines();

		// This function will enable or disable the virus definitions's command lines
		// based on the enable flag.  
		HRESULT EnableVirusDefinitionsCommandLines(HRESULT hEnable = S_FALSE); 

		// This function will remove all of NAV's command lines from LiveUpdate.
		HRESULT UnregisterCommandLines();

		// This function will change the version string for the MicroDefs command lines.
		// The change will happen in the registry copy of the command lines, not in
		// LiveUpdate's settings.  NOTE: The version string contains the text needed
		// to identify which one of the virus defs' command lines to update.
		HRESULT ModifyVirusDefsVersion( LPCTSTR szVersion );

		// This function will set the sequence number for all of the virus definitions'
		// command lines.
//		HRESULT SetSequenceNumberOnVirusDefsCommandLines(DWORD dwSequence );

	protected:
		// This structure contains all of the data for a single command line.
		struct TCommandLine
		{
			CstString		m_strProductName;
			CstString		m_strProductVersion;
			CstString		m_strProductLanguage;
			CstString		m_strProductType;
			CstString		m_strProductMoniker;
			CstString		m_strOtherSwitches;
			BOOL			m_bIsNAVCalback;
			BOOL			m_bIsGroupTogether;
		}; // TCommandLine
		
		// This function will get the next NAV command line.  The master version of 
		// all of NAV's command lines are stored in the registry.  These command lines
		// are used to update LiveUpdate's command lines.  The command lines are referenced
		// by unique assending numbers, starting with 1.
		BOOL GetNextCommandLine( DWORD dwCmdLineNumber, TCommandLine &structCmdLine );

		// This function will set the given NAV command lines product, version, language, type
		// moniker or other switches fields.  If any of these parameters are NULL, those 
		// parameters will not be set.
		BOOL SetCommandLine( DWORD dwCmdLineNumber, TCommandLine &structCmdLine );

		// This function will take a command line from the registry and compare it to 
		// the corresponding command line in LiveUpdate.  Based on the results of the
		// comparison, this function will decide how to update the command line in
		// LiveUpdate.
		HRESULT ProcessCommandLine( TCommandLine &structCmdLine, DWORD dwCommandLineNum );

		// This function will register the given registry command line with LiveUpdate.
		HRESULT RegisterCommandLine( TCommandLine &structCmdLine);

		//	This function will get the descriptive name for the command line and whether
		// or not the command line is Auto LiveUpdate safe.  All this information is 
		// generated depending on the product name of the command line.
		void GetCommandLineGeneratedData( CstString strProductName, BOOL bNavCallback, 
						 CstString& strDescriptiveText, BOOL &IsAutoUpdateSafe );

}; // CNavCommandLines
#endif // __NAV_COMMAND_LINES_H__