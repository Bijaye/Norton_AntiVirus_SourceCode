////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "eventdescriptions.h"
#include "AllNAVEvents.h"
#include "AVccModuleId.h"

#define THREAT_PACKET_VERSION 1
#define SEMISYNC_PACKET_VERSION 1

// Statics
long CEventDescriptions::m_lSessionID = 0;
long CEventDescriptions::m_lVirusID = 1001;
ccLib::CString CEventDescriptions::m_strVirusName = "Test.Virus";
ccLib::CString CEventDescriptions::m_strSpywareName = "Ad-Master";

CEventDescriptions::CEventDescriptions(void)
{
    m_vecEventTypes.resize ( LAST_EVENT);
    m_vecEventTypes[FileVirus] = "File - Virus";
    m_vecEventTypes[Spyware] = "File - Spyware";
    m_vecEventTypes[Mixed] = "File - Mixed";
    m_vecEventTypes[VirusLike] = "File - Virus-like";
//  m_vecEventTypes[ScriptBlocking] = "File - Script Blocking"; // took out for NAV 2006
    m_vecEventTypes[ContainerVirus] = "Container - Virus";
    m_vecEventTypes[ContainerSpyware] = "Container - Spyware";
    m_vecEventTypes[ContainerMixed] = "Container - Mixed";
    m_vecEventTypes[BRVirus] = "BR - Virus";
    m_vecEventTypes[MBRVirus] = "MBR - Virus";
    m_vecEventTypes[Scan] = "Scan - completed";
    m_vecEventTypes[Error] = "Error";
}

CEventDescriptions::~CEventDescriptions(void)
{
}

bool CEventDescriptions::MakeEvent ( CEventData& ReturnEvent /*out*/,
                                    long lInternalEventID /*in*/,
                                    LPCSTR szCustomText /*in*/)
{
    ReturnEvent.SetData ( AV::Event_Base_propSessionID, m_lSessionID );

    // What version of our packet are we sending?
    ReturnEvent.SetData ( AV::Event_Base_propVersion, THREAT_PACKET_VERSION );

    // Threat?
    //
    if ( Scan != lInternalEventID || 
         Error != lInternalEventID )
    {
        // *** Virus event
        //
        // ************************************************
        // BASE DATA
        //
        ReturnEvent.SetData ( AV::Event_Base_propType, AV::Event_ID_Threat );

        // The event is from us
        ReturnEvent.SetData ( AV::Event_Threat_propFeature, SAVRT_MODULE_ID_NAVAPSVC ); // see AVccModuleId.h

        ReturnEvent.SetData ( AV::Event_Threat_propDefsRevision, "032520030001");
        ReturnEvent.SetData ( AV::Event_Threat_propProductVersion, "11.5.0.999");

        //Event_Threat_propProcessID,         // L - Process ID that wrote the file (optional)
    }

    // sub type
    if ( FileVirus == lInternalEventID || 
         Spyware == lInternalEventID ||
         Mixed == lInternalEventID )
    {
        // threat event type
        ReturnEvent.SetData ( AV::Event_Threat_propEventSubType, AV::Event_Threat );
    }

    if ( ContainerVirus == lInternalEventID ||
         ContainerSpyware == lInternalEventID ||
         ContainerMixed == lInternalEventID )
        ReturnEvent.SetData ( AV::Event_Threat_propEventSubType, AV::Event_Threat_Container );

// *** FileVirus specific
//
    if ( FileVirus == lInternalEventID )
    {
        // file type
        ReturnEvent.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File  );

        // ************************************************
        // SUBFILE DATA
        //
        CEventData SubFile;

        if ( szCustomText && NULL != szCustomText[0] )
            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, szCustomText );
        else
            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, "c:\\test\\testvirus.exe" );

        //SubFile.SetData ( AV::Event_ThreatSubFile_propThreatEntryCount, 1);

        // ************************************************
        // THREAT DATA
        //
        CEventData AllThreats;

        // Make a temp threat
        CEventData threat;

        // Virus name and ID of the first entry
        //
        threat.SetData ( AV::Event_ThreatEntry_propVirusID, m_lVirusID );
        threat.SetData ( AV::Event_ThreatEntry_propVirusName, (LPCSTR) m_strVirusName );

        BYTE byAction = AV::Event_Action_Repaired;
        threat.SetData ( AV::Event_ThreatEntry_propActionCount, 1 );
        threat.SetData ( AV::Event_ThreatEntry_propActionData, &byAction, 1 );

        AllThreats.SetNode ( 0, threat );

        SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);

        CEventData AllFiles;
        AllFiles.SetNode ( 0, SubFile );
        ReturnEvent.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );

        return true;
    }

// *** Spyware event
//
    if ( Spyware == lInternalEventID )
    {
        // file type
        ReturnEvent.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File  );

        // ************************************************
        // SUBFILE DATA
        //
        CEventData SubFile;

        if ( szCustomText && NULL != szCustomText[0] )
            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, szCustomText );
        else
            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, "c:\\test\\admaster.exe" );

        // ************************************************
        // THREAT DATA
        //
        CEventData AllThreats;

        // Make a temp threat
        CEventData threat;

        // Virus name and ID of the first entry
        //
        threat.SetData ( AV::Event_ThreatEntry_propVirusID, m_lVirusID );
        threat.SetData ( AV::Event_ThreatEntry_propVirusName, (LPCSTR) m_strSpywareName );

        BYTE byAction = AV::Event_Action_Repaired;
        threat.SetData ( AV::Event_ThreatEntry_propActionCount, 1 );
        threat.SetData ( AV::Event_ThreatEntry_propActionData, &byAction, 1 );

        ccLib::CString strThreatCats;
        strThreatCats = "5 9";
        threat.SetData ( AV::Event_ThreatEntry_propThreatCatCount, 2 );
        threat.SetData ( AV::Event_ThreatEntry_propThreatCategories, (LPCSTR) strThreatCats );

        AllThreats.SetNode ( 0, threat );

        SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);

        CEventData AllFiles;
        AllFiles.SetNode ( 0, SubFile );
        ReturnEvent.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );

        return true;
    }
// *** File - mixed
//
    if ( Mixed == lInternalEventID )
    {
        // file type
        ReturnEvent.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File  );

        // ************************************************
        // SUBFILE DATA
        //
        CEventData SubFile;

        if ( szCustomText && NULL != szCustomText[0] )
            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, szCustomText );
        else
            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, "c:\\test\\admaster.exe" );

        // ************************************************
        // THREAT DATA
        //
        CEventData AllThreats;

        // Make a temp threat
        CEventData threatVirus;

        // Virus name and ID of the first entry
        //
        threatVirus.SetData ( AV::Event_ThreatEntry_propVirusID, m_lVirusID );
        threatVirus.SetData ( AV::Event_ThreatEntry_propVirusName, (LPCSTR) m_strVirusName );

        BYTE byAction = AV::Event_Action_Repaired;
        threatVirus.SetData ( AV::Event_ThreatEntry_propActionCount, 1 );
        threatVirus.SetData ( AV::Event_ThreatEntry_propActionData, &byAction, 1 );

        AllThreats.SetNode ( 0, threatVirus );

        // Make a temp threat
        CEventData threatSpyware;

        // Virus name and ID of the second entry
        //
        threatSpyware.SetData ( AV::Event_ThreatEntry_propVirusID, m_lVirusID );
        threatSpyware.SetData ( AV::Event_ThreatEntry_propVirusName, (LPCSTR) m_strSpywareName );

        byAction = AV::Event_Action_NoActionTaken;
        threatSpyware.SetData ( AV::Event_ThreatEntry_propActionCount, 1 );
        threatSpyware.SetData ( AV::Event_ThreatEntry_propActionData, &byAction, 1 );

        ccLib::CString strThreatCats;
        strThreatCats = "5 9";
        threatSpyware.SetData ( AV::Event_ThreatEntry_propThreatCatCount, 2 );
        threatSpyware.SetData ( AV::Event_ThreatEntry_propThreatCategories, (LPCSTR) strThreatCats );

        AllThreats.SetNode ( 1, threatSpyware );

        SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);

        CEventData AllFiles;
        AllFiles.SetNode ( 0, SubFile );
        ReturnEvent.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );

        return true;
    }

// *** File - Script Blocking
//
/* // Deleted for NAV 2006, no more script blocking feature

	if ( ScriptBlocking == lInternalEventID )
    {
        ReturnEvent.SetData ( AV::Event_Threat_propFeature, AV_MODULE_ID_SCRIPT_BLOCKING_UI ); // see AVccModuleId.h
        ReturnEvent.SetData ( AV::Event_Threat_propEventSubType, AV::Event_ScriptBlocking );

        // file type
        ReturnEvent.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File  );

        // ************************************************
        // SUBFILE DATA
        //
        CEventData SubFile;

        if ( szCustomText && NULL != szCustomText[0] )
            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, szCustomText );
        else
            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, "c:\\test\\testscript.js" );

        // ************************************************
        // THREAT DATA
        //
        CEventData AllThreats;

        // Make a temp threat
        CEventData threat;

        threat.SetData ( AV::Event_ThreatEntry_propObject, "FileObject" );
        threat.SetData ( AV::Event_ThreatEntry_propMethod, "DeleteEverything()" );

        BYTE byAction = AV::Event_Action_Allowed_Method;
        threat.SetData ( AV::Event_ThreatEntry_propActionCount, 1 );
        threat.SetData ( AV::Event_ThreatEntry_propActionData, &byAction, 1 );

        AllThreats.SetNode ( 0, threat );

        SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);

        CEventData AllFiles;
        AllFiles.SetNode ( 0, SubFile );
        ReturnEvent.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );

        return true;
    }
*/
// *** Virus-like event
//
    if ( VirusLike == lInternalEventID )
    {
        ReturnEvent.SetData ( AV::Event_Threat_propFeature, SAVRT_MODULE_ID_NAVAPSVC ); // see AVccModuleId.h
        ReturnEvent.SetData ( AV::Event_Threat_propEventSubType, AV::Event_VirusLike );

        // file type
        ReturnEvent.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File  );

        // ************************************************
        // SUBFILE DATA
        //
        CEventData SubFile;

        if ( szCustomText && NULL != szCustomText[0] )
            SubFile.SetData ( AV::Event_ThreatSubFile_propApplicationPath, szCustomText );
        else
            SubFile.SetData ( AV::Event_ThreatSubFile_propApplicationPath, "c:\\test\\malicious.exe" );

        // Target
        SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, "C:" );

        // ************************************************
        // THREAT DATA
        //
        CEventData AllThreats;

        // Make a temp threat
        CEventData threat;

        BYTE byAction = AV::Event_Action_Blocked;
        threat.SetData ( AV::Event_ThreatEntry_propActionCount, 1 );
        threat.SetData ( AV::Event_ThreatEntry_propActionData, &byAction, 1 );

        threat.SetData ( AV::Event_ThreatEntry_propAttempted_Action, AV::Event_VirusLike_Attempted_Action_HD_MBR_Write );

        AllThreats.SetNode ( 0, threat );

        SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);

        CEventData AllFiles;
        AllFiles.SetNode ( 0, SubFile );
        ReturnEvent.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );

        return true;
    }

// *** BR and MBR Virus specific
//
    if ( BRVirus == lInternalEventID || MBRVirus == lInternalEventID )
    {
        // file type
        if ( BRVirus == lInternalEventID )
            ReturnEvent.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_BootRecord  );
        else
            ReturnEvent.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_MasterBootRecord );

        // ************************************************
        // SUBFILE DATA
        //
        CEventData SubFile;

        if ( szCustomText && NULL != szCustomText[0] )
            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, szCustomText );
        else
            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, "c:\\" );

        // ************************************************
        // THREAT DATA
        //
        CEventData AllThreats;

        // Make a temp threat
        CEventData threat;

        // Virus name and ID of the first entry
        //
        threat.SetData ( AV::Event_ThreatEntry_propVirusID, m_lVirusID );
        threat.SetData ( AV::Event_ThreatEntry_propVirusName, (LPCSTR) m_strVirusName );

        BYTE byAction = AV::Event_Action_Repaired;
        threat.SetData ( AV::Event_ThreatEntry_propActionCount, 1 );
        threat.SetData ( AV::Event_ThreatEntry_propActionData, &byAction, 1 );

        AllThreats.SetNode ( 0, threat );

        SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);

        CEventData AllFiles;
        AllFiles.SetNode ( 0, SubFile );
        ReturnEvent.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );

        return true;
    }

// *** Container virus event
//
    if ( ContainerVirus == lInternalEventID )
    {
        // file type
        ReturnEvent.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File  );
        //ReturnEvent.SetData ( AV::Event_Threat_propSubFileCount, 2 ); 
        
        // file 0 = parent container, file 1 = infected file
        CEventData AllFiles;

        // Do the parent file action (whole container was deleted, etc.)
        {
            CEventData parentfile;

            // File name - we only care about the parent (above) and the final file name here.
            //
            if ( szCustomText && NULL != szCustomText[0] )
                parentfile.SetData ( AV::Event_ThreatSubFile_propFileName, szCustomText );
            else
                parentfile.SetData ( AV::Event_ThreatSubFile_propFileName, "c:\\test\\container.zip" );

            CEventData eventThreat;

            std::vector <BYTE> vecParentAction;
            vecParentAction.push_back ( AV::Event_Action_Repaired );
            eventThreat.SetData ( AV::Event_ThreatEntry_propActionData, &vecParentAction[0], vecParentAction.size () );
            eventThreat.SetData ( AV::Event_ThreatEntry_propActionCount, vecParentAction.size () );
            
            // Save
            CEventData AllThreats;
            AllThreats.SetNode (0, eventThreat);
            //parentfile.SetData ( AV::Event_ThreatSubFile_propThreatEntryCount, 1 );
            parentfile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats); 
            AllFiles.SetNode ( 0, parentfile );
        } // end parent file

        // ************************************************
        // SUBFILE DATA
        //
        for ( long lFileIndex = 1; lFileIndex < 3 /*# files+1(parent)*/; lFileIndex++)
        {
            CEventData SubFile;
            std::string strFileName;
            char szFileFormat [] = "testvirus%d.exe";
            char szFileName [MAX_PATH] = {0};
            sprintf ( szFileName, szFileFormat, lFileIndex );

            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, szFileName );
            //SubFile.SetData ( AV::Event_ThreatSubFile_propThreatEntryCount, 1);

            // Description = container name (from AP)
            std::string strFileDesc;
            strFileDesc = "container.zip\\";
            strFileDesc += szFileName;
            SubFile.SetData ( AV::Event_ThreatSubFile_propFileDescription, strFileDesc.c_str() );

            // ************************************************
            // THREAT DATA
            //
            CEventData AllThreats;

            // Make a temp threat
            CEventData threat;

            // Virus name and ID of the first entry
            //
            threat.SetData ( AV::Event_ThreatEntry_propVirusID, m_lVirusID );
            threat.SetData ( AV::Event_ThreatEntry_propVirusName, (LPCTSTR) m_strVirusName );

            std::vector <BYTE> vecAction;
            vecAction.push_back ( AV::Event_Action_RepairFailed );
            vecAction.push_back ( AV::Event_Action_Quarantined );
            threat.SetData ( AV::Event_ThreatEntry_propActionData, &vecAction[0], vecAction.size () );
            threat.SetData ( AV::Event_ThreatEntry_propActionCount, vecAction.size());

            AllThreats.SetNode ( 0, threat );

            SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);
            AllFiles.SetNode ( lFileIndex, SubFile ); // subfile (parent=0)
        }

        //ReturnEvent.SetData ( AV::Event_Threat_propSubFileCount, 1 );
        ReturnEvent.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );

        return true;
    }
// *** Container Spyware
//
    if ( ContainerSpyware == lInternalEventID )
    {
        // file type
        ReturnEvent.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File  );
        
        // file 0 = parent container, file 1 = infected file
        CEventData AllFiles;

        // Do the parent file action (whole container was deleted, etc.)
        {
            CEventData parentfile;

            // File name - we only care about the parent (above) and the final file name here.
            //
            if ( szCustomText && NULL != szCustomText[0] )
                parentfile.SetData ( AV::Event_ThreatSubFile_propFileName, szCustomText );
            else
                parentfile.SetData ( AV::Event_ThreatSubFile_propFileName, "c:\\test\\container.zip" );

            CEventData eventThreat;

            std::vector <BYTE> vecParentAction;
            vecParentAction.push_back ( AV::Event_Action_Repaired );
            eventThreat.SetData ( AV::Event_ThreatEntry_propActionData, &vecParentAction[0], vecParentAction.size () );
            eventThreat.SetData ( AV::Event_ThreatEntry_propActionCount, vecParentAction.size () );
            
            // Save
            CEventData AllThreats;
            AllThreats.SetNode (0, eventThreat);
            parentfile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats); 
            AllFiles.SetNode ( 0, parentfile );
        } // end parent file

        // ************************************************
        // SUBFILE DATA
        //
        for ( long lFileIndex = 1; lFileIndex < 3 /*# files+1(parent)*/; lFileIndex++)
        {
            CEventData SubFile;
            std::string strFileName;
            char szFileFormat [] = "admaster%d.exe";
            char szFileName [MAX_PATH] = {0};
            sprintf ( szFileName, szFileFormat, lFileIndex );

            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, szFileName );

            // Description = container name (from AP)
            std::string strFileDesc;
            strFileDesc = "container.zip\\";
            strFileDesc += szFileName;
			SubFile.SetData ( AV::Event_ThreatSubFile_propFileDescription, (strFileDesc.c_str()) );

            // ************************************************
            // THREAT DATA
            //
            CEventData AllThreats;

            // Make a temp threat
            CEventData threat;

            // Virus name and ID of the first entry
            //
            threat.SetData ( AV::Event_ThreatEntry_propVirusID, m_lVirusID );
            threat.SetData ( AV::Event_ThreatEntry_propVirusName, (LPCSTR) m_strSpywareName );

            std::vector <BYTE> vecAction;
            vecAction.push_back ( AV::Event_Action_RepairFailed );
            vecAction.push_back ( AV::Event_Action_Quarantined );
            threat.SetData ( AV::Event_ThreatEntry_propActionData, &vecAction[0], vecAction.size () );
            threat.SetData ( AV::Event_ThreatEntry_propActionCount, vecAction.size());

            ccLib::CString strThreatCats;
            strThreatCats = "5 9";
            threat.SetData ( AV::Event_ThreatEntry_propThreatCatCount, 2 );
            threat.SetData ( AV::Event_ThreatEntry_propThreatCategories, (LPCSTR) strThreatCats );

            AllThreats.SetNode ( 0, threat );

            SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);
            AllFiles.SetNode ( lFileIndex, SubFile ); // subfile (parent=0)
        }

        //ReturnEvent.SetData ( AV::Event_Threat_propSubFileCount, 1 );
        ReturnEvent.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );

        return true;
    }
// *** Container - Mixed
//
    if ( ContainerMixed == lInternalEventID )
    {
        // file type
        ReturnEvent.SetData ( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File  );
        
        // file 0 = parent container, file 1 = infected file
        CEventData AllFiles;

        // Do the parent file action (whole container was deleted, etc.)
        {
            CEventData parentfile;

            // File name - we only care about the parent (above) and the final file name here.
            //
            if ( szCustomText && NULL != szCustomText[0] )
                parentfile.SetData ( AV::Event_ThreatSubFile_propFileName, szCustomText );
            else
                parentfile.SetData ( AV::Event_ThreatSubFile_propFileName, "c:\\test\\container.zip" );

            CEventData eventThreat;

            std::vector <BYTE> vecParentAction;
            vecParentAction.push_back ( AV::Event_Action_Repaired );
            eventThreat.SetData ( AV::Event_ThreatEntry_propActionData, &vecParentAction[0], vecParentAction.size () );
            eventThreat.SetData ( AV::Event_ThreatEntry_propActionCount, vecParentAction.size () );
            
            // Save
            CEventData AllThreats;
            AllThreats.SetNode (0, eventThreat);
            parentfile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats); 
            AllFiles.SetNode ( 0, parentfile );
        } // end parent file

        // ************************************************
        // SUBFILE DATA
        //
        for ( long lFileIndex = 1; lFileIndex < 3 /*# files+1(parent)*/; lFileIndex++)
        {
            CEventData SubFile;
            std::string strFileName;
            char szFileFormat [] = "admaster%d.exe";
            char szFileName [MAX_PATH] = {0};
            sprintf ( szFileName, szFileFormat, lFileIndex );

            SubFile.SetData ( AV::Event_ThreatSubFile_propFileName, szFileName );

            // Description = container name (from AP)
            std::string strFileDesc;
            strFileDesc = "container.zip\\";
            strFileDesc += szFileName;
            SubFile.SetData ( AV::Event_ThreatSubFile_propFileDescription,  strFileDesc.c_str() );

            // ************************************************
            // THREAT DATA
            //
            CEventData AllThreats;

            // Make a temp threat
            CEventData threatVirus;

            // Virus name and ID of the first entry
            //
            threatVirus.SetData ( AV::Event_ThreatEntry_propVirusID, m_lVirusID );
            threatVirus.SetData ( AV::Event_ThreatEntry_propVirusName, (LPCSTR) m_strVirusName );

            std::vector <BYTE> vecAction;
            vecAction.push_back ( AV::Event_Action_RepairFailed );
            vecAction.push_back ( AV::Event_Action_Quarantined );
            threatVirus.SetData ( AV::Event_ThreatEntry_propActionData, &vecAction[0], vecAction.size () );
            threatVirus.SetData ( AV::Event_ThreatEntry_propActionCount, vecAction.size());

            AllThreats.SetNode ( 0, threatVirus );

            // Make a temp threat
            CEventData threatSpyware;

            // Virus name and ID of the second entry
            //
            threatSpyware.SetData ( AV::Event_ThreatEntry_propVirusID, m_lVirusID );
            threatSpyware.SetData ( AV::Event_ThreatEntry_propVirusName, (LPCSTR) m_strSpywareName );

            vecAction.clear();
            vecAction.push_back ( AV::Event_Action_NoActionTaken );
            threatSpyware.SetData ( AV::Event_ThreatEntry_propActionData, &vecAction[0], vecAction.size () );
            threatSpyware.SetData ( AV::Event_ThreatEntry_propActionCount, vecAction.size());

            ccLib::CString strThreatCats;
            strThreatCats = "5 9";
            threatSpyware.SetData ( AV::Event_ThreatEntry_propThreatCatCount, 2 );
            threatSpyware.SetData ( AV::Event_ThreatEntry_propThreatCategories, (LPCSTR) strThreatCats );

            AllThreats.SetNode ( 1, threatSpyware );

            SubFile.SetNode ( AV::Event_ThreatSubFile_propThreatData, AllThreats);
            AllFiles.SetNode ( lFileIndex, SubFile ); // subfile (parent=0)
        }

        //ReturnEvent.SetData ( AV::Event_Threat_propSubFileCount, 1 );
        ReturnEvent.SetNode ( AV::Event_Threat_propSubFileData, AllFiles );

        return true;
    }

// *** Error event
//
    if ( Error == lInternalEventID )
    {
        ReturnEvent.SetData ( AV::Event_Base_propType, AV::Event_ID_Error );
        ReturnEvent.SetData ( AV::Event_Error_propErrorID, 1001 );
        ReturnEvent.SetData ( AV::Event_Error_propModuleID, AV_MODULE_ID_MAIN_UI );
        ReturnEvent.SetData ( AV::Event_Error_propResult, E_FAIL );
        if ( '\0' != szCustomText[0] )
            ReturnEvent.SetData ( AV::Event_Error_propMessage, szCustomText );
        else
            ReturnEvent.SetData ( AV::Event_Error_propMessage, "You should really reinstall or something" );
        ReturnEvent.SetData ( AV::Event_Error_propProductVersion, "11.0.5.99");

        return true;
    }

    if ( Scan == lInternalEventID )
    {
        ReturnEvent.SetData ( AV::Event_Base_propType, AV::Event_ID_ScanAction );

        ReturnEvent.SetData ( AV::Event_ScanAction_propAction, AV::Event_ScanAction_ScanCompleted );         // L - Scan started, stopped, cancelled ...
        ReturnEvent.SetData ( AV::Event_ScanAction_propFeature, AV_MODULE_ID_NAVW );       // L - AutoProtect, Office scanner (use ccModuleId.h)
        if ( '\0' != szCustomText[0] )
            ReturnEvent.SetData ( AV::Event_ScanAction_propTaskName, szCustomText);
        else
            ReturnEvent.SetData ( AV::Event_ScanAction_propTaskName, "My Computer");      // S - My Computer, custom name, A:, ...
	
	    // The follwing DWORD properties are only present 
	    // if Event_ScanAction_propAction == Event_ScanAction_ScanCompleted.
	    ReturnEvent.SetData ( AV::Event_ScanAction_MBRsRepaired, 1 );       
	    ReturnEvent.SetData ( AV::Event_ScanAction_MBRsTotalInfected, 1 );
        ReturnEvent.SetData ( AV::Event_ScanAction_MBRsTotalScanned, 1 );
	    ReturnEvent.SetData ( AV::Event_ScanAction_BootRecsRepaired, 2);    
	    ReturnEvent.SetData ( AV::Event_ScanAction_BootRecsTotalInfected, 2);
 	    ReturnEvent.SetData ( AV::Event_ScanAction_BootRecsTotalScanned, 2);
	    ReturnEvent.SetData ( AV::Event_ScanAction_FilesRepaired, 5 );       
	    ReturnEvent.SetData ( AV::Event_ScanAction_FilesQuarantined, 4 );    
	    ReturnEvent.SetData ( AV::Event_ScanAction_FilesDeleted, 1 );        
	    ReturnEvent.SetData ( AV::Event_ScanAction_FilesTotalInfected, 10);
	    ReturnEvent.SetData ( AV::Event_ScanAction_FilesTotalScanned, 123456 );
        ReturnEvent.SetData ( AV::Event_ScanAction_FilesExcluded, 5 );       
        //ReturnEvent.SetData ( AV::Event_ScanAction_Display             // S - For memory effects - Process termination text

        return true;
    }
/*
    // We can recycle these variables if we are careful
    CEventProperties props; 
    CEventTypes type;

    // *** Virus alerts
    //
    type.lEventType = AV_Event_ID_VirusAlert;
    type.strEventDesc = _T("Virus Alert");
    type.lCustomTextIndex = AV_Event_VirusAlert_propFilePath;
    
    // Virus alert sub type
    //
    props.strPropDesc = _T("Virus Alert Type");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = AV_Event_VirusAlert;             
    props.strDefaultValue = _T("");
    type.m_mapEventProps[AV_Event_VirusAlert_propEventType] = props;

    // Virus ID
    props.strPropDesc = _T("Virus ID");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = 1001;             // pick a virus
    props.strDefaultValue = _T("");
    type.m_mapEventProps[AV_Event_VirusAlert_propVirusID] = props;

    // Virus Name
    props.strPropDesc = _T("Virus Name");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0;
    props.strDefaultValue = _T("Test.Virus");
    type.m_mapEventProps[AV_Event_VirusAlert_propVirusName] = props;

    // Action
    props.strPropDesc = _T("Action");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = AV_Event_Action_Infected;
    props.strDefaultValue = _T("");
    type.m_mapEventProps[AV_Event_VirusAlert_propAction] = props;

    // File path
    props.strPropDesc = _T("File Path");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0;
    props.strDefaultValue = _T("c:\\test\\testvirus.exe");
    type.m_mapEventProps[AV_Event_VirusAlert_propFilePath] = props;

    // Feature
    props.strPropDesc = _T("Feature");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = AV_MODULE_ID_NAVW; // see AVccModuleId.h
    props.strDefaultValue = _T("");
    type.m_mapEventProps[AV_Event_VirusAlert_propFeature] = props;

    // Type
    props.strPropDesc = _T("Type");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = AV_Event_VirusAlert_Type_File;
    props.strDefaultValue = _T("");
    type.m_mapEventProps[AV_Event_VirusAlert_propType] = props;

    // File description
    props.strPropDesc = _T("File description");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0;
    props.strDefaultValue = _T("This file was infected or something");      //Canidate for test data
    type.m_mapEventProps[AV_Event_VirusAlert_propFileDescription] = props;

    // Defs revision
    props.strPropDesc = _T("Defs revision");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0;
    props.strDefaultValue = _T("032520030001");  // MMDDYYRRRR (RRRR = revision #)
    type.m_mapEventProps[AV_Event_VirusAlert_propDefsRevision] = props;

    // Threat Category
    props.strPropDesc = _T("Threat categories");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0; 
    props.strDefaultValue = _T("0");  // SCAN_THREAT_TYPE_VIRUS - see navscan.h
    type.m_mapEventProps[AV_Event_VirusAlert_propCategory] = props;

    // Product version
    props.strPropDesc = _T("Product version");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0; 
    props.strDefaultValue = _T("10.0.0.99");
    type.m_mapEventProps[AV_Event_VirusAlert_propProductVersion] = props;

    m_vecEventTypes.push_back(type);
    // *** End Virus alerts

    // *** Virus-like
    //
    type.lEventType = AV_Event_ID_VirusAlert;
    type.strEventDesc = _T("Virus-like activity");
    type.lCustomTextIndex = AV_Event_VirusAlert_propFilePath;

    // Virus alert sub type
    //
    props.strPropDesc = _T("Virus Alert Type");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = AV_Event_VirusLike;             
    props.strDefaultValue = _T("");
    type.m_mapEventProps[AV_Event_VirusAlert_propEventType] = props;

    props.strPropDesc = _T("Attempted action");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = AV_Event_VirusLike_Attempted_Action_HD_MBR_Write;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_VirusLike_Attempted_Action] = props;
    
	props.strPropDesc = _T("Action taken");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = AV_Event_Action_Deleted;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_VirusAlert_propAction] = props;

	props.strPropDesc = _T("File Path");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0;
	props.strDefaultValue = _T("c:\\path\\file.exe");
    type.m_mapEventProps[AV_Event_VirusAlert_propFilePath] = props;

	props.strPropDesc = _T("Application Path");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0;
	props.strDefaultValue = _T("c:\\malicious.exe");
    type.m_mapEventProps[AV_Event_VirusLike_propApplicationPath] = props;

	props.strPropDesc = _T("Feature");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = SAVRT_MODULE_ID_NAVAPSVC; // use ccModuleId.h
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_VirusAlert_propFeature] = props;

	props.strPropDesc = _T("Type");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = AV_Event_VirusAlert_Type_File;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_VirusAlert_propType] = props;
    m_vecEventTypes.push_back(type);
    // *** End Virus-like
    
    // *** Script Blocking
    //
    type.lEventType = AV_Event_ID_VirusAlert;
    type.strEventDesc = _T("Script Blocking");
    type.lCustomTextIndex = AV_Event_VirusAlert_propFilePath;

    // Virus alert sub type
    //
    props.strPropDesc = _T("Virus Alert Type");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = AV_Event_ScriptBlocking;             
    props.strDefaultValue = _T("");
    type.m_mapEventProps[AV_Event_VirusAlert_propEventType] = props;


	props.strPropDesc = _T("Action taken");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = AV_Event_Action_Authorized;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_VirusAlert_propAction] = props;

	props.strPropDesc = _T("File Path");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0;
	props.strDefaultValue = _T("c:\\bad script.js");
    type.m_mapEventProps[AV_Event_VirusAlert_propFilePath] = props;

	props.strPropDesc = _T("Object");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0;
	props.strDefaultValue = _T("FileObject");
    type.m_mapEventProps[AV_Event_SBAction_propObject] = props;
			
	props.strPropDesc = _T("Method");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0;
	props.strDefaultValue = _T("DeleteEverything()");
    type.m_mapEventProps[AV_Event_SBAction_propMethod] = props;

	props.strPropDesc = _T("Product version");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0;
	props.strDefaultValue = _T("10.0.0.99");
    type.m_mapEventProps[AV_Event_SBAction_propProductVersion] = props;
    type.lEventType = AV_Event_ID_VirusAlert;
    m_vecEventTypes.push_back(type);
    // *** End Script Blocking

    // *** Scan activity
    //
    type.lEventType = AV_Event_ID_ScanAction;
    type.strEventDesc = _T("Scan activity");
    type.lCustomTextIndex = AV_Event_ScanAction_propTaskName;

	props.strPropDesc = _T("Action taken");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = AV_Event_ScanAction_ScanCompleted;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_ScanAction_propAction] = props;

	props.strPropDesc = _T("Feature");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = AV_MODULE_ID_NAVW;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_ScanAction_propFeature] = props;

	props.strPropDesc = _T("Task Name");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0;
    props.strDefaultValue = _T("My Computer");
    type.m_mapEventProps[AV_Event_ScanAction_propTaskName] = props;
	
	// The follwing DWORD properties are only present 
	// if AV_Event_ScanAction_propAction == AV_Event_ScanAction_ScanCompleted.
	props.strPropDesc = _T("MBRs Repaired");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = 1;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_ScanAction_MBRsRepaired] = props;         

	props.strPropDesc = _T("MBRs Infected");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = 1;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_ScanAction_MBRsTotalInfected] = props;
	
	props.strPropDesc = _T("MBRs Scanned");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = 1;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_ScanAction_MBRsTotalScanned] = props;

	props.strPropDesc = _T("BRs Repaired");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = 1;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_ScanAction_BootRecsRepaired] = props;

	props.strPropDesc = _T("BRs Infected");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = 1;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_ScanAction_BootRecsTotalInfected] = props;
			
	props.strPropDesc = _T("BRs Scanned");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = 1;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_ScanAction_BootRecsTotalScanned] = props;
				
	props.strPropDesc = _T("Files repaired");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = 1;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_ScanAction_FilesRepaired] = props;
					        
	props.strPropDesc = _T("Files Quarantined");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = 1;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_ScanAction_FilesQuarantined] = props;
						    
	props.strPropDesc = _T("Files deleted");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = 1;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_ScanAction_FilesDeleted] = props;
						        
	props.strPropDesc = _T("Files infected");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = 1;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_ScanAction_FilesTotalInfected] = props;
								
	props.strPropDesc = _T("Files scanned");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = 1;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_ScanAction_FilesTotalScanned] = props;
    m_vecEventTypes.push_back(type);
    // *** End Scan activity

    // *** NAV Error
    //
    type.lEventType = AV_Event_ID_Error;
    type.strEventDesc = _T("NAV Error");
    type.lCustomTextIndex = AV_Event_Error_propMessage;

	props.strPropDesc = _T("Error ID");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = 1001;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_Error_propErrorID] = props;

	props.strPropDesc = _T("Module ID");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = AV_MODULE_ID_MAIN_UI;		// ccModuleid.h
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_Error_propModuleID] = props;

	props.strPropDesc = _T("Error result");
    props.lEventPropType = CEventProperties::propLong;
    props.lDefaultValue = E_FAIL;
    props.strDefaultValue = _T("0");
    type.m_mapEventProps[AV_Event_Error_propResult] = props;

	props.strPropDesc = _T("Message");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0;
    props.strDefaultValue = _T("You should really reinstall or something");
    type.m_mapEventProps[AV_Event_Error_propMessage] = props;

	props.strPropDesc = _T("Product version");
    props.lEventPropType = CEventProperties::propString;
    props.lDefaultValue = 0;
    props.strDefaultValue = _T("10.0.0.99");
    type.m_mapEventProps[AV_Event_Error_propProductVersion] = props;

    m_vecEventTypes.push_back(type);
    // *** End NAV Error*/

    return false;
}
