#pragma once

class CCategoryToggle
{
public:
    CCategoryToggle(ccEraser::IFilter*);
    ~CCategoryToggle(void);

    bool TurnOnTemporarily(ccEraser::IAnomaly::Anomaly_Category);
    bool TurnOffTemporarily(ccEraser::IAnomaly::Anomaly_Category);

private:
    ccEraser::IFilterPtr m_spFilter;
    typedef std::map<ccEraser::IAnomaly::Anomaly_Category, bool> CATEGORY_MAP;
    CATEGORY_MAP m_mapCategoriesToggled;
};
