#pragma once
#include <comdef.h>
#include <atlstr.h>

// #define TRACEHR_CALLS
#define TRACEHR_TASKS

/*
 *	-------------------------------------------------------------------------------
 *	This is a simple helper class that helps to simplify the CCTRACE use
 *	for error handling and improve the debugging output altogether.
 *
 *	Options:
 *		TRACEHR_CALLS -- trace function call history
 *		TRACEHR_TASKS -- trace tasks
 *
 *	-------------------------------------------------------------------------------
 *	Started by Oleg Voloschuk in 2005
 *	-------------------------------------------------------------------------------
 *	Example:
 *
 *	HRESULT Test()
 *	{
 *		TRACEHR (h);					// -- constructor:  TraceHR h(__FUNCTION__)
 *		try
 *		{
 *			h %= "Do something";		// An optional task discription.
 *			h.Verify(true, E_ACCESSDENIED); // Throws an exception, if the condition is false.
 *			h = E_INVALIDARG;			// Does not throw an exception.
 *			h << E_ABORT;				// Throws an exeption.
 *		}
 *		catch (_com_error &e)
 *		{
 *			h = e;						// Does not throw an exception.
 *		}
 *		catch (...)
 *		{
 *			h << E_FAIL;				// Prints error log and throws an exeption.
 *		}
 *		return h;
 *	}
 * -------------------------
 * Trace log:
 *
 *	-- { Test
 *	Do something
 *	** Test() error: 0x80070057 'The parameter is incorrect.'  Task: Do something
 *	** Test() error: 0x80004004 'Operation aborted'  Task: Do something
 *	-- } Test (hr=0x80004004)
 *	-------------------------------------------------------------------------------
 */

///////////////////////////////////////////////////////////////////////////////////

#ifndef UNICODE

#define TRACEHR(h)	TraceHR h(__FUNCTION__)

#else

#define TRACEHR_WIDEN2(x) L ## x
#define TRACEHR_WIDEN(x) TRACEHR_WIDEN2(x)
#define TRACEHR__WFUNCTION__ TRACEHR_WIDEN(__FUNCTION__)

#define TRACEHR(h)	TraceHR h(TRACEHR__WFUNCTION__)

#endif

///////////////////////////////////////////////////////////////////////////////////

class TraceHR
{

// Prevent canonical behaviour
private:
	TraceHR(void);
    TraceHR(const TraceHR&);
    TraceHR& operator =(const TraceHR&);

///////////////////////////////////////////////////////////////////////////////////

protected:
	HRESULT m_hr;
	LPCTSTR m_func;
	LPCTSTR m_task;

	void TraceError(_com_error& e)
	{
		m_hr = e.Error();
		LPCTSTR msg = e.ErrorMessage();
		CCTRACEE(_T("** %s() error: 0x%08x '%s'  Task: %s"), m_func, m_hr, msg, m_task);
	}

public:

///////////////////////////////////////////////////////////////////////////////////

	TraceHR(LPCTSTR func)
	{
		m_hr = S_OK;
		m_func = func;
		m_task = NULL;

#ifdef TRACEHR_CALLS
		CCTRACEI(_T("-- { %s"), m_func);
#endif
	}

///////////////////////////////////////////////////////////////////////////////////

	~TraceHR(void)
	{
#ifdef TRACEHR_CALLS
		static LPCTSTR format = _T("-- } %s (hr=0x%08X)");

		if (SUCCEEDED(m_hr))
		{
			CCTRACEI(format, m_func, m_hr);
		}
		else
		{
			CCTRACEW(format, m_func, m_hr);
		}
#endif
	}

///////////////////////////////////////////////////////////////////////////////////

	// Task description

	void operator %= (LPCTSTR task)
	{
		m_task = task;
#ifdef TRACEHR_TASKS
		CCTRACEI(_T("%s() - %s"), m_func, m_task);
#endif
	}

///////////////////////////////////////////////////////////////////////////////////

	// Warning

	void operator /= (LPCTSTR msg)
	{
		CCTRACEW(_T("%s() ! %s  Task: %s"), m_func, msg, m_task);
	}

///////////////////////////////////////////////////////////////////////////////////

#ifdef UNICODE

	void operator /= (LPCSTR msg)
	{
		CString str(msg);
		operator /= ((LPCTSTR)str);
	}

#endif

///////////////////////////////////////////////////////////////////////////////////

	HRESULT operator = (HRESULT hr)
	{
		m_hr = hr;
		if (FAILED(hr))
		{
			TraceError(_com_error(hr));
		}
		return hr;
	}

///////////////////////////////////////////////////////////////////////////////////

	HRESULT operator << (HRESULT hr)
	{
		m_hr = hr;
		if (FAILED(hr))
		{
			_com_error e(hr);
			TraceError(e);
			throw e;
		}
		return hr;
	}

///////////////////////////////////////////////////////////////////////////////////

	_com_error& operator = (_com_error& e)
	{
		if (m_hr != e.Error())
		{
			TraceError(e);
		}
		return e;
	}

///////////////////////////////////////////////////////////////////////////////////

	void operator << (_com_error& e)
	{
		if (m_hr != e.Error())
		{
			TraceError(e);
		}
		throw e;
	}

///////////////////////////////////////////////////////////////////////////////////

	operator const HRESULT()
	{
		return m_hr;
	}

///////////////////////////////////////////////////////////////////////////////////

	void Verify(bool condition, HRESULT hFail = E_ABORT, LPCTSTR msg = NULL)
	{
		if (!condition)
		{
			if (msg != NULL)
				operator /= (msg);

			operator << (hFail);
		}
	}

///////////////////////////////////////////////////////////////////////////////////

	void Verify(BOOL condition, HRESULT hFail = E_ABORT, LPCTSTR msg = NULL)
	{
		if (!condition)
		{
			if (msg != NULL)
				operator /= (msg);

			operator << (hFail);
		}
	}

///////////////////////////////////////////////////////////////////////////////////
// SYMINTERFACE support

	typedef unsigned long SYMRESULT;	// Forward declaration if SYMINTERFACE.H is not included yet

	SYMRESULT operator << (SYMRESULT hr)
	{
		return (SYMRESULT)(operator << ((HRESULT)hr));
	}

	SYMRESULT operator = (SYMRESULT hr)
	{
		return (SYMRESULT)(operator = ((HRESULT)hr));
	}

///////////////////////////////////////////////////////////////////////////////////

};

