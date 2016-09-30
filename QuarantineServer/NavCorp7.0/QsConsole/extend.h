#ifndef _EXTEND_H_
#define _EXTEND_H_

#pragma once

class CExtendComputerManagement : public CSnapInItemImpl<CExtendComputerManagement, TRUE>
{
public:
    static const GUID* m_NODETYPE;
    static const OLECHAR* m_SZNODETYPE;
    static const OLECHAR* m_SZDISPLAY_NAME;
    static const CLSID* m_SNAPIN_CLASSID;

    CExtendComputerManagement()
    {
        memset(&m_scopeDataItem, 0, sizeof(SCOPEDATAITEM));
        memset(&m_resultDataItem, 0, sizeof(RESULTDATAITEM));
        m_bPresent = FALSE;
    }

    ~CExtendComputerManagement()
    {
    }

    IDataObject* m_pDataObject;
    CSnapInItem* m_pNode;
    BOOL         m_bPresent;    

    virtual void InitDataClass(IDataObject* pDataObject, CSnapInItem* pDefault)
    {
        m_pDataObject = pDataObject;
        // The default code stores off the pointer to the Dataobject the class is wrapping
        // at the time. 
        // Alternatively you could convert the dataobject to the internal format
        // it represents and store that information
    }

    CSnapInItem* GetExtNodeObject(IDataObject* pDataObject, CSnapInItem* pDefault)
    {
        // 
        // If we are here, then we are extending this node type
        // 
        m_bPresent = TRUE;

        // Modify to return a different CSnapInItem* pointer.
        return m_pNode;
    }

    STDMETHOD(Notify)( MMC_NOTIFY_TYPE event,
        long arg,
        long param,
		IComponentData* pComponentData,
		IComponent* pComponent,
		DATA_OBJECT_TYPES type)
	{
		return m_pNode->Notify( event,
                                arg,
                                param,
		                        pComponentData,
		                        pComponent,
		                        type );
	}
};


#define SET_EXTENSION_NODE(classname, pp) \
    m_##classname.m_pNode = (CSnapInItem* )pp

#define IS_EXTENSION_PRESENT(xx, classname) \
    xx->m_##classname.m_bPresent




class CExtendNSCStatic : public CSnapInItemImpl<CExtendNSCStatic, TRUE>
{
public:
    static const GUID* m_NODETYPE;
    static const OLECHAR* m_SZNODETYPE;
    static const OLECHAR* m_SZDISPLAY_NAME;
    static const CLSID* m_SNAPIN_CLASSID;

    CExtendNSCStatic()
    {
        memset(&m_scopeDataItem, 0, sizeof(SCOPEDATAITEM));
        memset(&m_resultDataItem, 0, sizeof(RESULTDATAITEM));
        m_bPresent = FALSE;
    }

    ~CExtendNSCStatic()
    {
    }

    IDataObject* m_pDataObject;
    CSnapInItem* m_pNode;
    BOOL         m_bPresent;    

    virtual void InitDataClass(IDataObject* pDataObject, CSnapInItem* pDefault)
    {
        m_pDataObject = pDataObject;
        // The default code stores off the pointer to the Dataobject the class is wrapping
        // at the time. 
        // Alternatively you could convert the dataobject to the internal format
        // it represents and store that information
    }

    CSnapInItem* GetExtNodeObject(IDataObject* pDataObject, CSnapInItem* pDefault)
    {
        // 
        // If we are here, then we are extending this node type
        // 
        m_bPresent = TRUE;

        // Modify to return a different CSnapInItem* pointer.
        return m_pNode;
    }

    STDMETHOD(Notify)( MMC_NOTIFY_TYPE event,
        long arg,
        long param,
		IComponentData* pComponentData,
		IComponent* pComponent,
		DATA_OBJECT_TYPES type)
	{
		return m_pNode->Notify( event,
                                arg,
                                param,
		                        pComponentData,
		                        pComponent,
		                        type );
	}
};



#endif 