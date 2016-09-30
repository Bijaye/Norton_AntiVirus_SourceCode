#pragma once

class CSimpleTimer
{
public:
    CSimpleTimer(void);
    ~CSimpleTimer(void);

    void Start(void);
    DWORD Stop(void);
    
    // Sets the timer back to zero
    void Reset(void);   

    // Gets the elapsed time when it was stopped 
    // (not the current elapsed time)
    //
    DWORD GetElapsedTime(void);

protected:
    DWORD m_dwStartTime;
    DWORD m_dwElapsedTime;
    
    CRITICAL_SECTION m_crit;
};
