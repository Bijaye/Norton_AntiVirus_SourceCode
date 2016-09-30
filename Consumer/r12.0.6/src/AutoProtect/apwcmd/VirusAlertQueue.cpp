#include "stdafx.h"
#include "virusalertqueue.h"
#include "VirusAlertPopup.h"
#include "NAVError.h"  // for reporting errors
#include "AVccModuleID.h"
#include "switches.h" // NAVW command-line switches
#include "ccSymInstalledApps.h" // for NAV path
#include "AVRESBranding.h"

#include "EventQueue.h"         // for RemoveFileVID
#include "ccCatch.h"
#include "time.h"				// time function
#include "ccSymStringImpl.h"	// ccSym::CStringImpl.h

#include "ISymMceCmdLoader.h"	// detect optional Media center components

#include "CfgwizNames.h"  // useful config wiz strings
#include "AVScanManagerLoader.h"  // scan manager single instance interface

bool CVirusAlertQueue::m_bDisplayAlert = true;
bool CVirusAlertQueue::m_bAutoScanSideEffects = false;

#define SPYWARE_TIME 60*1000     // 60 secs
#define SIDE_EFFECTS_TIME 15*1000 // 15 secs
#define SIDE_EFFECTS_TIMER_ID 0
#define SPYWARE_TIMER_ID 1
#define SPYWARE_IGNORED_SECONDS 1800    // 30 min
#define SPYWARE_IGNORED_TIME SPYWARE_IGNORED_SECONDS*1000 // 30 min in millisecs

CVirusAlertQueue::CVirusAlertQueue(void)
{
    // For subscriptions!
    // LiveSubscribe fails if you use MULTITHREADED.
    m_Options.m_eCOMModel = ccLib::CCoInitialize::eSTAModel;
    m_Options.m_bPumpMessages = 1;

    m_eventNewWork.Create ( NULL, FALSE, FALSE, NULL, NULL ); // create local auto-reset event
    m_hTimers[SIDE_EFFECTS_TIMER_ID] = NULL;
    m_hTimers[SPYWARE_TIMER_ID] = NULL;

    m_bSpywareScanRunning = false;
    
    // We do file comparison in lower case mode. According to MSDN not all characters
    // will convert properly to lower case unless setlocale() is called.
    // MSDN note:
    //      "You will need to call setlocale for _wcsicmp to work with Latin 1 characters.
    //      The C locale is in effect by default, so, for example, ä will not compare equal to Ä.
    //      Call setlocale with any locale other than the C locale before the call to _wcsicmp"

    // Setting locale with a blank string to set the native environment
    setlocale(LC_ALL, "");

    // Set up exclusions
    if(!SYM_SUCCEEDED(m_ExclusionMgrFac.Initialize()))
    {
        CCTRACEE(_T("CVirusAlertQueue::ctor - Could not init exclusionmgr factory."));
        throw(new std::runtime_error("Could not load exclusion DLL."));
    }

    if(!SYM_SUCCEEDED(m_ExclusionMgrFac.CreateObject(m_spExclusionMgr)))
    {
        CCTRACEE(_T("CVirusAlertQueue::ctor - Could not create exclusion manager."));
        throw(new std::runtime_error("Could not create exclusion manager."));
    }

    if(m_spExclusionMgr->initialize(false) != NavExclusions::Success)
    {
        CCTRACEE(_T("CVirusAlertQueue::ctor - Could not init exclusion manager."));
        throw(new std::runtime_error("Could not load exclusions."));
    }
}

CVirusAlertQueue::~CVirusAlertQueue(void)
{
    if (m_hTimers[SIDE_EFFECTS_TIMER_ID])
        CloseHandle (m_hTimers[SIDE_EFFECTS_TIMER_ID]);

    if (m_hTimers[SPYWARE_TIMER_ID])
        CloseHandle (m_hTimers[SPYWARE_TIMER_ID]);

    // Clean up exclusions
    m_spExclusionMgr->uninitialize(false);
    m_spExclusionMgr.Release();
}

void CVirusAlertQueue::AddVirusAlert ( CVirusAlertPopup* pVirusPopup )
{
    if ( !pVirusPopup)
        return;

    ccLib::CSingleLock lock (&m_critVirusAlerts, INFINITE, FALSE);
    m_qpVirusAlerts.push (pVirusPopup);
    
    // Containers don't trigger side-effects repairs!
    // MBR, Boot records don't trigger side-effects either.
    //
    if ( pVirusPopup->sContainerName.empty() &&
         AV::Event_Threat_ObjectType_File == pVirusPopup->vecData.at(0).lType)
    {
        try 
        {
            // Save the side-effects data
            //
            std::string strTempThreatCats; // pass in empty

            if ( addDelayedAlert (pVirusPopup->vecData.at(0).ulVirusID,
                                  pVirusPopup->vecData.at(0).sObjectName,
                                  pVirusPopup->vecData.at(0).sVirusName,
                                  strTempThreatCats))
            {
                resetTimer (SIDE_EFFECTS_TIMER_ID, SIDE_EFFECTS_TIME);
            }
        }
        catch (...)
        {
            CCTRACEE ("CVirusAlertQueue::AddViralAlert - no data");
        }
    }

    m_eventNewWork.SetEvent ();
}

void CVirusAlertQueue::AddSpywareAlert (unsigned long ulVirusID,
                                        std::wstring& strFileName,
                                        std::wstring& strThreatName,
                                        std::string&  strTheatCats,
                                        bool bCompressed)
{
    if ( !ulVirusID || strFileName.empty() || strThreatName.empty())
        return;

    try 
    {
        // Save the spyware data
        //
        if ( addDelayedAlert (ulVirusID, strFileName, strThreatName, strTheatCats, bCompressed))
        {
            resetTimer (SPYWARE_TIMER_ID, SPYWARE_TIME);
            m_eventNewWork.SetEvent (); // needed to pick up new timers
        }
    }
    catch (...)
    {
        CCTRACEE ("CVirusAlertQueue::AddViralAlert - no data");
    }
}


//////////////////////////////////////////////////////////////////////
// CVirusAlertQueue::Run()

int CVirusAlertQueue::Run()
{
	// Get product name from AVRES.DLL
	if(m_csProductName.IsEmpty())
	{
		CBrandingRes BrandRes;
		m_csProductName = BrandRes.ProductName();
	}

    ccLib::CMessageLock msgLock ( TRUE, FALSE );

    // Wait for timers/new event/shutdown
    // - side-effects timer up? Show side-effects alert
    // - spyware time up? Show Spyware alert
    //
    while ( true )
    {
        std::vector<HANDLE> vecWaitEvents;
        vecWaitEvents.push_back(m_Terminating.GetHandle());
        vecWaitEvents.push_back(m_eventNewWork.GetHandle());

        long lWaitSideEffects = 0;
        long lWaitSpyware = 0;

        if ( m_hTimers[SIDE_EFFECTS_TIMER_ID])
        {
            vecWaitEvents.push_back (m_hTimers[SIDE_EFFECTS_TIMER_ID]);
            lWaitSideEffects = vecWaitEvents.size()-1; // 2
        }

        if ( m_hTimers[SPYWARE_TIMER_ID])
        {
            vecWaitEvents.push_back (m_hTimers[SPYWARE_TIMER_ID]);
            lWaitSpyware = vecWaitEvents.size()-1; // 2 or 3
        }

		// Wait here for something to happen
        DWORD dwWaitResult = msgLock.Lock ( vecWaitEvents.size(), &vecWaitEvents[0], FALSE, INFINITE, FALSE );

        if ( WAIT_FAILED == dwWaitResult )
        {
            CCTRACEE ("Wait failed in CVirusAlertQueue");
    		break;
        }

        dwWaitResult -= WAIT_OBJECT_0;

		// exit signaled.
		if (0 == dwWaitResult)
			return 0;

        // New virus alert item. Display them all
        //
		if (1 == dwWaitResult)
        {
#ifdef AP_DISPLAY_VIRUS_ALERTS
            if (!displayVirusAlerts())
                return 0; // exit called during display!
#endif //AP_DISPLAY_VIRUS_ALERTS
        }

        if (lWaitSideEffects == dwWaitResult)
        {
            doSideEffectsAlert();
        }

        if (lWaitSpyware == dwWaitResult)
        {
            // Only do one spyware scan at a time
            if( m_bSpywareScanRunning || isCfgWizRunning() || isOtherScanRunning())
            {
                CCTRACEI ("CVirusAlertQueue::Run() - There is already a spyware scan running, reseting the spyware timer.");
                resetTimer (SPYWARE_TIMER_ID, SPYWARE_TIME);
            }
            else
            {
                doSpywareAlert();
                CloseHandle(m_hTimers[SPYWARE_TIMER_ID]);
                m_hTimers[SPYWARE_TIMER_ID] = NULL;
            }
        }
    }// end special alerts

    return 0;
}

void CVirusAlertQueue::doSideEffectsAlert (void)
{
    if ( m_listDelayedAlertData.size ())
    {
		SymMCE::ISymMceCmdLoader	mce;

		// All alerts for side effects are ignored if the MCE shell is maximized. We must remove them from the vector here.
		if (mce.IsMceMaximized())
		{
			CCTRACEI (_T("CVirusAlertQueue::doSideEffectsAlert - MCE maximized, all side effects alerts are ignored (No action)!"));
            ccLib::CSingleLock lock (&m_critDelayedAlertData, INFINITE, FALSE );

            // Iterate list and remove side effect items. It's done like this
            // because we are erasing while iterating.
            //
            iterDelayedAlertData iterDAD = m_listDelayedAlertData.begin ();

            while ( iterDAD != m_listDelayedAlertData.end () )
            {
                // We don't want Spyware threat cat stuff
                //
                if ( (*iterDAD).strThreatCats.empty())
                {
                    // Remove this file if it was just scanned
                    iterDAD = m_listDelayedAlertData.erase(iterDAD);
                    continue;
                }
                else
                    iterDAD++;
            }
		}
        else if ( m_bAutoScanSideEffects )
            doSideEffectsScan (true);
        else
        {
            CSideEffectsPopup::DATA Data;
            CSideEffectsPopup popup;
            
            // lock the file/VID list
            {
                ccLib::CSingleLock lock ( &m_critDelayedAlertData, INFINITE, FALSE );

                // Make a copy of our data so we don't have to lock it open.
                //
                for ( iterDelayedAlertData iter = m_listDelayedAlertData.begin();
                      iter != m_listDelayedAlertData.end ();
                      iter++)
                {
                    // We don't want Spyware threat cat stuff
                    //
                    if ( (*iter).strThreatCats.empty())
                    {
                        Data.ulVirusID = (*iter).ulVirusID;
                        Data.strThreatName = (*iter).strThreatName;
                        popup.vecSideEffects.push_back(Data);
                    }
                }
            } // end lock scope
            
            if ( CSideEffectsPopup::RESULT_SCAN == popup.DoModal())
            {   
                doSideEffectsScan (false);
            }
            else
            {
                ccLib::CSingleLock lock (&m_critDelayedAlertData, INFINITE, FALSE );

                // User Ignored this alert. Remove the vids from 
                // that popup.
                //
                for ( CSideEffectsPopup::iterVecData iterData = popup.vecSideEffects.begin();
                          iterData < popup.vecSideEffects.end();
                          iterData++)
                { 
                    // Search each item and remove it. It's done like this
                    // because we are erasing while iterating.
                    //
                    iterDelayedAlertData iterDAD = m_listDelayedAlertData.begin ();

                    while ( iterDAD != m_listDelayedAlertData.end () )
                    {
                        // Match on virus ID since the user was only presented this data.
                        // They have chosen to IGNORE all of those VIDs, for now.
                        if ((*iterDAD).ulVirusID == (*iterData).ulVirusID)
                        {
                            // Remove this file if it was just scanned
                            iterDAD = m_listDelayedAlertData.erase(iterDAD);
                            continue;
                        }
                        else
                            iterDAD++;
                    }
                }
            }
        }

        CloseHandle(m_hTimers[SIDE_EFFECTS_TIMER_ID]);
        m_hTimers[SIDE_EFFECTS_TIMER_ID] = NULL;
    }
}

#ifdef AP_DISPLAY_VIRUS_ALERTS
bool CVirusAlertQueue::displayVirusAlerts()
{
    // New work to do. Loop until we process all the virus alert or are told
    // to exit.
    //
    ccLib::CMessageLock msgLock ( TRUE, FALSE );

    while ( WAIT_TIMEOUT == msgLock.Lock (m_Terminating.GetHandle(), NULL))
    {
        // Display all the virus alerts
        CVirusAlertPopup* pUI = NULL;
        {
            ccLib::CSingleLock lock (&m_critVirusAlerts, INFINITE, FALSE);
            if ( !m_qpVirusAlerts.empty())
            {
                pUI = m_qpVirusAlerts.front();
                m_qpVirusAlerts.pop();
            }
        } // end lock scope

        // Virus alert to display
        if ( pUI )
        {
            for ( size_t iIndex = 0; iIndex < pUI->vecData.size(); iIndex++)
            {
                try
                {
					SymMCE::ISymMceCmdLoader	mce;

					// All alerts for viral content are informational; therefore we surpress them if the MCE shell is running.
					// We should be a little more careful, and still alert if we were unable to help.
					if (mce.IsMceMaximized())
					{
						CCTRACEI (_T("CVirusAlertQueue::displayAlert - MCE maximized, no alert displayed!"));
                        iIndex = pUI->vecData.size();
						break;
					}

					int iReturn = pUI->DoModal (iIndex);
					if ( iReturn == CVirusAlertPopup::RESULT_IGNORE_CONTAINER )
					{
						CCTRACEI (_T("CVirusAlertQueue::displayAlert - User chose to ignore rest of container"));
                        iIndex = pUI->vecData.size();
						break;
					}
	            }
	            catch(...) // what can this throw??
	            {
		            // Ignore errors.
                    CCTRACEE (_T("CVirusAlertQueue::displayAlert - domodal"));
	            }
            }

            // Destroy the event.
            delete (pUI);
            pUI = NULL;
        }
        else
            return true; // no more alerts
    }//end virus alerts

    return false; // exit!!
}
#endif // AP_DISPLAY_VIRUS_ALERTS

void CVirusAlertQueue::doSideEffectsScan (bool bSilent)
{
    // Build scan task and launch scanner
    AV::AVScanTask_CScanTaskFactory factoryScanTask;
    IScanTaskPtr pTask;

    SYMRESULT result = factoryScanTask.CreateObject(pTask);
    if ( SYM_SUCCEEDED (result))
    {
        ccLib::CSingleLock lock (&m_critVirusAlerts, INFINITE, FALSE);

        // Setup a temporary buffer for ANSI conversion
        std::string strAnsi;

        for ( iterDelayedAlertData iter = m_listDelayedAlertData.begin();
                iter != m_listDelayedAlertData.end ();
                iter++)
        {
            // We don't want Spyware threat cat stuff
            //
            if ( (*iter).strThreatCats.empty())
            {
                // Only scan new files, not scans that are already running.
                //
                if ( (*iter).hScan == INVALID_HANDLE_VALUE )
                {
                    wideStrToAnsiStr ((*iter).strFileName.c_str(), strAnsi );

                    if ( !pTask->AddItem ( typeFileVID, subtypeFile, NULL, strAnsi.c_str(), (*iter).ulVirusID))
                        CCTRACEE ("CVirusAlertQueue::doSideEffectsScan - failed to side-effect AddItem %s", strAnsi.c_str());
                }
            }
        }

        HANDLE hScan = INVALID_HANDLE_VALUE;
        if (!launchScan (pTask, hScan, false, bSilent))
        {
            // Report the failure to the user
            ErrorMessageBox ( IDS_ERR_AP_SPYWARE_SCAN );                            
        }
        else
        {
            // Scan is good, now we must mark all those files as scanning!
            //
            for ( iterDelayedAlertData iter = m_listDelayedAlertData.begin();
                    iter != m_listDelayedAlertData.end ();
                    iter++)
            {
                // We don't want Spyware threat cat stuff
                //
                if ( (*iter).strThreatCats.empty())
                {
                    (*iter).hScan = hScan;
                }
            }

            if ( !m_pScanMonitor.get ())
            {
                m_pScanMonitor = std::auto_ptr<CScanMonitor> (new CScanMonitor(this));

                // Start it.
                if ( !m_pScanMonitor->Create(NULL, 0, 0 ))
                {
                    CCTRACEE (_T("CVirusAlertQueue::doSideEffectsScan - Failed to create the Scan Monitor thread bailing out."));
                    return;
                }
            }

            m_pScanMonitor->AddScan (hScan);
        }
    }
    else
        CCTRACEE ("CVirusAlertQueue::doSideEffectsScan - failed to create scan task factory for side-effects");
}

void CVirusAlertQueue::doSpywareAlert ()
{
#ifdef AP_SPYWARE_SUPPORT
    while (true)
    {
        // Spyware time
        //
        CSpywarePopup spywareUI;

        // lock the file/VID list
        {
            ccLib::CSingleLock lock ( &m_critDelayedAlertData, INFINITE, FALSE );

            // We loop from the beginning each time. If the user "ignores" it
            // we remove it from the list. If they "scan" we launch the whole
            // list.
            //
            spywareUI.ulVirusID = 0;
            
            for ( iterDelayedAlertData iter = m_listDelayedAlertData.begin();
                  iter != m_listDelayedAlertData.end ();
                  iter++)
            {
                // We only want Spyware threat cat stuff that isn't
                // being scanned and isn't ignored.
                //
                if ( !(*iter).strThreatCats.empty() &&
                     INVALID_HANDLE_VALUE == (*iter).hScan &&
                     !isVIDIgnored ((*iter).ulVirusID) )
                {
                    spywareUI.ulVirusID = (*iter).ulVirusID;
                    spywareUI.strThreatCats = (*iter).strThreatCats;
                    spywareUI.strThreatName = (*iter).strThreatName;
                    break;
                }
            }

            // No more spyware alerts
            if ( spywareUI.ulVirusID == 0)
                return;

        } // end lock scope

		SymMCE::ISymMceCmdLoader	mce;
        int iResult = CSpywarePopup::RESULT_IGNORE;

		// Spyware alerts require a decision, so we will ask the user to return to the Desktop (2' UI)
		if (SYM_SUCCEEDED(mce.DoNotAlertToDesktop(CStringW(m_csProductName), SymMCE::MCE_ALERT_CHECK_STATE)))
		{
			CCTRACEI (_T("CVirusAlertQueue::doSpywareAlert - MCE maximized, We need to message the user before we display the dialog!"));
			iResult = CSpywarePopup::RESULT_IGNORE;
		}
		else
		{
			iResult = spywareUI.DoModal ();
		}

        // If the user chose to scan, do a scan for ALL items in the 
        // current list. The user can exclude from the scanner.
        //
        if ( CSpywarePopup::RESULT_SCAN == iResult )
        {
            // Set the spyware scan is running flag so we do not produce anymore spyware alerts
            // until this scan has completed.
            m_bSpywareScanRunning = true;

            // lock the file/VID list
            ccLib::CSingleLock lock ( &m_critDelayedAlertData, INFINITE, FALSE );

            // Build scan task and launch scanner
            AV::AVScanTask_CScanTaskFactory factoryScanTask;
            IScanTaskPtr pTask;
            std::string strAnsi;
            listScanned Scanned; // temp list of files we are going to scan

            SYMRESULT result = factoryScanTask.CreateObject(pTask);
            if ( SYM_SUCCEEDED (result))
            {
                for ( iterDelayedAlertData iter = m_listDelayedAlertData.begin();
                      iter != m_listDelayedAlertData.end ();
                      iter++)
                {
                    // We only want Spyware threat cat stuff that isn't
                    // being scanned.
                    //
                    if (!(*iter).strThreatCats.empty() &&
                        INVALID_HANDLE_VALUE == (*iter).hScan )
                    {
                        // Setup a temporary buffer
                        wideStrToAnsiStr ((*iter).strFileName.c_str(), strAnsi );

                        // Set the item type as either file-VID or compressed file-VID so
                        // the scan manager can determine whether to run a reverse eraser scan
                        // or just scan the container
                        SCANITEMTYPE itemType = typeFileVID;
                        if( (*iter).bCompressed == true )
                            itemType = typeFileVIDCompressed;

                        if ( !pTask->AddItem ( itemType, subtypeFile, NULL, strAnsi.c_str(), (*iter).ulVirusID))
                            CCTRACEE ("CVirusAlertQueue::doSpywareAlert - failed to spyware item %s", strAnsi.c_str());

                        Scanned.push_back (iter);
                    }
                }

                HANDLE hScan = INVALID_HANDLE_VALUE;
                bool bScanResult = false;
                bScanResult = launchScan (pTask, hScan, true);

                if ( bScanResult )
                {
                    // Scan is good, now we must mark all those files as scanning!
                    //
                    for ( iterScanned iterScannedFiles = Scanned.begin();
                          iterScannedFiles != Scanned.end();
                          iterScannedFiles++)
                    { 
                        (**iterScannedFiles).hScan = hScan;
                    }

                    if ( !m_pScanMonitor.get ())
                    {
                        m_pScanMonitor = std::auto_ptr<CScanMonitor> (new CScanMonitor(this));

                        // Start it.
                        if ( !m_pScanMonitor->Create(NULL, 0, 0 ))
                        {
                            CCTRACEE (_T("CVirusAlertQueue::doSpywareAlert - Failed to create the Scan Monitor thread bailing out."));
                            m_bSpywareScanRunning = false;
                            return;
                        }
                    }

                    m_pScanMonitor->AddScan (hScan);
                    return;
                }
                else
                {
                    // Report the failure to the user
                    ErrorMessageBox ( IDS_ERR_AP_SPYWARE_SCAN ); 
                    
                    // Remove this VID from the list and try the next one
                    for ( iterScanned iterScannedFiles = Scanned.begin();
                          iterScannedFiles != Scanned.end();
                          iterScannedFiles++)
                    { 
                        m_listDelayedAlertData.erase(*iterScannedFiles);
                    }
                }
            }
            else
            {
                CCTRACEE ("CVirusAlertQueue::doSpywareAlert - failed to create scan task factory for spyware");
            }
        }
        else if ( CSpywarePopup::RESULT_IGNORE == iResult )
        {
            // Ignore - Add 30-minute exclusion

            // Acquire threat name
            cc::IStringPtr spThreatName = ccSym::CStringImpl::CreateStringImpl(spywareUI.strThreatName.c_str());

            ccLib::CSingleLock lock ( &m_critDelayedAlertData, INFINITE, FALSE );
            if(!spThreatName)
            {
                CCTRACEE(_T("CVirusAlertQueue::doSpywareAlert - Error while "\
                            "repackaging threat name into IString."));
            }
            else
            {

                // Reload manager to avoid duplicates
                if(this->m_spExclusionMgr->reloadState() != NavExclusions::Success)
                {
                    CCTRACEE(_T("CVirusAlertQueue::doSpywareAlert - Error while "\
                                "reloading exclusions."));
                }

                // Add, which will take the greater expiry time of the current
                // exclusion or the argument we pass in IF this is a duplicated
                // add.
                if(this->m_spExclusionMgr->addExclusionAnomaly(spywareUI.ulVirusID, 
                    NULL, time(NULL)+SPYWARE_IGNORED_SECONDS, spThreatName,
                    NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL)
                    == _INVALID_ID)
                {
                    CCTRACEE(_T("CVirusAlertQueue::doSpywareAlert - Error while "\
                                "adding VID exclusion."));
                }
                else
                {
                    // Commit.
                    if(this->m_spExclusionMgr->saveState() != NavExclusions::Success)
                    {
                        CCTRACEW(_T("CVirusAlertQueue::doSpywareAlert - Error "\
                                    "while saving exclusions to CC Settings."));
                    }
                }
            }

            iterDelayedAlertData iter = m_listDelayedAlertData.begin ();

            while ( iter != m_listDelayedAlertData.end () )
            {
                // We only want Spyware threat cat stuff that isn't
                // being scanned.
                //
                if ((*iter).ulVirusID == spywareUI.ulVirusID )
                {
                    // Remove this file from the list
                    iter = m_listDelayedAlertData.erase (iter);
                    continue;
                }
                else
                    iter++;
            }

        }
        else if ( CSpywarePopup::RESULT_EXCLUDE == iResult )
        {
            // Exclude these files from Spyware detection.

            // Temp buffer for threat name
            cc::IStringPtr spThreatName;

            ccLib::CSingleLock lock ( &m_critDelayedAlertData, INFINITE, FALSE );

            // Reload state to avoid dupes
            if(this->m_spExclusionMgr->reloadState() != NavExclusions::Success)
            {
                CCTRACEE(_T("CVirusAlertQueue::doSpywareAlert - Could not load "\
                            "current exclusion state."));
            }

            iterDelayedAlertData iter = m_listDelayedAlertData.begin ();

// Don't do this any more; we're not file based!
//            std::set<std::wstring> setFilesExcluded;

            while ( iter != m_listDelayedAlertData.end () )
            {
                // Exlcude all Spyware files with the
                // same VID as the one the user chose to exclude.
                //
                if ((*iter).ulVirusID == spywareUI.ulVirusID )
                {
                    // Capture the threat name to an IStringPtr
                    spThreatName.Attach(ccSym::CStringImpl::CreateStringImpl(spywareUI.strThreatName.c_str()));
                    if(!spThreatName)
                    {
                        CCTRACEE(_T("CVirusAlertQueue::doSpywareAlert - failed "\
                                    "to acquire IString copy of threat name."));
                        iter++;
                        // funky error
                    }
                    else
                    {
                        // Add the spyware VID exclusion, timeout: forever.
                        // Dupes will result in accepting the stricter set of
                        // criteria (greater state/later expiry).
                        if(this->m_spExclusionMgr->addExclusionAnomaly(
                            spywareUI.ulVirusID, NULL, 0, spThreatName, 
                            NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL)
                            == _INVALID_ID)
                        {
                            CCTRACEW ("CVirusAlertQueue::doSpywareAlert - failed to add threat exclusion file %s", spThreatName);
                        }
/*  NO!!                else
                        {
                            // Add this to our list of excluded file paths
                            setFilesExcluded.insert((*iter).strFileName);
                        } */

                        // Remove this item from the list
                        iter = m_listDelayedAlertData.erase (iter);
                    }
                }
                else
                    iter++;
            }

            // Commit exclusions
            if(this->m_spExclusionMgr->saveState() != NavExclusions::Success)
            {
                CCTRACEW(_T("CVirusAlertQueue::doSpywareAlert - Failed to save "\
                            "new exclusions to CC Settings."));
            }

/*** NO!!
            // Now we have a set of excluded file paths and we want to remove any Spyware
            // items from our alerting list that have the same path since the manual scanner
            // will not detect non-viral threats for these paths anymore
            iter = m_listDelayedAlertData.begin ();
            while ( iter != m_listDelayedAlertData.end () )
            {
                // Is this a Spyware item with the same path as something we've excluded?
                if( !(*iter).strThreatCats.empty() && setFilesExcluded.find((*iter).strFileName) != setFilesExcluded.end() )
                {
                    // Remove this item from the list
                    CCTRACEI ("CVirusAlertQueue::doSpywareAlert - Removing spyware item for threat %ls because it's path was excluded %ls", (*iter).strThreatName.c_str(), (*iter).strFileName.c_str());
                    iter = m_listDelayedAlertData.erase (iter);
                }
                else
                    iter++;
            }
***/
        }
        else
            assert(false);
    }// loop
#endif //AP_SPYWARE_SUPPORT
}

bool CVirusAlertQueue::wideStrToOemStr ( LPCWSTR lpWide /*in*/, std::string& strOem /*out*/ )
{
    // Setup a temporary buffer
    std::vector<char> vOemBuff;
    int iBuffSize = (wcslen (lpWide)*2) + 1;
	vOemBuff.reserve( iBuffSize );

	// Convert the wstring to an OEM string
    if ( WideCharToMultiByte( CP_OEMCP, 0,
                              lpWide, -1,
                              &vOemBuff[0], iBuffSize, NULL, NULL ))
    {
        strOem = &vOemBuff[0];
        return true;
    }

    CCTRACEE ("CVirusAlertQueue::wideStrToOemStr - error converting to OEM %d", ::GetLastError());

    return false;
}

bool CVirusAlertQueue::wideStrToAnsiStr ( LPCWSTR lpWide /*in*/, std::string& strAnsi /*out*/ )
{
    // Setup a temporary buffer
    std::vector<char> vAnsiBuff;
    int iBuffSize = (wcslen (lpWide)*2) + 1;
    vAnsiBuff.reserve( iBuffSize );

    // Convert the wstring to an ANSI string
    if ( WideCharToMultiByte( CP_THREAD_ACP, WC_NO_BEST_FIT_CHARS,
        lpWide, -1,
        &vAnsiBuff[0], iBuffSize, NULL, NULL ))
    {
        strAnsi = &vAnsiBuff[0];

        if( GetFileAttributes(strAnsi.c_str()) == -1 )
        {
            CCTRACEW("CVirusAlertQueue::wideStrToAnsiStr() - GetFileAttributes(%s) failed for the ANSI converted path.", strAnsi.c_str());

            // Try to use the short path name
            CAtlStringW wstrShort;
            if( 0 == GetShortPathNameW(lpWide, wstrShort.GetBuffer(iBuffSize), iBuffSize ) )
            {
                CCTRACEE("CVirusAlertQueue::wideStrToAnsiStr() - GetShortPathName() failed.");
                return false;
            }

            wstrShort.ReleaseBuffer();

            CCTRACEI("CVirusAlertQueue::wideStrToAnsiStr() - GetShortPathNameW returned %ls.", wstrShort.GetString());

            iBuffSize = (wstrShort.GetLength()*2) + 1;
            vAnsiBuff.reserve( iBuffSize );
            if ( WideCharToMultiByte( CP_THREAD_ACP, WC_NO_BEST_FIT_CHARS,
                wstrShort, -1,
                &vAnsiBuff[0], iBuffSize, NULL, NULL ))
            {
                strAnsi = &vAnsiBuff[0];
                if( GetFileAttributes(strAnsi.c_str()) == -1 )
                {
                    CCTRACEE("CVirusAlertQueue::wideStrToAnsiStr() - The SFN ansi path %s is still not valid.", strAnsi.c_str());
                    return false;
                }
                else
                {
                    CCTRACEI("CVirusAlertQueue::wideStrToAnsiStr() - The valid SFN ansi path is %s.", strAnsi.c_str());
                    return true;
                }
            }
            else
            {
                CCTRACEE("CVirusAlertQueue::wideStrToAnsiStr() - WideCharToMultiByte() failed for SFN.");
                return false;
            }
        }

        return true;
    }

    return false;
}

HRESULT CVirusAlertQueue::ErrorMessageBox(long nErrorId)
{
	HRESULT hr = S_OK;
	CString csText;

	try
	{
		CString csFormat;
		csFormat.LoadString(_Module.GetResourceInstance(), nErrorId);

		csText = csFormat;

		// Format mesage with product name
		switch(nErrorId)
		{
		case IDS_ERR_AP_SPYWARE_SCAN:
			csText.Format(csFormat, m_csProductName);
			break;

		default:
			break;
		}

		// Debug Log this error first
		CCTRACEE(csText);

		// Get a NAVError object
		CComBSTR bstrText = csText;
		CComBSTR bstrNAVErr(_T("NAVError.NAVCOMError"));
		CComPtr <INAVCOMError> spNavError;

		// Create, Populate, Log, and Display the error
		hr = spNavError.CoCreateInstance(bstrNAVErr, NULL, CLSCTX_INPROC_SERVER);
        if ( SUCCEEDED(hr))
        {
		    hr = spNavError->put_ModuleID(SAVRT_MODULE_ID_NAVAPSVC);		// Module ID
		    hr = spNavError->put_ErrorID(nErrorId); // Error ID is the resource ID for the error message
		    hr = spNavError->put_Message(bstrText);
		    hr = spNavError->LogAndDisplay(0);
        }
	}
	catch(_com_error& e)
	{
		hr = e.Error();
	}

    if(FAILED(hr) && !csText.IsEmpty())
	{
		// If failed, display the error in the form of win32 message box.
		::MessageBox(NULL, csText, m_csProductName, MB_ICONERROR);	
	}

	return hr;
}

void CVirusAlertQueue::resetTimer (long lTimerID, int iTimeLength)
{
    // 0 = side-effects, 1 = spyware
    if ( !m_hTimers[lTimerID] )
    {
        m_hTimers[lTimerID] = CreateWaitableTimer ( NULL, FALSE, NULL );
    }

    if ( m_hTimers[lTimerID] )
    {
        LARGE_INTEGER liTime;
        liTime.QuadPart = - (iTimeLength * 10000);
        if (!SetWaitableTimer ( m_hTimers[lTimerID], &liTime, 0 /*one time*/, NULL, NULL, FALSE ))
        {
            CCTRACEE ("Failed to set Timer - %d, %d", lTimerID, ::GetLastError());
            m_hTimers[lTimerID] = NULL;
        }
    }
    else
        CCTRACEE ("Failed to create timer - %d, %d", lTimerID, ::GetLastError ());
}

void CVirusAlertQueue::SetDisplayAlert ( bool bDisplayAlert )
{
    m_bDisplayAlert = bDisplayAlert;
}

void CVirusAlertQueue::SetAutoScanSideEffects ( bool bAutoScanSideEffects )
{
    m_bAutoScanSideEffects = bAutoScanSideEffects;
}

// This is identical to checking whether something is excluded
bool CVirusAlertQueue::isVIDIgnored ( unsigned long ulVirusID )
{
    NavExclusions::IExclusion::ExclusionState eState;
    ccLib::CSingleLock lock (&m_critIgnoredVIDs, INFINITE, FALSE );

    // Has this VID been ignored. Has it been less than 30 min?
    //
    if(this->m_spExclusionMgr->reloadState() == NavExclusions::Success)
    {
        eState = this->m_spExclusionMgr->isExcludedAnomaly(ulVirusID, NULL,
            NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL);

        if(eState & NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        CCTRACEE(_T("CVirusAlertQueue::isVIDIgnored - Received error when "\
                    "trying to reload exclusion manager state."));
        return false;
    }
}

// true == new, added to list
bool CVirusAlertQueue::addDelayedAlert (unsigned long ulVirusID,
                                        std::wstring& strFileName,
                                        std::wstring& strThreatName,
                                        std::string&  strThreatCats,
                                        bool bCompressed /*= false*/)
{
    // Don't add items that we are ignoring for 30 minutes or that a currently running
    // manual scan has already performed a scan on
    if ( isVIDIgnored (ulVirusID) || m_ManualScanFiles.FileBeingScanned(strFileName.c_str()))
        return false;

    ccLib::CSingleLock lock (&m_critDelayedAlertData, INFINITE, FALSE );

    for ( iterDelayedAlertData iter = m_listDelayedAlertData.begin();
          iter != m_listDelayedAlertData.end ();
          iter++)
    {
        // Don't add duplicate items
        //
        if ((*iter).ulVirusID == ulVirusID &&
            0 == wcsicmp((*iter).strFileName.c_str(), strFileName.c_str()) )
        {
            // Found it
           
            CCTRACEI ("CVirusAlertQueue::addIfFileVIDNew - DUPLICATE File = %ls VID = %lu", strFileName.c_str(), ulVirusID);
            return false;
        }
    }

    // Can't find it, must be new. Add it.
    //
    DELAYED_ALERT_DATA newData;
    newData.ulVirusID = ulVirusID;
    newData.strFileName = strFileName;
    newData.strThreatCats = strThreatCats;
    newData.strThreatName = strThreatName;
    newData.hScan = INVALID_HANDLE_VALUE;
    newData.bCompressed = bCompressed;

    m_listDelayedAlertData.push_back (newData);
    CCTRACEI ("CVirusAlertQueue::addIfFileVIDNew - NEW File = %ls VID = %ld. There are now %d items.", strFileName.c_str(), ulVirusID, m_listDelayedAlertData.size());

    return true;
}

// This indicates one of the scans with items we are monitoring has
// completed so we can remove it's items
void CVirusAlertQueue::manualScanCompleteEvent (long lScanCookie)
{
    m_ManualScanFiles.ScanCompleted(lScanCookie);
}

// This is to track non-viral files that the manual scan is already working
// with so we do not alert on them (of course this is assuming we haven't
// already alerted).
bool CVirusAlertQueue::manualScanFileEvent (long lScanCookie,
                                            unsigned long ulVirusID,
                                            LPCWSTR cpszwStrFile)
{
    // Exclusive access to the list data
    ccLib::CSingleLock lock (&m_critDelayedAlertData, INFINITE, FALSE );

    // Get the LFN
    CAtlStringW wstrLFN;
    DWORD dwBufLen = MAX_PATH+1;
    DWORD dwRet = GetLongPathNameW(cpszwStrFile, wstrLFN.GetBuffer(dwBufLen), dwBufLen);
    wstrLFN.ReleaseBuffer();

    if( dwRet > dwBufLen )
    {
        // Need to try with a bigger buffer
        dwBufLen = dwRet + 1;
        dwRet = GetLongPathNameW(cpszwStrFile, wstrLFN.GetBuffer(dwBufLen), dwBufLen);
        wstrLFN.ReleaseBuffer();
    }

    if( dwRet == 0 || dwRet > dwBufLen )
    {
        wstrLFN = cpszwStrFile;
    }

    CCTRACEI ("CVirusAlertQueue::manualScanFileEvent - Checking for File = %ls VID = %ld Cookie is = %d.", wstrLFN.GetString(), ulVirusID, lScanCookie);

    // First remove any non-viral items currently in our list since they are already being scanned
    iterDelayedAlertData iter = m_listDelayedAlertData.begin();
    while ( iter != m_listDelayedAlertData.end () )
    {
        // Find any identical non-viral file name items that are not already being scanned
        //
        if( !(*iter).strThreatCats.empty() && (*iter).hScan == INVALID_HANDLE_VALUE )
        {
            int nComparison = _wcsicmp((*iter).strFileName.c_str(), wstrLFN);

            if ( nComparison == 0 )
            {
                //
                // Found it!
                //

                // Remove this item and set the iterator to the next available item for checking
                iter = m_listDelayedAlertData.erase(iter);

                CCTRACEI ("CVirusAlertQueue::manualScanFileEvent - Matched file = %ls. Removed from list and resetting timer. List now contains %d items.", cpszwStrFile, m_listDelayedAlertData.size());

                // Since the manual scan has a hit on a file that we were going to alert for let's reset the spyware
                // timer just in case the same scan comes across more items we are going to alert on. This should
                // reduce the number of duplicate alerts.
                if ( m_hTimers[SIDE_EFFECTS_TIMER_ID] )
                {
                    resetTimer (SPYWARE_TIMER_ID, SPYWARE_TIME);
                }

                continue;
            }
        }

        iter++;
    }

    // Now add this item to our manual scan tracking data class
    m_ManualScanFiles.AddFile(lScanCookie, wstrLFN);

    return true;
}
// Caller must close the hScan handle when they are done with it!
//
bool CVirusAlertQueue::launchScan (IScanTask* pTask, HANDLE& hScan, bool bScanNonVirals, bool bSilent/*false*/)
{
    char szTempFilePath[MAX_PATH] = {0};

    // Launch scanner
	// Get a temp file name.
    DWORD dwError = generateTempScanFile( szTempFilePath, MAX_PATH );
    if(dwError != NOERROR)
    {
        CCTRACEE ("CVirusAlertQueue::launchScan - failed to generateTempScanFile %d", dwError);
		return false;
    }

    pTask->SetPath ( szTempFilePath );
    pTask->SetType ( scanCustom );
    
	if (!pTask->Save())
	{
        CCTRACEE ("CVirusAlertQueue::launchScan - failed to save task %d", ::GetLastError()); 
		DeleteFile ( szTempFilePath );
        return false;
    }

    // Command line is c:\progra~1\norton~1\navw32.exe <sideeffects> /ttask:"c:\...\scanxxxx.sca"
    //
    std::string strExePath;
    std::string strParameters;

    ccLib::CString strNAVDir;
    ccSym::CInstalledApps::GetNAVDirectory (strNAVDir);

    TCHAR szShortNAVDir [MAX_PATH] = {0};
    TCHAR szShortTaskName [MAX_PATH] = {0};

    ::GetShortPathName ( strNAVDir, szShortNAVDir, MAX_PATH );
    ::GetShortPathName ( szTempFilePath, szShortTaskName, MAX_PATH );

    strExePath = szShortNAVDir;
    strExePath += _T("\\navw32.exe");

    // This is strange but the first command line argument will be nuked if it
    // is not preceded by a space
    strParameters = _T(" ");

    // Force enabling of non-viral threat scanning
    if ( bScanNonVirals )
    {
        strParameters += SWITCH_ENABLE_NONVIRAL;
        strParameters += _T(" ");
    }

    if( bSilent )
    {
        strParameters += SWITCH_SILENT;
        strParameters += _T(" ");
    }

    strParameters +=  _T("/ttask:\"");
    strParameters += szShortTaskName;
    strParameters += _T("\"");

    STARTUPINFO rSI;
    PROCESS_INFORMATION rPI;

	ZeroMemory(&rSI, sizeof(rSI));
	rSI.cb = sizeof(rSI);
    ZeroMemory(&rPI, sizeof(rPI));

    DWORD dwCreationFlags = CREATE_NEW_PROCESS_GROUP;

    if ( bSilent )
        dwCreationFlags = dwCreationFlags | IDLE_PRIORITY_CLASS;

    CCTRACEI("CVirusAlertQueue::launchScan() - Executable = %s, Parameters = %s", strExePath.c_str(), strParameters.c_str());

	BOOL bRet = CreateProcess( strExePath.c_str(),
                               (LPSTR)strParameters.c_str(),
                               NULL, NULL, FALSE, 
				               dwCreationFlags, NULL, NULL, &rSI, &rPI);

	if (!bRet)
    {
		CCTRACEE ("CVirusAlertQueue::launchScan() - failed to launch spyware scan");
        hScan = INVALID_HANDLE_VALUE;
        return false;
    }

	CloseHandle(rPI.hThread);
    hScan = rPI.hProcess;
    return true;
}

// GHS: 362840 I am return the reason for the failure if it is NOT ERROR_FILE_EXISTS
DWORD CVirusAlertQueue::generateTempScanFile( LPSTR szOut, int nBufferSize )
{
	char szTempDir[ MAX_PATH ];
	char szTempFile[ 32 ];

    DWORD dwLastError = NOERROR;
	// Fetch temp directory.
	if( 0 == ::GetTempPath(MAX_PATH, szTempDir) )
    {
        dwLastError = ::GetLastError();
        return (dwLastError == NOERROR)?(DWORD)(-1):dwLastError;
    }

	// Keep trying to generate a temp file.
	int iIndex = 0;
	for(;; ++iIndex)
	{
		// Copy directory name to output buffer.
		strncpy( szOut, szTempDir, nBufferSize );

		// Construct file name.  File will be in the form
		// scanxxxx.sca.
		wsprintf( szTempFile, "scan%d.%s", iIndex, szTaskFileExtension );
		strncat( szOut, szTempFile, nBufferSize );

		// Try to create the file.
		HANDLE hFile = CreateFile( szOut, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL );
		if( hFile != INVALID_HANDLE_VALUE )
		{
			// Success.  Close the handle and return.
			CloseHandle( hFile );
			return NOERROR;
		}

		// An error occured.  If the error states that the file
		// already exists, keep processing, else bail out.
        // GHS: 362840 I am return the reason for the failure if it is NOT ERROR_FILE_EXISTS
        DWORD dwLastError = ::GetLastError();
		if( dwLastError != ERROR_FILE_EXISTS )
            return dwLastError;
	}

	// Return error status.
	return NOERROR;
}

//ScanMonitorSink
void CVirusAlertQueue::ScanFinished ( HANDLE hScan )
{
    if ( INVALID_HANDLE_VALUE != hScan )
    {
        ccLib::CSingleLock lock (&m_critDelayedAlertData, INFINITE, FALSE );

        // Search each item and remove it. It's done like this
        // because we are erasing while iterating.
        //
        iterDelayedAlertData iter = m_listDelayedAlertData.begin ();

        while ( iter != m_listDelayedAlertData.end () )
        {
            // Remove items that match the scan which has completed
            //
            if ( hScan == (*iter).hScan )
            {
                CCTRACEI ("CVirusAlertQueue::ScanFinished() - Removing item for file %ls scan handle %d.", (*iter).strFileName.c_str(), (*iter).hScan);

                if( m_bSpywareScanRunning )
                {
                    // If this was the single spyware scan allowed to run reset the value now
                    if( (*iter).strThreatCats.empty() == false )
                    {
                        CCTRACEI ("CVirusAlertQueue::ScanFinished() - The single spyware scan allowed is completed. Allowing new spyware alerts.");
                        m_bSpywareScanRunning = false;
                    }
                }

                // Remove this file if it was just scanned
                iter = m_listDelayedAlertData.erase(iter);
                continue;
            }
            else
                iter++;
        }

        CloseHandle(hScan);
    }
}

bool CVirusAlertQueue::isCfgWizRunning()
{
    // If the event can be opened, CfgWiz is running.
    StahlSoft::CSmartHandle CfgWizEvent;
    CfgWizEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, CfgWiz::SZ_GLOBAL_RUNNING_EVENT);
    if((HANDLE)CfgWizEvent != (HANDLE) NULL)
        return true;

    return false;
}

bool CVirusAlertQueue::isOtherScanRunning()
{
    SYMRESULT sr = SYMERR_UNKNOWN;
    bool bAlreadyRunning = false;

    // SingleInstance interface
    AV::AVScanManager_ISingleInstanceFactory cSingleInstanceFactory;
    ISingleInstancePtr spSingleInstance;

    sr = cSingleInstanceFactory.CreateObject(&spSingleInstance);
    if(SYM_SUCCEEDED(sr) && spSingleInstance)
    {
        DWORD dwOwnerProcess = NULL, dwOwnerThread = NULL;

        HRESULT hr;
        hr = spSingleInstance->AlreadyExists(_T("scanMyComputer"), bAlreadyRunning, dwOwnerProcess, dwOwnerThread);
        if(SUCCEEDED(hr))
        {
            if(bAlreadyRunning)
                return true;
        }
        else
        {
            // If this call failed, it is strange, but non-fatal,
            //  and we should continue running.
            CCTRACEE(_T("CVirusAlertQueue::isOtherScanRunning - ISingleInstance::AlreadyExists() failed. hr=0x%08X. Non fatal, continuing."), hr);
        }

        hr = spSingleInstance->AlreadyExists(_T("scanQuick"), bAlreadyRunning, dwOwnerProcess, dwOwnerThread);
        if(SUCCEEDED(hr))
        {
            if(bAlreadyRunning)
                return true;
        }
        else
        {
            // If this call failed, it is strange, but non-fatal,
            //  and we should continue running.
            CCTRACEE(_T("CVirusAlertQueue::isOtherScanRunning - ISingleInstance::AlreadyExists() failed. hr=0x%08X. Non fatal, continuing."), hr);
        }

    }
    else
    {
        // If this object can't be created, it is weird, but non-fatal.
        CCTRACEE(_T("CVirusAlertQueue::isOtherScanRunning - Unable to create single instance object.  Non fatal, continuing."));
    }

    return false;
}


//
// CManualScannedFiles implementation
//
CManualScannedFiles::CManualScannedFiles()
{
}

CManualScannedFiles::~CManualScannedFiles()
{
}

bool CManualScannedFiles::AddFile(long lCookie, LPCWSTR pcwszFileName)
{
    if( pcwszFileName == NULL )
    {
        CCTRACEE ("CManualScannedFiles::AddFile() - File is invalid");
        return false;
    }

    ccLib::CSingleLock lock (&m_critSec, INFINITE, FALSE );

    // See if this cookie is already in our map
    iterMapScannedFiles IterMap = m_mapFilesBeingScanned.find(lCookie);
    if( IterMap == m_mapFilesBeingScanned.end() )
    {
        // We need to add the cookie to our map
        setFiles setToAdd;

        ccLib::CExceptionInfo exceptionInfo(_T("CManualScannedFiles::AddFile(new cookie)"));
        try
        {
            ATL::CAtlStringW wstrFile = pcwszFileName;
            wstrFile.MakeLower();
            setToAdd.insert(wstrFile);
            m_mapFilesBeingScanned[lCookie] = setToAdd;

            CCTRACEI("CManualScannedFiles::AddFile() - Added NEW ITEM file = %ls cookie = %d to the manual scan files map. Map now contains %d items.", pcwszFileName, lCookie, m_mapFilesBeingScanned.size());
        }
        CCCATCHMEM(exceptionInfo);

        if ( exceptionInfo.IsException() )
        {
            return false;
        }
    }
    else
    {
        // We found this cookie already in our map, just add the file
        ccLib::CExceptionInfo exceptionInfo(_T("CManualScannedFiles::AddFile(existing cookie)"));
        try
        {
            ATL::CAtlStringW wstrFile = pcwszFileName;
            wstrFile.MakeLower();
            (*IterMap).second.insert(wstrFile);

            CCTRACEI("CManualScannedFiles::AddFile() - Added file = %ls to existing cookie set = %d. Set now contains %d files.", pcwszFileName, lCookie, (*IterMap).second.size());
        }
        CCCATCHMEM(exceptionInfo);

        if ( exceptionInfo.IsException() )
        {
            return false;
        }
    }

    return true;
}

bool CManualScannedFiles::FileBeingScanned(LPCWSTR pcwszFileName)
{
    ccLib::CSingleLock lock (&m_critSec, INFINITE, FALSE );

    if( m_mapFilesBeingScanned.empty() )
    {
        CCTRACEI(_T("CManualScannedFiles::FileBeingScanned() - Nothing in the map to check."));
        return false;
    }

    // Wrap with a CAtlString
    ATL::CAtlStringW wstrFile;

    ccLib::CExceptionInfo exceptionInfo(_T("CManualScannedFiles::FileBeingScanned()"));
    try
    {
        wstrFile = pcwszFileName;
        wstrFile.MakeLower();
    }
    CCCATCHMEM(exceptionInfo);

    if ( exceptionInfo.IsException() )
    {
        return false;
    }

    // Check in every set we have in our map
    for( iterMapScannedFiles IterMap = m_mapFilesBeingScanned.begin(); IterMap != m_mapFilesBeingScanned.end(); IterMap++ )
    {
        // Check for the file in this set
        if( (*IterMap).second.find(wstrFile) != (*IterMap).second.end() )
        {
            CCTRACEI(_T("CManualScannedFiles::FileBeingScanned() - File %ls is being scanned by scanner with cookie %d."), wstrFile.GetString(), (*IterMap).first );
            return true;
        }
    }

    return false;
}

void CManualScannedFiles::ScanCompleted(long lCookie)
{
    ccLib::CSingleLock lock (&m_critSec, INFINITE, FALSE );

    // Find the cookie in our map
    iterMapScannedFiles IterMap = m_mapFilesBeingScanned.find(lCookie);

    if( IterMap != m_mapFilesBeingScanned.end() )
    {
        // Remove it
        m_mapFilesBeingScanned.erase(IterMap);
        CCTRACEI(_T("CManualScannedFiles::ScanCompleted() - Removed files from scan map with cookie %d. Map now contains %d file sets."), lCookie, m_mapFilesBeingScanned.size());
    }
    else
    {
        CCTRACEW(_T("CManualScannedFiles::ScanCompleted() - No files from scan cookie %d"), lCookie);
    }
}
