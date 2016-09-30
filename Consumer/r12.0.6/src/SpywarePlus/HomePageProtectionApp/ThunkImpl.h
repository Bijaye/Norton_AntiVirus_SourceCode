#ifndef __THUNKIMPL__H__
#define __THUNKIMPL__H__

#pragma once

#pragma warning( disable : 4311 )

////////////////////////////////////////////////////////////////////////////////
// The class _ThunkImpl is a renamed version of Andrew Nosenko CAuxThunk implementation.
// Thanks Andrew, it's a fantastic class!
//
// Copyright (c) 1997-2001 by Andrew Nosenko <andien@nozillium.com>,
// (c) 1997-1998 by Computer Multimedia System, Inc.,
// (c) 1998-2001 by Mead & Co Limited
//
// http://www.nozillium.com/atlaux/
// Version: 1.10.0021
//
#ifndef _M_IX86
	#pragma message("_ThunkImpl/ is implemented for X86 only!")
#endif

#pragma pack(push, 1)

template <class T>
class _ThunkImpl
{
private:

	BYTE	m_mov;			// mov ecx, %pThis
	DWORD	m_this; 		//
	BYTE	m_jmp;			// jmp func
	DWORD	m_relproc;		// relative jmp

protected:
	
	typedef void (T::*TMFP)();
	void InitThunk(TMFP method, const T* pThis)
	{
		union { DWORD func; TMFP method; } addr;
		addr.method = (TMFP)method;
		m_mov  = 0xB9;
		m_this = (DWORD)pThis;
		m_jmp  = 0xE9;
		m_relproc = addr.func - (DWORD)(this+1);

		::FlushInstructionCache(GetCurrentProcess(), this, sizeof(*this));
	}
	FARPROC GetThunk() const {
		_ASSERTE(m_mov == 0xB9);
		return (FARPROC)this; }
};
#pragma pack(pop) // _ThunkImpl

#endif