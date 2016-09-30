////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CEventTest.h"
#include <atlstr.h>

const wchar_t * version = L"\t-------------------------VERSION 1.2----------------------------\n";
const wchar_t * copyRight =  L"\t**************************************************************\n"
                             L"\t* PROPRIETARY/CONFIDENTIAL.Use of this product is subject to *\n"
                             L"\t* license terms.Copyright 2004,2005 Symantec Corporation     *\n"
                             L"\t* All rights reserved.                                       *\n"                           
                             L"\t**************************************************************\n";

#define RETURNBOOL(EXP,MSG) if(EXP){\
                                _tprintf(L"\t" MSG L" succeeds\n");\
                                flag = true;}\
                            else {\
                                _tprintf(L"\t" MSG L" Fails\n");\
                                return false;}
#define SENDEVENT(EXP,MSG,INDX) if(CString(argv[nEventIndx]).CompareNoCase(_T(MSG))==0 && 0!=INDX){\
                                    for(int i=0;i<nTimes;i++)\
                                    {\
                                        if (EXP){\
                                          _tprintf(L"\t" L##MSG L" succeeds\n");\
                                          flag = true;}\
                                        else {\
                                          _tprintf(L"\t" L##MSG L" Fails\n");\
                                          return false;}\
                                    }\
                                }
bool ParseCommandLineArgs(int argc, _TCHAR* argv[], int &nEventIndx, 
                          int &nStringIndx,int &nStringIndx2, int &nTimesIndex, int& nValueIndx, int& nLengthIndx);
bool SendEvent(_TCHAR* argv[], 
               int nEventIndx, int nStringIndx,int nStringIndx2, int nTimesIndx, int nValueIndx, int nLengthIndx);
void PrintUsage();



int _tmain(int argc, _TCHAR* argv[])
{
    //Inialize COM
    CoInitialize(NULL);
    ///============================================================================
   int nEventIndx = 0, nStringIndx = 0,nStringIndx2 = 0, nTimesIndx = 0, nValueIndx = 0, nLengthIndx = 0;

   try{
    if (ParseCommandLineArgs(argc, argv, nEventIndx, nStringIndx,nStringIndx2, nTimesIndx, nValueIndx, nLengthIndx))
    {
        if (!SendEvent(argv, nEventIndx, nStringIndx,nStringIndx2,nTimesIndx, nValueIndx, nLengthIndx)){
            _tprintf(_T("\tSomething went wrong, Please check the arguements first!\n"));
            //Uninitialize com
            CoUninitialize();
            exit(1);
        }
            _tprintf(_T("\tFinish sending events \n"));
    }
    else
        PrintUsage();
        //Uninitialize com
        CoUninitialize();
        exit(1);
        }
    catch (std::exception& e){
        _tprintf(L"we got std exception!!, %s \n",e.what());
        CoUninitialize();
        exit(1);
    }
    catch (...){
        _tprintf(L"we got Unknown exception!! \n");
        CoUninitialize();
        //exit(1);
        throw;
    }
    //Uninitialize com
    CoUninitialize();
    exit(0);
}

bool ParseCommandLineArgs(int argc, _TCHAR* argv[], 
                          int &nEventIndx, int &nStringIndx,int &nStringIndx2, int &nTimesIndx, 
                          int& nValueIndx, int& nLengthIndx)
{
   if (argc < 3) {
      return false;
   }
   int i = 1;
   while (i < argc) {
      if (_tcscmp(argv[i], _T("-e")) == 0 && (i != argc-1)) {
         nEventIndx = ++i;
      } else if (_tcscmp(argv[i], _T("-s")) == 0 && (i != argc-1)) {
         nStringIndx = ++i;
      } else if (_tcscmp(argv[i], _T("-s2")) == 0 && (i != argc-1)) {
         nStringIndx2 = ++i;
      } else if (_tcscmp(argv[i], _T("-t")) == 0 && (i != argc-1)) {
         nTimesIndx = ++i;
      } else if (_tcscmp(argv[i], _T("-v")) == 0 && (i != argc-1)) {
         nValueIndx = ++i;
      } else if (_tcscmp(argv[i], _T("-l")) == 0 && (i != argc-1)) {
         nLengthIndx = ++i;
      }  else {
         return false;
      }
      ++i;
   }
   return (nEventIndx != 0);
}
void PrintUsage()
{
   _tprintf(copyRight);
   _tprintf(version);
   _tprintf(_T("\tUsage: ClientEvtGen.exe -e eventName -s StringValue -t ExecutionTimes\n"));
   _tprintf(_T("\tExample: ClientEvtGen.exe -e addAuthUser -s \"Test\" -t 5 \n")); 
   _tprintf(_T("\tOptions:-e startSymProtect \n")); 
   _tprintf(_T("\t        -e stopSymProtect \n")); 
   _tprintf(_T("\t        -e addAuthUser -s [user name]\n")); 
   _tprintf(_T("\t        -e getAuthUserCount \n")); 
   _tprintf(_T("\t        -e getAuthUserName -s [index] \n")); 
   _tprintf(_T("\t        -e deleteAuthUser -s [user name]\n")); 
   _tprintf(_T("\t        -e addAuthDir -s [Directory]\n")); 
   _tprintf(_T("\t        -e addAuthFile -s [File location]\n")); 
   _tprintf(_T("\t        -e deleteAuthLocation -s [location]\n")); 
   _tprintf(_T("\t        -e getAuthLocationCount \n")); 
   _tprintf(_T("\t        -e getAuthLocationName -s [index] \n")); 
   _tprintf(_T("\t        -e addAuthMD5Hash -s [HashName] -v [HashValue] -l [FileLength]\n")); 
   _tprintf(_T("\t        -e addAuthSHA1Hash -s [HashName] -v [HashValue] -l [FileLength]\n")); 
   _tprintf(_T("\t        -e deleteAuthHash -s [HashName]\n")); 
   _tprintf(_T("\t        -e getHashCount \n")); 
   _tprintf(_T("\t        -e getInstallSecurityLevel \n")); 
   _tprintf(_T("\t        -e getAuthorizedMSICount \n")); 
   _tprintf(_T("\t        -e addMSILocation -s [location] [-s2 [location]]\n")); 
   _tprintf(_T("\t        -e addMSIGUIDLocation -s [GUID] -s2 [Location]\n")); 
   _tprintf(_T("\t        -e addMSIGUID -s [GUID(string)] [-s2 [GUID(string)]]\n")); 
   _tprintf(_T("\t        -e deleteMSILocation -s [location]\n")); 
   _tprintf(_T("\t        -e deleteMSIGUID -s [GUID(string)]\n")); 
   _tprintf(_T("\t        -e addDevice -s [device Number]\n")); 
   _tprintf(_T("\t        -e deleteDevice -s [device Number]\n")); 
   _tprintf(_T("\t        -e getDeviceStatus -s [device Number]\n")); 
   _tprintf(_T("\t        -e refreshManifests \n")); 
   _tprintf(_T("\t        -e updateEvent \n")); 
   _tprintf(_T("\t        -e getSymProtectStatus \n")); 
   _tprintf(_T("\t        -e enableFileProtection \n")); 
   _tprintf(_T("\t        -e enableFileProtectionLogOnly \n")); 
   _tprintf(_T("\t        -e disableFileProtection \n")); 
   _tprintf(_T("\t        -e enableRegistryProtection \n")); 
   _tprintf(_T("\t        -e enableRegistryProtectionLogOnly \n")); 
   _tprintf(_T("\t        -e disableRegistryProtection \n")); 
   _tprintf(_T("\t        -e enableProcessProtection \n")); 
   _tprintf(_T("\t        -e enableProcessProtectionLogOnly \n")); 
   _tprintf(_T("\t        -e disableProcessProtection \n")); 
   _tprintf(_T("\t        -e enableNamedObjectProtection \n")); 
   _tprintf(_T("\t        -e enableNamedObjectProtectionLogOnly \n")); 
   _tprintf(_T("\t        -e disableNamedObjectProtection \n")); 
   _tprintf(_T("\t        -e getFileProtectionLevel \n")); 
   _tprintf(_T("\t        -e getRegistryProtectionLevel \n")); 
   _tprintf(_T("\t        -e getProcessProtectionLevel \n")); 
   _tprintf(_T("\t        -e getNamedObjectProtectionLevel \n"));
   _tprintf(_T("\t        -e validateSettings \n")); 
   _tprintf(_T("\t        -e configurationUpdate \n")); 
   _tprintf(_T("\t        -e forceRefresh \n")); 
   _tprintf(_T("\t        -e sendSystemNotify \n")); 
   _tprintf(_T("\t        -e sendOldStartEvent \n")); 
   _tprintf(_T("\t        -e sendOldStopEvent \n")); 
   _tprintf(_T("\t        -e isSymProtectTemporarilyDisabled \n"));
   _tprintf(_T("\t        -e getDisabledTimeRemaining \n"));
   _tprintf(_T("\t        -e getSymProtectComponentState \n"));
   _tprintf(_T("\t        -e disableSymProtect -s [minutes]\n"));
   _tprintf(_T("\t        -e cancelDisableSymProtect\n"));
   _tprintf(_T("\t        -e setSymProtectComponentState -s [enable or disable]\n"));
   _tprintf(_T("\t        -e addNotificationFilter -s [path]\n"));
   _tprintf(_T("\t        -e deleteNotificationFilter -s [path]\n"));
   _tprintf(_T("\t        -e getNotificationFilterCount\n"));
   _tprintf(_T("\t        -e getNotificationFilter -s [filterIndex]\n"));
   _tprintf(_T("\t        -e setInstallSecurityLevel -v [1(ALLOW_NONE),2(ALLOW_ALL),\
               \t                            3(ALLOW_AUTHORIZED)]\n"));
}
bool SendEvent(_TCHAR* argv[], 
               int nEventIndx, int nStringIndx,int nStringIndx2, int nTimesIndx, int nValueIndx, int nLengthIndx)
{
    CEventTest ct;
    bool flag = false;
    int nTimes = _wtoi(argv[nTimesIndx]);
    if (0 == nTimes)
        nTimes = 1;
    SENDEVENT(ct.SendStartSymProtect(),"startSymProtect",1);
    SENDEVENT(ct.SendStopSymProtect(),"stopSymProtect",1);
    SENDEVENT(ct.SendAddUser(argv[nStringIndx]),"addAuthUser",nStringIndx);
    SENDEVENT(ct.SendGetUserCount(),"getAuthUserCount",1);
    SENDEVENT(ct.SendGetLocationCount(),"getAuthLocationCount",1);
    SENDEVENT(ct.SendGetLocationName(_wtol(argv[nStringIndx])),"getAuthLocationName",nStringIndx);
    SENDEVENT(ct.SendGetUserName(_wtol(argv[nStringIndx])),"getAuthUserName",nStringIndx);
    SENDEVENT(ct.SendDeleteUser(argv[nStringIndx]),"deleteAuthUser",nStringIndx);
    SENDEVENT(ct.SendAddLocation(argv[nStringIndx],true),"addAuthFile",nStringIndx);
    SENDEVENT(ct.SendAddLocation(argv[nStringIndx],false),"addAuthDir",nStringIndx);
    SENDEVENT(ct.SendDeleteLocation(argv[nStringIndx]),"deleteAuthLocation",nStringIndx);
    SENDEVENT(ct.SendAddMD5Hash(argv[nStringIndx],argv[nValueIndx], _wtol(argv[nLengthIndx])),"addAuthMD5Hash",nStringIndx);
    SENDEVENT(ct.SendGetHashCount(),"getHashCount",1);
    SENDEVENT(ct.sendSetInstallSecurityLevel(_wtoi(argv[nValueIndx])),"setInstallSecurityLevel",nValueIndx);
    SENDEVENT(ct.sendQueryInstallSecurityLevel(),"getInstallSecurityLevel",1);
    SENDEVENT(ct.sendQueryMSIAuthorizationCount(),"getAuthorizedMSICount",1);
    SENDEVENT(ct.SendAddSHA1Hash(argv[nStringIndx],argv[nValueIndx], _wtol(argv[nLengthIndx])),"addAuthSHA1Hash",nStringIndx);
    SENDEVENT(ct.SendDeleteHash(argv[nStringIndx]),"deleteAuthHash",nStringIndx);
    SENDEVENT(ct.SendAddMSIGUIDLocation(argv[nStringIndx],argv[nStringIndx2]),"addMSIGUIDLocation",nStringIndx);
    SENDEVENT(ct.SendDeleteMSILocation(argv[nStringIndx]),"deleteMSILocation",nStringIndx);
    SENDEVENT(ct.SendDeleteMSIGUID(argv[nStringIndx]),"deleteMSIGUID",nStringIndx);
    SENDEVENT(ct.SendAddDeviceEnabled(_wtol(argv[nStringIndx])),"addDevice",nStringIndx);
    SENDEVENT(ct.SendAddDeviceDisabled(_wtol(argv[nStringIndx])),"deleteDevice",nStringIndx);
    SENDEVENT(ct.SendGetDeviceStatus(_wtol(argv[nStringIndx])),"getDeviceStatus",nStringIndx);
    SENDEVENT(ct.SendRefreshManifests(),"refreshManifests",1);
    SENDEVENT(ct.SendUpdateEvent(),"updateEvent",1);
    SENDEVENT(ct.SendGetSymProtectStatus(),"getSymProtectStatus",1);
    SENDEVENT(ct.SendSetFileProtection(true,false),"enableFileProtection",1);
    SENDEVENT(ct.SendSetFileProtection(true,true),"enableFileProtectionLogOnly",1);
    SENDEVENT(ct.SendSetFileProtection(false,false),"disableFileProtection",1);
    SENDEVENT(ct.SendSetRegistryProtection(true,false),"enableRegistryProtection",1);
    SENDEVENT(ct.SendSetRegistryProtection(true,true),"enableRegistryProtectionLogOnly",1);
    SENDEVENT(ct.SendSetRegistryProtection(false,false),"disableRegistryProtection",1);
    SENDEVENT(ct.SendSetProcessProtection(true,false),"enableProcessProtection",1);
    SENDEVENT(ct.SendSetProcessProtection(true,true),"enableProcessProtectionLogOnly",1);
    SENDEVENT(ct.SendSetProcessProtection(false,false),"disableProcessProtection",1);
	SENDEVENT(ct.SendSetNamedObjectProtection(true,false),"enableNamedObjectProtection",1);
    SENDEVENT(ct.SendSetNamedObjectProtection(true,true),"enableNamedObjectProtectionLogOnly",1);
    SENDEVENT(ct.SendSetNamedObjectProtection(false,false),"disableNamedObjectProtection",1);
    SENDEVENT(ct.SendQueryFileProtectionLevel(),"getFileProtectionLevel",1);
    SENDEVENT(ct.SendQueryRegistryProtectionLevel(),"getRegistryProtectionLevel",1);
    SENDEVENT(ct.SendQueryProcessProtectionLevel(),"getProcessProtectionLevel",1);
	SENDEVENT(ct.SendQueryNamedObjectProtectionLevel(),"getNamedObjectProtectionLevel",1);
    SENDEVENT(ct.SendSettingsValidation(),"validateSettings",1);
    SENDEVENT(ct.SendConfigurationUpdate(),"configurationUpdate",1);
    SENDEVENT(ct.SendForceRefreshEvent(),"forceRefresh",1);
    SENDEVENT(ct.SendSystemNotify(),"sendSystemNotify",1);
    SENDEVENT(ct.SendOldStartEvent(),"sendOldStartEvent",1);
    SENDEVENT(ct.SendOldStopEvent(),"sendOldStopEvent",1);
	SENDEVENT(ct.SendIsSymProtectTemporarilyDisabled(),"isSymProtectTemporarilyDisabled",1);
	SENDEVENT(ct.SendGetDisabledTimeRemaining(),"getDisabledTimeRemaining",1);
	SENDEVENT(ct.SendGetSymProtectComponentState(),"getSymProtectComponentState",1);
	SENDEVENT(ct.SendDisableSymProtect(_wtoi(argv[nStringIndx])),"disableSymProtect",nStringIndx);
	SENDEVENT(ct.SendCancelDisableSymProtect(),"cancelDisableSymProtect",1);
	SENDEVENT(ct.SendSetSymProtectComponentState(argv[nStringIndx]),"setSymProtectComponentState",nStringIndx);
    SENDEVENT(ct.SendAddNotificationFilter(argv[nStringIndx]), "addNotificationFilter", nStringIndx);
    SENDEVENT(ct.SendDeleteNotificationFilter(argv[nStringIndx]), "deleteNotificationFilter", nStringIndx);
    SENDEVENT(ct.SendGetNotificationFilterCount(), "getNotificationFilterCount", 1);
    SENDEVENT(ct.SendGetNotificationFilter(_wtol(argv[nStringIndx])), "getNotificationFilter", nStringIndx);

    if (CString(argv[nEventIndx]).CompareNoCase(_T("addMSILocation")) == 0 ){
        for (int i= 0 ; i< nTimes ; i++){
                if (nStringIndx2 == 0){
                    RETURNBOOL(ct.SendAddMSILocation(argv[nStringIndx],NULL),L"addMSILocation");}
                else 
                    RETURNBOOL(ct.SendAddMSILocation(argv[nStringIndx],argv[nStringIndx2]),L"addMSILocation");
            }
    }
    if (CString(argv[nEventIndx]).CompareNoCase(_T("addMSIGUID")) == 0 ){
        for (int i= 0 ; i< nTimes ; i++)
            {
                if (nStringIndx2 == 0){
                    RETURNBOOL(ct.SendAddMSIGUID(argv[nStringIndx],NULL),L"addMSIGUID");}
                else 
                    RETURNBOOL(ct.SendAddMSIGUID(argv[nStringIndx],argv[nStringIndx2]),L"addMSIGUID");
            }
    }
    if (!flag){
        _tprintf(L"\tNo input options match, please check your parameter!! \n");
        return false;}
    else
       return true;
}