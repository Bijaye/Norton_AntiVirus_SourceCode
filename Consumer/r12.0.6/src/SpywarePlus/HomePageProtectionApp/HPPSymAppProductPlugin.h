#pragma once

class CHPPSymAppProductPlugin : 	
    public ISymBaseImpl< CSymThreadSafeRefCount >, 
    public ICCAppProductPlugin
{
public:
    CHPPSymAppProductPlugin(void);
    virtual ~CHPPSymAppProductPlugin(void);


    SYM_INTERFACE_MAP_BEGIN()                
        SYM_INTERFACE_ENTRY( IID_CCAppProductPlugin, ICCAppProductPlugin ) 
    SYM_INTERFACE_MAP_END()                  

    // ICCAppProductPlugin methods
    //
    virtual SYMRESULT VerifyProduct();
    virtual unsigned int GetPluginDLLCount();
    virtual const char * GetPluginDLLPath( unsigned int uIndex );
    virtual const char * GetProductName();


};
