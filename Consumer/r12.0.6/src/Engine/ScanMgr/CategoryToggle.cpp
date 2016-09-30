#include "Stdafx.h"
#include "categorytoggle.h"

CCategoryToggle::CCategoryToggle(ccEraser::IFilter* pFilter)
{
    if( pFilter == NULL )
    {
        CCTRACEE(_T("CCategoryToggle::CCategoryToggle() - ERROR - Filter is NULL."));
        return;
    }

    m_spFilter = pFilter;
}

CCategoryToggle::~CCategoryToggle(void)
{
    if( m_spFilter == NULL )
    {
        CCTRACEE(_T("CCategoryToggle::~CCategoryToggle() - ERROR - Filter is NULL."));
        return;
    }

    // Walk our map and reset the filter to it's original state
    ccEraser::eResult eRes = ccEraser::Success;
    CATEGORY_MAP::iterator Iter;
    for( Iter = m_mapCategoriesToggled.begin(); Iter != m_mapCategoriesToggled.end(); Iter++ )
    {
        if( (*Iter).second == true )
        {
            // We toggled this category ON, turn it off now
            if( ccEraser::Failed(eRes = m_spFilter->RemoveAnomalyCategory((*Iter).first)) )
            {
                CCTRACEE(_T("CCategoryToggle::~CCategoryToggle() - IFilter::RemoveAnomalyCategory(%d) failed. eRes = %d"), (*Iter).first, eRes);
            }
            else
            {
                CCTRACEI(_T("CCategoryToggle::~CCategoryToggle() - Successfully turned category %d back to OFF."), (*Iter).first);
            }
        }
        else
        {
            // We toggled this category OFF, turn it off now
            if( ccEraser::Failed(eRes = m_spFilter->AddAnomalyCategory((*Iter).first)) )
            {
                CCTRACEE(_T("CCategoryToggle::~CCategoryToggle() - IFilter::AddAnomalyCategory(%d) failed. eRes = %d"), (*Iter).first, eRes);
            }
            else
            {
                CCTRACEI(_T("CCategoryToggle::~CCategoryToggle() - Successfully turned category %d back to ON."), (*Iter).first);
            }
        }
    }
}

bool CCategoryToggle::TurnOnTemporarily(ccEraser::IAnomaly::Anomaly_Category cat)
{
    if( m_spFilter == NULL )
    {
        CCTRACEE(_T("CCategoryToggle::TurnOnTemporarily() - ERROR - Filter is NULL."));
        return false;
    }

    // Get the current state of this category for the filter
    bool bInFilter = false;
    ccEraser::eResult eRes = ccEraser::Success;
    if( ccEraser::Failed(eRes = m_spFilter->InAnomalyFilter(cat, bInFilter)) )
    {
        CCTRACEE(_T("CCategoryToggle::TurnOnTemporarily() - Cannot find out if this category is in the filter or not, doing nothing. eRes = %d"), eRes);
        return false;
    }

    // If this category is not in the filter then we need to turn it on
    if( !bInFilter )
    {
        if( ccEraser::Failed(eRes = m_spFilter->AddAnomalyCategory(cat)) )
        {
            CCTRACEE(_T("CCategoryToggle::TurnOnTemporarily() - IFilter::AddAnomalyCategory(%d) failed. eRes = %d"), cat, eRes);
            return false;
        }

        CCTRACEI(_T("CCategoryToggle::TurnOnTemporarily() - Category %d is temporarily turned on."), cat);

        // Now the category is added to the filter, save it to our map so we remember to
        // turn it off
        if( m_mapCategoriesToggled.find(cat) != m_mapCategoriesToggled.end() )
        {
            // Interesting, the client previously toggled the category off temporarily so
            // when all is said and done with this class the end state for the category
            // should be set back to on therefore I will not modify the current map
            CCTRACEW(_T("CCategoryToggle::TurnOnTemporarily() - Category %d was already temporarily turned off. At the end this category will stay on."), cat);
        }
        else
            m_mapCategoriesToggled[cat] = true;
    }
    else
        CCTRACEI(_T("CCategoryToggle::TurnOnTemporarily() - Category %d was already on."), cat);

    return true;
}

bool CCategoryToggle::TurnOffTemporarily(ccEraser::IAnomaly::Anomaly_Category cat)
{
    if( m_spFilter == NULL )
    {
        CCTRACEE(_T("CCategoryToggle::TurnOff() - ERROR - Filter is NULL."));
        return false;
    }

    // Get the current state of this category for the filter
    bool bInFilter = false;
    ccEraser::eResult eRes = ccEraser::Success;
    if( ccEraser::Failed(eRes = m_spFilter->InAnomalyFilter(cat, bInFilter)) )
    {
        CCTRACEE(_T("CCategoryToggle::TurnOffTemporarily() - Cannot find out if this category is in the filter or not, doing nothing. eRes = %d"), eRes);
        return false;
    }

    // If this category is in the filter then we need to turn it off
    if( bInFilter )
    {
        if( ccEraser::Failed(eRes = m_spFilter->RemoveAnomalyCategory(cat)) )
        {
            CCTRACEE(_T("CCategoryToggle::TurnOffTemporarily() - IFilter::RemoveAnomalyCategory(%d) failed. eRes = %d"), cat, eRes);
            return false;
        }

        CCTRACEI(_T("CCategoryToggle::TurnOffTemporarily() - Category %d is temporarily turned off."), cat);

        // Now the category is removed from the filter, save it to our map so we remember to
        // turn it on
        if( m_mapCategoriesToggled.find(cat) != m_mapCategoriesToggled.end() )
        {
            // Interesting, the client previously toggled the category on temporarily so
            // when all is said and done with this class the end state for the category
            // should be set back to off therefore I will not modify the current map
            CCTRACEW(_T("CCategoryToggle::TurnOffTemporarily() - Category %d was already temporarily turned on. At the end this category will stay off."), cat);
        }
        else
            m_mapCategoriesToggled[cat] = false;
    }
    else
    {
        CCTRACEI(_T("CCategoryToggle::TurnOffTemporarily() - Category %d was already off, nothing to do."), cat);
    }

    return true;
}