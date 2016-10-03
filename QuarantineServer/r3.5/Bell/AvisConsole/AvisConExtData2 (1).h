#ifndef __AVISCONEXTDATA_H_
#define __AVISCONEXTDATA_H_
#include "resource.h"
#include <atlsnap.h>
#include "AvisConsole.h"
//#include "AvisCon.h"

#include "AttributeAccess.h"

//#include "PlatformAttributesPage.h"
#include "SampleAttributesPage.h"
#include "SampleActionsPage.h"


//static int g_ExtDataClassTotalCount = 0;
extern int g_ExtDataClassTotalCount;



//#include <afxdlgs.h>	//jhill


class CAvisConExtData : public CSnapInItemImpl<CAvisConExtData, TRUE>
{
public:
	static const GUID* m_NODETYPE;
	static const OLECHAR* m_SZNODETYPE;
	static const OLECHAR* m_SZDISPLAY_NAME;
	static const CLSID* m_SNAPIN_CLASSID;

	//CComPtr<IControlbar> m_spControlBar;

	BEGIN_SNAPINCOMMAND_MAP(CAvisConExtData, FALSE)
        // TOOLBAR
        SNAPINCOMMAND_ENTRY(ID_SUBMIT_TO_AVIS, OnSubmitItem )
        SNAPINCOMMAND_ENTRY(ID_DELIVER_DEFS_FROM_AVIS, OnDeliverItem )
        // MENUS
        SNAPINCOMMAND_ENTRY(ID_TASK_SUBMITITEMFORAUTOMATICANALYSIS     , OnSubmitItem)
        SNAPINCOMMAND_ENTRY(ID_TASK_DELIVERNEWDEFINITIONSFORTHISMACHINE, OnDeliverItem)
	END_SNAPINCOMMAND_MAP()

	SNAPINMENUID(IDR_AVISCON_MENU)

	BEGIN_SNAPINTOOLBARID_MAP(CAvisConExtData)
		SNAPINTOOLBARID_ENTRY(IDR_TOOLBAR1)	    // IDR_TOOLBAR2 jhill 2/14/00
	END_SNAPINTOOLBARID_MAP()

	CAvisConExtData();
	~CAvisConExtData();

	STDMETHOD(CreatePropertyPages)(LPPROPERTYSHEETCALLBACK lpProvider,
		long handle, 
		IUnknown* pUnk,
		DATA_OBJECT_TYPES type);

	STDMETHOD(QueryPagesFor)(DATA_OBJECT_TYPES type)
	{
		//if (type == CCT_SCOPE || type == CCT_RESULT)
		if( type == CCT_RESULT )
			return S_OK;
		return S_FALSE;
	}

	IDataObject* m_pDataObject;
	virtual void InitDataClass(IDataObject* pDataObject, CSnapInItem* pDefault)
	{
		m_pDataObject = pDataObject;
		// The default code stores off the pointer to the Dataobject the class is wrapping
		// at the time. 
		// Alternatively you could convert the dataobject to the internal format
		// it represents and store that information
    	fWidePrintString("CAvisConExtData::InitDataClass called  ObjectID= %d", m_iExtDataObjectID);
	}

	CSnapInItem* GetExtNodeObject(IDataObject* pDataObject, CSnapInItem* pDefault)
	{
		// Modify to return a different CSnapInItem* pointer.
    	//fWidePrintString("CAvisConExtData::GetExtNodeObject  called  ObjectID= %d", m_iExtDataObjectID);
		return pDefault;
	}

    HRESULT OnSubmitItem( bool& bHandled, CSnapInObjectRootBase * pObj );
    HRESULT OnDeliverItem( bool& bHandled, CSnapInObjectRootBase * pObj );


    STDMETHOD(AddMenuItems)(LPCONTEXTMENUCALLBACK piCallback,
        long  *pInsertionAllowed,
		DATA_OBJECT_TYPES type);



public:
    int    m_iExtDataObjectID;


// Private data
private:
    // Flag indicating presence of a property sheet this item.
    BOOL                       m_bPropPage;
    BOOL                       m_bAreWeFirstInstance;

//    CPlatformAttributesPage*   m_pPlatformAttributesPage;
    CSampleAttributesPage*     m_pSampleAttributesPage;
    CSampleActionsPage*        m_pSampleActionsPage;


};




#endif