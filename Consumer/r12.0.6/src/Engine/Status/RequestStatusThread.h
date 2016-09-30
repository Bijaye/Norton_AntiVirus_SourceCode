#pragma once

class CRequestStatusThreadSink
{
public:
    virtual void OnRequestStatusReceived (CEventData& edNewData) = 0;
};

class CRequestStatusThread : public ccLib::CThread
{
public:
    CRequestStatusThread(CRequestStatusThreadSink* pSink);
    virtual ~CRequestStatusThread(void);

    // CThread
    int Run (void);
    void SetData ( CEventData* pedData ); // This will be delete when the thread exits

private:
    CRequestStatusThreadSink* m_pSink;
    std::auto_ptr<CEventData> m_pedData;
    CRequestStatusThread(void){};
};
