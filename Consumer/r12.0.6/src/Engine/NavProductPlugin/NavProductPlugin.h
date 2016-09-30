// NavProductPlugin.h: interface for the CNavProductPlugin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVPRODUCTPLUGIN_H__9FC470AF_C12A_420B_ABBE_BCA6BB400B2F__INCLUDED_)
#define AFX_NAVPRODUCTPLUGIN_H__9FC470AF_C12A_420B_ABBE_BCA6BB400B2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CNavProductPlugin : 	
	public ISymBaseImpl< CSymThreadSafeRefCount >, 
	public ICCAppProductPlugin
{
public:
	CNavProductPlugin();
	virtual ~CNavProductPlugin();

	SYM_INTERFACE_MAP_BEGIN()                
		SYM_INTERFACE_ENTRY( IID_CCAppProductPlugin, ICCAppProductPlugin ) 
	SYM_INTERFACE_MAP_END()                  

	//
	// ICCAppProductPlugin methods
	//
	virtual SYMRESULT VerifyProduct();
	virtual unsigned int GetPluginDLLCount();
	virtual const char * GetPluginDLLPath( unsigned int uIndex );
	virtual const char * GetProductName();

protected:
	void buildPluginList();
	void reportFatalError(UINT uMessageID);
	void getNAVGUID();
	bool areNAVFilesRemoved();

private:
	// The list of plugins
    ::std::vector<::std::string>		m_vPlugins;
	
	// Static arrays of dll names.
	static char*		m_sNAVPluginDllNames[];
    static char*		m_sNAVOptionalPluginDllNames[];
	static char*		m_sCommonPluginDllNames[];
	char				m_sNAVGUID[MAX_PATH];
};

#endif // !defined(AFX_NAVPRODUCTPLUGIN_H__9FC470AF_C12A_420B_ABBE_BCA6BB400B2F__INCLUDED_)
