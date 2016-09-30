//******************************************************************************
// author: Christopher Brown
//******************************************************************************
#pragma once
#pragma warning( disable : 4311 )

#ifndef _M_IX86
	#pragma message("_ThunkImpl/ is implemented for X86 only!")
#endif

#pragma pack(push, 1)

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template <class T>
class _ThunkImpl
{
public:

    _ThunkImpl() : m_lpThunkedCode(NULL)
    {
    }

    ~_ThunkImpl()
    {
        if( m_lpThunkedCode )
            VirtualFree( m_lpThunkedCode , 0, MEM_RELEASE );
    }

protected:

    typedef void (T::*TMFP)();
    BOOL InitThunk(TMFP method, const T* pThis)
    {
        if( ! m_lpThunkedCode )
            m_lpThunkedCode = (LPTHUNKEDCODE) VirtualAlloc( 0, sizeof(THUNKEDCODE), MEM_COMMIT, PAGE_EXECUTE_READWRITE );

        _ASSERTE( m_lpThunkedCode );

        BOOL bResult = FALSE;

        if( m_lpThunkedCode )
            bResult = m_lpThunkedCode->_InitThunk( method, pThis );

        return bResult;
    }

    FARPROC GetThunk() const 
    {
        FARPROC lpResult = NULL;

        if( m_lpThunkedCode )
            lpResult = m_lpThunkedCode->_GetThunk();

        return lpResult; 
    }

private:

    typedef struct tagThunkedCode
    {
        BYTE	m_mov;			// mov ecx, %pThis
	    DWORD	m_this; 		//
	    BYTE	m_jmp;			// jmp func
	    DWORD	m_relproc;		// relative jmp

        BOOL _InitThunk(TMFP method, const T* pThis)
        {
            union { DWORD func; TMFP method; } addr;
            addr.method = (TMFP)method;

            m_mov  = 0xB9;
            m_this = (DWORD)pThis;
            m_jmp  = 0xE9;
            m_relproc = addr.func - (DWORD)(this+1);

            ::FlushInstructionCache( GetCurrentProcess(), this, sizeof(*this) );

            return TRUE;
        }

        FARPROC _GetThunk() const 
        {
            _ASSERTE(m_mov == 0xB9);
            return (FARPROC)this; 
        }

    } THUNKEDCODE, * LPTHUNKEDCODE;

    LPTHUNKEDCODE m_lpThunkedCode;

};

#pragma pack(pop) // _ThunkImpl

