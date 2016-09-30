// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//-----------------------------------------------------------------------------
// RFile: Implementation of the following classes:
//
//          ValidateScriptToRun
//          ReadScript
//          RunScript
//          CProcessor
//
//
//  Revision History
//
//  Who                     When        What / Why
//
//  Tedn@Thuridion.com      12/12/03    
//                                      Fixed bugs in CProcessor::GetCmdLine which
//                                          prevented application recognizing script 
//                                          files on the command line.
//                                      Added Trace Statements for debugging aid.
//                                      Replaced return type int with enumerated type for 
//                                          ValidateScriptToRun:OpenFile
//                                          ValidateScriptToRun:ValidatePlatform
//                                          ValidateScriptToRun:ValidateProgramName
//                                          ValidateScriptToRun:ReadFile
//                                      Replaced return type int with bool for         
//                                          ValidateScriptToRun:FileExist
//                                      Improved error reporting in CallRunRobot
//             
//  Tedn@Thuridion.com      12/16/03    Created new functions:
//                                          IsSymantecProduct 
//                                          RunScript::CallSendAffirmativeToWindowWithButtonClick
//                                          RunScript::CallSendTextToEditWindow
//										Modified CProcessor::DoWork to allow processing
//											of multiple script files until a script
//											with an existing application is found.
//                                          
//  Tedn@Thuridion.com      12/17/03    Added handling for following new Script Commands:
//                                          SendTextToEditWindow
//                                          SetINIEntry
//                                          SendAffirmationToWindowAfterButtonClick
//                                      Modified RunScript::Run to handle new Script commands.   
//
//  Tedn@Thuridion.com      12/19/03    Added new commands to SyntaxCheck functions to validate
//											number of commands passed on script command line.
//										Trim spaces from CommandLine so spaces are interpreted
//											as script file names.
//										
//  Tedn@Thuridion.com      12/22/03    1)  Echo Script file name to Console so the user knows
//										something is happening.
//										2)  Made check for /MYS on command line un-case sensitive
//										to get rid of compiler warning.
//										3)  Check for /ALL on command line to indicate user desire
//										to process all scripts in current directory.
//  Tedn@Thuridion.com      12/23/03    1) CallRunProgram will now return a unique error code if
//										the uninstall program is not found.  This is used to 
//										echo the result to the console.
//										2)  Fix for detecting /mys switch.
//										3)  Improved console echoing.
//  Tedn@Thuridion.com      12/24/03    Replaced call uninstall calls using WinExec with CreateProcess. 
//                                      Create Process is better because it returns right away and
//										allows processing of dialog prompts before a long delay.
//  Tedn@Thuridion.com      12/30/03    1)  Moved bUninstall switch into member variable from
//										global.  2)  Added support for /IP switch to allow script files
//										to be run against all platforms 3) cleaned up
//										command line switch handling.
//                                      4) Removed ValidateScriptToRun::ValidatePlatform()
//										and replaced with utility function IsPlatFormMatch.
//										5)  Fixed bug where the Platform was not being checked
//										when running single script files.	
//										6)  Added new call to DisplayUsage if no script is
//										specified, but /MYS or /IP switch is.	
//  Tedn@Thuridion.com      12/31/03    Removed extra spaces in command line to fix bug
//										where spaces were being interpreted as a script file.
//
//-----------------------------------------------------------------------------

#include <io.h>
#include  <stdlib.h>
#include "stdafx.h"
#include "rfile.h"
#include "resource.h"
#include "regkey.h"
#include "uirobot.h"
#include "shutdown.h"
#include "errorlog.h"
#include "wndmsg.h"
#include "autorem.h"
#include "uiscript.h"
#include <time.h>
#include "trace.h"
#include "utils.h"
//#define DEBUGANY 1

BOOL IsNT()
{
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&os);
    //SAVASSERT(result != 0);
    return(os.dwPlatformId == VER_PLATFORM_WIN32_NT);

}


//---------------------------------------------------
//return true = platform match
bool IsPlatformMatch(CString strPlatform)
{
    BOOL bIsNT = IsNT();
	strPlatform.MakeUpper();

    if (bIsNT && ((strPlatform == "WINNT") || (strPlatform == "WINXP")))
        return true;
    if (((!bIsNT) && (strPlatform == "WIN95")) || ((!bIsNT) && (strPlatform == "WIN98")))
        return true;

    return false;
}


CString TrimEnds(CString OldString)
//trim spaces off both ends of a string and return as function result
{
    CString NewStr;

    NewStr = OldString;
    NewStr.TrimLeft();
    NewStr.TrimRight();
    return NewStr;
}

//---------------------------------------------------
ValidateScriptToRun::ValidateScriptToRun(CString TPath)
{
    pFile1   = new CStdioFile();
    FileName1 = "";
    bSyntaxError = FALSE;
    bFirstSyntaxError = TRUE;
    ScriptPath = "";
    SecondRun = FALSE;
    Token = 0;
    TokenValue = "";
    ProgramNameIsGood = FALSE;
    IsPlatform = FALSE;
    TempPath = TPath;
    StartLineCount = 0;
}

//---------------------------------------------------
ValidateScriptToRun::~ValidateScriptToRun()
{
    if (pFile1)
    {
        delete pFile1;
        pFile1 = NULL;
    }

}

//---------------------------------------------------
enumValidateRetCode ValidateScriptToRun::OpenFile()
{
    //#START AKENNED
    CString szFullPath("");
    if ( OurFullPath.ReverseFind( '\\' ) != ( OurFullPath.GetLength() - 1 ) )
        szFullPath = OurFullPath + "\\" + FileName1;
    else
        szFullPath = OurFullPath + FileName1;
    //#END AKENNED

    //if (!pFile1->Open(FileName1, CFile::modeRead, &Err)) {
    if ( !pFile1->Open( szFullPath , CFile::modeRead, &Err ) )
    {
        if (!Err.m_cause)
        {
            return VALIDATE_SCRIPT_SUCCESS;
        } else
        {
            Error(IDS_STR1001);
            return VALIDATE_SCRIPT_FAILURE;
        }
    }
    return VALIDATE_SCRIPT_SUCCESS;
}

//---------------------------------------------------
int ValidateScriptToRun::CloseFile()
{
    if (pFile1)
    {
        pFile1->Close();
    }
    return 0;
}

//---------------------------------------------------
int ValidateScriptToRun::IdentifyToken(CString ts)
{
    CString token = ts;
    token.MakeLower();

    if (token == "getuninstallprogramname")
    {
        Token = GETUNINSTALLPROGRAMNAMETOKEN;
        return 0;
    }
    if (token == "getuninstallprogramnamestuffpassword")
    {
        Token = GETUNINSTALLPROGRAMNAMESTUFFPASSWORDTOKEN;
        return 0;
    }
    if (token == "getuninstallshieldprogramname")
    {
        Token = GETUNINSTALLSHIELDPROGRAMNAMETOKEN;
        return 0;
    }
    if (token == "platform")
    {
        Token = PLATFORMTOKEN;
        return 0;
    }
    if (token == "runprogram")
    {
        Token = RUNPROGRAMTOKEN;
        return 0;
    }
    if (token == "setuninstallprogramifregistryexists")
    {
        Token = SETUNINSTALLPROGRAMIFREGISTRYEXISTSTOKEN;
        return 0;
    }

    Token = ERRORTOKEN;
    return -1;
}


//---------------------------------------------------
bool ValidateScriptToRun::FileExist (const CString FileName)
{
    CFileStatus status;
    extern CFile cfile;
    int iLoc = FileName.Find( "MsiExec.exe" );
    if ( iLoc < 0 )
    { 
        if ( CFile::GetStatus((LPCTSTR)FileName, status))
        {  // static function
            return true;
        }
        return false;
    } 
    return false; 
}

//---------------------------------------------------
enumValidateRetCode ValidateScriptToRun::ValidateProgramName()
//read the registry to verify the path and program to run to perform
//the uninstall.  This is the AntiVirus' uninstall program.
//Return 0 = success
//      <1 = on error
{
    unsigned long VLen = 1024;
    char skey[1025];
    int pos, pos2;
    CString ts, ts2, ts3;
    HKEY hKey;

    pos = TokenValue.Find(',');
    if (pos > -1)
    {
        ts = TokenValue.Left(pos);
        ts = TrimEnds(ts);
        ts2 = TokenValue.Right(TokenValue.GetLength() - (pos+1));
        //find the second ','
        pos = ts2.Find(',');
        if (pos > -1)
        {
            ts3 = ts2.Right(ts2.GetLength() - (pos+1));
            ts3 = TrimEnds(ts3);
            ts2 = ts2.Left(pos);
            ts2 = TrimEnds(ts2);

            //This is for comparison only
            ts.MakeUpper();

            if (ts == "HKEY_LOCAL_MACHINE")
            {
                hKey = HKEY_LOCAL_MACHINE;
            } else
            {
                if (ts == "HKEY_CURRENT_USER")
                {
                    hKey = HKEY_CURRENT_USER;
                } else
                {
                    return VALIDATE_SCRIPT_FAILURE;
                }
            }

            //Read the Registry
            int len = ts2.GetLength();
            strcpy(skey, ts2.GetBuffer(len + 1));
            CRegistryKey *RegKey = new CRegistryKey(hKey, (LPCTSTR)skey, FALSE);
            ts2.ReleaseBuffer();
            if (RegKey)
            {
                RegKey->Open();
                skey[0] = 0;
                if (!RegKey->ReadValue(ts3, skey, &VLen))
                {
                    //success
                    ts = skey;
                    //remove any parameters here
                    pos = ts.Find(".");
                    if (pos > -1)
                    {
                        ts2 = ts.Right(ts.GetLength() - (pos+1));
                        pos2 = ts2.Find(" ");
                        if (pos2 > -1)
                        {
                            pos += pos2;
                            ts3 = ts.Left(pos+1);
                            UninstallProgramName = ts3;
                        } else
                        {
                            UninstallProgramName = TrimEnds(ts);
                        }
                    } else
                    {
                        UninstallProgramName = ts;
                    }

                    //AKENNED - Added this for registry key names that are GUIDS
                    pos = ts2.Find("{");
                    if (pos > -1)
                    {
                        ProgramNameIsGood = TRUE;
                        delete RegKey;
                        return VALIDATE_SCRIPT_SUCCESS;
                    }

                    if (true == FileExist(UninstallProgramName))
                    {
                        ProgramNameIsGood = TRUE;
                    }
                } else
                {
                    delete RegKey;
                    return VALIDATE_SCRIPT_SUCCESS;
                }
                delete RegKey;
            } else
            {
                return VALIDATE_SCRIPT_FAILURE;
            }

        } else
        {
            return VALIDATE_SCRIPT_FAILURE;
        }
    } else
    {
        return VALIDATE_SCRIPT_FAILURE;
    }
    return VALIDATE_SCRIPT_SUCCESS;
}

//---------------------------------------------------
int ValidateScriptToRun::ValidateProgramAndRegistry()
//verify the program (uninstall) exists
//read the registry to verify its existance
//Return 0 = success
//      <1 = on error
{
    unsigned long VLen = 1024;
    char skey[1025];
    int pos, pos2;
    CString ts, ts2, ts3, ts4;
    HKEY hKey;

    pos = TokenValue.Find(',');
    if (pos > -1)
    {
        ts = TokenValue.Left(pos);
        ts = TrimEnds(ts);
        ts2 = TokenValue.Right(TokenValue.GetLength() - (pos+1));
        //find the second ','
        pos = ts2.Find(',');
        if (pos > -1)
        {
            ts3 = ts2.Right(ts2.GetLength() - (pos+1));
            ts2 = ts2.Left(pos);
            ts2 = TrimEnds(ts2);
            //find the third ','
            pos = ts3.Find(',');
            if (pos > -1)
            {
                ts4 = ts3.Right(ts3.GetLength() - (pos+1));
                ts4 = TrimEnds(ts4);
                ts3 = ts3.Left(pos);
                ts3 = TrimEnds(ts3);

                //This is for comparison only
                ts2.MakeUpper();

                if (ts2 == "HKEY_LOCAL_MACHINE")
                {
                    hKey = HKEY_LOCAL_MACHINE;
                } else
                {
                    if (ts2 == "HKEY_CURRENT_USER")
                    {
                        hKey = HKEY_CURRENT_USER;
                    } else
                    {
                        return -1;
                    }
                }

                //Read the Registry
                int len = ts3.GetLength();
                strcpy(skey, ts3.GetBuffer(len + 1));
                CRegistryKey *RegKey = new CRegistryKey(hKey, (LPCTSTR)skey, FALSE);
                ts3.ReleaseBuffer();
                if (RegKey)
                {
                    RegKey->Open();
                    skey[0] = 0;
                    if (!RegKey->ReadValue(ts4, skey, &VLen))
                    {
                        //success
//            ts = skey;
                        //remove any parameters here
                        pos = ts.Find(".");
                        if (pos > -1)
                        {
                            ts3 = ts.Right(ts.GetLength() - (pos+1));
                            pos2 = ts3.Find(" ");
                            if (pos2 > -1)
                            {
                                pos += pos2;
                                ts4 = ts.Left(pos+1);
                                UninstallProgramName = ts4;
                            } else
                            {
                                UninstallProgramName = TrimEnds(ts);
                            }
                        } else
                        {
                            UninstallProgramName = ts;
                        }

                        // AKENNED - Added in check for GUID name in registry key
                        pos = ts2.Find("{");
                        if (pos > -1)
                        {
                            ProgramNameIsGood = TRUE;
							delete RegKey;
                            return 0;
                        }

                        if (true == FileExist (UninstallProgramName))
                        {
                            ProgramNameIsGood = TRUE;
                        }
                    } else
                    {
                        delete RegKey;
                        return 0;
                    }
                    delete RegKey;
                } else
                {
                    return -1;
                }

            } else
            {
                return -1;
            }
        } else
        {
            return -1;
        }
    } else
    {
        return -1;
    }
    return 0;
}

//---------------------------------------------------
enumValidateRetCode ValidateScriptToRun::ReadFile(bool bIgnorePlatformCheck)
//Return 0 = success
//      <1 = on error
{
    CString ts, ts2;
    int pos;
    //enumValidateRetCode ret = VALIDATE_SCRIPT_SUCCESS;

    while (pFile1->ReadString(ts))
    {
        ts = TrimEnds(ts);
        //check for blank line
        if (!ts.IsEmpty())
        {
            pos = ts.Find(';');
            if (pos == 0)
            {
                ts = _T("comment");
                ts2 = _T("");  //this is a comment so token value is empty
            } else
            {
                pos = ts.Find('=');
                if (pos > -1)
                {
                    ts2 = ts.Right(ts.GetLength() - (pos + 1));
                    ts2 = TrimEnds(ts2);
                    ts = ts.Left(pos);
                } else
                {
                    ts2 = _T("");  //there is no =, so whole string is token
                }
            }
        } else
        {
            ts = _T("comment");
            ts2 = _T("");  //this is a comment so token value is empty
        }
        if (!IdentifyToken(ts))
        {
            TokenValue = ts2;
            if (!SyntaxCheck())
            {
                switch (Token)
                {
                case GETUNINSTALLSHIELDPROGRAMNAMETOKEN :
                case GETUNINSTALLPROGRAMNAMETOKEN :
                case GETUNINSTALLPROGRAMNAMESTUFFPASSWORDTOKEN :
                case RUNPROGRAMTOKEN :
                    if (VALIDATE_SCRIPT_FAILURE == ValidateProgramName())
                    {
                        return VALIDATE_SCRIPT_FAILURE;
                    } else
                    {
                        if (!ProgramNameIsGood)
                        {
                            return VALIDATE_SCRIPT_SUCCESS;
                        }
                    }
                    break;
                case SETUNINSTALLPROGRAMIFREGISTRYEXISTSTOKEN :
                    if (ValidateProgramAndRegistry())
                    {
                        return VALIDATE_SCRIPT_FAILURE; //error
                    } else
                    {
                        return VALIDATE_SCRIPT_SUCCESS;
                    }
                    break;
                case PLATFORMTOKEN :
					if (bIgnorePlatformCheck)
					{
						IsPlatform = true;  //  over-ride
                        return VALIDATE_SCRIPT_SUCCESS;
					}
                    else if (IsPlatformMatch(TokenValue))
					{	
						IsPlatform = true;  //  over-ride
						return VALIDATE_SCRIPT_SUCCESS;
					}
					else
						return VALIDATE_SCRIPT_FAILURE;
                    break;
                }
                if (IsPlatform && ProgramNameIsGood)
                {
                    //if all is good break out
                    return VALIDATE_SCRIPT_SUCCESS;
                }
            } else
            {
                return VALIDATE_SCRIPT_FAILURE;
            }
        }
    }  //while
    return VALIDATE_SCRIPT_FAILURE;
}

//---------------------------------------------------
int ValidateScriptToRun::SyntaxCheck()
//Return 0 = success
//      <1 = on error
{
    //this is where the tokenvalue is checked for validity
    int i = 0;
    int pos;
    CString ts, ts2, ts3, ts4;

    switch (Token)
    {
    case GETUNINSTALLSHIELDPROGRAMNAMETOKEN :
    case GETUNINSTALLPROGRAMNAMETOKEN :
    case GETUNINSTALLPROGRAMNAMESTUFFPASSWORDTOKEN :
    case SENDAFFIRMATIVETOWINDOWWITHBUTTONCLICK :   
    case SENDTEXTTOEDITWINDOW :   

        //should have 3 params
        pos = TokenValue.Find(',');
        if (pos > -1)
        {
            ts = TokenValue.Left(pos);
            ts = TrimEnds(ts);
            ts2 = TokenValue.Right(TokenValue.GetLength() - (pos + 1));
            //find the second ','
            pos = ts2.Find(',');
            if (pos > -1)
            {
                ts3 = ts2.Right(ts2.GetLength() - (pos + 1));
                ts3 = TrimEnds(ts3);
                ts2 = ts2.Left(pos);
                ts2 = TrimEnds(ts2);
            } else
            {
                //Error Invalid Syntax
                SyntaxError(Token, i);
                bSyntaxError= TRUE;
            }
        } else
        {
            //Error Invalid Syntax
            SyntaxError(Token, i);
            bSyntaxError= TRUE;
        }
        break;
    case SETUNINSTALLPROGRAMIFREGISTRYEXISTSTOKEN :
    case SETINIENTRY :   //  TNN - Added new command

        //should have 4 params
        pos = TokenValue.Find(',');
        if (pos > -1)
        {
            ts = TokenValue.Left(pos);
            ts = TrimEnds(ts);
            ts3 = TokenValue.Right(TokenValue.GetLength() - (pos + 1));
            //find the second ','
            pos = ts3.Find(',');
            if (pos > -1)
            {
                ts2= ts3.Left(pos);
                ts2 = TrimEnds(ts2);
                ts3 = ts3.Right(TokenValue.GetLength() - (pos + 1));
                //find the second ','
                pos = ts3.Find(',');
                if (pos > -1)
                {
                    ts4 = ts3.Right(ts3.GetLength() - (pos + 1));
                    ts4 = TrimEnds(ts4);
                    ts3 = ts3.Left(pos);
                    ts3 = TrimEnds(ts3);
                } else
                {
                    //Error Invalid Syntax
                    SyntaxError(Token, i);
                    bSyntaxError= TRUE;
                }
            } else
            {
                //Error Invalid Syntax
                SyntaxError(Token, i);
                bSyntaxError= TRUE;
            }
        } else
        {
            //Error Invalid Syntax
            SyntaxError(Token, i);
            bSyntaxError= TRUE;
        }
        break;
    case PLATFORMTOKEN :
        //should have one param
        if (TokenValue != "")
        {
            pos = TokenValue.Find(',');
            if (pos > -1)
            {
                // Error in Syntax
                SyntaxError(Token, i);
                bSyntaxError = TRUE;
            }
        } else
        {
            SyntaxError(Token, i);
            bSyntaxError = TRUE;
        }
        TokenValue.MakeUpper();
        if ((TokenValue != "WIN95") && (TokenValue != "WIN98") && (TokenValue != "WINNT") && (TokenValue != "WINXP"))
        {
            SyntaxError(Token, i);
            bSyntaxError = TRUE;
        }
        break;
    default :
        break;
    }
    if (bSyntaxError)
    {
        return -1;
    }
    return 0;
}

//---------------------------------------------------
void ValidateScriptToRun::SyntaxError(int Token, int LineNum)
{
    if (bFirstSyntaxError)
    {
        ErrorLog *ELog = new ErrorLog(TempPath, FileName1);
        if (ELog)
        {
            ELog->WriteErrorToLog(Token, LineNum);
            delete ELog;
            ELog = NULL;
        }
        bFirstSyntaxError = FALSE;
    }
}

//---------------------------------------------------
void ValidateScriptToRun::Error(const int StrId)
{
    if (bFirstSyntaxError)
    {
        ErrorLog *ELog = new ErrorLog(TempPath, FileName1);
        if (ELog)
        {
            ELog->WriteError(StrId);
            delete ELog;
            ELog = NULL;
        }
        bFirstSyntaxError = FALSE;
    }
}


//---------------------------------------------------
int ValidateScriptToRun::OldGetCmdLine()
{
    int pos;
    CString ts,ts2;
    char path[200];

    CWinApp *MainWin = AfxGetApp();
    if (MainWin != NULL)
    {
        lstrcpy(path, GetCommandLine());
        ts = path;
        //remove the initial quote from the EXE name and path
        pos = ts.Find('"');
        if (pos == 0)
        {
            ts = ts.Right(ts.GetLength() - (pos + 1));
        }

        //remove the second quote from the EXE name and path
        pos = ts.Find('"');
        if (pos > -1)
        {
            //if a script name was passed in save it off
            ts2 = ts.Right(ts.GetLength() - (pos + 2));
            ts = ts.Left(pos);
        }

        //if we had a script name, save it to FileName1
        if (ts2 != "")
        {
            FileName1 = ts2;
        }

        //Save off the path and filename for second run
        OurFileName = ts;
        //add quotes if needed          //!!j 02-04
        if (OurFileName.Left(1) == "\"")
        {
            OurFileName = "\"" + OurFileName;
            OurFileName += "\"";
        }

        //strip the path out for the .EXE
        pos = ts.ReverseFind('\\');
        if (pos > -1)
        {
            OurFullPath = ts.Left(pos);
        }

        //if the script file has a path strip it out
        pos = FileName1.ReverseFind('\\');
        if (pos > -1)
        {
            ScriptPath = FileName1.Left(pos);
        } else
        {
            ScriptPath = OurFullPath;
        }
    } else
    {
        Error(IDS_STR1002);
        return -1;
    }
    return 0;
}

//---------------------------------------------------
int ValidateScriptToRun::SetCmdLineVars(CString ourfilename, CString ourfullpath, CString scripttorun, CString scriptpath)
{
    FileName1 = scripttorun;
    OurFileName = ourfilename;
    OurFullPath = ourfullpath;
    ScriptPath = scriptpath;

    return 0;
}

//---------------------------------------------------
int ValidateScriptToRun::CheckSecondRun()
//Return 0 = success
//      <1 = on error
{
    unsigned long VLen = 1024;
    char skey[1025];
    DWORD nLineNumber;
    CRegistryKey *RegKey;

    //Read the Registry for the Filename
    RegKey = new CRegistryKey(HKEY_LOCAL_MACHINE,
                              "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Nuke",
                              FALSE);
    if (RegKey)
    {
        if ( ERROR_SUCCESS == RegKey->Open() )
        {
            skey[0] = 0;
            if (!RegKey->ReadValue("ScriptFileName", skey, &VLen))
            {
                SecondRun = TRUE;
                FileName1 = skey;
            }
            if (!RegKey->ReadValue("LineNumber", nLineNumber))
            {
                StartLineCount = nLineNumber;
            }
            skey[0] = 0;
            VLen = 1024;
            if (!RegKey->ReadValue("SecondRunProgramName", skey, &VLen))
            {
                SecondRunProgramName = skey;
            }
        }
        delete RegKey;
    }

    //Cleanup the Registry for Nuke
    RegKey = new CRegistryKey(HKEY_LOCAL_MACHINE,
                              "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
                              FALSE);
    if (RegKey)
    {
        if ( ERROR_SUCCESS == RegKey->Open() )
            RegKey->DeleteKey("Nuke");
        delete RegKey;
    }
    //Clean Up the Registry for the Run Here
    if (IsNT())
    {
        RegKey = new CRegistryKey(HKEY_CURRENT_USER,
                                  "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows",
                                  FALSE);
        if (RegKey)
        {
            if ( ERROR_SUCCESS == RegKey->Open() )
                RegKey->DeleteValue("run");
            delete RegKey;
        }
    } else
    {
        RegKey = new CRegistryKey(HKEY_LOCAL_MACHINE,
                                  "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
                                  FALSE);
        if (RegKey)
        {
            if ( ERROR_SUCCESS == RegKey->Open() )
                RegKey->DeleteValue("AV32");
            delete RegKey;
        }
    }
    return 0;
}

//---------------------------------------------
ReadScript::ReadScript(ScriptStruct *ss, int SLNum, CString FName1, CString OFName,
                       CString OFPath, CString SPath, CString SRPName, CString TPath)
{
    SS = ss;
    pFile1 = new CStdioFile();

    SS->NumEntries = 0;
    SS->ScriptFileName = FName1;   //set the script filename for reporting errors
    bSyntaxError = FALSE;
    bFirstSyntaxError = TRUE;

    StartLineCount = SLNum;
    FileName1 = FName1;
    OurFileName = OFName;
    OurFullPath = OFPath;
    ScriptPath = SPath;
    SecondRunProgramName = SRPName;
    TempPath = TPath;
}

//---------------------------------------------------
ReadScript::~ReadScript()
{
    if (pFile1)
    {
        delete pFile1;
        pFile1 = NULL;
    }
}

//---------------------------------------------------
int ReadScript::OpenFile()
//Return 0 = success
//      <1 = on error
{
    if (!pFile1->Open(FileName1, CFile::modeRead, &Err))
    {
        if (!Err.m_cause)
        {
            return 0;
        } else
        {
            Error(IDS_STR1003);
            return -1;
        }
    }
    return 0;
}

//---------------------------------------------------
int ReadScript::CloseFile()
//Return 0 = success
//      <1 = on error
{
    if (pFile1)
    {
        pFile1->Close();
    }
    return 0;
}

//---------------------------------------------------
int ReadScript::IdentifyToken(CString ts)
//Return 0 = success
//      <1 = on error
{
    CString token = ts;
    token.MakeLower();
    if (token == "setregistry")
    {
        SS->TS[SS->NumEntries].token = SETREGISTRYTOKEN;
        return 0;
    }
    if (token == "clearregistry")
    {
        SS->TS[SS->NumEntries].token = CLEARREGISTRYTOKEN;
        return 0;
    }
    if (token == "readregistry")
    {
        SS->TS[SS->NumEntries].token = READREGISTRYTOKEN;
        return 0;
    }
    if (token == "restartcomputer")
    {
        SS->TS[SS->NumEntries].token = RESTARTCOMPUTERTOKEN;
        return 0;
    }
    if (token == "debug")
    {
        SS->TS[SS->NumEntries].token = DEBUGTOKEN;
        return 0;
    }
    if (token == "comment")
    {
        SS->TS[SS->NumEntries].token = COMMENTTOKEN;
        return 0;
    }
    if (token == "runprogram")
    {
        SS->TS[SS->NumEntries].token = RUNPROGRAMTOKEN;
        return 0;
    }
    if (token == "getuninstallprogramname")
    {
        SS->TS[SS->NumEntries].token = GETUNINSTALLPROGRAMNAMETOKEN;
        return 0;
    }
    if (token == "getuninstallprogramnamestuffpassword")
    {
        SS->TS[SS->NumEntries].token = GETUNINSTALLPROGRAMNAMESTUFFPASSWORDTOKEN;
        return 0;
    }
    if (token == "runrobot")
    {
        SS->TS[SS->NumEntries].token = RUNROBOTTOKEN;
        return 0;
    }
    if (token == "getuninstallscriptpath")
    {
        SS->TS[SS->NumEntries].token = GETUNINSTALLSCRIPTPATHTOKEN;
        return 0;
    }
    if (token == "addprogramparameters")
    {
        SS->TS[SS->NumEntries].token = ADDPROGRAMPARAMETERSTOKEN;
        return 0;
    }
    if (token == "pause")
    {
        SS->TS[SS->NumEntries].token = PAUSETOKEN;
        return 0;
    }
    if (token == "sendwindowsmessage")
    {
        SS->TS[SS->NumEntries].token = SENDWINDOWSMESSAGETOKEN;
        return 0;
    }
    if (token == "setwindowsuninstallprogram")
    {
        SS->TS[SS->NumEntries].token = SETWINDOWSUNINSTALLPROGRAMTOKEN;
        return 0;
    }
    if (token == "removefromautoexec")
    {
        SS->TS[SS->NumEntries].token = REMOVEFROMAUTOEXECTOKEN;
        return 0;
    }
    if (token == "getuninstallprogrampath")
    {
        SS->TS[SS->NumEntries].token = GETUNINSTALLPROGRAMPATHTOKEN;
        return 0;
    }
    if (token == "setuninstallscriptname")
    {
        SS->TS[SS->NumEntries].token = SETUNINSTALLSCRIPTNAMETOKEN;
        return 0;
    }
    if (token == "setuninstallprogramname")
    {
        SS->TS[SS->NumEntries].token = SETUNINSTALLPROGRAMNAMETOKEN;
        return 0;
    }
    if (token == "pausefornotificationofcompletion")
    {
        SS->TS[SS->NumEntries].token = PAUSEFORNOTIFICATIONOFCOMPLETION;
        return 0;
    }
    if (token == "getuninstallshieldprogramname")
    {
        SS->TS[SS->NumEntries].token = GETUNINSTALLSHIELDPROGRAMNAMETOKEN;
        return 0;
    }
    if (token == "silent")
    {
        SS->TS[SS->NumEntries].token = SILENTTOKEN;
        return 0;
    }

    if (token == "setregistryforuninstallsecondrun")
    {
        SS->TS[SS->NumEntries].token = SETREGISTRYFORUNINSTALLSECONDRUNTOKEN;
        return 0;
    }

    if (token == "runrobotsetrunregistry")
    {
        SS->TS[SS->NumEntries].token = RUNROBOTSETRUNREGISTRYTOKEN;
        return 0;
    }

    if (token == "setupforsecondrun")
    {
        SS->TS[SS->NumEntries].token = SETUPFORSECONDRUNTOKEN;
        return 0;
    }

    if (token == "platform")
    {
        SS->TS[SS->NumEntries].token = PLATFORMTOKEN;
        return 0;
    }

    if (token == "stopprogramforreboot")
    {
        SS->TS[SS->NumEntries].token = STOPPROGRAMFORREBOOTTOKEN;
        return 0;
    }

    if (token == "sendwindowsmessageandwait")
    {
        SS->TS[SS->NumEntries].token = SENDWINDOWSMESSAGEANDWAITTOKEN;
        return 0;
    }

    if (token == "runrobotsendcancel")
    {
        SS->TS[SS->NumEntries].token = RUNROBOTSENDCANCELTOKEN;
        return 0;
    }

    if (token == "sendaffirmativetowindow")
    {
        SS->TS[SS->NumEntries].token = SENDAFFIRMATIVETOWINDOWTOKEN;
        return 0;
    }

    if (token == "sendaffirmativetowindowafterenable")
    {
        SS->TS[SS->NumEntries].token = SENDAFFIRMATIVETOWINDOWAFTERENABLETOKEN;
        return 0;
    }
    if (token == "sendaffirmativetowindowwithbuttonclick")
    {
        SS->TS[SS->NumEntries].token = SENDAFFIRMATIVETOWINDOWWITHBUTTONCLICK;
        return 0;
    }
    if (token == "sendtexttoeditwindow")
    {
        SS->TS[SS->NumEntries].token = SENDTEXTTOEDITWINDOW;
        return 0;
    }
    if (token == "setinientry")
    {
        SS->TS[SS->NumEntries].token = SETINIENTRY;
        return 0;
    }
    if (token == "setuninstallprogramifregistryexists")
    {
        SS->TS[SS->NumEntries].token = SETUNINSTALLPROGRAMIFREGISTRYEXISTSTOKEN;
        return 0;
    }

    SS->TS[SS->NumEntries].token = ERRORTOKEN;
    return -1;
}

//---------------------------------------------------
int ReadScript::ReadFile()
//Return 0 = success
//      <1 = on error
{
    CString ts, ts2;
    int pos;

    while (pFile1->ReadString(ts))
    {
        ts = TrimEnds(ts);
        //check for blank line
        if (!ts.IsEmpty())
        {
            pos = ts.Find(';');
            if (pos == 0)
            {
                ts = _T("comment");
                ts2 = _T("");  //this is a comment so token value is empty
            } else
            {
                pos = ts.Find('=');
                if (pos > -1)
                {
                    ts2 = ts.Right(ts.GetLength() - (pos+1));
                    ts2 = TrimEnds(ts2);
                    ts = ts.Left(pos);
                } else
                {
                    ts2 = _T("");  //there is no =, so whole string is token
                }
            }
        } else
        {
            ts = _T("comment");
            ts2 = _T("");  //this is a comment so token value is empty
        }
        SS->TS[SS->NumEntries].tokenvalue = ts2;
        IdentifyToken(ts);
        SS->NumEntries++;
    }
    return 0;
}


//---------------------------------------------------
int ReadScript::SyntaxCheck(ScriptStruct *SS)
//Return 0 = success
//      <1 = on error
{
    //this is where the tokenvalue is checked for validity
    int i = 0;
    int pos;
    CString ts, ts2, ts3, ts4;

    while (i < SS->NumEntries)
    {
        switch (SS->TS[i].token)
        {
        case SETREGISTRYTOKEN :
        case SETUNINSTALLPROGRAMIFREGISTRYEXISTSTOKEN :
            //should have 4 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);
                    pos = ts3.Find(',');
                    if (pos > -1)
                    {
                        ts4 = ts3.Right(ts3.GetLength() - (pos+1));
                        ts4 = TrimEnds(ts4);
                        ts3 = ts3.Left(pos);
                        ts3 = TrimEnds(ts3);
                    } else
                    {
                        //Error Invalid Syntax
                        SyntaxError(SS->TS[i].token, i);
                        bSyntaxError = TRUE;
                    }
                } else
                {
                    //Error Invalid Syntax
                    SyntaxError(SS->TS[i].token, i);
                    bSyntaxError = TRUE;
                }
            } else
            {
                //Error Invalid Syntax
                SyntaxError(SS->TS[i].token, i);
                bSyntaxError = TRUE;
            }
            break;
        case CLEARREGISTRYTOKEN :
        case READREGISTRYTOKEN :
        case GETUNINSTALLSCRIPTPATHTOKEN :
        case GETUNINSTALLPROGRAMPATHTOKEN :
        case GETUNINSTALLPROGRAMNAMETOKEN :
        case GETUNINSTALLPROGRAMNAMESTUFFPASSWORDTOKEN :
        case GETUNINSTALLSHIELDPROGRAMNAMETOKEN :
        case SENDWINDOWSMESSAGEANDWAITTOKEN :
        case SENDAFFIRMATIVETOWINDOWAFTERENABLETOKEN :
	    case SENDAFFIRMATIVETOWINDOWWITHBUTTONCLICK :
	    case SENDTEXTTOEDITWINDOW :   
        case RUNPROGRAMTOKEN :
            //should have 3 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);
                } else
                {
                    //Error Invalid Syntax
                    SyntaxError(SS->TS[i].token, i);
                    bSyntaxError = TRUE;
                }
            } else
            {
                //Error Invalid Syntax
                SyntaxError(SS->TS[i].token, i);
                bSyntaxError = TRUE;
            }
            break;
        case SENDAFFIRMATIVETOWINDOWTOKEN :
        case SENDWINDOWSMESSAGETOKEN :
            //should have 2 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                ts2 = TrimEnds(ts2);
            } else
            {
                //Error Invalid Syntax
                SyntaxError(SS->TS[i].token, i);
                bSyntaxError = TRUE;
            }
            break;
        case RUNROBOTSETRUNREGISTRYTOKEN:
            //should have 1 or 2 params
            if (SS->TS[i].tokenvalue.GetLength() == 0)
            {
                SyntaxError(SS->TS[i].token, i);
                bSyntaxError = TRUE;
            } else
            {
                pos = SS->TS[i].tokenvalue.Find(',');
                if (pos > -1)
                {
                    ts = SS->TS[i].tokenvalue.Left(pos);
                    ts = TrimEnds(ts);
                    ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                    ts2 = TrimEnds(ts2);
                    pos = ts2.Find(',');
                    if (pos > -1)
                    {
                        ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                        ts3 = TrimEnds(ts3);
                        ts2 = ts2.Left(pos);
                        ts2 = TrimEnds(ts2);
                    }
                }
            }
            break;
        case SETUPFORSECONDRUNTOKEN :
        case DEBUGTOKEN :
        case PAUSETOKEN :
        case ADDPROGRAMPARAMETERSTOKEN :
        case SETWINDOWSUNINSTALLPROGRAMTOKEN :
        case REMOVEFROMAUTOEXECTOKEN :
        case SETUNINSTALLSCRIPTNAMETOKEN :
        case SETUNINSTALLPROGRAMNAMETOKEN :
        case PLATFORMTOKEN:
            //should have one param
            if (SS->TS[i].tokenvalue != "")
            {
                pos = SS->TS[i].tokenvalue.Find(',');
                if (pos > -1)
                {
                    // Error in Syntax
                    SyntaxError(SS->TS[i].token, i);
                    bSyntaxError = TRUE;
                }
            } else
            {
                SyntaxError(SS->TS[i].token, i);
                bSyntaxError = TRUE;
            }
            break;
        case RUNROBOTTOKEN :
            //one parameter optional
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                //Error Invalid Syntax
                SyntaxError(SS->TS[i].token, i);
                bSyntaxError = TRUE;
            }
            break;
        case RESTARTCOMPUTERTOKEN :
            //should have empty token value
            if (SS->TS[i].tokenvalue != "")
            {
                //Error in Syntax
                SyntaxError(SS->TS[i].token, i);
                bSyntaxError = TRUE;
            }
            if (!IsNT())
            {
                Error(IDS_STR1004);
                bSyntaxError = TRUE;
            }
            break;
        case PAUSEFORNOTIFICATIONOFCOMPLETION :
            //one parameter optional
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                //Error Invalid Syntax
                SyntaxError(SS->TS[i].token, i);
                bSyntaxError = TRUE;
            }
/* This is ok for NT
                if (!IsNT()){
                    SyntaxError(SS->TS[i].token, i);
          bSyntaxError = TRUE;
                }
*/
            break;
        case SETREGISTRYFORUNINSTALLSECONDRUNTOKEN:
        case STOPPROGRAMFORREBOOTTOKEN :
        case SILENTTOKEN :
            //should have empty token value
            if (SS->TS[i].tokenvalue != "")
            {
                //Error in Syntax
                SyntaxError(SS->TS[i].token, i);
                bSyntaxError = TRUE;
            }
            break;
        case ERRORTOKEN :
            //should not get here
            SyntaxError(SS->TS[i].token, i);
            bSyntaxError = TRUE;
            break;
        default :

            break;
        }
        i++;
    }
    if (bSyntaxError)
    {
        return -1;
    }
    return 0;
}

//---------------------------------------------------
void ReadScript::SyntaxError(int Token, int LineNum)
{
    if (bFirstSyntaxError)
    {
        ErrorLog *ELog = new ErrorLog(TempPath, FileName1);
        if (ELog)
        {

            ELog->WriteErrorToLog(Token, LineNum);

            delete ELog;
            ELog = NULL;
        }
        bFirstSyntaxError = FALSE;
    }
}

//---------------------------------------------------
void ReadScript::Error(const int StrId)
{
    if (bFirstSyntaxError)
    {
        ErrorLog *ELog = new ErrorLog(TempPath, FileName1);
        if (ELog)
        {
			CString errorString;
		    VERIFY(errorString.LoadString(StrId));  
            TraceEx("ReadScript::Error - %s\n",errorString);

            ELog->WriteError(StrId);

            delete ELog;
            ELog = NULL;
        }
        bFirstSyntaxError = FALSE;
    }
}

//------------------------------------------------
RunScript::RunScript(const BOOL secondrun, ScriptStruct *ss, CString Path,
                     CString TPath, CString ournewpath, CString ournewfilelocation)
{
    bSecondRun = secondrun;
    SS = ss;
    ScriptPath = Path;
    InteractDuration = 0;
    UninstallScriptPath = "";
    UninstallScriptName = "";
    UninstallProgramPath = "";
    UninstallProgramName = "";
    UninstallParameters = "";
    strOurNewPath = ournewpath;
    strOurNewFileLocation = ournewfilelocation;
    RunWinUninstall = FALSE;
    TempPath = TPath;

    // TNN - Changed initial value to true, because setting to false was preventing error from being written to status.ini file.
	//       For example, if a script was run for a program that was not on the computer (i.e. user ran incorrect script),
	//       then no error was ever reported.
    /// bFirstSyntaxError = FALSE;
    bFirstSyntaxError = true;		// TNN - Setting to false was preventing error from being written to status.ini file
}

//---------------------------------------------------
RunScript::~RunScript()
{
}

/*-----------------------------*/
/* Return string (thread safe) */
/*-----------------------------*/
#if defined( C_MSVC32 )
__declspec(thread) char DTX[ 21 ];
#else
static char DTX[ 21 ];
#endif

/*-------------------*/
/* dateTime function */
/*-------------------*/
char *dateTime() {

    struct tm *SDT;                     /* ptr returned by localtime */
    time_t t;                           /* structure for time func   */
    struct
    {
        char co_dtsep[2];
        char co_tmsep[2];
    } c;
    t = time( NULL );                   /* get time in seconds       */
    SDT = localtime( &t );              /* populate time structure   */
    GetProfileString( "intl", "sDate", "-", c.co_dtsep, 2 );
    GetProfileString( "intl", "sTime", ":", c.co_tmsep, 2 );
    sssnprintf( DTX, sizeof(DTX), "%04i%s%02i%s%02i  %02i%s%02i%s%02i",
             ( SDT->tm_year ) + 1900,
             c.co_dtsep,
             ( SDT->tm_mon  ) + 1,
             c.co_dtsep,
             SDT->tm_mday,
             SDT->tm_hour,
             c.co_tmsep,
             SDT->tm_min,
             c.co_tmsep,
             SDT->tm_sec );
    return DTX;

} /* end */

extern "C" int hexhash_of_password( char *, char * );

//---------------------------------------------------
int RunScript::Run(int LineNum, CString rsFileName, bool bUninstallSymantecOk, bool bIgnorePlatformCheck)
//Return 0 = success
//      <1 = on error
{
    HKEY hKey=0;
    int i;
    int pos;
    int NumSecs;
    CString ts, ts2, ts3, ts4;
    char uninstallCmdLine [512];


    i = LineNum;

    while (i < SS->NumEntries)
    {
        switch (SS->TS[i].token)
        {
        case SETREGISTRYTOKEN :
            //should have 4 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);
                    pos = ts3.Find(',');
                    if (pos > -1)
                    {
                        ts4 = ts3.Right(ts3.GetLength() - (pos+1));
                        ts4 = TrimEnds(ts4);
                        ts3 = ts3.Left(pos);
                        ts3 = TrimEnds(ts3);
                        //Do Work Here
                        if (!GetHKey(ts, hKey))
                        {
                            if (SetRegistryString(hKey, ts2, ts3, ts4, FALSE))
                            {
                                Error(IDS_STR1005);
                                return -1;
                            }
                        } else
                        {
                            Error(IDS_STR1006);
                            return -1;
                        }
                    } else
                    {
                        Error(IDS_STR1007);
                        return -1;
                    }
                } else
                {
                    Error(IDS_STR1070);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1071);
                return -1;
            }
            break;
        case SETINIENTRY :

            //should have 4 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);
                    pos = ts3.Find(',');
                    if (pos > -1)
                    {
                        ts4 = ts3.Right(ts3.GetLength() - (pos+1));
                        ts4 = TrimEnds(ts4);
                        ts3 = ts3.Left(pos);
                        ts3 = TrimEnds(ts3);
                        //Do Work Here

						//Set the INI entry
						char szWinInitFile[1024];

						GetWindowsDirectory(szWinInitFile, 1024);
						lstrcat(szWinInitFile, "\\");
						lstrcat(szWinInitFile, ts);
						if (!WritePrivateProfileString(ts2, ts3, ts4, szWinInitFile))
						   Error(IDS_STRING1122);

                    }
				}
            } 
            break;

        case SENDTEXTTOEDITWINDOW :
        case SENDAFFIRMATIVETOWINDOWAFTERENABLETOKEN :
        case SENDAFFIRMATIVETOWINDOWWITHBUTTONCLICK :
            //should have 3 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);
                    //Do Work Here
                    int Duration = atoi(ts3);
					if (SS->TS[i].token == SENDAFFIRMATIVETOWINDOWWITHBUTTONCLICK)
						CallSendAffirmativeToWindowWithButtonClick(ts, ts2, Duration);
					else if (SS->TS[i].token == SENDTEXTTOEDITWINDOW)
						CallSendTextToEditWindow (ts, ts2, Duration);
					else
						CallSendAffirmativeToWindowAfterEnable(ts, ts2, Duration);
                } else
                {
                    Error(IDS_STR1072);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1073);
                return -1;
            }
            break;
        case CLEARREGISTRYTOKEN :
            //should have 3 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);
                    //Do Work Here
                    if (!GetHKey(ts, hKey))
                    {
                        if (ClearRegistration(hKey, ts2, ts3))
                        {
                            Error(IDS_STR1008);
                            return -1;
                        }
                    } else
                    {
                        Error(IDS_STR1009);
                        return -1;
                    }
                } else
                {
                    Error(IDS_STR1010);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1074);
                return -1;
            }
            break;
        case READREGISTRYTOKEN :
            //should have 3 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);
                    //Do Work Here
                } else
                {
                    Error(IDS_STR1075);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1076);
                return -1;
            }
            break;
        case GETUNINSTALLSHIELDPROGRAMNAMETOKEN :
            //should have 3 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);
                    if (!GetHKey(ts, hKey))
                    {
                        if (GetUninstallShieldProgramAndPath(hKey, ts2, ts3))
                        {
                            Error(IDS_STR1011);
                            return -1;
                        }
                    } else
                    {
                        Error(IDS_STR1012);
                        return -1;
                    }
                } else
                {
                    Error(IDS_STR1013);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1077);
                return -1;
            }
            break;
        case GETUNINSTALLPROGRAMNAMETOKEN :
            //should have 3 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);
                    if (!GetHKey(ts, hKey))
                    {
                        if (GetUninstallProgramAndPath(hKey, ts2, ts3))
                        {
                            Error(IDS_STR1014);
                            return -1;
                        }
                    } else
                    {
                        Error(IDS_STR1015);
                        return -1;
                    }
                } else
                {
                    Error(IDS_STR1016);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1078);
                return -1;
            }
            break;
        case GETUNINSTALLPROGRAMNAMESTUFFPASSWORDTOKEN :
            //should have 3 params
            char datePW[11];
            char challengePW[9];

            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);
                    if (!GetHKey(ts, hKey))
                    {
                        if (GetUninstallProgramAndPath(hKey, ts2, ts3))
                        {
                            Error(IDS_STR1017);
                            return -1;
                        }
                        strncpy( datePW, dateTime(), 10 );
                        datePW[4] = '/';
                        datePW[7] = '/';
                        datePW[10] = '\0';

                        if ( hexhash_of_password( datePW, challengePW ) != 0 )
                        {
                            return -2;  //no memory
                        } else
                        {
                            // challengePW contains the challenge password
                            UninstallProgramName += " /R /P:";
                            UninstallProgramName += challengePW;
                        }

                    } else
                    {
                        Error(IDS_STR1018);
                        return -1;
                    }
                } else
                {
                    Error(IDS_STR1019);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1020);
                return -1;
            }
            break;
        case GETUNINSTALLSCRIPTPATHTOKEN :
            //should have 3 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);
                    //Do Work Here
                    if (!GetHKey(ts, hKey))
                    {
                        UninstallScriptPath = GetUninstallPath(hKey, ts2, ts3);
                        if (UninstallScriptPath.GetLength() == 0)
                        {
                            Error(IDS_STR1021);
                            return -1;
                        }
                    } else
                    {
                        Error(IDS_STR1022);
                        return -1;
                    }
                } else
                {
                    Error(IDS_STR1023);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1079);
                return -1;
            }
            break;
        case GETUNINSTALLPROGRAMPATHTOKEN :
            //should have 3 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);
                    //Do Work Here
                    if (!GetHKey(ts, hKey))
                    {
                        UninstallProgramPath = GetUninstallPath(hKey, ts2, ts3);
                        if (UninstallProgramPath.GetLength() == 0)
                        {
                            Error(IDS_STR1024);
                            return -1;
                        }
                    } else
                    {
                        Error(IDS_STR1026);
                        return -1;
                    }
                } else
                {
                    Error(IDS_STR1026);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1080);
                return -1;
            }
            break;
        case SENDWINDOWSMESSAGEANDWAITTOKEN :
            //should have 3 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);
                    //Do Work Here
                    ts3 = TrimEnds(ts3);
                    int Duration = atoi(ts3);
                    CallSendWindowsMessageAndWait(ts, ts2, Duration);
                } else
                {
                    //Error Invalid Syntax
                    Error(IDS_STR1081);
                    return -1;
                }
            } else
            {
                //Error Invalid Syntax
                Error(IDS_STR1082);
                return -1;
            }
            break;
        case RUNPROGRAMTOKEN :
            //should have 3 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts2.Find(',');
                if (pos > -1)
                {
                    ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                    ts3 = TrimEnds(ts3);
                    ts2 = ts2.Left(pos);
                    ts2 = TrimEnds(ts2);

                    //Do Work Here
                    if (!GetHKey(ts, hKey))
                    {
						int retCode = CallRunProgram(hKey, ts2, ts3, bUninstallSymantecOk);
                        
                        if (retCode == UNINSTALL_NOT_ALLOWED)
                        {
                            Error(IDS_STRING1120);
                            return retCode;
                        }
						else if (retCode < 0)
                        {
                            Error(IDS_STR1027);
                            return retCode;
                        }
                    } else
                    {
                        Error(IDS_STR1028);
                        return -1;
                    }
                } else
                {
                    Error(IDS_STR1029);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1083);
                return -1;
            }
            break;
        case ADDPROGRAMPARAMETERSTOKEN :
            //should have one param
            if (SS->TS[i].tokenvalue != "")
            {
                pos = SS->TS[i].tokenvalue.Find(',');
                if (pos > -1)
                {
                    Error(IDS_STR1084);
                    return -1;
                } else
                {
                    //Do Work Here
                    UninstallParameters=SS->TS[i].tokenvalue;
                }
            } else
            {
                Error(IDS_STR1085);
                return -1;
            }
            break;
        case SETUPFORSECONDRUNTOKEN :
            //should have one param
            if (SS->TS[i].tokenvalue != "")
            {
                pos = SS->TS[i].tokenvalue.Find(',');
                if (pos > -1)
                {
                    Error(IDS_STR1086);
                    return -1;
                } else
                {
                    //Do Work Here
                    UninstallParameters=SS->TS[i].tokenvalue;
                    if (CheckAndSetRegistryForSecondRun((DWORD)i, rsFileName, SS->TS[i].tokenvalue))
                    {
                        Error(IDS_STR1030);
                        return -1;
                    }
                }
            } else
            {
                Error(IDS_STR1031);
                return -1;
            }
            break;
        case REMOVEFROMAUTOEXECTOKEN :
            //should have one param
            if (SS->TS[i].tokenvalue != "")
            {
                pos = SS->TS[i].tokenvalue.Find(',');
                if (pos > -1)
                {
                    Error(IDS_STR1087);
                    return -1;
                } else
                {
                    //Do Work Here
                    RemoveFromAutoExec(NULL, SS->TS[i].tokenvalue);
                }
            } else
            {
                Error(IDS_STR1088);
                return -1;
            }
            break;
        case SENDAFFIRMATIVETOWINDOWTOKEN :
            //should have 2 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                ts2 = TrimEnds(ts2);
                int Duration = atoi(ts2);
                CallSendAffirmativeToWindow(ts, Duration);
            } else
            {
                //Error Invalid Syntax
                Error(IDS_STR1089);
                return -1;
            }
            break;
        case SENDWINDOWSMESSAGETOKEN :
            //should have 2 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                ts2 = TrimEnds(ts2);
                CallSendWindowsMessage(ts, ts2);
            } else
            {
                //Error Invalid Syntax
                Error(IDS_STR1090);
                return -1;
            }
            break;
        case SETWINDOWSUNINSTALLPROGRAMTOKEN :
            //one param
            if (SS->TS[i].tokenvalue != "")
            {
                pos = SS->TS[i].tokenvalue.Find(',');
                if (pos > -1)
                {
                    Error(IDS_STR1091);
                    return -1;
                } else
                {
                    ts = SS->TS[i].tokenvalue;
                    ts = TrimEnds(ts);
                    //do work here
                    GetWindowsDirectory(uninstallCmdLine, MAX_PATH);
                    UninstallProgramName.Format("%s\\%s", uninstallCmdLine, ts);
                    RunWinUninstall = TRUE;
                }
            } else
            {
                Error(IDS_STR1092);
                return -1;
            }
            break;
        case SETUNINSTALLSCRIPTNAMETOKEN :
            //should have one param
            if (SS->TS[i].tokenvalue != "")
            {
                pos = SS->TS[i].tokenvalue.Find(',');
                if (pos > -1)
                {
                    Error(IDS_STR1093);
                    return -1;
                } else
                {
                    ts = TrimEnds(SS->TS[i].tokenvalue);
                    //Do Work Here
                    if (UninstallScriptPath != "")
                    {
                        UninstallScriptName = UninstallScriptPath+ts;
                    } else
                    {
                        Error(IDS_STR1032);
                        return -1;
                    }
                }
            } else
            {
                Error(IDS_STR1033);
                return -1;
            }
            break;
        case SETUNINSTALLPROGRAMNAMETOKEN :
            //should have one param
            if (SS->TS[i].tokenvalue != "")
            {
                pos = SS->TS[i].tokenvalue.Find(',');
                if (pos > -1)
                {
                    Error(IDS_STR1094);
                    return -1;
                } else
                {
                    ts = TrimEnds(SS->TS[i].tokenvalue);
                    //Do Work Here
                    if (UninstallProgramPath != "")
                    {
                        UninstallProgramName = UninstallProgramPath+ts;
                    } else
                    {
                        Error(IDS_STR1034);
                        return -1;
                    }
                }
            } else
            {
                Error(IDS_STR1035);
                return -1;
            }
            break;
        case RUNROBOTSETRUNREGISTRYTOKEN :
            //2 param with a second optional
            if (SS->TS[i].tokenvalue != "")
            {
                pos = SS->TS[i].tokenvalue.Find(',');
                if (pos > -1)
                {
                    ts = SS->TS[i].tokenvalue.Left(pos);
                    ts = TrimEnds(ts);
                    ts2 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                    ts2 = TrimEnds(ts2);
                    pos = ts2.Find(',');
                    if (pos > -1)
                    {
                        ts3 = ts2.Right(ts2.GetLength() - (pos+1));
                        ts3 = TrimEnds(ts3);
                        ts2 = ts2.Left(pos);
                        ts2 = TrimEnds(ts2);
                        NumSecs = atoi(ts3);
                        InteractDuration = NumSecs * 1000L;
                    } else
                    {
                        //default to 5 minutes
                        InteractDuration = 300000;
                    }
                } else
                {
                    Error(IDS_STR1095);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1096);
                return -1;
            }

            if (CallSpecialRunRobot((DWORD)i, rsFileName, ts, ts2))
            {
                Error(IDS_STR1037);
                return -1;
            }
            break;
        case RUNROBOTTOKEN :
            //optional one param
            if (SS->TS[i].tokenvalue != "")
            {
                pos = SS->TS[i].tokenvalue.Find(',');
                if (pos > -1)
                {
                    Error(IDS_STR1037);
                    return -1;
                } else
                {
                    ts = SS->TS[i].tokenvalue;
                    ts = TrimEnds(ts);
                    NumSecs = atoi(ts);
                    InteractDuration = NumSecs * 1000L;
                }
            } else
            {
                //default to 5 minutes
                InteractDuration = 30000;
            }

			//  Determine if this is a symantec product.
			ASSERT(hKey);
			if (!bUninstallSymantecOk)
				if (IsSymantecProduct (hKey, ts2, ts3))
				{
					Error(IDS_STRING1120);
					return -1;
				}

            if (CallRunRobot(FALSE))
            {
                Error(IDS_STR1038);
                return -1;
            }
            if (bSecondRun)
            {
                ts = SecondRunProgramName;
                pos = ts.ReverseFind('\\');
                if (pos > -1)
                {
                    ts2 = ts.Left(pos);
                    DeleteDirectoryAndContents(ts2);
                    if (IsNT())
                    {
                        DWORD res;
                        res = MoveFileEx(OurFileName, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
                        if (!res)
                        {
                        }
                        res = MoveFileEx(ts2, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
                        if (!res)
                        {
                        }
                    } else
                    {
                        char szWinInitFile[1024];

                        GetWindowsDirectory(szWinInitFile, 1024);
                        lstrcat(szWinInitFile, "\\WININIT.INI");
                        WritePrivateProfileString("Rename", "NUL", OurFileName, szWinInitFile);
                        WritePrivateProfileString("Rename", "NUL", ts2, szWinInitFile);
                    }
                }
            }
            break;
        case RUNROBOTSENDCANCELTOKEN :
/* //!!j temp for debug
        //optional one param
                if (SS->TS[i].tokenvalue != "") {
                    pos = SS->TS[i].tokenvalue.Find(',');
                    if (pos > -1) {
            Error(IDS_STR1039);
                        return -1;
                    } else {
                    ts = SS->TS[i].tokenvalue;
                        ts = TrimEnds(ts);
                        NumSecs = atoi(ts);
            InteractDuration = NumSecs * 1000L;
                    }
                } else {
                    //default to 5 minutes
                    InteractDuration = 30000;
                }
                if (CallRunRobot(TRUE)) {
          Error(IDS_STR1040);
                    return -1;
                }
*/
            break;
        case RESTARTCOMPUTERTOKEN :
            //should have empty token value
            if (SS->TS[i].tokenvalue != "")
            {
                Error(IDS_STR1041);
                return -1;
            }
            if (RestartComputer((DWORD)i , rsFileName))
            {
                Error(IDS_STR1042);
                return -1;
            }
            break;
        case SETREGISTRYFORUNINSTALLSECONDRUNTOKEN :
            //should have empty token value
            if (SS->TS[i].tokenvalue != "")
            {
                Error(IDS_STR1097);
                return -1;
            }
            if (!GetHKey("HKEY_CURRENT_USER", hKey))
            {
                if (SetRegistryString(hKey, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows", "run", OurFileName, FALSE))
                {
                    Error(IDS_STR1043);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1044);
                return -1;
            }
            break;
        case PAUSEFORNOTIFICATIONOFCOMPLETION :
            //one parameter optional
            if (PauseForNotification((DWORD)i , rsFileName, SS->TS[i].tokenvalue))
            {
                Error(IDS_STR1098);
                return -1;
            }
            return 1;
            break;
        case STOPPROGRAMFORREBOOTTOKEN :
            //should have empty token value
            if (SS->TS[i].tokenvalue != "")
            {
                Error(IDS_STR1099);
                return -1;
            }
            if (StopProgramForReboot((DWORD)i , rsFileName))
            {
                Error(IDS_STR1100);
                return -1;
            }
            return 1;
            break;
        case SETUNINSTALLPROGRAMIFREGISTRYEXISTSTOKEN :
            //should have 4 params
            pos = SS->TS[i].tokenvalue.Find(',');
            if (pos > -1)
            {
                ts = SS->TS[i].tokenvalue.Left(pos);
                ts = TrimEnds(ts);
                ts3 = SS->TS[i].tokenvalue.Right(SS->TS[i].tokenvalue.GetLength() - (pos+1));
                //find the second ','
                pos = ts3.Find(',');
                if (pos > -1)
                {
                    ts2 = ts3.Left(pos);
                    ts2 = TrimEnds(ts2);
                    ts3 = ts3.Right(ts3.GetLength() - (pos+1));
                    //find the second ','
                    pos = ts3.Find(',');
                    if (pos > -1)
                    {
                        ts4 = ts3.Right(ts3.GetLength() - (pos+1));
                        ts4 = TrimEnds(ts4);
                        ts3 = ts3.Left(pos);
                        ts3 = TrimEnds(ts3);
                        if (!GetHKey(ts2, hKey))
                        {
                            unsigned long VLen = 1024;
                            char skey[1025];

                            //Read the Registry
                            CRegistryKey *RegKey = new CRegistryKey(hKey, ts3, FALSE);
                            if (RegKey)
                            {
                                RegKey->Open();
                                skey[0] = 0;
                                if (!RegKey->ReadValue(ts4, skey, &VLen))
                                {
                                    //success
                                    UninstallProgramName = ScriptPath + "\\";
                                    UninstallProgramName += ts;
                                    SecondRunProgramName = strOurNewPath + "\\";
                                    SecondRunProgramName += ts;   //this will be used for reboot
                                } else
                                {
                                    return -1;
                                }
                                delete RegKey;
                            } else
                            {
                                return -1;
                            }
                        } else
                        {
                            Error(IDS_STR1018);
                            return -1;
                        }
                    } else
                    {
                        Error(IDS_STR1019);
                        return -1;
                    }
                } else
                {
                    Error(IDS_STR1019);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1020);
                return -1;
            }
            break;
        case SILENTTOKEN :
            //should have empty token value
            if (SS->TS[i].tokenvalue != "")
            {
                Error(IDS_STR1101);
                return -1;
            }
            break;
        case DEBUGTOKEN :
            //should have one param
            if (SS->TS[i].tokenvalue != "")
            {
                pos = SS->TS[i].tokenvalue.Find(',');
                if (pos > -1)
                {
                    Error(IDS_STR1102);
                    return -1;
                }
            } else
            {
                Error(IDS_STR1103);
                return -1;
            }
            break;
        case ERRORTOKEN :
            //should not get here
            return -1;
            break;

        case PLATFORMTOKEN :
            // Check for platform match
			if (!bIgnorePlatformCheck)
			{
				if (!IsPlatformMatch(SS->TS[i].tokenvalue))
				{
					Error(IDS_STRING1117);
					return -1;
				}
			}

            break;

        }
        i++;
    }
    return 0;
}

//---------------------------------------------------
int RunScript::GetHKey(CString szHKey, HKEY &hKey)
//Convert a string into a HKEY
//Return 0 = success
//      <1 = on error
{
    szHKey.MakeUpper();
    if (szHKey == "HKEY_LOCAL_MACHINE")
    {
        hKey = HKEY_LOCAL_MACHINE;
        return 0;
    } else
    {
        if (szHKey == "HKEY_CURRENT_USER")
        {
            hKey = HKEY_CURRENT_USER;
            return 0;
        }
    }
    return -1;
}

//---------------------------------------------------
bool RunScript::CheckExistProgram()
//There are times when the uninstall will not return an error, but will not uninstall the AV.
//This function is used to see if the uninstall program remains.
//Return 0 = success, if file to uninstall exists
//      <1 = on error
{
    CString RunStr;

#ifdef DEBUGANY
    {//!!debug  #include "stdio.h" #include "stdlib.h"
        FILE *f = fopen("c:\\debug.log", "at+");
        if (f)
        {
            fprintf(f, "cep 1\n");
            fclose(f);
        }
    }
#endif
    if (SecondRunProgramName.GetLength() == 0)
    {
			if (UninstallProgramName.GetLength() == 0)
			{
				//return -2;  //this should only happen if this function was called before the


				///  TNN - Comment above says this should never happen but it will always
				///        happen if you are using the RunProgram script command.
				///        Hmmm.... Will need to look into this further.
				return false;
				//variables were set, this would be a programmer error, and would
				//be caught in development.
			}

        RunStr = UninstallProgramName;
    } else
    {
        RunStr = SecondRunProgramName;
    }

#ifdef DEBUGANY
    {//!!debug  #include "stdio.h" #include "stdlib.h"
        FILE *f = fopen("c:\\debug.log", "at+");
        if (f)
        {
            fprintf(f, "cep 2 [%s]\n", RunStr);
            fclose(f);
        }
    }
#endif
//!!  Sleep(5000);  //wait
#ifdef DEBUGANY
    {//!!debug  #include "stdio.h" #include "stdlib.h"
        FILE *f = fopen("c:\\debug.log", "at+");
        if (f)
        {
            fprintf(f, "cep 3\n");
            fclose(f);
        }
    }
#endif
    CFileStatus status;
    extern CFile cfile;
#ifdef DEBUGANY
    {//!!debug  #include "stdio.h" #include "stdlib.h"
        FILE *f = fopen("c:\\debug.log", "at+");
        if (f)
        {
            fprintf(f, "cep 4\n");
            fclose(f);
        }
    }
#endif
    if ( CFile::GetStatus(RunStr, status))
    {  // static function
#ifdef DEBUGANY
        {//!!debug  #include "stdio.h" #include "stdlib.h"
            FILE *f = fopen("c:\\debug.log", "at+");
            if (f)
            {
                fprintf(f, "cep 5\n");
                fclose(f);
            }
        }
#endif
        return true;
    }
#ifdef DEBUGANY
    {//!!debug  #include "stdio.h" #include "stdlib.h"
        FILE *f = fopen("c:\\debug.log", "at+");
        if (f)
        {
            fprintf(f, "cep 6\n");
            fclose(f);
        }
    }
#endif
    return false;
}

//---------------------------------------------------
int RunScript::CallRunProgram(HKEY hKey, CString SubKey, CString ReadKey, bool bUninstallSymantecOk)
//read the registry to obtain a path and program to run to perform
//the uninstall.  This is the AntiVirus' uninstall program. This will call
//WinExec and not wait for the AV program to complete
//Return 0 = success
//      <1 = on error
{
	int retVal;
    unsigned long VLen = 1024;
    char skey[1025];
    int ret;
    CString szTitle;

    //Read the Registry
    int len = SubKey.GetLength();
    strcpy(skey, SubKey.GetBuffer(len+1));
    CRegistryKey *RegKey = new CRegistryKey(hKey, (LPCTSTR)skey, FALSE);
    SubKey.ReleaseBuffer();
    if (RegKey)
    {
        RegKey->Open();
        skey[0] = 0;
        if (!RegKey->ReadValue(ReadKey, skey, &VLen))
        {
            CString szMSI( skey );

			// Make sure the uninstall file exists, and display appropriate message if it does not.
			///CFileStatus status;
			///if ( false == CFile::GetStatus((LPCTSTR)szMSI, status))
			if (0 == szMSI.GetLength())
			{  // static function
				Error(IDS_STRING1121);

				//  Indicate that the program was not found
				retVal = PROGRAM_NOT_FOUND;
				goto Done;
			}

            if ( szMSI.Find( "MsiExec.exe" ) != -1 )
            {

                int iLoc = szMSI.Find( "/" );
                szMSI = szMSI.Left( iLoc - 1 ) + " /x " + szMSI.Right( szMSI.GetLength() - ( iLoc + 2 ) ) + " /q " + UninstallParameters;

                memset( skey, '\0', sizeof( skey ) );
                BOOL bFoundSymantec = FALSE;                
                if (!RegKey->ReadValue("DisplayName", skey, &VLen))
                {
                    szTitle.Format( "%s", skey );
                    //
                    // Checking For Symantec Products
                    //
                    if ( szTitle.Find( "Symantec", 0 ) >= 0 )
                    {
                        bFoundSymantec = TRUE;
                    } else if ( szTitle.Find( "Norton", 0 ) >= 0 )
                    {
                        bFoundSymantec = TRUE;
                    }
                    szTitle.Format( "Uninstalling: %s", skey );
                } else
                {
                    szTitle = "Uninstalling Product";
                }

                if ( TRUE == bFoundSymantec )
                {
                    if ( FALSE == bUninstallSymantecOk )
                    {
						retVal = UNINSTALL_NOT_ALLOWED;
						goto Done;
                    }
                }

                int iXCoord = 300,
                iYCoord = 300, 
                iWidth  = 400,
                iHeight = 30;

                HDC hdc = GetWindowDC( NULL ); 
                if ( NULL != hdc )
                {
                    iXCoord = ( GetDeviceCaps( hdc, HORZRES ) / 2 ) - ( iWidth / 2 );
                    iYCoord = ( GetDeviceCaps( hdc, VERTRES ) / 2 ) - ( iHeight / 2 );
                    ReleaseDC( NULL, hdc );
                }

				#ifdef _CMDUI
					//  Update command line
					CString str = szTitle;
					str += "\n";
					_tprintf(str);
				#endif

                //
                // Display window indicating that something is happening...
                //
                HINSTANCE hInst = (HINSTANCE)( GetModuleHandle( (LPSTR)NULL ) );
                HWND hWnd = CreateWindow("Static", szTitle, WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_POPUP | WS_VISIBLE, iXCoord, iYCoord,
                                         iWidth, iHeight, (HWND)NULL, NULL, hInst, NULL);
                UpdateWindow( hWnd );

                //===================================================
                // AKENNED - Adding CreateProcess for MSI Uninstalls
                //
                STARTUPINFO m_SInfo;
                PROCESS_INFORMATION m_PInfo;

                ZeroMemory( &m_SInfo, sizeof( STARTUPINFO ) );
                m_SInfo.cb = sizeof( STARTUPINFO );



                BOOL bRet = ::CreateProcess( NULL,(LPTSTR)(LPCTSTR)szMSI,NULL, NULL, FALSE, 0, NULL, NULL, &m_SInfo, &m_PInfo );
                if ( TRUE == bRet )
                {
                    WaitForSingleObject( (HANDLE)m_PInfo.hProcess, INFINITE );
                }

                CloseHandle( m_PInfo.hThread );
                CloseHandle( m_PInfo.hProcess );

                //
                // Close out window indicating that something was happening...
                //
                if ( hWnd )
                {
                    DestroyWindow( hWnd );
                    hWnd = NULL;
                }

                ret = 32;
                //
                //===================================================				
            } else
            {
#if 0		  //  COMMENTED OUT TO USE CreateProcess instead of WinExec.  (See below for explaination)
				#ifdef _CMDUI
					if (!RegKey->ReadValue("DisplayName", skey, &VLen))
						szTitle.Format( "%s", skey );

					//  Update command line
					CString str = "Uninstalling... " + szTitle;
					str += "\n";
					_tprintf(str);
				#endif

                ret = WinExec( szMSI, SW_SHOW);
#else

				//  Use Create Process instead of WinExec.  Create Process is better because it returns right away and
				//  allows processing of dialog prompts before a long delay.  WinExec should only be used for legacy
				//  16-bit apps (see MS documentation)

				memset( skey, '\0', sizeof( skey ) );
				if (!RegKey->ReadValue("DisplayName", skey, &VLen))
				{
					szTitle.Format( "%s", skey );
					szTitle.Format( "Uninstalling: %s", skey );
				} else
				{
					szTitle = "Uninstalling Product";
				}

				#ifdef _CMDUI
					szTitle += "\n";
					_tprintf(szTitle);
				#endif


				//  If the /mys override switch was not set then check for 
				//  Symantec Products
				if (!bUninstallSymantecOk)
				{
					if ( szTitle.Find( "Symantec", 0 ) >= 0 ) {
						retVal = UNINSTALL_NOT_ALLOWED;
						goto Done;
					}
					else if ( szTitle.Find( "Norton", 0 ) >= 0 ) {
						retVal = UNINSTALL_NOT_ALLOWED;
						goto Done;
					}
				}


                STARTUPINFO m_SInfo;
                PROCESS_INFORMATION m_PInfo;

                ZeroMemory( &m_SInfo, sizeof( STARTUPINFO ) );
                m_SInfo.cb = sizeof( STARTUPINFO );


                BOOL bRet = ::CreateProcess( NULL,(LPTSTR)(LPCTSTR)szMSI,NULL, NULL, FALSE, 0, NULL, NULL, &m_SInfo, &m_PInfo );
                if ( TRUE == bRet )
                {
					//  Wait at least a couple of seconds and for user prompt before continuing
					DWORD dwTimeout = 2000;
					WaitForInputIdle(m_PInfo.hProcess, dwTimeout);

                    ///WaitForSingleObject( (HANDLE)m_PInfo.hProcess, INFINITE );
                }

                CloseHandle( m_PInfo.hThread );
                CloseHandle( m_PInfo.hProcess );

                ret = 32;
#endif

            }
            delete RegKey; //AKENNED
            if (ret > 31)
            {
                retVal = 0;
				goto Done;
            } else
            {
				retVal = -1;
                goto Done;
            }
        } else
        {
            retVal = -1;
			goto Done;
        }        
    } else
    {
        retVal = -1;
		goto Done;
    }
    retVal = 0;

Done:
	if (RegKey) delete RegKey;
	return retVal;
}

//---------------------------------------------------
CString RunScript::GetUninstallPath(HKEY hKey, CString SubKey, CString ReadKey)
//Read the registry to obtain the path for the uninstall
//AntiVirus program getting uninstalled
{
    unsigned long uVLen = 99;
    long VLen = 99;
    char skey[MAX_PATH];
    char skey2[MAX_PATH];
    int pos;
    int ret;
    CString RetStr;
    CString TempStr;

    //Read the Registry
    int len = SubKey.GetLength();
    strcpy(skey, SubKey.GetBuffer(len+1));
    SubKey.ReleaseBuffer();
    CRegistryKey *RegKey = new CRegistryKey(hKey, (LPCTSTR)skey, FALSE);
    if (RegKey)
    {
        RegKey->Open();
        skey[0] = 0;
        if (ReadKey.GetLength() == 0)
        {
            ret = RegKey->ReadKeyValue(skey, &VLen);
        } else
        {
            ret = RegKey->ReadValue(ReadKey, skey, &uVLen);
        }
        if (!ret)
        {
            GetShortPathName(skey, skey2, MAX_PATH);
            RetStr = skey2;
            //remove any executable file name from the end if it exists
            pos = TempStr.Find(".exe");
            if (pos > -1)
            {
                pos = TempStr.ReverseFind('\\');
                if (pos > -1)
                {
                    RetStr = RetStr.Left(pos + 1);
                }
            }

            //if the last character in not a \ add one
            pos = RetStr.ReverseFind('\\');
            if (pos != RetStr.GetLength())
            {
                RetStr = RetStr + _T("\\");
            }

        } else
        {
            RetStr = "";
			delete RegKey;
            return RetStr;
        }
        delete RegKey;
    } else
    {
        RetStr = "";
        return RetStr;
    }
    return RetStr;
}

//---------------------------------------------------
int RunScript::GetUninstallShieldProgramAndPath(HKEY hKey, CString SubKey, CString ReadKey)
//read the registry to obtain a path and program to run to perform
//the uninstall.  This is the InstallShield uninstall program's command line.
//insert the silet parameters
//Return 0 = success
//      <1 = on error
{
    unsigned long VLen = 1024;
    char skey[1025];
    CString ts, ts2, ts3, NewStr, fName, fName2;
    int pos;
    UIScript *UIT = NULL;
    UIScript *UIS = NULL;
    BOOL FoundString=FALSE;

    //Read the Registry
    int len = SubKey.GetLength();
    strcpy(skey, SubKey.GetBuffer(len+1));
    CRegistryKey *RegKey = new CRegistryKey(hKey, (LPCTSTR)skey, FALSE);
    SubKey.ReleaseBuffer();
    if (RegKey)
    {
        RegKey->Open();
        skey[0] = 0;
        if (!RegKey->ReadValue(ReadKey, skey, &VLen))
        {
            UninstallProgramName = skey;
            ReadKey.ReleaseBuffer();
            pos = UninstallProgramName.Find("-i");
            if (pos > -1)
            {
                fName = UninstallProgramName.Right(UninstallProgramName.GetLength() - (pos+2));
                pos = fName.Find(".");
                if (pos > -1)
                {
                    fName2 = fName.Left(pos);
                    fName2 = fName2 + ".new";
                } else
                {
                    goto ErrorExit;;
                }
                UIS = new UIScript();
                if (UIS)
                {
                    if (!UIS->OpenFile(fName, FALSE))
                    {
                        UIT = new UIScript();
                        if (UIT)
                        {
                            if (!UIT->OpenFile(fName2, TRUE))
                            {

                                while (!UIS->ReadString())
                                {
                                    ts = UIS->FileString;
                                    pos = ts.Find("UNINST.EXE");
                                    if (pos > -1)
                                    {
                                        pos = ts.Find("-f");
                                        if (pos > -1)
                                        {
                                            ts2 = ts.Left(pos);
                                            ts3 = ts.Right(ts.GetLength() - (pos));
                                            NewStr = ts2 + "-y -a " + ts3;
                                            UIT->WriteString(NewStr);
                                            FoundString = TRUE;
                                        } else
                                        {
                                            goto ErrorExit;
                                        }
                                    } else
                                    {
                                        UIT->WriteString(ts);
                                    }
                                }
                            } else
                            {
                                goto ErrorExit;
                            }
                        } else
                        {
                            goto ErrorExit;
                        }
                    } else
                    {
                        goto ErrorExit;
                    }
                } else
                {
                    goto ErrorExit;
                }
            } else
            {
				TraceEx ("RunScript::GetUninstallShieldProgramAndPath Exited, expected -i in string\n",NULL);
                goto ErrorExit;
            }
        } else
        {
            ReadKey.ReleaseBuffer();
            goto ErrorExit;
        }
    } else
    {
        return -1;
    }
    UIT->CloseFile();
    UIS->CloseFile();

    //Delete old script file
    if (UIS->RemoveFile(fName))
    {
        goto FileErrorExit;
    }
    //Rename New File to Old name
    if (UIT->RenameFile(fName2, fName))
    {
        goto FileErrorExit;
    }

    if (UIT)
    {
        delete UIT;
    }
    if (UIS)
    {
        delete UIS;
    }
    if (RegKey)
    {
        delete RegKey;
    }
    return 0;

    FileErrorExit:
    if (UIT)
    {
        delete UIT;
    }
    if (UIS)
    {
        delete UIS;
    }
    if (RegKey)
    {
        delete RegKey;
    }
    return -1;

    ErrorExit:

    if (UIT)
    {
        UIT->CloseFile();
        delete UIT;
    }
    if (UIS)
    {
        UIS->CloseFile();
        delete UIS;
    }
    if (RegKey)
    {
        delete RegKey;
    }
    return -1;
}

//---------------------------------------------------
int RunScript::GetUninstallProgramAndPath(HKEY hKey, CString SubKey, CString ReadKey)
//read the registry to obtain a path and program to run to perform
//the uninstall.  This is the AntiVirus' uninstall program.
//Return 0 = success
//      <1 = on error
{
    unsigned long VLen = 1024;
    char skey[1025];

    //Read the Registry
    int len = SubKey.GetLength();
    strcpy(skey, SubKey.GetBuffer(len+1));
    CRegistryKey *RegKey = new CRegistryKey(hKey, (LPCTSTR)skey, FALSE);
    SubKey.ReleaseBuffer();
    if (RegKey)
    {
        RegKey->Open();
        skey[0] = 0;
        if (!RegKey->ReadValue(ReadKey, skey, &VLen))
        {
            UninstallProgramName = skey;
        } else
        {
            return -1;
        }
        delete RegKey;
    } else
    {
        return -1;
    }
    return 0;
}

//---------------------------------------------------
int RunScript::CallSpecialRunRobot(DWORD LineCount, CString ScriptFileName, CString ReadKey, CString HoldWinName)
//Call an automated function that will help us to run
//unattended.
//All Uninstall program name, parameters, and script names must be set
//prior to calling this function
//Return 0 = success
//      <1 = on error
{
    CString RunStr;
    int ret;

    CopyOurDirectoryToTemp();

    if (SecondRunProgramName.GetLength() == 0)
    {
        if (UninstallProgramName.GetLength() == 0)
        {
            Error(IDS_STR1046);
            return -1;
        }
        if (InteractDuration == 0)
        {
            Error(IDS_STR1047);
            return -1;
        }
        RunStr.Format("%s %s%s",UninstallProgramName, UninstallParameters, UninstallScriptName);
    } else
    {
        RunStr=SecondRunProgramName;
    }
    if (!RunWinUninstall)
    {
        if (SetRegistryString(HKEY_LOCAL_MACHINE,
                              "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Nuke",
                              "ScriptFileName",
                              ScriptFileName,
                              TRUE))
        {
            return -1;
        }
        //Write the Line Number to start on after reboot
        if (SetRegistryDword(HKEY_LOCAL_MACHINE,
                             "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Nuke",
                             "LineNumber",
                             (LineCount+1),
                             TRUE))
        {
            return -1;
        }

        if (IsNT())
        {
            if (SetRegistryString(HKEY_CURRENT_USER,
                                  "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows",
                                  "run",
                                  strOurNewFileLocation,
                                  TRUE))
            {
                return -1;
            }
        } else
        {
            if (SetRegistryString(HKEY_LOCAL_MACHINE,
                                  "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
                                  "AV32",
                                  strOurNewFileLocation,
                                  TRUE))
            {
                return -1;
            }
        }
        CSpecialProgramRobot robot(RunStr, InteractDuration, strOurNewFileLocation, ReadKey, IsNT(), HoldWinName);
        enumRunRetCodes retCode = robot.RunProgram();
		if (retCode == Error_CantCreateProcess)
        {
            Error(IDS_STRING1119);
            return -1;
        }
        else if (retCode != NoError)
        {
            Error(IDS_STR1048);  // defaults to Time out error.
            return -1;
        }
    } else
    {
        ret = WinExec(RunStr, SW_SHOW);
        if (ret > 31)
        {
            return 0;
        } else
        {
            return -1;
        }
    }
    return 0;

}

/// TNN - new function.
bool RunScript::IsSymantecProduct (HKEY hKey, CString SubKey, CString ReadKey)
{
	bool bRet = false;
    unsigned long VLen = 1024;
    char skey[1025];

    //Read the Registry
    int len = SubKey.GetLength();
    strcpy(skey, SubKey.GetBuffer(len+1));
    CRegistryKey *RegKey = new CRegistryKey(hKey, (LPCTSTR)skey, FALSE);
    SubKey.ReleaseBuffer();
    if (RegKey)
    {
        RegKey->Open();
        skey[0] = 0;
        if (!RegKey->ReadValue(ReadKey, skey, &VLen))
        {
            CString szMSI( skey );
            if ( szMSI.Find( "MsiExec.exe" ) != -1 )
            {
                int iLoc = szMSI.Find( "/" );
                szMSI = szMSI.Left( iLoc - 1 ) + " /x " + szMSI.Right( szMSI.GetLength() - ( iLoc + 2 ) ) + " /q ";

                memset( skey, '\0', sizeof( skey ) );
                CString szTitle;
                if (!RegKey->ReadValue("DisplayName", skey, &VLen))
                {
                    szTitle.Format( "%s", skey );
                    //
                    // Checking For Symantec Products
                    //
					if ( szTitle.Find( "Symantec", 0 ) >= 0 ) {						
                        bRet = true;
						goto Done;
					}
                    else if ( szTitle.Find( "Norton", 0 ) >= 0 ) {						
                        bRet = true;
						goto Done;
					}
                    szTitle.Format( "Uninstalling: %s", skey );
                } 
			}
			else
			{
				//  No DisplayName, so check the string for any sign of Symantec
				if ( szMSI.Find( "Navnt", 0 ) >= 0 ) {						
                    bRet = true;
					goto Done;
				}
                else if ( szMSI.Find( "navustub", 0 ) >= 0 ) {						
                    bRet = true;
					goto Done;
				}
			}
		}
	}
Done:
	if (RegKey) delete RegKey;
	return bRet;
}


//---------------------------------------------------
int RunScript::CallRunRobot(BOOL SendCancel)
//Call an automated function that will help us to run
//unattended.
//All Uninstall program name, parameters, and script names must be set
//prior to calling this function
//Return 0 = success
//      <1 = on error
{
    CString RunStr;
    int ret;
    if (!bSecondRun)
    {


        if (InteractDuration == 0)
        {
            Error(IDS_STR1050);
            return -1;
        }
        RunStr.Format("%s %s%s",UninstallProgramName, UninstallParameters, UninstallScriptName);
    } else
    {
        RunStr = SecondRunProgramName;
    }
    if (!RunWinUninstall)
    {

		//  TNN - Check if the file to run exists.
///		CFileStatus status;
///		if ( false == CFile::GetStatus((LPCTSTR)UninstallProgramName, status))
///		{  // static function
///			Error(IDS_STRING1121);
///			return -1;
///		}

        CProgramRobot robot(RunStr, InteractDuration, SendCancel);
        enumRunRetCodes retCode = robot.RunProgram();
		if (retCode == Error_CantCreateProcess)
        {
            Error(IDS_STRING1119);
            return -1;
        }
        else if (retCode != NoError)
        {
            Error(IDS_STR1051);  // defaults to Time out error.
            return -1;
        }
    } else
    {
        ret = WinExec(RunStr, SW_SHOW);
        if (ret > 31)
        {
            return 0;
        } else
        {
            return -1;
        }
    }
    return 0;
}

//---------------------------------------------------
int RunScript::SetRegistryString(HKEY hKey, CString SubKey, CString WriteKey, CString Value, BOOL Create)
//set the specified registry setting.  Create it if necessary.
//Return 0 = success
//      <1 = on error
{
    char skey[100];
    char svalue[100];
    int len;

    len = SubKey.GetLength();
    strcpy(skey, SubKey.GetBuffer(len+1));
    SubKey.ReleaseBuffer();
    CRegistryKey *RegKey = new CRegistryKey(hKey, (LPCTSTR)skey, Create);

    len = WriteKey.GetLength();
    strcpy(skey, WriteKey.GetBuffer(len + 1));
    WriteKey.ReleaseBuffer();

    len = Value.GetLength();
    strcpy(svalue, Value.GetBuffer(len + 1));
    Value.ReleaseBuffer();

    if (RegKey)
    {
        RegKey->Open();
        if (!RegKey->WriteValue((LPCTSTR)skey, (LPTSTR)svalue) == ERROR_SUCCESS)
        {
			delete RegKey;
            return -1;
        }
    }
    delete RegKey;
    return 0;
}

//---------------------------------------------------
int RunScript::SetRegistryDword(HKEY hKey, CString SubKey, CString WriteKey, DWORD Value, BOOL Create)
//set the specified registry setting.  Create it if necessary.
//Return 0 = success
//      <1 = on error
{
    char skey[100];
    int len;

    len = SubKey.GetLength();
    strcpy(skey, SubKey.GetBuffer(len+1));
    SubKey.ReleaseBuffer();
    CRegistryKey *RegKey = new CRegistryKey(hKey, (LPCTSTR)skey, Create);

    len = WriteKey.GetLength();
    strcpy(skey, WriteKey.GetBuffer(len+1));
    WriteKey.ReleaseBuffer();

    if (RegKey)
    {
        RegKey->Open();
        if (!RegKey->WriteValue((LPCTSTR)skey, Value) == ERROR_SUCCESS)
        {
			delete RegKey;
            return -1;
        }
    }
    delete RegKey;

    return 0;
}

//---------------------------------------------------
int RunScript::ClearRegistration(HKEY hKey, CString SubKey, CString ClearKey)
//remove the specified registry key
//Return 0 = success
//      <1 = on error
{
    char skey[100];
    int len;

    len = SubKey.GetLength();
    strcpy(skey, SubKey.GetBuffer(len+1));
    SubKey.ReleaseBuffer();
    CRegistryKey *RegKey = new CRegistryKey(hKey, (LPCTSTR)skey, FALSE);

    len = ClearKey.GetLength();
    strcpy(skey, ClearKey.GetBuffer(len+1));
    ClearKey.ReleaseBuffer();

    if (RegKey)
    {
        RegKey->Open();
        if (!RegKey->DeleteValue((LPCTSTR)skey) == ERROR_SUCCESS)
        {
			delete RegKey;
            return -1;
        }
    }
    delete RegKey;
    return 0;
}

//---------------------------------------------------
int RunScript::ReadRegistryString(HKEY hKey, CString SubKey, CString ReadKey, CString &RetStr)
//read the registry to get the Run Key for After Reboot.
//Return 0 = success
//      <1 = on error
{
    unsigned long VLen = 1024;
    char skey[1025];
    long ret = 0;

    //Read the Registry
    int len = SubKey.GetLength();
    strcpy(skey, SubKey.GetBuffer(len+1));
    CRegistryKey *RegKey = new CRegistryKey(hKey, (LPCTSTR)skey, FALSE);
    SubKey.ReleaseBuffer();
    if (RegKey)
    {
        ret = RegKey->Open();
        if (ret)
        {
			delete RegKey;
            return -1;
        }
        VLen = 1024;
        skey[0] = 0;
        ret = RegKey->ReadValue(ReadKey, skey, &VLen);
        if (!ret)
        {
            RetStr=skey;
        } else
        {
			delete RegKey;
            return -1;
        }
        delete RegKey;
    } else
    {
        return -1;
    }
    return 0;
}

//---------------------------------------------------
int RunScript::CheckAndSetRegistryForSecondRun(DWORD LineCount, CString ScriptFileName, CString RunKeyName)
//Determines if a second run is needed and then writes the line number, ScriptFile Name and the
//FileToRun keys to the registry in preparation for the second run.
//Return 0 = success
//      <1 = on error
{
    BOOL NeedSecondRun = FALSE;
    int i, pos;
    HKEY hKey;
    CString filename;
    CString strTemp;

    if (LineCount < (DWORD)SS->NumEntries)
    {
        //check the rest of the tokens to see if they are comments
        i = LineCount;
        while (i < SS->NumEntries)
        {
            switch (SS->TS[i].token)
            {
            case COMMENTTOKEN :
                break;
            default :
                NeedSecondRun = TRUE;
                break;
            }
            i++;
        }
        if (NeedSecondRun)
        {
            CopyOurDirectoryToTemp();

            strTemp = ScriptFileName;
            pos = strTemp.ReverseFind('\\');
            if (pos > -1)
            {
                ScriptFileName = strOurNewPath + "\\";
                ScriptFileName += strTemp.Right(strTemp.GetLength() - (pos+1));
            }
            //Write the Script File Name
            if (SetRegistryString(HKEY_LOCAL_MACHINE,
                                  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Nuke",
                                  "ScriptFileName",
                                  ScriptFileName,
                                  TRUE))
            {
                return -1;
            }
            //Write the Line Number to start on after reboot
            if (SetRegistryDword(HKEY_LOCAL_MACHINE,
                                 "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Nuke",
                                 "LineNumber",
                                 (LineCount+1),
                                 TRUE))
            {
                return -1;
            }
            //read in the
            if (RunKeyName != "")
            {
                if (IsNT())
                {
                    if (!GetHKey("HKEY_LOCAL_MACHINE", hKey))
                    {
                        //read in the Run Key if exists read it and write it back out
                        if (ReadRegistryString(hKey, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", RunKeyName, SecondRunProgramName))
                        {
                            Error(IDS_STR1055);
                            return -1;
                        }
                        strTemp = SecondRunProgramName;
                        pos = strTemp.ReverseFind('\\');
                        if (pos > -1)
                        {
                            SecondRunProgramName = strOurNewPath + "\\";
                            SecondRunProgramName += strTemp.Right(strTemp.GetLength() - (pos+1));
                        }
                        if (ClearRegistration(hKey, "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", RunKeyName))
                        {
                            Error(IDS_STR1055);
                            return -1;
                        }
                        //Now set the registry for the second run with our filename
                        if (SetRegistryString(hKey, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", "Nuke", strOurNewFileLocation, FALSE))
                        {
                            Error(IDS_STR1052);
                            return -1;
                        }
                        //Now set the registry for the second run with the AV filename
                        if (SecondRunProgramName.GetLength() == 0)
                        {
                            if ((UninstallScriptName.GetLength() > 0) || (UninstallParameters.GetLength() > 0))
                            {
                                filename.Format("%s %s%s",UninstallProgramName, UninstallParameters, UninstallScriptName);
                            }
                        }
                        if (SetRegistryString(HKEY_LOCAL_MACHINE,
                                              "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Nuke",
                                              "SecondRunProgramName",
                                              SecondRunProgramName,
                                              TRUE))
                        {
                            Error(IDS_STR1053);
                            return -1;
                        }
                    } else
                    {
                        Error(IDS_STR1054);
                        return -1;
                    }
                } else
                {
                    if (!GetHKey("HKEY_LOCAL_MACHINE", hKey))
                    {
                        //read in the Run Key if exists read it and write it back out
                        if (ReadRegistryString(hKey, "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", RunKeyName, SecondRunProgramName))
                        {
                            Error(IDS_STR1055);
                            return -1;
                        }
                        strTemp = SecondRunProgramName;
                        pos = strTemp.ReverseFind('\\');
                        if (pos > -1)
                        {
                            SecondRunProgramName = strOurNewPath + "\\";
                            SecondRunProgramName += strTemp.Right(strTemp.GetLength() - (pos+1));
                        }
                        //Now set the registry for the second run with our filename
                        if (SetRegistryString(hKey, "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", "AV32", strOurNewFileLocation, TRUE))
                        {
                            Error(IDS_STR1056);
                            return -1;
                        }
                        //Now set the registry for the second run with the AV filename
                        if (SetRegistryString(hKey,
                                              "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Nuke",
                                              "SecondRunProgramName",
                                              SecondRunProgramName,
                                              TRUE))
                        {
                            Error(IDS_STR1057);
                            return -1;
                        }

                        //Cleanup the Registry for The AV Run key
                        if (ClearRegistration(hKey, "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", RunKeyName))
                        {
                            Error(IDS_STR1058);
                            return -1;
                        }

                    } else
                    {
                        Error(IDS_STR1059);
                        return -1;
                    }
                }
            } else
            {
                if (IsNT())
                {
                    if (!GetHKey("HKEY_CURRENT_USER", hKey))
                    {
/*  !!W removed because IBM NT was not writing to the same registry setting as was in previous version
                    //read in the Run Key if exists read it and write it back out
                        if (ReadRegistryString(hKey, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows", "run", filename)) {
                            Error("Failed To Get Second Run Program Name");
                            return -1;
                        }
*/
                        //Now set the registry for the second run with our filename
                        if (SetRegistryString(hKey, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows", "run", OurFileName, FALSE))
                        {
                            Error(IDS_STR1060);
                            return -1;
                        }
                        //check to make sure that the SetUpForSecondRun token has not been performed
                        if (SecondRunProgramName.GetLength() == 0)
                        {
                            filename.Format("%s %s%s",UninstallProgramName, UninstallParameters, UninstallScriptName);
                        }
/*  !!W removed because IBM NT was not writing to the same registry setting as was in previous version
                        filename.MakeUpper();
                        OurFileName.MakeUpper();
                        if (filename != OurFileName) {
*/
                        //Now set the registry for the second run with the AV filename
                        if (SetRegistryString(HKEY_LOCAL_MACHINE,
                                              "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Nuke",
                                              "SecondRunProgramName",
                                              filename,
                                              TRUE))
                        {
                            Error(IDS_STR1061);
                            return -1;
                        }
                    } else
                    {
                        Error(IDS_STR1063);
                        return -1;
                    }
                }
            }
        }
    }
    return 0;
}

//---------------------------------------------------
int RunScript::RestartComputer(DWORD LineCount, CString FileName)
//Set the Registry settings to specify filename and line num for second run
//set up for second run only if not last token and remaining tokens are not comments
//then reboot the computer
//Return 0 = success
//      <1 = on error
{
    CheckAndSetRegistryForSecondRun(LineCount, FileName, "");
    if (!SystemShutdown(ScriptPath))
    {
        return -1;
    }
    return 0;
}

//---------------------------------------------------
int RunScript::PauseForNotification(DWORD LineCount, CString FileName, CString RunKey)
//Set the Registry settings to specify filename and line num for second run
//set up for second run only if not last token and remaining tokens are not comments
//then pops a MessageBox
//Return 0 = success
//      <1 = on error
{
    if (!CheckAndSetRegistryForSecondRun(LineCount, FileName, RunKey))
    {
        CString tStr, tStr2;
        VERIFY(tStr.LoadString(IDS_STR1104));
        VERIFY(tStr2.LoadString(IDS_STR1105));
        MessageBox(NULL, tStr, tStr2, MB_OK);
    } else
    {
        Error(IDS_STR1063);
    }

    return 0;
}

//---------------------------------------------------
int RunScript::StopProgramForReboot(DWORD LineCount, CString FileName)
//Set the Registry settings to specify filename and line num for second run
//set up for second run only if not last token and remaining tokens are not comments
//exits
//Return 0 = success
//      <1 = on error
{
    if (!CheckAndSetRegistryForSecondRun(LineCount, FileName, ""))
    {
        return 1;
    } else
    {
        Error(IDS_STR1064);
    }

    return 0;
}

//---------------------------------------------------
int RunScript::CallSendWindowsMessage(CString WinName, CString msg)
//Send a shutdown message to the specified window name
//Return 0 = success
//      -1 = could not send message
{
    int ret = -1;
    UINT nMsg;

    nMsg = atoi(msg);
    CWindowMessage *shutdownMsg = new CWindowMessage(WinName, NULL);
    if (shutdownMsg)
    {
        if (shutdownMsg->Send(nMsg))
        {
            ret = 0;
        }
        delete shutdownMsg;
    }
    return ret;
}

//---------------------------------------------------
int RunScript::CallSendWindowsMessageAndWait(CString WinName, CString msg, int Duration)
//Send a shutdown message to the specified window name
//Return 0 = success
//      -1 = could not send message
{
    int ret = -1;
    UINT nMsg;

    nMsg = atoi(msg);
    CWindowMessageAndWait *shutdownMsg = new CWindowMessageAndWait(NULL, WinName, Duration);
    if (shutdownMsg)
    {
        if (shutdownMsg->Send(nMsg))
        {
            ret = 0;
        }
        delete shutdownMsg;
    }
    return ret;
}

//---------------------------------------------------
int RunScript::CallSendAffirmativeToWindow(CString WinName, int Duration)
//Send a shutdown message to the specified window name
//Return 0 = success
//      -1 = could not send message
{
    int ret = -1;
    CWindowMessageAffirm *affirmMsg = new CWindowMessageAffirm(NULL, WinName, Duration);
    if (affirmMsg)
    {
        if (affirmMsg->Send())
        {
            ret = 0;
        }
		else
		{
            TraceEx("CallSendAffirmativeToWindow::Error - Could not find Window %s\n",WinName);
		}
        delete affirmMsg;
    }
    return ret;
}

//---------------------------------------------------
int RunScript::CallSendAffirmativeToWindowAfterEnable(CString WinName, CString ButtonName, int Duration)
//Send a shutdown message to the specified window name
//Return 0 = success
//      -1 = could not send message
{
    int ret = -1;
    CWindowMessageAffirm *affirmMsg = new CWindowMessageAffirm(NULL, WinName, Duration);
    if (affirmMsg)
    {
        if (affirmMsg->SendAfterEnable(ButtonName))
        {
            ret = 0;
        } else
        {
        }
        delete affirmMsg;
    }
    return ret;
}


//---------------------------------------------------
int RunScript::CallSendAffirmativeToWindowWithButtonClick(CString WinName, CString ButtonName, int Duration)
//Send a clicked message to the parent dialog from the named button
//Return 0 = success
//      -1 = could not send message
{
    int ret = -1;
    CWindowMessageAffirm *affirmMsg = new CWindowMessageAffirm(NULL, WinName, Duration);
    if (affirmMsg)
    {
        if (affirmMsg->SendButtonClick(ButtonName))
        {
            ret = 0;
        } else
        {
        }
        delete affirmMsg;
    }
    return ret;
}


//---------------------------------------------------
int RunScript::CallSendTextToEditWindow(CString WinName, CString strText, int Duration)
//Send a clicked message to the parent dialog from the named button
//Return 0 = success
//      -1 = could not send message
{
    int ret = -1;
    CWindowMessageAffirm *affirmMsg = new CWindowMessageAffirm(NULL, WinName, Duration);
    if (affirmMsg)
    {
        if (affirmMsg->SendEditText(WinName, strText))
        {
            ret = 0;
        } else
        {
        }
        delete affirmMsg;
    }
    return ret;
}


//---------------------------------------------------
void RunScript::Error(const int StrId)
//report an error to the STATUS.INI file
{
    if (bFirstSyntaxError)
    {
        ErrorLog *ELog = new ErrorLog(TempPath, SS->ScriptFileName);
        if (ELog)
        {

			CString errorString;
		    VERIFY(errorString.LoadString(StrId));  
            TraceEx("RunScript::Error - %s\n",errorString);

            ELog->WriteError(StrId);

            delete ELog;
            ELog = NULL;
        }
        bFirstSyntaxError = FALSE;
    }
}

//-----------------------------------------
int RunScript::CopyOurDirectoryToTemp()
{
    int ret = 0;
    BOOL bDirectoryExists = FALSE;
    CString FileName;
    CString OrigFile;
    CString NewFile;
    CString strOrigDir;

    if (!GetCurrentDirectory(1024, strOrigDir.GetBuffer(1024)))
    {
        strOrigDir.ReleaseBuffer();
        return -5; //should never happen
    }
    strOrigDir.ReleaseBuffer();
    if (SetCurrentDirectory(strOurNewPath))
    {
        SetCurrentDirectory(strOrigDir);
        bDirectoryExists = TRUE;
    }

    if (!bDirectoryExists)
    {
        if (CreateDirectory(strOurNewPath, NULL))
        {
            bDirectoryExists = TRUE;   //created directory
        }
    }
    if (bDirectoryExists)
    {
        if (!FindFirstJetFile(ScriptPath, FileName))
        {
            do
            {
                OrigFile = ScriptPath + "\\";
                OrigFile += FileName;
                NewFile = strOurNewPath + "\\";
                NewFile += FileName;
                if (!CopyFile(OrigFile, NewFile, FALSE))
                {
                    ret = -2;
                }
            } while (!FindNextJetFile(FileName));
        } else
        {
            ret = -1;
        }
    }

    return ret;
}

//---------------------------------------------------
int RunScript::FindNextJetFile(CString &FName)
//Return 0 = success
//      <1 = on error
{
    FName = "";
    if (FindNextFile(Findfh, &Findfd))
    {
        if (Findfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY |
                                       FILE_ATTRIBUTE_SYSTEM |
                                       FILE_ATTRIBUTE_HIDDEN) )
        {
            int ret = 1;
            do
            {
                ret = FindNextJetFile(FName);
            } while (ret > 0);
            if (ret < 0)
            {
                return ret;
            }
        }
        FName = Findfd.cFileName;
        return 0;
    }
    return -1;  //none found
}

//---------------------------------------------------
int RunScript::FindFirstJetFile(CString SearchForPath, CString &FName)
//Return 0 = success
//      <1 = on error
{
    int ret = 0;
    CString strSearchFor;

    strSearchFor = SearchForPath + "\\*.*";
    FName = "";
    Findfh = FindFirstFile(strSearchFor, &Findfd);
    if (Findfh != INVALID_HANDLE_VALUE)
    {
        if (Findfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY |
                                       FILE_ATTRIBUTE_SYSTEM |
                                       FILE_ATTRIBUTE_HIDDEN) )
        {
            ret = FindNextJetFile(FName);
            if (ret < 0)
            {
                return ret;
            }
        }
        if (!ret)
        {
            FName = Findfd.cFileName;
        }
        return ret;
    }
    return -1;  //none found
}

//---------------------------------------------------
int RunScript::DeleteDirectoryAndContents(CString strPath)
{
    int ret = 0;
    CString strFileName;
    CString strFilePath;

    strFileName = strPath;
    if (!FindFirstJetFile(strPath, strFileName))
    {
        do
        {
            strFilePath = strPath + "\\";
            strFilePath += strFileName;
            if (!DeleteFile(strFilePath))
            {
                ret = -1;
            }
        } while (!FindNextJetFile(strFileName));
    }
    FindClose(Findfh);
    if (!RemoveDirectory(strPath))
    {
        ret = -2;
    }
    return ret;
}

//-----------------------------------------
CProcessor::CProcessor()
{
    bWroteToFile = FALSE; //AKENNED
    pVS = NULL;
}

//-----------------------------------------
CProcessor::~CProcessor()
{
    if (pVS)
    {
        delete pVS;
    }
}

//-----------------------------------------
int CProcessor::Error(const int StrID)
//Return 0 = success
//      <1 = on error
{
#ifdef DEBUGANY
    {//!!debug  #include "stdio.h" #include "stdlib.h"
        FILE *f = fopen("c:\\debug.log", "at+");
        if (f)
        {
            fprintf(f, "::Error strourfullpath[%s] strscriptfilename[%s]\n", strOurFullPath, strScriptFileName);
            fclose(f);
        }
    }
#endif
    ELog = new ErrorLog(strOurFullPath, strScriptFileName);
    if (ELog)
    {
		CString errorString;
	    VERIFY(errorString.LoadString(StrID));  
        TraceEx("ErrorLog::Error - %s\n",errorString);

        ELog->WriteError(StrID);
        delete ELog;
        ELog = NULL;
        return 0;
    }
    return -1;
}

//-----------------------------------------
int CProcessor::Error( const CString strErr )
//Return 0 = success
//      <1 = on error
{
#ifdef DEBUGANY
    {//!!debug  #include "stdio.h" #include "stdlib.h"
        FILE *f = fopen("c:\\debug.log", "at+");
        if (f)
        {
            fprintf(f, "::Error strourfullpath[%s] strscriptfilename[%s]\n", strOurFullPath, strScriptFileName);
            fclose(f);
        }
    }
#endif
    ELog = new ErrorLog(strOurFullPath, strScriptFileName);
    if (ELog)
    {
        ELog->WriteError( strErr, FALSE );
        delete ELog;
        ELog = NULL;
        return 0;
    }
    return -1;
}

//-----------------------------------------
int CProcessor::Success(const int StrID)
//Return 0 = success
//      <1 = on error
{
    ELog = new ErrorLog(pVS->TempPath, strScriptFileName);
    if (ELog)
    {
        ELog->WriteErrorSuccess(StrID);
        delete ELog;
        ELog = NULL;
        return 0;
    }
    return -1;
}


		
//-----------------------------------------------------------------
//  Checks for a switch in the command line and returns true
//  if it is found.  Also removes the switch from the command line.
//-----------------------------------------------------------------
bool CProcessor::CheckforCommandSwitch (CString strSwitch, CString& strCmdLine)
{
	bool bSwitchFound = false;
    int iLocation = strCmdLine.Find(strSwitch);
    if ( iLocation >= 0 )
    {
		bSwitchFound = true;
        strCmdLine = strCmdLine.Left( iLocation - 1 ) + strCmdLine.Right( ( strCmdLine.GetLength() - iLocation ) - strSwitch.GetLength() );
    } 

	return bSwitchFound;
}




//---------------------------------------------------
int CProcessor::GetCmdLine()
{
    int pos;
    CString ts,ts2;
    CString strTemp;
    char path[200] = {0};

    CWinApp *MainWin = AfxGetApp();
    if (MainWin != NULL)
    {
        lstrcpy(path, GetCommandLine());
        //
        // AKENNED - check for uninstall
        //
        CString strCmd( path );
		
		strCmd.MakeUpper();

		//  Make sure spaces don't get interpreted as script files.
		strCmd.TrimLeft();
		strCmd.TrimRight();

		//  Check for MYS switch which indicates it's ok to uninstall Symantec Products.
		m_bUninstallSymantecOk = CheckforCommandSwitch ("/MYS", strCmd);


		//  Check for IP switch which indicates to ignore platform line in script file.
		m_bIgnorePlatform = CheckforCommandSwitch ("/IP", strCmd);


		//  Check for ALL switch which indicates process all script files.
		m_bProcessAllScripts = CheckforCommandSwitch ("/ALL", strCmd);


		//  Remove any extra spaces between application and switches to prevent them as being interpreted as a filename.
		strCmd.TrimLeft();
		strCmd.TrimRight();

		lstrcpy(path, strCmd);

        ts = path;

        //remove the initial quote from the EXE name and path
        pos = ts.Find('"');
        if (pos == 0)
        {
            ts = ts.Right(ts.GetLength() - (pos + 1));
        }


        //remove the second quote from the EXE name and path
        pos = ts.Find('"');
        if (pos > -1)
        {
            //if a script name was passed in save it off
            ts2 = ts.Right(ts.GetLength() - (pos + 2));
            ts = ts.Left(pos);
        }

        //if we had a script name, save it to FileName1
        if (ts2 != "")
        {
            strFileName1 = ts2;
			strFileName1.MakeUpper();
			pos = strFileName1.Find(".AUT");
			if (pos == -1)
				strFileName1 += ".AUT";

        }
		else 
		{


			//------------------------------------------------------------------------
			//  Added this so that you can enter a single script from the command line in a DOS box.
			//  Previously if you entered AV32 Test.aut, the Test.aut script would be ignored
			//  unless the AV32 was in quotes.  For example: 
			//                 "AV32" test.aut 
			//  would work, but
			//                  AV32 test.aut
			//  would ignore the test script name and process all scripts.	
			//
			//  TedN - 12-12-03
			//------------------------------------------------------------------------
			ts.MakeUpper();
		    pos = ts.Find("AV32.EXE");  //  Locate executable name.

		    if (pos == -1)
				pos = ts.Find("\\AV32 "); //  Try again to locate executable name
				
		    if (pos == -1)
			{
				pos = ts.Find("AV32 "); //  Try again to locate executable name
				if (pos > 0)		    //  Make sure this is not in the middle of a path name
					pos = -1;
			}

	        if (pos > -1)  //  If we found executable, see if there is a single script file.
			{
				strFileName1 =  ts.Right(ts.GetLength() - pos);
			    pos = strFileName1.Find(" ");
		        if (pos > -1)
				{
					strFileName1 =  strFileName1.Right(strFileName1.GetLength() - pos);
					strFileName1.TrimLeft();
					strFileName1.MakeUpper();

					pos = strFileName1.Find(".AUT");
					if (pos == -1)
						strFileName1 += ".AUT";
				}
				else
					strFileName1 = "";

				//MessageBox (NULL, strFileName1, NULL, MB_OK);
			}
		}

        //Save off the path and filename for second run
        strOurFileName = ts;
        //add quotes if needed          //!!j 02-04
        if (strOurFileName.Left(1) == "\"")
        {
            strOurFileName = "\"" + strOurFileName;
            strOurFileName += "\"";
        }

        //strip the path out for the .EXE
        pos = ts.ReverseFind('\\');
        if (pos > -1)
        {
            strOurFullPath = ts.Left(pos);
        } else
        {
            GetCurrentDirectory(128, strOurFullPath.GetBuffer(129));
            strOurFullPath.ReleaseBuffer();
        }

        //if the script file has a path strip it out
        pos = strFileName1.ReverseFind('\\');
        if (pos > -1)
        {
            strScriptPath = strFileName1.Left(pos);
        } else
        {
            strScriptPath = strOurFullPath;
        }
        GetTempPath(1024, strOurNewPath.GetBuffer(1024));
        strOurNewPath.ReleaseBuffer();
        strOurNewPath += "jettemp";

        strOurNewFileLocation = strOurNewPath + "\\";
        strTemp = strOurFileName;
        pos = strTemp.ReverseFind('\\');
        if (pos >= 0)
        {
            strOurNewFileLocation += strTemp.Right(strTemp.GetLength() - (pos+1));
        }

		//  If user has not specified a script, exit.
		if (!m_bProcessAllScripts && (0 == strFileName1.GetLength()))
		{
			Utils::DisplayUsage();
			return -1;
		}


    } else
    {
        Error(IDS_STR1002);
        return -1;
    }
    return 0;
}

//-----------------------------------------
int CProcessor::RemoveStatusFile()
//Return 0 = success
//      <1 = on error
{
    CString pFileName;
//  int len;
//  char *pFileName;

//  filename = pVS->OurFullPath + "\\STATUS.INI";
//  len = filename.GetLength();
//  pFileName = filename.GetBuffer(len+1);
    pFileName = pVS->OurFullPath + "\\STATUS.INI";

    TRY
    {
        CFile::Remove(pFileName);
    }
    CATCH(CFileException, e) {
#ifdef _DEBUG
        afxDump << "File " << pFileName << " cannot be removed\n";
#endif
    }
    END_CATCH

    return 0;
}

//-----------------------------------------
int CProcessor::ProcessScript(ValidateScriptToRun *pVS)
//Return 0 = success
//      <1 = on error
{
    ScriptStruct SS;
    int ret = 0;
    RunScript *rsp = NULL;
    ReadScript *rs = NULL;
    CString filename;

#ifdef DEBUGANY
    {//!!debug  #include "stdio.h" #include "stdlib.h"
        FILE *f = fopen("c:\\debug.log", "at+");
        if (f)
        {
            fprintf(f, "processScript\n");
            fclose(f);
        }
    }
#endif
    //Now run the Script Reading and syntax checking since we found the
    //correct script to use
    if (!ret)
    {
        rs = new ReadScript(&SS, pVS->StartLineCount, pVS->FileName1, pVS->OurFileName,
                            pVS->OurFullPath, pVS->ScriptPath, pVS->SecondRunProgramName,
                            pVS->OurFullPath);
        if (rs)
        {

            filename = rs->OurFullPath + "\\STATUS.INI";

            if (!rs->OpenFile())
            {
                if (rs->ReadFile())
                {
                    ret = -2;
                } else
                {
                    //do syntax checking here
                    ret = rs->SyntaxCheck(&SS);
                }
                rs->CloseFile();
            } else
            {
                if (!bWroteToFile)
                {
                    bWroteToFile=TRUE;
                    Error(IDS_STR1065);
                }
                ret = -1;
            }
        } else
        {
        }
    }

    //If all Syntax checking has succeeded now run the uninstall
    if (!ret)
    {
        rsp = new RunScript(pVS->SecondRun, &SS, rs->ScriptPath, rs->TempPath, strOurNewPath, strOurNewFileLocation);
        if (rsp)
        {
            rsp->SecondRunProgramName = rs->SecondRunProgramName;
            rsp->OurFileName = rs->OurFileName;

            ret = rsp->Run(rs->StartLineCount, rs->FileName1, m_bUninstallSymantecOk, m_bIgnorePlatform);
        } else
        {
            ret = -1;
        }
    }

#ifdef DEBUGANY
    {//!!debug  #include "stdio.h" #include "stdlib.h"
        FILE *f = fopen("c:\\debug.log", "at+");
        if (f)
        {
            fprintf(f, "processscript 2\n");
            fclose(f);
        }
    }
#endif
    if (!ret)
    {
#ifdef DEBUGANY
        {//!!debug  #include "stdio.h" #include "stdlib.h"
            FILE *f = fopen("c:\\debug.log", "at+");
            if (f)
            {
                fprintf(f, "before write file\n");
                fclose(f);
            }
        }
#endif

		//  TNN - I have commented this out because it causes the program to 
		//  report that the program was not installed when it was.  For example,
		//  if the script file consists of a RUNPROGRAM command, then
		//  UninstallProgramName will not be set which results in a
		//  failure being returned from this function.
        bool bProgramStillExists = false;
        bProgramStillExists = rsp->CheckExistProgram();

        //write the success or failure to the ini file
        if (!bProgramStillExists)
        {
			#ifdef DEBUGANY
            {//!!debug  #include "stdio.h" #include "stdlib.h"
                FILE *f = fopen("c:\\debug.log", "at+");
                if (f)
                {
                    fprintf(f, "write (success)\n");
                    fclose(f);
                }
            }
			#endif
            /// TNN - This causes existing Status.ini to be overwritten...   Success(IDS_STR1066);
        } else
        {
			#ifdef DEBUGANY
            {//!!debug  #include "stdio.h" #include "stdlib.h"
                FILE *f = fopen("c:\\debug.log", "at+");
                if (f)
                {
                    fprintf(f, "write (error)\n");
                    fclose(f);
                }
            }
			#endif
            Error(IDS_STR1067);
        }
    }

    if (rsp)
    {
        delete rsp;
        rsp = NULL;
    }

    if (rs)
    {
        delete rs;
        rs = NULL;
    }

#ifdef DEBUGANY
    {//!!debug  #include "stdio.h" #include "stdlib.h"
        FILE *f = fopen("c:\\debug.log", "at+");
        if (f)
        {
            fprintf(f, "processscript 3\n");
            fclose(f);
        }
    }
#endif
    return ret;
}


//  TNN
//  Because this code only creates a file when there is an error, it is necessary
//  to force it to create a file ahead of time.  This is very convoluted, but it
//  eliminates the need to rewite the Status file handling right now.
void ValidateScriptToRun::WriteScriptHeader(CString& strPath, CString& strScriptFile)
{
    ErrorLog *ELog = new ErrorLog(strPath, strScriptFile);
    if (ELog)
        {
			CString strTitle;
		    strTitle.Format("\nScript [%s] Results:\n",strScriptFile);

	        ELog->WriteTitleToLog(strTitle);

            delete ELog;
            ELog = NULL;
        }
        bFirstSyntaxError = FALSE;
}


void ValidateScriptToRun::WriteScriptLine(CString& strPath, CString& strScriptFile, int strID)
{
    ErrorLog *ELog = new ErrorLog(strPath, "status.ini");
    if (ELog)
        {
			CString str;
		    VERIFY(str.LoadString(strID));  

			CString strTitle;
		    strTitle.Format("\nScript [%s] Results: %s\n",strScriptFile, str);

	        ELog->WriteTitleToLog(strTitle);

            delete ELog;
            ELog = NULL;
        }
}




//-----------------------------------------
int CProcessor::DoWork()
//Return 0 = success
//      <1 = on error
{
    CString strTemp;
    int ret = 0;
    BOOL bFoundFile = FALSE;
    BOOL bWroteToFile = FALSE;
    BOOL bScriptSuccess = FALSE;
    char szTPath[1025] = {0}; //AKENNED
#ifdef DEBUGANY
    {//!!debug  #include "stdio.h" #include "stdlib.h"
        FILE *f = fopen("c:\\debug.log", "at+");
        if (f)
        {
            fprintf(f, "dowork\n");
            fclose(f);
        }
    }
#endif

    if (GetCmdLine() < 0)
		return -1;

    pVS = new ValidateScriptToRun(szTPath);
    if (pVS)
    {

        //Set vars from Command Line
        if (!pVS->SetCmdLineVars(strOurFileName, strOurFullPath, strFileName1, strScriptPath))
        {

            //Check Our Registry for 2nd run
            if (pVS->CheckSecondRun())
            {
                ret = -1;
            } else
            {
                RemoveStatusFile();
                if (!pVS->SecondRun)
                {
#ifdef DEBUGANY
                    {//!!debug  #include "stdio.h" #include "stdlib.h"
                        FILE *f = fopen("c:\\debug.log", "at+");
                        if (f)
                        {
                            fprintf(f, "not second run\n");
                            fclose(f);
                        }
                    }
#endif
                    //if we didn't pass in a script file name on the command line
                    if (pVS->FileName1.GetLength() == 0)
                    {
                        //Loop and find the proper script to use
                        //if (!FindFirstNuke(strScriptFileName)) {
                        int iLocation = strOurFullPath.ReverseFind( '\\' ); //AKENNED
                        if ( iLocation != strOurFullPath.GetLength() - 1 )//AKENNED
                            strOurFullPath += "\\"; //AKENNED

                        strScriptFileName =  strOurFullPath + "*.aut"; //AKENNED
                        if ( !FindFirstNuke( strScriptFileName ) )
                        { //AKENNED
                            do
                            {

							///	#ifdef _CMDUI
							///		CString str = strScriptFileName;
							///		str += "\n";
							///		_tprintf(str);
							///	#endif

								// TNN - Insert the Name of the script file we are processing here so the 
								//       user will be able to see which messages apply to which script files. 
								pVS->WriteScriptHeader(strOurFullPath, strScriptFileName); 

                                pVS->IsPlatform = FALSE;
               ///                 pVS->ProgramNameIsGood = FALSE;
                                pVS->ProgramNameIsGood = true;          
                                pVS->FileName1 = strScriptFileName;

                                if (VALIDATE_SCRIPT_SUCCESS == pVS->OpenFile())
                                {
                                    if (VALIDATE_SCRIPT_FAILURE == pVS->ReadFile(m_bIgnorePlatform))
                                    {
                                        pVS->CloseFile();

										//  TNN - Added Error message, previously nothing was reported and there
										//  was no indication that the script had not been processed.
                                        Error( IDS_STRING1117 );

								#ifdef _CMDUI
									CString str = strScriptFileName;
									str += "     Platform mismatch.\n";
									_tprintf(str);
								#endif


                                        ret = -2;
                                    } else
                                    {
                                        pVS->CloseFile();
                                        if (pVS->IsPlatform && pVS->ProgramNameIsGood)
                                        {
                                            bFoundFile = TRUE;
#ifdef DEBUGANY
                                            {//!!debug  #include "stdio.h" #include "stdlib.h"
                                                FILE *f = fopen("c:\\debug.log", "at+");
                                                if (f)
                                                {
                                                    fprintf(f, "dw1.0\n");
                                                    fclose(f);
                                                }
                                            }
#endif
                                            pVS->FileName1 = pVS->OurFullPath + "\\" + pVS->FileName1;
                                            ret = ProcessScript(pVS);
                                            if (!ret)
                                            {
                                                bScriptSuccess = TRUE;

												//  TNN - Added to indicate success in the currently open status file.
									            pVS->WriteScriptLine(strOurFullPath, strScriptFileName, IDS_STR1066);
												#ifdef _CMDUI
													CString str = strScriptFileName;
													str += "  Uninstall called.\n";
													_tprintf(str);
												#endif

                                            } else
                                            {
                                                CString strError;
                                                time_t osBinaryTime;
                                                time( &osBinaryTime ) ;
                                                CTime dtmTime( osBinaryTime );

                                                strError.Format( "%d/%d/%d %d:%d:%d\n", 
                                                                 dtmTime.GetMonth(), 
                                                                 dtmTime.GetDay(), 
                                                                 dtmTime.GetYear(), 
                                                                 dtmTime.GetHour(), 
                                                                 dtmTime.GetMinute(),
                                                                 dtmTime.GetSecond() );
                                                Error( strError );

												#ifdef _CMDUI
													CString str = strScriptFileName;

													if (ret == PROGRAM_NOT_FOUND)
														str += "  Uninstall program not found.\n";
													else
														str += "  Skipped.\n";

													_tprintf(str);
												#endif

												/*  TNN - Removed. Since this gives no additional relevant information, and clutters the report.
                                                strError.Format( "File: %s, could not be processed correctly. Make sure product exists.\n", strScriptFileName );
                                                Error( strError );
												*/
                                                bScriptSuccess = FALSE;
                                            }
                                            //break;
                                        }
                                    }
                                } else
                                {
                                    if (!bWroteToFile)
                                    {
                                        bWroteToFile = TRUE;
#ifdef DEBUGANY
                                        {//!!debug  #include "stdio.h" #include "stdlib.h"
                                            FILE *f = fopen("c:\\debug.log", "at+");
                                            if (f)
                                            {
                                                fprintf(f, "dw1.1\n");
                                                fclose(f);
                                            }
                                        }
#endif
                                        Error(IDS_STR1069);
                                    }
                                    ret = -1;
                                    break;
                                }
                            } while ((! FindNextNuke(strScriptFileName)) && (!bScriptSuccess));
                            FindClose( Findfh ); //AKENNED
                        }
                    } else
                    {

						#ifdef _CMDUI
							CString strHeader = "\n" + pVS->FileName1;
							strHeader += "\n";
						#endif

                        if (pVS->ScriptPath.GetLength() == 0)
                        {
                            pVS->ScriptPath = pVS->OurFullPath;
                            strScriptFileName = pVS->FileName1;
                            pVS->FileName1 = pVS->ScriptPath + "\\" + pVS->FileName1;
                        }
                        bFoundFile = TRUE;
                        ret = ProcessScript(pVS);
						//  TNN - Indicate Success
						if (!ret)
						{
							//  TNN - Added to indicate success in the currently open status file.
							pVS->WriteScriptLine(strOurFullPath, pVS->FileName1, IDS_STR1066);  

							#ifdef _CMDUI
								strHeader += "   Uninstall successfully called.\n";
							#endif
						}
						#ifdef _CMDUI
						else
						{
								strHeader += "   Failed to uninstall.  See Status.INI for details.\n";
						}
						_tprintf (strHeader);
						#endif
						
                    }
                } else
                {
                    bFoundFile = TRUE;
                    ret = ProcessScript(pVS);
					//  TNN - Indicate Success
					if (!ret)
						pVS->WriteScriptLine(strOurFullPath, strScriptFileName, IDS_STR1066);  //  TNN - Added to indicate success in the currently open status file.
                }

            }
        } else
        {
            ret = -1;
        }
    } else
    {
        ret = -1;
    }

#ifdef DEBUGANY
    {//!!debug  #include "stdio.h" #include "stdlib.h"
        FILE *f = fopen("c:\\debug.log", "at+");
        if (f)
        {
            fprintf(f, "dw1.2\n");
            fclose(f);
        }
    }
#endif
    if ((!bFoundFile) && (pVS))
    {
#ifdef DEBUGANY
        {//!!debug  #include "stdio.h" #include "stdlib.h"
            FILE *f = fopen("c:\\debug.log", "at+");
            if (f)
            {
                fprintf(f, "dw1.3\n");
                fclose(f);
            }
        }
#endif
        if (!bWroteToFile)
        {
            //default to error (nothing was written and we are finished)
            Error(IDS_STR1068);
            bWroteToFile = TRUE;
#ifdef DEBUGANY
            {//!!debug  #include "stdio.h" #include "stdlib.h"
                FILE *f = fopen("c:\\debug.log", "at+");
                if (f)
                {
                    fprintf(f, "dw1.4\n");
                    fclose(f);
                }
            }
#endif
        }
        if (!bScriptSuccess)
        {
            ret = -1;
        }
    }

#ifdef DEBUGANY
    {//!!debug  #include "stdio.h" #include "stdlib.h"
        FILE *f = fopen("c:\\debug.log", "at+");
        if (f)
        {
            fprintf(f, "dw1.5\n");
            fclose(f);
        }
    }
#endif
    if (pVS)
    {
        delete pVS;
        pVS = NULL;
    }
    return ret;
}

//---------------------------------------------------
int CProcessor::FindNextNuke(CString &FName)
//Return 0 = success
//      <1 = on error
{
    FName = "";
    if (FindNextFile(Findfh, &Findfd))
    {
        if (Findfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY |
                                       FILE_ATTRIBUTE_SYSTEM |
                                       FILE_ATTRIBUTE_HIDDEN) )
        {
            int ret = 1;
            do
            {
                ret = FindNextNuke(FName);
            } while (ret > 0);
            if (ret < 0)
            {
                return ret;
            }
        }
        FName = Findfd.cFileName;
        return 0;
    }
    return -1;  //none found
}

//---------------------------------------------------
int CProcessor::FindFirstNuke(CString &FName)
//Return 0 = success
//      <1 = on error
{
    int ret = 0;
    Findfh = FindFirstFile( FName, &Findfd);
    if (Findfh != INVALID_HANDLE_VALUE)
    {
        if (Findfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY |
                                       FILE_ATTRIBUTE_SYSTEM |
                                       FILE_ATTRIBUTE_HIDDEN) )
        {
            ret = FindNextNuke(FName);
            if (ret < 0)
            {
                return ret;
            }
        }
        if (!ret)
        {
            FName = Findfd.cFileName;
        }
        return ret;
    }
    return -1;  //none found
}


