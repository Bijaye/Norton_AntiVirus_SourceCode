////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "CEventTest.h"
#include <sstream>

class CEventTestOne
{
public:
    CEventTestOne(void)
    {
        if(!m_cet.Initialize())
            wprintf(L"Fail to init CeventTest member in CEventTestOne!!");
    };
    ~CEventTestOne(void){};
    // Dynamicly add 100 different users and locations 
    inline void StartOne()
    {
        std::wstringstream wstm2;
        wstm2 << L"TestUser@12345_%^&*";
        for (int i = 0;i<100;i++){
            std::wstringstream wstm1;
            wstm1 << L"c:\\tmp\\"<<i << L"\\";
            wstm2 << i;
            m_cet.SendAddUser(wstm2.str().c_str());
            wprintf(L"SendAddUser: %s\n", wstm2.str().c_str());
            m_cet.SendAddLocation(wstm1.str().c_str(),true);
            wprintf(L"SendAddlocation: %s\n", wstm1.str().c_str());
            //m_cet.SendSetSymProtectEnabled(true);
        }
        
    };
    // Add auth user and location 100 times with same content
    inline void StartTwo()
    {
        //m_cet.SendSetSymProtectEnabled(true);
        for (int i =0 ; i <100 ;i++)
        {
            m_cet.SendAddUser(L"Test");
            wprintf(L"SendAddUser:Test\n");
            m_cet.SendAddLocation(L"Test",true);
            wprintf(L"SendAddLocation:Test\n");
        }
    }
    //Stop SymProtect 
    inline void StartThree()
    {
        for(int i=0; i<2; i++){
            m_cet.SendStopSymProtect();
            wprintf(L"SendStopSymProtect \n");
            //check the status
            m_cet.SendGetSymProtectStatus();
            m_cet.SendStartSymProtect();
            wprintf(L"SendStartSymProtect \n");
            //check the status
            m_cet.SendGetSymProtectStatus();
        }
    }
    //Get Status of SymProtect
    inline void StartFour()
    {
        m_cet.SendStartSymProtect();
        wprintf(L"SendStartSymProtect \n");
        //check the status
        m_cet.SendGetSymProtectStatus();
    }
private:
    CEventTest m_cet;
};
