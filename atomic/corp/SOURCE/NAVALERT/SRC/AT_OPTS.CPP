/////////////////////////////////////////////////////////////////////
// AT_OPTS.CPP - Created - 10/21/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVALERT/VCS/at_opts.cpv   1.6   16 Jul 1997 21:57:26   KSACKIN  $
//
// Description:
//    This will be the main implementation file for the CAlertOptions
//    class.  This file will have the code for the loading, saving,
//    and iterating through the many options for alerting.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVALERT/VCS/at_opts.cpv  $
// 
//    Rev 1.6   16 Jul 1997 21:57:26   KSACKIN
// Stopped using WritePrivateProfileString to write our our general options.
// We were seeing caching problems on Win95 when we were using the profile
// string functions, and then writing other options directly to the file
// afterward.  We will now write all strings to the file directly, and then use
// the GetPrivateProfileString function to read our Profile String like options,
// and still read in the rest as we always have.
//
//    Rev 1.5   13 Jun 1997 17:15:22   JBRENNA
// Added a cache flush after writing the private profile string options.
//
//    Rev 1.4   23 May 1997 14:45:34   GDZIECI
// Implemented LoadReceiveOptions() and SaveReceiveOptions().
//
//    Rev 1.3   22 May 1997 14:30:00   DSACKIN
// Converted back to TCHAR
//
//    Rev 1.2   14 May 1997 10:15:24   GDZIECI
// Added preliminary support for SNMP targets.
//
//    Rev 1.1   06 May 1997 18:06:30   GDZIECI
// Created LoadReceiveOptions() and SaveReceiveOptions() member functions
// (empty right now) to load and save data from to the [Receive Options] section
// to enable/disable listening for ATP, IPX and IP based alerts.
//
//    Rev 1.0   18 Apr 1997 13:57:16   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:11:52   RFULLER
// Initial revision
//
//    Rev 1.14   04 Feb 1997 22:10:18   ksackin
// Fixed a bug where if any one of the options failed to load or save, we would
// still write or read the rest.
//
//    Rev 1.13   16 Jan 1997 01:02:12   ksackin
// Added the function RemoveNextTarget to complement the GetNextTarget.  This
// new function will do what GetNextTarget did, but will actually remove the
// object from it's current list so that you now control it.  You can then
// add it to other lists and delete the object when you are finished with it.
//
//    Rev 1.12   07 Jan 1997 13:44:50   ksackin
// Added two new options for the SMTP Configuration.  Added an Author and a
// subject field.
//
//    Rev 1.11   12 Dec 1996 12:46:56   TSmith
// Added support for CActivityLogTarget class.
//
//    Rev 1.10   04 Dec 1996 23:13:28   DSACKIN
// Changed all instances of LPCTSTR to LPCWSTR for use with non-UNICODE apps.
//
//    Rev 1.9   03 Dec 1996 14:27:48   ksackin
// Added saving and loading for values to enable or disable the remote alerting
// for the AP and for the Scanner.  This was put into the General Options
// category.
//
//    Rev 1.8   25 Nov 1996 21:53:18   ksackin
// Added support for disabling all of the remote alert options through a general
// switch of m_bRemoteAlertsEnabled.
//
//    Rev 1.7   23 Nov 1996 21:22:18   ksackin
// Added a function to fixup the name of the options file if it is a relative
// path to have a '.\' in front of it.  This is for INI file support.  If this
// is not here, it will dump the INI file in the windows directory.
//
//    Rev 1.6   23 Nov 1996 20:43:48   ksackin
// Changed declaration for m_szOptions file name to a CString.
//
//    Rev 1.5   23 Nov 1996 19:50:30   ksackin
// Added support for saving and loading SMTP Configuration Options, which for
// now is just an SMTP Post Office string.  This will be saved and loaded by
// calling to PrivateProfileString functions on our options file before saving
// and loading all of the alert targets.
//
//    Rev 1.4   21 Nov 1996 16:31:42   ksackin
// Added functionality to call base class for it's options before adding each
// target's options.  This allows for pre and post processing.
//
//    Rev 1.3   15 Nov 1996 02:39:12   KSACKIN
// Fixed some error returns, and also added two member function.  The first was
// a ResetPosition() Member for the CAlertOptions class to allow for resetting
// the iteration position.  The second was a function to get the default name
// for our navalert.dat file.  We need to fix up the entire name for this item
// instead of a relative path.  This is because we are run in the directory of
// NT\System32.  We need to get our file from the install directory.
//
//    Rev 1.2   14 Nov 1996 01:21:50   KSACKIN
// Added the function ResetPosition() to the CAlertOptions class.  This function
// is so that a client can manually reset the iteration position of the alert
// target list.  This is used when a client stops in the middle of a list, and
// the list iterator is not reset automatically.
//
//    Rev 1.1   12 Nov 1996 22:47:38   KSACKIN
// Modified classes to be MFC Extension classes.  This meant changing the
// exported functions back, and then using a macro in ihe class definition.
//
//    Rev 1.0   08 Nov 1996 15:34:04   KSACKIN
// Initial revision.
//
//    Rev 1.1   07 Nov 1996 01:05:22   KSACKIN
// Changed some string handling for UNICODE support, as well as modified some
// exported functions.
//
//    Rev 1.0   29 Oct 1996 15:33:20   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "afxwin.h"                     // Standard AFX include for windows.
#include "AT_Opts.h"                    // Main header file for options.
#include "AT_Str.h"                     // String resources.
#include "dllrcsta.h"                   // Resource state class.


/////////////////////////////////////////////////////////////////////
// CAlertOptions::CAlertOptions
//
// Purpose:
//    Class Declaration with file name passed.
//
// Parameters:
//    LPCWSTR cszFileName - [IN] Initial file name for options file.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/21/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlertOptions::CAlertOptions(
    LPCTSTR cszFileName   // = NULL (default)
    )
{
    m_bAPRemoteAlertsEnabled = TRUE;
    m_bScannerRemoteAlertsEnabled = TRUE;

    m_bATPAlertsEnabled = FALSE;
    m_bIPXAlertsEnabled = FALSE;
    m_bIPAlertsEnabled  = FALSE;

                                        // This parameter defaults to NULL
    if ( cszFileName )
        {
                                        // Copy file name to internal file name.
        m_cszOptionsFileName = cszFileName;
        }
    else
        {
                                        // If we did not get passed a file name,
                                        // initialize this member to a zero length
                                        // string.
        m_cszOptionsFileName.Empty();
        }

                                        // Do not call load if there is no file
                                        // name to load.
    if ( m_cszOptionsFileName.GetLength() )
        {
                                        // Load function uses internal file
                                        // name, if not passed one.
        LoadOptions();
        }
}


/////////////////////////////////////////////////////////////////////
// CAlertOptions::~CAlertOptions
//
// Purpose:
//    Class Destructor.
//
// Parameters:
//    None
//
// Returns:
//    None
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/21/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlertOptions::~CAlertOptions( void )
{
                                        // Destroy all of the items
                                        // that are in the list.  This
                                        // will also free up all of the
                                        // memory allocated.
    DestroyAll();
}

/////////////////////////////////////////////////////////////////////
// CAlertOptions::LoadOptions
//
// Purpose:
//    Load the options from the file name that you have internally,
//    or with the parameter that you get passed.
//
// Parameters:
//    LPCWSTR cszFileName - [IN] File name to load. (NULL is default)
//                               If this occurs, use internal file
//                               name if present, otherwise, return.
//
// Returns:
//    Success of the options load.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/21/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
int CAlertOptions::LoadOptions(
    LPCTSTR cszFileName  // = NULL (default)
    )
{
CStdioFile cOptionsFile;
CString cszSection;
CString cszDefaultFileName;
int nReturn = TRUE;

                                        // Setup the module state so that
                                        // we may load strings from the correct
                                        // resource using the
                                        // CString::LoadString() member function.
    CDLLResourceState resourceSwitch;


                                        // Empty the list and free all memory
                                        // associated with each item in that
                                        // list. (Make sure that this list
                                        // is actually empty)
    DestroyAll();

                                        // If file name is non NULL, we are
                                        // going to copy the file name passed
                                        // in to our internal file name.
    if ( NULL != cszFileName )
        {
        m_cszOptionsFileName = cszFileName;
        }
    else
        {
                                        // If we still have no file name, try
                                        // to load the default file name from
                                        // our string table.
        if( GetDefaultOptionsFileName( &cszDefaultFileName ) )
            {
            m_cszOptionsFileName = cszDefaultFileName;
            }
        }

                                        // Now that we have processed the
                                        // passed in file name, we need to
                                        // verify that we have a valid file
                                        // name.
    if ( m_cszOptionsFileName.GetLength() )
        {
                                        // Loads General configuration options.
        LoadGeneralOptions();
                                        // Loads SMTP Configuration Options.
        LoadSMTPConfig();
                                        // Loads Receive Alerts Options.
        LoadReceiveOptions();

                                        // Attempt to open the file.  We are
                                        // opening it in share read, but non
                                        // share write.
        if ( cOptionsFile.Open( m_cszOptionsFileName,
                                CFile::modeRead | CFile::shareDenyWrite ) )
            {

                                        // We do not care if any of these
                                        // sections are empty, so we do not
                                        // check the return type.
            LoadSection( &cOptionsFile, TARGET_EMAIL );
            LoadSection( &cOptionsFile, TARGET_PAGER );
            LoadSection( &cOptionsFile, TARGET_MESSAGE_LOCAL );
            LoadSection( &cOptionsFile, TARGET_MESSAGE_NETWORK );
            LoadSection( &cOptionsFile, TARGET_FORWARD );
            LoadSection( &cOptionsFile, TARGET_NLM );
            LoadSection( &cOptionsFile, TARGET_EVENTLOG );
            LoadSection( &cOptionsFile, TARGET_ACTIVITY_LOG );
            LoadSection( &cOptionsFile, TARGET_SNMP );

            ResetPosition();

            cOptionsFile.Close();
            }
        else
            {
            nReturn=FALSE;
            }
        }

    return nReturn;
}

/////////////////////////////////////////////////////////////////////
// CAlertOptions::SaveOptions
//
// Purpose:
//    Save the options from the file name that you have internally,
//    which should be the file that you loaded, or with the parameter
//    that you get passed.
//
// Parameters:
//    LPCWSTR cszFileName - [IN] File name to save options as. (NULL
//                               is default)  If this occurs, use
//                               internal file name if present,
//                               otherwise, return FALSE.
//
// Returns:
//    Success of the options save.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/21/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
int CAlertOptions::SaveOptions(
    LPCTSTR cszFileName  // = NULL (default)
    )
{
CStdioFile cOptionsFile;
CString cszSection;
int nReturn = TRUE;

                                        // If file name is non NULL, we are
                                        // going to copy the file name passed
                                        // in to our internal file name.
    if ( NULL != cszFileName )
        {
        m_cszOptionsFileName = cszFileName;
        }

                                        // Now that we have processed the
                                        // passed in file name, we need to
                                        // verify that we have a valid file
                                        // name.
    if ( m_cszOptionsFileName.GetLength() )
        {
                                        // Attempt to create the file you would
                                        // like to save your options as.
                                        // We are going to open this file and
                                        // truncate it so that we can write our
                                        // new option file out cleanly.
                                        // Open file for writing with share read
                                        // and no share write.
        if ( cOptionsFile.Open( m_cszOptionsFileName,
                                CFile::modeCreate |
                                CFile::modeWrite |
                                CFile::shareDenyWrite ) )
            {
                                        // Saves General Options.
            SaveGeneralOptions( &cOptionsFile );
                                        // Saves SMTP Configuration Options.
            SaveSMTPConfig( &cOptionsFile );
                                        // Saves Receive Alerts Options.
            SaveReceiveOptions( &cOptionsFile );

                                        // Save all of the sections.  These
                                        // functions will also write out a section
                                        // title for each type writting.
                                        // We do not care if any of these
                                        // sections are empty, so we do not
                                        // check the return type.
            SaveSection( &cOptionsFile, TARGET_EMAIL );
            SaveSection( &cOptionsFile, TARGET_PAGER );
            SaveSection( &cOptionsFile, TARGET_MESSAGE_LOCAL );
            SaveSection( &cOptionsFile, TARGET_MESSAGE_NETWORK );
            SaveSection( &cOptionsFile, TARGET_FORWARD );
            SaveSection( &cOptionsFile, TARGET_NLM );
            SaveSection( &cOptionsFile, TARGET_EVENTLOG );
            SaveSection( &cOptionsFile, TARGET_ACTIVITY_LOG );
            SaveSection( &cOptionsFile, TARGET_SNMP );

            cOptionsFile.Close();
            }
        else
            {
            nReturn=FALSE;
            }
        }
    else
        {
        nReturn=FALSE;
        }

    return nReturn;
}


/////////////////////////////////////////////////////////////////////
// CAlertOptions::LoadSection
//
// Purpose:
//    Load a section of target types from the passed in file class.
//    This function will create the classes, read the packed strings,
//    and call the classes unpack function.  When that is done, it
//    will add it to the CAlertTargetList class that will add it
//    in sorted order based upon priority.
//
// Parameters:
//    CStdioFile *lpcInputFile - [IN] File class to load the section
//                                    from.  We will reset this file
//                                    and search through it for the
//                                    correct section, and then read
//                                    in all of the targets of that
//                                    type.
//    int nTargetType          - [IN] Type of targets to load.  This
//                                    funciton will determine the
//                                    section name and class type
//                                    based on this value.
//
// Returns:
//    BOOLEAN as to whether the load was successfull or not.  This
//    funtion will return FALSE if a critical parsing error occurs.
//    (Not inclusive of missing section.)
//
// See Also:
//    AlertOps.h for target type declaration.
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertOptions::LoadSection(
    CStdioFile *lpcInputFile,
    int nTargetType
    )
{
CAlertTarget *lpcAlertTarget;
CString cszSectionName;
CString cszPackedString;
BOOL bFinished = FALSE;
BOOL bRetVal = TRUE;


                                        // Attempt to read in the section name
                                        // from our string table based on it's
                                        // type.
    if ( GetSectionNameForTarget( nTargetType, &cszSectionName ) )
        {
                                        // Remove all unwanted characters.
        cszSectionName.TrimRight();
        cszSectionName.TrimLeft();

                                        // Add Braces to section name if needed.
        AddBracesIfNeeded( &cszSectionName );

                                        // Try and seek to a section with the
                                        // same name as that we are sending to
                                        // it.
        if ( SeekToSection( lpcInputFile, cszSectionName ) )
            {
                                        // Enter loop to read in all of the
                                        // targets of this type.  This loop
                                        // will exit if we hit the end of the
                                        // file, or another section header.
            do
                {
                                        // Attempt to read a string from our
                                        // file.  This will return FALSE if we
                                        // are at the end of the file.
                if ( lpcInputFile->ReadString( cszPackedString ) )
                    {
                    cszPackedString.TrimRight();
                    cszPackedString.TrimLeft();

                                        // Make sure that line read is not
                                        // empty.  If it is, it will chow
                                        // the GetAt() function next.
                    if ( 0 != cszPackedString.GetLength() )
                        {
                                        // Make sure that the last parameter is
                                        // not an empty item by adding one more
                                        // ';' at the end.  We do this by
                                        // packing on an empty value.
                        PackParameter( &cszPackedString, _T("") );

                                        // If we found a new section, then we
                                        // need to exit out of our loop.
                        if ( '[' != cszPackedString.GetAt(0) )
                            {
                                        // Create a new instance of this Target
                                        // Type.
                            if ( lpcAlertTarget = CreateNewTargetOfType( nTargetType ) )
                                {
                                        // Convert the common data among CAlertTargets
                                        // from the base class into a packed
                                        // string.
                                if ( TRUE == lpcAlertTarget->ConvertCommonFromPacked( &cszPackedString ) )
                                    {
                                        // Call through to the virtual convert
                                        // function for this type to unpack the
                                        // saved data string into it's local
                                        // data structure.  If this function fails,
                                        // we are going to ignore this item.  Later
                                        // we may add line number errors, but for
                                        // now, this is all.
                                    if ( TRUE == lpcAlertTarget->ConvertFromPacked( &cszPackedString ) )
                                        {
                                        // We have a valid target, now we
                                        // just add the target to our target
                                        // list.  If this fails, we exit with a
                                        // critical error.
                                        if ( TRUE == m_cAlertTargetList.AddTarget( lpcAlertTarget ) )
                                            {
                                        // Set this to signify that this target
                                        // was added successfully, and that we
                                        // do not need to free the memory.
                                            lpcAlertTarget = NULL;
                                            }
                                        else
                                            {
                                        // An error occurred when trying to add
                                        // the new target to the Alert Target
                                        // List,so we return FALSE.
                                            bRetVal = FALSE;
                                            bFinished = TRUE;
                                            }
                                        }
                                    }
                                if ( NULL != lpcAlertTarget )
                                    {
                                        // If we added successfully, this
                                        // should be null.  If it is not, we
                                        // encountered an error, and must free
                                        // up this information.
                                    delete lpcAlertTarget;
                                    }
                                }
                            else
                                {
                                        // If an error occurred while creating
                                        // a new instance of this class type.
                                        // Exit function.
                                    bRetVal = FALSE;
                                    bFinished = TRUE;
                                }
                            }
                        else
                            {
                                        // Hit a new section, so exit out of
                                        // our loop.
                            bFinished = TRUE;
                            }
                        }
                    }
                else
                    {
                                        // End of file was hit, so we return.
                    bFinished = TRUE;
                    }
                }
            while ( FALSE == bFinished );

            }
        else
            {
            bRetVal = FALSE;
            }
        }
    else
        {
        bRetVal = FALSE;
        }

   return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CAlertOptions::SaveSection
//
// Purpose:
//    Save a section of target types to the passed in file class.
//    This function will first write a section header, and then query
//    the classes to create the packed strings.  It will then
//    take those string and write them out to the file.
//
// Parameters:
//    CStdioFile *lpcOutputFile - [IN] File class to save the section
//                                     to.
//    int nTargetType           - [IN] Type of targets to save.  This
//                                     funciton will determine the
//                                     section name and class type
//                                     based on this value.
//
// Returns:
//    BOOLEAN as to whether the save was successfull or not.  This
//    funtion will return FALSE if a critical packing error occurs.
//    (Not inclusive of empty section.)
//
// See Also:
//    AlertOps.h for target type declaration.
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertOptions::SaveSection(
    CStdioFile *lpcOutputFile,
    int nTargetType
    )
{
CAlertTarget *lpcAlertTarget;
CString cszSectionName;
CString cszPackedString;
BOOL bFinished = FALSE;
BOOL bRetVal = TRUE;


                                        // Ensure that there is at least 1 target
                                        // of this type.  If not, we are just
                                        // going to exit.  Return TRUE because
                                        // no error actually occurred, there were
                                        // just no targets of that type.
    if ( m_cAlertTargetList.GetTargetCount( nTargetType ) )
        {
                                        // Attempt to read in the section name
                                        // from our string table based on it's
                                        // type.
        if ( GetSectionNameForTarget( nTargetType, &cszSectionName ) )
            {
                                        // Remove all unwanted characters.
            cszSectionName.TrimRight();
            cszSectionName.TrimLeft();

                                        // Add Braces to section name if needed.
            AddBracesIfNeeded( &cszSectionName );

                                        // Enter a try block.  If anything bad
                                        // happens in here, we are going to fail
                                        // the save.  Vaious errors such as disk
                                        // full could occur here.
            try
                {
                                        // Write section header to file.
                cszSectionName += _T('\n');
                lpcOutputFile->WriteString( cszSectionName );

                                        // Reset the position of the current
                                        // iterator so that we can get all of
                                        // the targets of our type.
                m_cAlertTargetList.ResetPosition();

                                        // Enter loop to output all of the
                                        // targets of this type.
                                        // GetNextTarget will return NULL when
                                        // end of the list is hit.
                while ( lpcAlertTarget = m_cAlertTargetList.GetNextTarget( nTargetType ) )
                    {
                                        // Preprocessing string for base class
                                        // data.  This will pack on data common
                                        // to all CAlertTarget base classes.
                    if ( TRUE == lpcAlertTarget->ConvertCommonToPacked( &cszPackedString ) )
                        {
                                        // Call through to the virtual convert
                                        // function for this type to pack it's
                                        // saved data int a string for us to
                                        // write to the file.
                        if ( TRUE == lpcAlertTarget->ConvertToPacked( &cszPackedString ) )
                            {
                                        // We have a valid packed string, now we
                                        // write this string to the file.
                            cszPackedString += _T('\n');
                            lpcOutputFile->WriteString( cszPackedString );
                            }
                        else
                            {
                                        // An error occured when trying to pack
                                        // the parameter list for this target
                                        // type.  Return FALSE.
                            bRetVal = FALSE;
                            }
                        }
                    else
                        {
                                        // An error occured when trying to pack
                                        // the base class's parameter list for
                                        // this target.  Return FALSE.
                        bRetVal = FALSE;
                        }
                    }
                                        // Write blank line after each target type.
                lpcOutputFile->WriteString( _T("\n") );
                }
            catch (...)
                {
                                        // Something bad happened when trying
                                        // to write items to the file.  Probably
                                        // the disk we are writing to is full.
                                        // Could also be an out of memory error
                                        // when packing parameters.
                bRetVal = FALSE;
                }
            }
        else
            {
                                        // Could not get the section name from
                                        // our resource.
            bRetVal = FALSE;
            }
        }

   return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CAlertOptions::SaveGeneralOptions
//
// Purpose:
//    Saves the general options for the Nav Alert Options.
//    This file name should already be set at this point.
//
// Parameters:
//    CStdioFile *lpcOptionsFile - [IN] Open file to save options to.
//
// Returns:
//    TRUE  - Loaded options correctly.
//    FALSE - Error loading options.
//
// See Also:
//    None.
//
/////////////////////////////////////////////////////////////////////
// 11/21/96 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertOptions::SaveGeneralOptions(
    CStdioFile *lpcOptionsFile
    )
{
CString cszGeneralOptionsSectionName;
BOOL bRetVal = TRUE;

                                        // Switch to this DLL's resource state.
    CDLLResourceState switchState;

                                        // Load the section name for these
                                        // options.
    if ( cszGeneralOptionsSectionName.LoadString( IDS_SECTION_GENERAL_OPTIONS ) )
        {
                                        // Add braces to this section if
                                        // needed.  We want to write this out
                                        // as if it was an INI file.
        AddBracesIfNeeded( &cszGeneralOptionsSectionName );
        cszGeneralOptionsSectionName += _T("\n");

                                        // Write out the section name to the
                                        // file.
        lpcOptionsFile->WriteString( cszGeneralOptionsSectionName );

                                        // Save the AP Remote Alerts Enabled
                                        // BOOL value.
        if ( FALSE == SaveBOOLOptionWithID( IDS_KEY_AP_REMOTE_ALERTS_ENABLED,
                                            m_bAPRemoteAlertsEnabled,
                                            lpcOptionsFile
                                            ) )
            {
            bRetVal = FALSE;
            }

                                        // Save the Scanner Remote Alerts Enabled
                                        // BOOL value.
        if ( FALSE == SaveBOOLOptionWithID( IDS_KEY_SCANNER_REMOTE_ALERTS_ENABLED,
                                            m_bScannerRemoteAlertsEnabled,
                                            lpcOptionsFile
                                            ) )
            {
            bRetVal = FALSE;
            }

                                        // Write an empty line at the end of
                                        // each section.
        lpcOptionsFile->WriteString( _T("\n") );
        }

                                        // Return will be FALSE if any of the
                                        // sections fail to be written.

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CAlertOptions::LoadGeneralOptions
//
// Purpose:
//    Load's the general options for the Nav Alert Options.
//    This file name should already be set at this point.
//
// Parameters:
//    None.
//
// Returns:
//    TRUE  - Loaded options correctly.
//    FALSE - Error loading options.
//
// See Also:
//    None.
//
/////////////////////////////////////////////////////////////////////
// 11/21/96 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertOptions::LoadGeneralOptions( void )
{
CString cszGeneralOptionsSectionName;
BOOL bRetVal = TRUE;

                                        // Switch to this DLL's resource state.
    CDLLResourceState switchState;

                                        // Load the section name for these
                                        // options.
    if ( cszGeneralOptionsSectionName.LoadString( IDS_SECTION_GENERAL_OPTIONS ) )
        {
                                        // Load the AP Remote Alerts Enabled
                                        // BOOL value.
        if ( FALSE == LoadBOOLOptionWithID( cszGeneralOptionsSectionName,
                                            IDS_KEY_AP_REMOTE_ALERTS_ENABLED,
                                            TRUE,
                                            &m_bAPRemoteAlertsEnabled,
                                            m_cszOptionsFileName
                                            ) )
            {
            bRetVal = FALSE;
            }

                                        // Load the Scanner Remote Alerts Enabled
                                        // BOOL value.
        if ( FALSE == LoadBOOLOptionWithID( cszGeneralOptionsSectionName,
                                            IDS_KEY_SCANNER_REMOTE_ALERTS_ENABLED,
                                            TRUE,
                                            &m_bScannerRemoteAlertsEnabled,
                                            m_cszOptionsFileName
                                            ) )
            {
            bRetVal = FALSE;
            }
        }

                                        // Return will be FALSE if any of the
                                        // sections fail to be read.
    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CAlertOptions::SaveSMTPConfig
//
// Purpose:
//    Saves the SMTP Configuration options to our options file.
//    This file name should already be set at this point.
//
// Parameters:
//    CStdioFile *lpcOptionsFile - [IN] Open file to save options to.
//
// Returns:
//    TRUE  - Saved options correctly.
//    FALSE - Error saving options.
//
// See Also:
//    None.
//
/////////////////////////////////////////////////////////////////////
// 11/21/96 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertOptions::SaveSMTPConfig(
    CStdioFile *lpcOptionsFile
    )
{
CString cszSMTPOptionsSectionName;
BOOL bRetVal = TRUE;

                                        // Switch to this DLL's resource state.
    CDLLResourceState switchState;

                                        // Load the section name for these
                                        // options.
    if ( cszSMTPOptionsSectionName.LoadString( IDS_SECTION_SMTP_CONFIG ) )
        {
                                        // Add braces to this section if
                                        // needed.  We want to write this out
                                        // as if it was an INI file.
        AddBracesIfNeeded( &cszSMTPOptionsSectionName );
        cszSMTPOptionsSectionName += _T("\n");

                                        // Write out the section name to the
                                        // file.
        lpcOptionsFile->WriteString( cszSMTPOptionsSectionName );

                                        // Save the SMTP Post office string.
        if ( FALSE == SaveOptionWithID( IDS_KEY_SMTP_POSTOFFICE,
                                        m_cszSMTPPostOffice,
                                        lpcOptionsFile
                                        )  )
            {
            bRetVal = FALSE;
            }

                                        // Save the SMTP author address.
        if ( FALSE == SaveOptionWithID( IDS_KEY_SMTP_AUTHOR,
                                        m_cszSMTPAuthor,
                                        lpcOptionsFile
                                        )  )
            {
            bRetVal = FALSE;
            }
                                        // Save the SMTP mail subject.
        if ( FALSE == SaveOptionWithID( IDS_KEY_SMTP_SUBJECT,
                                        m_cszSMTPSubject,
                                        lpcOptionsFile
                                        )  )
            {
            bRetVal = FALSE;
            }

                                        // Write an empty line at the end of
                                        // each section.
        lpcOptionsFile->WriteString( _T("\n") );
        }

                                        // Return will be FALSE if any of the
                                        // sections fail to be read.
    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CAlertOptions::LoadSMTPConfig
//
// Purpose:
//    Load's the SMTP Configuration options from our options file.
//    This file name should already be set at this point.
//
// Parameters:
//    None.
//
// Returns:
//    TRUE  - Loaded options correctly.
//    FALSE - Error loading options.
//
// See Also:
//    None.
//
/////////////////////////////////////////////////////////////////////
// 11/21/96 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertOptions::LoadSMTPConfig( void )
{
CString cszSMTPOptionsSectionName;
BOOL bRetVal = TRUE;

                                        // Switch to this DLL's resource state.
    CDLLResourceState switchState;

                                        // Load the section name for these
                                        // options.
    if ( cszSMTPOptionsSectionName.LoadString( IDS_SECTION_SMTP_CONFIG ) )
        {
                                        // Load the SMTP Post office string.
        if ( FALSE == LoadOptionWithID( cszSMTPOptionsSectionName,
                                        IDS_KEY_SMTP_POSTOFFICE,
                                        _T(""),
                                        &m_cszSMTPPostOffice,
                                        m_cszOptionsFileName
                                        ) )
            {
            bRetVal = FALSE;
            }

                                        // Load the SMTP author address.
        if ( FALSE == LoadOptionWithID( cszSMTPOptionsSectionName,
                                        IDS_KEY_SMTP_AUTHOR,
                                        _T(""),
                                        &m_cszSMTPAuthor,
                                        m_cszOptionsFileName
                                        )  )
            {
            bRetVal = FALSE;
            }
                                        // Load the SMTP mail subject.
        if ( FALSE == LoadOptionWithID( cszSMTPOptionsSectionName,
                                        IDS_KEY_SMTP_SUBJECT,
                                        _T(""),
                                        &m_cszSMTPSubject,
                                        m_cszOptionsFileName
                                        )  )
            {
            bRetVal = FALSE;
            }
        }

                                        // Return will be FALSE if any of the
                                        // sections fail to be read.
    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// CAlertOptions::SaveReceiveOptions
//
// Purpose:
//    Saves the Receive Options section data.
//    This file name should already be set at this point.
//
// Parameters:
//    CStdioFile *lpcOptionsFile - [IN] Open file to save options to.
//
// Returns:
//    TRUE  - Saved options correctly.
//    FALSE - Error saving options.
//
// See Also:
//    None.
//
/////////////////////////////////////////////////////////////////////
// 05/06/97 - gdzieci - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertOptions::SaveReceiveOptions(
    CStdioFile *lpcOptionsFile
    )
{
    CString cszReceiveOptionsSectionName;
    BOOL bRetVal = TRUE;

                                        // Switch to this DLL's resource state.
    CDLLResourceState switchState;

                                        // Load the section name for these
                                        // options.
    if ( cszReceiveOptionsSectionName.LoadString( IDS_SECTION_RECEIVE_OPTIONS ) )
        {
                                        // Add braces to this section if
                                        // needed.  We want to write this out
                                        // as if it was an INI file.
        AddBracesIfNeeded( &cszReceiveOptionsSectionName );
        cszReceiveOptionsSectionName += _T("\n");

                                        // Write out the section name to the
                                        // file.
        lpcOptionsFile->WriteString( cszReceiveOptionsSectionName );

                                        // Save the ATP Alerts Enabled
                                        // BOOL value.
        if ( FALSE == SaveBOOLOptionWithID( IDS_KEY_ATP_ALERTS_ENABLED,
                                            m_bATPAlertsEnabled,
                                            lpcOptionsFile
                                            ) )
            {
            bRetVal = FALSE;
            }

                                        // Save the IPX Alerts Enabled
                                        // BOOL value.
        if ( FALSE == SaveBOOLOptionWithID( IDS_KEY_IPX_ALERTS_ENABLED,
                                            m_bIPXAlertsEnabled,
                                            lpcOptionsFile
                                            ) )
            {
            bRetVal = FALSE;
            }

                                        // Save the IP Alerts Enabled
                                        // BOOL value.
        if ( FALSE == SaveBOOLOptionWithID( IDS_KEY_IP_ALERTS_ENABLED,
                                            m_bIPAlertsEnabled,
                                            lpcOptionsFile
                                            ) )
            {
            bRetVal = FALSE;
            }

                                        // Write an empty line at the end of
                                        // each section.
        lpcOptionsFile->WriteString( _T("\n") );
        }

                                        // Return will be FALSE if any of the
                                        // sections fail to be written.

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// CAlertOptions::LoadReceiveOptions
//
// Purpose:
//    Loads the Receive Options section data.
//    This file name should already be set at this point.
//
// Parameters:
//    None.
//
// Returns:
//    TRUE  - Loaded options correctly.
//    FALSE - Error loading options.
//
// See Also:
//    None.
//
/////////////////////////////////////////////////////////////////////
// 05/06/97 - gdzieci - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertOptions::LoadReceiveOptions( void )
{
    CString cszReceiveOptionsSectionName;
    BOOL bRetVal = TRUE;

                                        // Switch to this DLL's resource state.
    CDLLResourceState switchState;

                                        // Load the section name for these
                                        // options.
    if ( cszReceiveOptionsSectionName.LoadString( IDS_SECTION_RECEIVE_OPTIONS ) )
        {
                                        // Load the ATP Alerts Enabled
                                        // BOOL value.
        if ( FALSE == LoadBOOLOptionWithID( cszReceiveOptionsSectionName,
                                            IDS_KEY_ATP_ALERTS_ENABLED,
                                            FALSE,
                                            &m_bATPAlertsEnabled,
                                            m_cszOptionsFileName
                                            ) )
            {
            bRetVal = FALSE;
            }

                                        // Load the IPX Alerts Enabled
                                        // BOOL value.
        if ( FALSE == LoadBOOLOptionWithID( cszReceiveOptionsSectionName,
                                            IDS_KEY_IPX_ALERTS_ENABLED,
                                            FALSE,
                                            &m_bIPXAlertsEnabled,
                                            m_cszOptionsFileName
                                            ) )
            {
            bRetVal = FALSE;
            }
        }

        // Listening for IP alerts is not implemented yet, so we don't want
        // to turn them on. The following code should be uncommented out
        // when IP alerts are implemented.

                                        // Load the IP Alerts Enabled
                                        // BOOL value.
        //if ( FALSE == LoadBOOLOptionWithID( cszReceiveOptionsSectionName,
        //                                    IDS_KEY_IP_ALERTS_ENABLED,
        //                                    FALSE,
        //                                    &m_bIPAlertsEnabled,
        //                                    m_cszOptionsFileName
        //                                    ) )
        //    {
        //    bRetVal = FALSE;
        //    }

                                        // Return will be FALSE if any of the
                                        // sections fail to be read.
    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// CAlertOptions::GetTargetCount
//
// Purpose:
//    Get the number of targets of a particular target type or set
//    of target types.
//
// Parameters:
//    int nTargetType - [IN] Combination of target type flags to
//                           retrieve a count for.  Can be a single
//                           type or TARGET_ALL for all types.
//
// Returns:
//    DWORD count of targets of that type.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/27/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CAlertOptions::GetTargetCount(
    int nTargetType  //= TARGET_ALL (default)
    )
{
    return m_cAlertTargetList.GetTargetCount( nTargetType );
}


/////////////////////////////////////////////////////////////////////
// CAlertOptions::AddTarget
//
// Purpose:
//    Function to add a target to the current Alert Target list.
//    This target can be of any derived class from a CAlertTarget,
//    such as a CPager target or a CForward Target.
//
// Parameters:
//    CAlertTarget *lpcAlertTarget - [IN] address of the alert target
//                                   to add.  This should be
//                                   pre-allocated.  This object will
//                                   be put directly into the list.
//                                   Another version of this function
//                                   will follow that will add a
//                                   target that will copy the target
//                                   you send, instead of inserting
//                                   that particular target.
//
// Returns:
//    BOOLEAN of the success of the target addition to the list.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertOptions::AddTarget(
    CAlertTarget *lpcAlertTarget
    )
{
BOOL bRetVal=TRUE;

    bRetVal = m_cAlertTargetList.AddTarget( lpcAlertTarget );

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CAlertOptions::RemoveTarget
//
// Purpose:
//    Remove a specific target from the list of targets.  This value
//    is a pointer to the base class of that target.  To accomplish
//    this, we are going to call to our list of targets to remove it
//    for you, and return the value that call returned to us.
//
// Parameters:
//    CAlertTarget *lpcTarget - [IN] Pointer to the object to remove
//                              from our list.  This should be
//                              gotten through calls to GetNextTarget
//                              which will return this to you.
//
// Returns:
//    BOOLEAN as to whether the item was found and deleted
//    successfully.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/25/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertOptions::RemoveTarget(
    CAlertTarget *lpcTarget
    )
{
BOOL bRetVal;

                                        // Remove the target requested.
    bRetVal = m_cAlertTargetList.RemoveTarget( lpcTarget );

                                        // Reset the current iteration
                                        // position.
    m_cAlertTargetList.ResetPosition();

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// CAlertOptions::DestroyAll
//
// Purpose:
//    This function will remove all items from our CAlertTargetList
//    and call the destructor for each of those targets to destroy
//    the data associated with it.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/25/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CAlertOptions::DestroyAll( void )
{
    m_cAlertTargetList.DestroyAll();
}

/////////////////////////////////////////////////////////////////////
// CAlertOptions::GetNextTarget
//
// Purpose:
//    This function is a call through function to the
//    m_cAlertTargetList->GetNextTarget() function.  It will be used
//    to iterate through the target list held by the CAlertOptions
//    class.
//
// Parameters:
//    int nEventType - [IN] Target type to iterate though.  Can be a
//                          combination of flags, or TARGET_ALL for
//                          all target types. (Default = TARGET_ALL )
//
// Returns:
//    CAlertTarget * - Pointer to a CAlertTarget object.  This will
//                     be the next iteration target, or NULL if the
//                     end of the list was reached before a target of
//                     this type was reached.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/29/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlertTarget *CAlertOptions::GetNextTarget(
    int nEventType  // = TARGET_ALL
    )
{
   return m_cAlertTargetList.GetNextTarget( nEventType );
}
/////////////////////////////////////////////////////////////////////
// CAlertOptions::RemoveNextTarget
//
// Purpose:
//    This function is a call through function to the
//    m_cAlertTargetList->RemoveNextTarget() function.  It will be
//    used to remove a target from the CAlertOptions class so that
//    you can do such things as add it to another CObList.  After you
//    do this, you are responsable for deleting this object.
//
// Parameters:
//    int nEventType - [IN] Target type to remove though.  Can be a
//                          combination of flags, or TARGET_ALL for
//                          all target types. (Default = TARGET_ALL )
//
// Returns:
//    CAlertTarget * - Pointer to a CAlertTarget object.  This will
//                     be the next iteration target, or NULL if the
//                     end of the list was reached before a target of
//                     this type was reached.  You will not have to
//                     delete this object youself, since you now own
//                     it.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/29/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlertTarget *CAlertOptions::RemoveNextTarget(
    int nEventType  // = TARGET_ALL
    )
{
   return m_cAlertTargetList.RemoveNextTarget( nEventType );
}


/////////////////////////////////////////////////////////////////////
// CAlertOptions::ResetPosition
//
// Purpose:
//    This function will reset the current iteration position for the
//    alert target list.  This will need to be called between
//    subsequent searches if you leave the current iteration pointer
//    in the middle of the list.  If you do not call this function,
//    you may be in the middle of the list, when you expected to be
//    at the head.  This function will not need to be called,
//    however, if you are simply iterating through the entire list of
//    a single item, or of all items.  When the end of the list is
//    hit by a normal iteration loop, the iteration position will
//    automatically be reset to the head of the list.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//    CAlertTargetList::ResetPosition.
//
/////////////////////////////////////////////////////////////////////
// 11/14/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CAlertOptions::ResetPosition( void )
{
                                        // Reset the position of the current
                                        // iterator so that we can re-iterate
                                        // the targets.
    m_cAlertTargetList.ResetPosition();

}

/////////////////////////////////////////////////////////////////////
// CAlertOptions::GetDefaultOptionsFileName
//
// Purpose:
//    Loads the default file name for the Alert Options, and then
//    prepends the path to it that this module was loaded from.
//
// Parameters:
//    CString *lpcszDefaultFileName - [OUT] Address of the return
//                                    string.  This will only contain
//                                    information if we loaded the
//                                    file name successfully.
//
// Returns:
//    BOOLEAN as to the success of the file name load, and also, the
//    file name in the lpcszDefaultFileName parameter.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/14/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlertOptions::GetDefaultOptionsFileName(
    CString *lpcszDefaultFileName
    )
{
HMODULE hModule;
TCHAR szTempStr[MAX_PATH];
LPCTSTR szMovePtr;
BOOL bRetVal=FALSE;

                                        // This is done to make sure that we
                                        // have the correct resource handle.
    CDLLResourceState resourceSwitch;

                                        // Load the default file name string
                                        // from our resource.
    if ( lpcszDefaultFileName->LoadString( IDS_DEFAULT_ALERT_OPTIONS_FILE_NAME ) )
        {
                                        // Get the module handle for a call to
                                        // GetModuleFileName()
        hModule = (HMODULE)AfxGetResourceHandle();

                                        // If we got a valid module handle...
        if ( NULL != hModule )
            {
                                        // Attempt to get the module file name
                                        // of this module.
            if ( 0 != GetModuleFileName( hModule, szTempStr, MAX_PATH ) )
                {
                                        // Find the last '\\' if any.
                szMovePtr = _tcsrchr( szTempStr, _T('\\') );

                                        // If we found one...
                if ( NULL != szMovePtr )
                    {
                                        // Chop off the file name.
                    *(_tcsinc( szMovePtr )) = NULL;
                    }
                else
                    {
                                        // If we did not find a '\\' character
                                        // in the string, then set the string
                                        // to nothing so that the only
                                        // characters that we have in our
                                        // string are of the default file name.
                    szTempStr[0] = NULL;
                    }

                                        // Tack on the default file name to the
                                        // path of this module.
                _tcscat( szTempStr, *lpcszDefaultFileName );

                                        // Copy the full path from our
                                        // temporary string, back to our return
                                        // string.
                *lpcszDefaultFileName = szTempStr;

                                        // Return True.
                bRetVal = TRUE;
                }
            }
        }

    if ( FALSE == bRetVal )
        {
                                        // Something went wrong, so we should
                                        // make sure that the default file name
                                        // parameter is empty when we return.
        lpcszDefaultFileName->Empty();
        }

    return bRetVal;
}



/////////////////////////////////////////////////////////////////////
// Helper function (Non Class)

/////////////////////////////////////////////////////////////////////
// BOOL SaveOptionWithID
//
// Purpose:
//    To save a parameter to an INI file based upon a string resource
//    ID, and the parameter to save.  This function will write options
//    as if it were an INI file.  We cannot use INI file writes
//    because of caching concerns on Win95.
//
// Parameters:
//    UINT    nKeyNameID         - [IN] String ID of the Key name.
//    LPCWSTR cszParameterToSave - [IN] Parameter to save.
//    CStdioFile *lpcOptionsFile - [IN] Open file to save options to.
//
// Returns:
//    TRUE  - Save was successfull.
//    FALSE - Error when saving to file.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/23/96 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SaveOptionWithID(
    UINT        nKeyNameID,
    LPCTSTR     cszParameterToSave,
    CStdioFile *lpcOptionsFile
    )
{
CString cszOutputString;
BOOL bRetVal = FALSE;

                                        // Switch to this DLL's resource state.
    CDLLResourceState switchState;

                                        // Exception handling...
    try
        {
                                        // Load the string for the key name of
                                        // this option.
        if ( cszOutputString.LoadString( nKeyNameID ) )
            {
                                        // Tack on the '=' sign and the value
                                        // to this string.
            cszOutputString += _T("=");
            cszOutputString += cszParameterToSave;
            cszOutputString += _T("\n");

                                        // Write the option out to the file.
            lpcOptionsFile->WriteString( cszOutputString );

                                        // Return Success.
            bRetVal = TRUE;
            }
        }
    catch ( CMemoryException )
        {
                                        // An Error occurred. Return FALSE.
        bRetVal = FALSE;
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// BOOL LoadOptionWithID
//
// Purpose:
//    To load a parameter from an INI file based upon a string resource
//    ID, and the parameter to load.  This function will simply be a
//    call-through for GetPrivateProfileString;
//
// Parameters:
//    LPCWSTR cszSectionName     - [IN]  Section name to write within.
//    UINT    nKeyNameID         - [IN]  String ID of the Key name.
//    LPCWSTR cszDefaultValue    - [IN]  Default value for this param.
//    CString *lpcReturnString   - [OUT] Return string for option.
//    LPCWSTR cszFileName        - [IN]  File Name.
//
// Returns:
//    TRUE  - Save was successfull.
//    FALSE - Error when saving to file.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/23/96 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL LoadOptionWithID(
    LPCTSTR  cszSectionName,
    UINT     nKeyNameID,
    LPCTSTR  cszDefaultValue,
    CString *lpcReturnString,
    LPCTSTR  cszFileName
    )
{
CString cszFixedupFileName = cszFileName;
CString cszKeyName;
BOOL bRetVal = FALSE;

                                        // Switch to this DLL's resource state.
    CDLLResourceState switchState;

                                        // Fixup file name if it is a local
                                        // file name.
    FixupINIFileName( &cszFixedupFileName );

                                        // Exception handling...
    try
        {
                                        // Load the string for the key name of
                                        // this option.
        if ( cszKeyName.LoadString( nKeyNameID ) )
            {
                                        // Write the option out to the file.
            if ( GetPrivateProfileString( cszSectionName,
                                          cszKeyName,
                                          cszDefaultValue,
                                          lpcReturnString->GetBufferSetLength( MAX_PATH ),
                                          MAX_PATH,
                                          cszFixedupFileName ) )
                {
                bRetVal = TRUE;
                }

                                        // Release the buffer that we got to
                                        // load the string.
            lpcReturnString->ReleaseBuffer();
            }
        }
    catch ( CMemoryException )
        {
                                        // An Error occurred. Return FALSE.
        bRetVal = FALSE;
        }


    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// BOOL LoadBOOLOptionWithID
//
// Purpose:
//    To load a parameter from an INI file based upon a string resource
//    ID, and the parameter to load.  This function will simply be a
//    call-through for GetPrivateProfileString.  It will also convert
//    the return value to a BOOL for you.
//
// Parameters:
//    LPCWSTR cszSectionName - [IN]  Section name to write within.
//    UINT    nKeyNameID     - [IN]  String ID of the Key name.
//    BOOL    bDefaultValue  - [IN]  Default BOOl value for this param.
//    BOOL   *lpbReturnVal   - [OUT] Return BOOL value for option.
//    LPCWSTR cszFileName    - [IN]  File Name.
//
// Returns:
//    TRUE  - Save was successfull.
//    FALSE - Error when saving to file.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/23/96 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL LoadBOOLOptionWithID(
    LPCTSTR  cszSectionName,
    UINT     nKeyNameID,
    BOOL     bDefaultValue,
    BOOL    *lpbReturnVal,
    LPCTSTR  cszFileName
    )
{
CString cszDefaultValue;
CString cszReturnString;
BOOL bRetVal = FALSE;

    cszDefaultValue.Format( _T("%d"), (INT) bDefaultValue );

                                        // Call through to the load option with
                                        // ID function.
    if ( TRUE == ( bRetVal = LoadOptionWithID( cszSectionName,
                                               nKeyNameID,
                                               cszDefaultValue,
                                               &cszReturnString,
                                               cszFileName ) ) )
        {
                                        // Convert the option that we loaded to
                                        // a bool.
        *lpbReturnVal = (BOOL) _ttoi( cszReturnString );
        }
    else
        {
                                        // Set return to default value.
        *lpbReturnVal = bDefaultValue;
        }


    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// BOOL SaveBOOLOptionWithID
//
// Purpose:
//    To save a parameter to an INI file based upon a string resource
//    ID, and the parameter to save.  This function will simply be a
//    call-through for GetPrivateProfileString.  It will convert the
//    BOOL to a string to write to the option file.
//
// Parameters:
//    UINT    nKeyNameID         - [IN]  String ID of the Key name.
//    BOOL    bValueToSave       - [IN]  Value to save to the file.
//    CStdioFile *lpcOptionsFile - [IN] Open file to save options to.
//
// Returns:
//    TRUE  - Save was successfull.
//    FALSE - Error when saving to file.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/23/96 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SaveBOOLOptionWithID(
    UINT        nKeyNameID,
    BOOL        bValueToSave,
    CStdioFile *lpcOptionsFile
    )
{
CString cszSaveString;
BOOL bRetVal = FALSE;

    cszSaveString.Format( _T("%d"), (INT) bValueToSave );

                                        // Call through to the Save option with
                                        // ID function.
    bRetVal = SaveOptionWithID( nKeyNameID,
                                cszSaveString,
                                lpcOptionsFile );

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// void FixupINIFileName
//
// Purpose:
//    Function to fixup an ini file name string to have a '.\' if it
//    is simply a local directory file name.
//
// Parameters:
//    CString *lpcFileNameToFixup - [IN][OUT] Address of string to
//                                  fixup if neccessary.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/23/96 - ksackin - Function Created.
/////////////////////////////////////////////////////////////////////
void FixupINIFileName(
    CString *lpcFileNameToFixup
    )
{
                                        // Trap NULL string scenario.
    if ( NULL == lpcFileNameToFixup )
        {
                                        // No fixup neccessary for a NULL
                                        // string.
        return;
        }

                                        // If this string is not UNC or root
                                        // of current drive.
    if ( 0 != _tcsncmp( *lpcFileNameToFixup, _T("\\"), 1 ) )
        {
                                        // Item does not have a drive specifier.
        if ( FALSE == ((lpcFileNameToFixup->GetLength() >= 2) &&
                       (lpcFileNameToFixup->GetAt(1) == _T(':'))) )
            {
                                        // Fixup the file name with a '.\' in
                                        // front.
            *lpcFileNameToFixup = _T(".\\") + (*lpcFileNameToFixup);
            }
        }
}

/////////////////////////////////////////////////////////////////////
// GetSectionNameForTarget
//
// Purpose:
//    This function will take a target type and switch on it to find
//    the identifier for the string ID.  When it has this, it will
//    load the string from the resource and return it in the passed
//    up CString.
//
// Parameters:
//    int nTargetType           - [IN]  Target type to read in.
//    CString *lpcszSectionName - [OUT] Address of a CString for a
//                                      section name.
//
// Returns:
//    BOOLEAN value of whether it could get the identifier and load
//    the string from the string table.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 10/23/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL GetSectionNameForTarget(
    int nTargetType,
    CString *lpcszSectionName
    )
{
BOOL    bRetVal=TRUE;
int     nStringTableID;

                                        // Setup the module state so that
                                        // we may load strings from the correct
                                        // resource using the
                                        // CString::LoadString() member function.
    CDLLResourceState resourceSwitch;


    switch ( nTargetType )
        {
        case TARGET_EMAIL:
            nStringTableID = IDS_SECTION_EMAIL;
            break;

        case TARGET_PAGER:
            nStringTableID = IDS_SECTION_PAGER;
            break;

        case TARGET_MESSAGE_LOCAL:
            nStringTableID = IDS_SECTION_MESSAGE_LOCAL;
            break;

        case TARGET_MESSAGE_NETWORK:
            nStringTableID = IDS_SECTION_MESSAGE_NETWORK;
            break;

        case TARGET_FORWARD:
            nStringTableID = IDS_SECTION_FORWARD;
            break;

        case TARGET_NLM:
            nStringTableID = IDS_SECTION_NLM;
            break;

        case TARGET_EVENTLOG:
            nStringTableID = IDS_SECTION_EVENTLOG;
            break;

        case TARGET_ACTIVITY_LOG:
            nStringTableID = IDS_SECTION_ACTIVITY_LOG;
            break;

        case TARGET_SNMP:
            nStringTableID = IDS_SECTION_SNMP;
            break;

        default:
            nStringTableID = 0;
            break;
        }

                                        // If we matched the ID with a string
                                        // table ID, load the string.
    if ( 0 != nStringTableID )
        {
                                        // If there is not enough memory to
                                        // load the string, this will catch the
                                        // exception.
        try
            {
                                        // Attempt to load the string.
            if ( FALSE == lpcszSectionName->LoadString( nStringTableID) )
                {
                                        // Load failed, return FALSE.
                bRetVal = FALSE;
                }
            }

        catch( CMemoryException )
            {
                                        // Catch a memory allocation error in
                                        // CString.
            bRetVal = FALSE;
            }
        }


    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// AddBracesIfNeeded
//
// Purpose:
//    This function will determine if a section string has braces
//    around it or not, and if it does not, it will add them.
//
// Parameters:
//    CString *lpcszSectionName - [IN][OUT] Section name to determine
//                                          if braces need to be
//                                          added to.
//
// Returns:
//    Modifies lpcszSectionName string with braces if needed.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/24/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void AddBracesIfNeeded(
    CString *lpcszSectionName
    )
{
                                        // If this string is not empty...
    if ( 0 != lpcszSectionName->GetLength() )
        {
        try
            {
                                        // If there is no leading brace, add
                                        // one.
            if ( lpcszSectionName->GetAt( 0 ) != '[' )
                {
                *lpcszSectionName = _T("[") + *lpcszSectionName;
                }

                                        // Check for trailing brace, and add
                                        // one if neccessary.
            if ( lpcszSectionName->GetAt( lpcszSectionName->GetLength() - 1 ) != ']' )
                {
                *lpcszSectionName += _T("]");
                }
            }
        catch( CMemoryException )
            {
                                        // Return without crashing...
            }
        }
}

/////////////////////////////////////////////////////////////////////
// SeekToSection
//
// Purpose:
//    This function will take a pointer to a CStdioFile and a section
//    name (complete with brackets), and seek through the file for
//    that section.  It will leave you directly after that section,
//    or will return NULL if it cannot be found.
//
// Parameters:
//    CStdioFile   *lpcInputFile    - [IN] File to search through for
//                                         the section passed.
//    LPCWSTR       szSectionToFind - [IN] Section name to search for,
//                                         complete with braces around
//                                         the section.
//
// Returns:
//    BOOLEAN value of the success of the seek.  If the section was
//    found, the file is left directly after the section seached for.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/24/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SeekToSection(
    CStdioFile *lpcInputFile,
    LPCTSTR szSectionToFind
    )
{
CString cszTempString;
BOOL    bReturn = TRUE;
BOOL    bFinished = FALSE;

                                        // Reset the file so that we do not
                                        // miss the search item.  This file
                                        // could be at any position right now.
    lpcInputFile->SeekToBegin();

                                        // Loop while searching for the seek
                                        // string.
    do
        {
                                        // Try to read in a string from the
                                        // file.
        if ( lpcInputFile->ReadString( cszTempString ) )
            {

                                        // Trim the leading and trailing spaces
                                        // off of this string.  Also get rid of
                                        // any newline characters.
            cszTempString.TrimRight();
            cszTempString.TrimLeft();

                                        // Compare the string read in with the
                                        // string we are searching for.
            if ( ! _tcsicmp( cszTempString, szSectionToFind ) )
                {
                                        // Item found, exit out of this loop.
                                        // (The file pointer is directly after
                                        // the section name).
                bFinished = TRUE;
                }
            }
        else
            {
                                        // If the read failed, then we are
                                        // going to exit out of our search
                                        // loop, and return a failed search.
            bReturn = FALSE;
            bFinished = TRUE;
            }
        }
    while ( FALSE == bFinished ) ;

    return bReturn;
}


/////////////////////////////////////////////////////////////////////
// CreateNewTargetOfType
//
// Purpose:
//    This function will take an identifier of an alert target type,
//    create an instance of that type, and then return the base class
//    of that new target.
//
// Parameters:
//    int nTargetType - [IN] Target type to create.
//
//
// Returns:
//    A base pointer to a newly created class of the type you
//    specified.
//    If the type sent is unrecognized, or an object fails to create,
//    it will return a NULL.
//
// See Also:
//    Targets.h for list of target types.
//
/////////////////////////////////////////////////////////////////////
// 10/24/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CAlertTarget *CreateNewTargetOfType(
    int nTargetType
    )
{
CAlertTarget *  lpcAlertTarget;

    switch ( nTargetType )
        {
        case TARGET_EMAIL:
            lpcAlertTarget = (CAlertTarget *) new( CEMailTarget );
            break;

        case TARGET_PAGER:
            lpcAlertTarget = (CAlertTarget *) new( CPagerTarget );
            break;

        case TARGET_MESSAGE_LOCAL:
            lpcAlertTarget = (CAlertTarget *) new( CLocalMsgTarget );
            break;

        case TARGET_MESSAGE_NETWORK:
            lpcAlertTarget = (CAlertTarget *) new( CNetMsgTarget );
            break;

        case TARGET_FORWARD:
            lpcAlertTarget = (CAlertTarget *) new( CForwardTarget );
            break;

        case TARGET_NLM:
            lpcAlertTarget = (CAlertTarget *) new( CNLMTarget );
            break;

        case TARGET_EVENTLOG:
            lpcAlertTarget = (CAlertTarget *) new( CEventLogTarget );
            break;

        case TARGET_ACTIVITY_LOG:
            lpcAlertTarget = (CAlertTarget *) new( CActivityLogTarget );
            break;

        case TARGET_SNMP:
            lpcAlertTarget = (CAlertTarget *) new( CSNMPTarget );
            break;

        default:
            lpcAlertTarget = NULL;
            break;
        }

    return lpcAlertTarget;
}
