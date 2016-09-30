/************************************************************
 *	File:			safearray.h
 *	Purpose:		Compiler COM Support Class-style
 *                  SAFEARRAY wrapper class
 *	Author:			Ray Frederick Djajadinata
 *  Last Modified:  August 17, 1998 1:16 PM
 ***********************************************************/

#ifndef SAFEARRAY_H
#define SAFEARRAY_H

#include <tchar.h>
#include <comdef.h>
#include <stdarg.h>


/*
 *  VT_I2               [V][T][P][S][C]  2 byte signed int
 *  VT_I4               [V][T][P][S][C]  4 byte signed int
 *  VT_R4               [V][T][P][S][C]  4 byte real
 *  VT_R8               [V][T][P][S][C]  8 byte real
 *  VT_CY               [V][T][P][S][C]  currency
 *  VT_DATE             [V][T][P][S][C]  date
 *  VT_BSTR             [V][T][P][S][C]  OLE Automation string
 *  VT_DISPATCH         [V][T][P][S][C]  IDispatch *
 *  VT_BOOL             [V][T][P][S][C]  True=-1, False=0
 *  VT_VARIANT          [V][T][P][S]     VARIANT *
 *  VT_UNKNOWN          [V][T]   [S][C]  IUnknown *
 *  VT_DECIMAL          [V][T]   [S][C]  16 byte fixed point
 *  VT_UI1              [V][T][P][S][C]  unsigned char
 */

// Comment this to enable signed/unsigned mismatch warning
#pragma warning(disable:4018)
#pragma once

//***********************************************************
// This VBTYPE enum is defined so we can refer to
// VB types conveniently. Remember that:
//
// VB           => C++
// ----------------------------
// Integer      => short
// Object       => IDispatch*
// Unknown      => IUnknown*

enum VBTYPE {
	Integer = VT_I2,
	Long = VT_I4,
	Single = VT_R4,
	Double = VT_R8,
	Currency = VT_CY,
	Date = VT_DATE,
	String = VT_BSTR,
	Object = VT_DISPATCH,
	Boolean = VT_BOOL,
	Variant = VT_VARIANT,
	Unknown = VT_UNKNOWN,
	Decimal = VT_DECIMAL,
	Byte = VT_UI1
};

#if 0
//***********************************************************
// Undocumented function SafeArrayGetVartype
HRESULT (__stdcall *SafeArrayGetVartype)(SAFEARRAY* psa, VARTYPE* vt);

struct tagInitFuncPtr {
	tagInitFuncPtr() {
		// Initialize the pointer
		SafeArrayGetVartype =
			(HRESULT (__stdcall*)(tagSAFEARRAY*, unsigned short*))GetProcAddress(GetModuleHandle(_T("Oleaut32.dll")), _T("SafeArrayGetVartype"));
	}
} InitFuncPtr;
#endif


#pragma warning( disable : 4290 )

//***********************************************************
// _safearray_t declaration
template <unsigned short cDims = 1, class T = VARIANT, VBTYPE vbt = Variant>
class _safearray_t {
public:
	//-------------------------------------------------------
	// Constructors & Destructors
	_safearray_t() throw();
	_safearray_t(unsigned long lFirstDimension, ...) throw(_com_error);
	_safearray_t(const SAFEARRAY* psaSrc) throw(_com_error);
	_safearray_t(const _safearray_t& satSrc) throw(_com_error);
	_safearray_t(const VARIANT& varSrc) throw(_com_error);
	_safearray_t(const VARIANT* pvarSrc) throw(_com_error);
	~_safearray_t() throw(_com_error);

	//-------------------------------------------------------
	// Methods
	// Returns the wrapped SAFEARRAY*
	SAFEARRAY* GetRawSafeArray() const throw() {
		return m_psa;
	}

	// Redimensions an array, just like VB's ReDim Preserve
	void ReDim(long lNewRightLBound, unsigned long lNewRightElements) throw(_com_error);

	// Returns the number of elements for a specified dimension
	unsigned short GetDim() const throw() {
		return (m_psa->cDims);
	}

	// VB equivalent of UBound and LBound
	long GetUBound(unsigned short sDimensionth = 1) throw() {
		return ((m_psa->rgsabound[(m_psa->cDims)-sDimensionth]).lLbound) +
			((m_psa->rgsabound[(m_psa->cDims)-sDimensionth]).cElements) - 1;
	}
	long GetLBound(unsigned short sDimensionth = 1) throw() {
		return ((m_psa->rgsabound[(m_psa->cDims)-sDimensionth]).lLbound);
	}

	// Returns the number of elements for a specified dimension
	unsigned long GetLength(unsigned short sDimensionth = 1) throw() {
		return ((m_psa->rgsabound[(m_psa->cDims)-sDimensionth]).cElements);
	}

	// Returns the type of the array elements
	VBTYPE GetVartype() const throw() {
		return m_type;
	}

	//-------------------------------------------------------
	// Conversions
	operator SAFEARRAY*() const throw(_com_error);
	operator VARIANT() const throw(_com_error);

	//-------------------------------------------------------
	// Assignment & Indexing Operators
	_safearray_t& operator=(const SAFEARRAY* psaSrc) throw(_com_error);
	_safearray_t& operator=(const _safearray_t& satSrc) throw(_com_error);
	_safearray_t& operator=(const VARIANT& varSrc) throw(_com_error);
	_safearray_t& operator=(const VARIANT* pvarSrc) throw(_com_error);
	T& operator()(long lFirstIndex, ...) throw(_com_error);

	//-------------------------------------------------------
	// Comparison Operators
	bool operator==(const SAFEARRAY* psaSrc) throw(_com_error);
	bool operator==(const _safearray_t& satSrc) throw(_com_error);
	bool operator==(const VARIANT& varSrc) throw(_com_error);
	bool operator==(const VARIANT* pvarSrc) throw(_com_error);

	//-------------------------------------------------------
	// _safearray_t will point to an external SAFEARRAY*
	void Attach(SAFEARRAY*& saSrc) throw(_com_error);
	SAFEARRAY* Detach() throw(_com_error);

	//-------------------------------------------------------
	// Destroy the SAFEARRAY*
	void Destroy() throw(_com_error);

private:
	//-------------------------------------------------------
	// Members
	SAFEARRAY* m_psa;
	VBTYPE m_type;
};

//***********************************************************
// _safearray_t definition

//***********************************************************
// Constructors & Destructor
template <unsigned short cDims, class T, VBTYPE vbt>
_safearray_t<cDims, T, vbt>::_safearray_t() throw() {
	m_psa = NULL;
	m_type = vbt;
}

template <unsigned short cDims, class T, VBTYPE vbt>
_safearray_t<cDims, T, vbt>::_safearray_t(unsigned long lFirstDimension, ... ) throw(_com_error) {
	va_list dimensions;
	va_start(dimensions, lFirstDimension);

	SAFEARRAYBOUND rgsabound[cDims];
	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = lFirstDimension;
	for(int ctr = 1; ctr < cDims; ctr++) {
		rgsabound[ctr].lLbound = 0;
		rgsabound[ctr].cElements = va_arg(dimensions, unsigned long);
	}
	m_psa = ::SafeArrayCreate((VARTYPE)vbt, cDims, rgsabound);
	if(m_psa == NULL) {
		_com_issue_error(E_OUTOFMEMORY);
	}
	_com_util::CheckError(::SafeArrayLock(m_psa));
	m_type = vbt;
	va_end(dimensions);
	return;
}

template <unsigned short cDims, class T, VBTYPE vbt>
_safearray_t<cDims, T, vbt>::_safearray_t(const SAFEARRAY* psaSrc) throw(_com_error) {
	VARTYPE vt;
	_com_util::CheckError(::SafeArrayGetVartype(const_cast<SAFEARRAY*>(psaSrc), &vt));
	if(vt == vbt) {
		_com_util::CheckError(::SafeArrayCopy(const_cast<SAFEARRAY*>(psaSrc), &m_psa));
		_com_util::CheckError(::SafeArrayLock(m_psa));
		m_type = vbt;
	} else {
		_com_issue_error(DISP_E_TYPEMISMATCH);
	}
	return;
}

template <unsigned short cDims, class T, VBTYPE vbt>
_safearray_t<cDims, T, vbt>::_safearray_t(const _safearray_t& satSrc) throw(_com_error) {
	if(satSrc.GetVartype() == vbt) {
		_com_util::CheckError(::SafeArrayCopy(satSrc.GetRawSafeArray(), &m_psa));
		_com_util::CheckError(::SafeArrayLock(m_psa));
		m_type = vbt;
	} else {
		_com_issue_error(DISP_E_TYPEMISMATCH);
	}
	return;
}

template <unsigned short cDims, class T, VBTYPE vbt>
_safearray_t<cDims, T, vbt>::_safearray_t(const VARIANT& varSrc) throw(_com_error) {
	if((varSrc.vt & ~VT_ARRAY) == vbt) {
		_com_util::CheckError(::SafeArrayCopy(varSrc.parray, &m_psa));
		_com_util::CheckError(::SafeArrayLock(m_psa));
		m_type = vbt;
	} else {
		_com_issue_error(DISP_E_TYPEMISMATCH);
	}
	return;
}

template <unsigned short cDims, class T, VBTYPE vbt>
_safearray_t<cDims, T, vbt>::_safearray_t(const VARIANT* pvarSrc) throw(_com_error) {
	if(((pvarSrc->vt) & ~VT_ARRAY) == vbt) {
		_com_util::CheckError(::SafeArrayCopy(pvarSrc->parray, &m_psa));
		_com_util::CheckError(::SafeArrayLock(m_psa));
		m_type = vbt;
	} else {
		_com_issue_error(DISP_E_TYPEMISMATCH);
	}
	return;
}

template <unsigned short cDims, class T, VBTYPE vbt>
_safearray_t<cDims, T, vbt>::~_safearray_t() throw(_com_error) {
	if(m_psa) {
		_com_util::CheckError(::SafeArrayUnlock(m_psa));
		_com_util::CheckError(::SafeArrayDestroy(m_psa));
	}
}

//***********************************************************
// Overloaded assignment operators
template <unsigned short cDims, class T, VBTYPE vbt>
_safearray_t<cDims, T, vbt>& _safearray_t<cDims, T, vbt>::operator=(const SAFEARRAY* psaSrc) throw(_com_error) {
	VARTYPE vt;
	_com_util::CheckError(::SafeArrayGetVartype(const_cast<SAFEARRAY*>(psaSrc), &vt));
	if(m_psa) {
		_com_util::CheckError(::SafeArrayUnlock(m_psa));
		_com_util::CheckError(::SafeArrayDestroy(m_psa));
	}
	if(vt == vbt) {
		_com_util::CheckError(::SafeArrayCopy(const_cast<SAFEARRAY*>(psaSrc), &m_psa));
		_com_util::CheckError(::SafeArrayLock(m_psa));
	} else {
		_com_issue_error(DISP_E_TYPEMISMATCH);
	}
	return (*this);
}

template <unsigned short cDims, class T, VBTYPE vbt>
_safearray_t<cDims, T, vbt>& _safearray_t<cDims, T, vbt>::operator=(const _safearray_t& satSrc) throw(_com_error) {
	if(m_psa) {
		_com_util::CheckError(::SafeArrayUnlock(m_psa));
		_com_util::CheckError(::SafeArrayDestroy(m_psa));
	}
	if(satSrc.GetVartype() == m_type) {
		_com_util::CheckError(::SafeArrayCopy(satSrc.GetRawSafeArray(), &m_psa));
		_com_util::CheckError(::SafeArrayLock(m_psa));
	} else {
		_com_issue_error(DISP_E_TYPEMISMATCH);
	}
	return (*this);
}

template <unsigned short cDims, class T, VBTYPE vbt>
_safearray_t<cDims, T, vbt>& _safearray_t<cDims, T, vbt>::operator=(const VARIANT& varSrc) throw(_com_error) {
	if(m_psa) {
		_com_util::CheckError(::SafeArrayUnlock(m_psa));
		_com_util::CheckError(::SafeArrayDestroy(m_psa));
	}
	// If the variant does not contain an array, bug off...
	if((varSrc.vt & ~VT_ARRAY) == m_type) {
		_com_util::CheckError(::SafeArrayCopy(varSrc.parray, &m_psa));
		_com_util::CheckError(::SafeArrayLock(m_psa));
	} else {
		_com_issue_error(DISP_E_TYPEMISMATCH);
	}
	return (*this);
}

template <unsigned short cDims, class T, VBTYPE vbt>
_safearray_t<cDims, T, vbt>& _safearray_t<cDims, T, vbt>::operator=(const VARIANT* pvarSrc) throw(_com_error) {
	if(m_psa) {
		_com_util::CheckError(::SafeArrayUnlock(m_psa));
		_com_util::CheckError(::SafeArrayDestroy(m_psa));
	}
	if(((pvarSrc->vt) & ~VT_ARRAY) == m_type) {
		_com_util::CheckError(::SafeArrayCopy(pvarSrc->parray, &m_psa));
		_com_util::CheckError(::SafeArrayLock(m_psa));
	} else {
		_com_issue_error(DISP_E_TYPEMISMATCH);
	}
	return (*this);
}

//***********************************************************
// Overloaded indexing operators
template <unsigned short cDims, class T, VBTYPE vbt>
T& _safearray_t<cDims, T, vbt>::operator()(long lFirstIndex, ...) throw(_com_error) {
	// Check if index is within boundaries
	if( (lFirstIndex < GetLBound(1)) || (lFirstIndex > GetUBound(1)) ) {
		_com_issue_error(DISP_E_BADINDEX);
	}
	va_list indices;
	va_start(indices, lFirstIndex);
	long *prgindices = new long[m_psa->cDims];

	prgindices[0] = lFirstIndex;
	for(unsigned short ctr = 1; ctr < (m_psa->cDims); ctr++) {
		prgindices[ctr] = va_arg(indices, long);
		// Check if others are, too
		if( (prgindices[ctr] < GetLBound(ctr+1)) || (prgindices[ctr] > GetUBound(ctr+1)) ) {
			_com_issue_error(DISP_E_BADINDEX);
		}
	}
	T* pContent;
	_com_util::CheckError(::SafeArrayPtrOfIndex(m_psa, prgindices, (void**)&pContent));
	delete [] prgindices;
	va_end(indices);
	return (*pContent);
}

//***********************************************************
// Methods
template <unsigned short cDims, class T, VBTYPE vbt>
void _safearray_t<cDims, T, vbt>::ReDim(long lNewRightLBound, unsigned long lNewRightElements) throw(_com_error) {
	// If array is not static and can be resized
	// (Just like VB array declared with ReDim),
	// then go on
	if( !((m_psa->fFeatures) & FADF_STATIC) || !((m_psa->fFeatures) & FADF_FIXEDSIZE) ) {
		SAFEARRAYBOUND* prgsaNewBound = new SAFEARRAYBOUND[m_psa->cDims];
		// Only change the rightmost dimension
		// Remember that in SAFEARRAY, dimensions are stored
		// in reversed order...
		for(unsigned short ctr = m_psa->cDims - 1; ctr > 0; ctr--) {
			prgsaNewBound[ctr].lLbound = m_psa->rgsabound[ctr].lLbound;
			prgsaNewBound[ctr].cElements = m_psa->rgsabound[ctr].cElements;
		}
		prgsaNewBound[0].lLbound = lNewRightLBound;
		prgsaNewBound[0].cElements = lNewRightElements;
		// Do redim here
		// Must unlock array first
		_com_util::CheckError(::SafeArrayUnlock(m_psa));
		_com_util::CheckError(::SafeArrayRedim(m_psa, prgsaNewBound));
		_com_util::CheckError(::SafeArrayLock(m_psa));
		delete [] prgsaNewBound;
	} else {
		_com_issue_error(DISP_E_ARRAYISLOCKED);
	}
	return;
}

//***********************************************************
// Conversions
template <unsigned short cDims, class T, VBTYPE vbt>
_safearray_t<cDims, T, vbt>::operator VARIANT() const throw(_com_error) {
	VARIANT varDest;
	::VariantInit(&varDest);
	varDest.vt = (m_type | VT_ARRAY);
	SAFEARRAY* psaOut;
	_com_util::CheckError(::SafeArrayCopy(m_psa, &psaOut));
	varDest.parray = psaOut;
	return varDest;
}

template <unsigned short cDims, class T, VBTYPE vbt>
_safearray_t<cDims, T, vbt>::operator SAFEARRAY*() const throw(_com_error) {
	SAFEARRAY* psaOut;
	_com_util::CheckError(::SafeArrayCopy(m_psa, &psaOut));
	return psaOut;
}

//***********************************************************
// Comparison operators
template <unsigned short cDims, class T, VBTYPE vbt>
bool _safearray_t<cDims, T, vbt>::operator==(const SAFEARRAY* psaSrc) throw(_com_error) {
	VARTYPE vt;
	_com_util::CheckError(::SafeArrayGetVartype(const_cast<SAFEARRAY*>(psaSrc), &vt));
	// If it is of a different type, not equal
	if(vt != vbt) {
		return false;
	}

	// Do further checking
	return ::IsSafeArrayEqual(m_psa, const_cast<SAFEARRAY*>(psaSrc));
}

template <unsigned short cDims, class T, VBTYPE vbt>
bool _safearray_t<cDims, T, vbt>::operator==(const _safearray_t& satSrc) throw(_com_error) {
	if(m_type != satSrc.GetVartype()) {
		return false;
	}

	// Do further checking
	return ::IsSafeArrayEqual(m_psa, satSrc.GetRawSafeArray());
}

template <unsigned short cDims, class T, VBTYPE vbt>
bool _safearray_t<cDims, T, vbt>::operator==(const VARIANT& varSrc) throw(_com_error) {
	if((m_type | VT_ARRAY) != varSrc.vt) {
		return false;
	}

	// Do further checking
	return ::IsSafeArrayEqual(m_psa, varSrc.parray);
}

template <unsigned short cDims, class T, VBTYPE vbt>
bool _safearray_t<cDims, T, vbt>::operator==(const VARIANT* pvarSrc) throw(_com_error) {
	if((m_type | VT_ARRAY) != (pvarSrc->vt)) {
		return false;
	}

	// Do further checking
	return ::IsSafeArrayEqual(m_psa, pvarSrc->parray);
}

//***********************************************************
// Attach Detach pair
template <unsigned short cDims, class T, VBTYPE vbt>
void _safearray_t<cDims, T, vbt>::Attach(SAFEARRAY*& saSrc) throw(_com_error) {
	if(m_psa) {
		_com_util::CheckError(::SafeArrayUnlock(m_psa));
		_com_util::CheckError(::SafeArrayDestroy(m_psa));
	}
	m_psa = saSrc;
	saSrc = NULL;
	_com_util::CheckError(::SafeArrayLock(m_psa));
	return;
}

template <unsigned short cDims, class T, VBTYPE vbt>
SAFEARRAY* _safearray_t<cDims, T, vbt>::Detach() throw(_com_error) {
	SAFEARRAY* psaOut;
	if(m_psa) {
		_com_util::CheckError(::SafeArrayUnlock(m_psa));
	}
	psaOut = m_psa;
	m_psa = NULL;
	return psaOut;
}

template <unsigned short cDims, class T, VBTYPE vbt>
void _safearray_t<cDims, T, vbt>::Destroy() throw(_com_error) {
	if(m_psa) {
		_com_util::CheckError(::SafeArrayUnlock(m_psa));
		_com_util::CheckError(::SafeArrayDestroy(m_psa));
	}
	m_psa = NULL;
}


//***********************************************************
// Helper comparison function
bool IsSafeArrayEqual(SAFEARRAY* psa1, SAFEARRAY* psa2) throw(_com_error) {
	bool bEqual = false;

	// Check dimension of each
	if(psa1->cDims != psa2->cDims) {
		return false;
	}

	// Count the total elements in the array while checking bounds
	unsigned long arraysize = 1;
	for(unsigned short ctr = 0; ctr < psa1->cDims; ctr++) {
		if( (psa1->rgsabound[ctr].cElements == psa2->rgsabound[ctr].cElements) &&
			(psa1->rgsabound[ctr].lLbound == psa2->rgsabound[ctr].lLbound) ) {
			arraysize *= psa1->rgsabound[ctr].cElements;
		} else {
			return false;
		}
	}

	// Count the total bytes
	arraysize *= SafeArrayGetElemsize(psa1);

	// Access the data
	void* pData1;
	void* pData2;
	_com_util::CheckError(::SafeArrayAccessData(psa1, &pData1));
	_com_util::CheckError(::SafeArrayAccessData(psa2, &pData2));

	if(!memcmp(pData1, pData2, arraysize)) {
		bEqual = true;
	}

	// Release the array locks
	_com_util::CheckError(::SafeArrayUnaccessData(psa2));
	_com_util::CheckError(::SafeArrayUnaccessData(psa1));
	return bEqual;
}

#endif // SAFEARRAY_H
