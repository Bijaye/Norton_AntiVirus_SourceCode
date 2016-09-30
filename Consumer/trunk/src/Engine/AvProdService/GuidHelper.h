#pragma once

class CGuidHelper 
{
public:
    CGuidHelper()
    {
        m_guid = GUID_NULL;
    };

    CGuidHelper(const GUID& g)
    {
        m_guid = g;
    };

    virtual ~CGuidHelper()
    {
    };

    bool operator==(const GUID& g)
    {
        return !::memcmp(&m_guid, &g, sizeof(GUID));
    };

    bool operator==(const CGuidHelper& g)
    { 
        return operator==(g.m_guid);
    };

    operator GUID&()
    { 
        return m_guid;
    };

    operator GUID*()
    { 
        return &m_guid; 
    };

    operator const GUID*() const
    { 
        return &m_guid; 
    };

    bool operator!=(const GUID& g)
    {
        return ::memcmp(&m_guid, &g, sizeof(GUID)) != 0;
    };

    bool operator!=(const CGuidHelper& g)
    {
        return operator!=(g.m_guid);
    };

    CGuidHelper& operator=(const GUID& g)
    {
        if( ::memcmp(&m_guid, &g, sizeof(GUID)) != 0 )
        {
            copy(g);
        }
        return *this;
    };
    
    CGuidHelper& operator=(const CGuidHelper& g)
    {
        if(this != &g )
        {
            copy(g.m_guid);
        }

        return *this;
    };

    bool operator<(const CGuidHelper& g1) const
    {
        RPC_STATUS status;
        return ::UuidCompare(const_cast<GUID*>(&m_guid), 
            const_cast<GUID*>(&g1.m_guid), &status)==-1;
    }
    
    bool operator>(const CGuidHelper& g1) const
    {
        return !this->operator<(g1) && ::memcmp(&g1.m_guid, 
            &m_guid, sizeof(GUID));
    }
protected:
    GUID m_guid;
    void copy(const CGuidHelper& g); 
    void copy(const GUID& g);
};

inline void CGuidHelper::copy(const CGuidHelper& g)
{
    copy(g.m_guid);
}

inline void CGuidHelper::copy(const GUID& g)
{
    ::memcpy(&m_guid, (void*)&g, sizeof(GUID));
}
