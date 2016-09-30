// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005, Symantec Corporation, All rights reserved.


#pragma once

#ifndef __cplusplus
#error tlsimpersonate.h requires C++ to work. We recommend you covert your file to C++.
#endif

#if !defined WIN32 
#error tlsimpersonate.h is a win32 specific header
#endif

#include "impersonate.h"

///////////////////////////////////////////////////
//
// this class creates a TLS slot that is used to store
// CImpersonate class. At different places we need to toggle permission
// to be able to write the information to the correct log files
class CTlsImpersonate
{
public:
    CTlsImpersonate(bool bClearTlsSlot=true);
    ~CTlsImpersonate();

	/** Get the impersonation object used by this thread from thread local storage.
	 * This allows any code executing on the same thread to retrieve the
	 * impersonation object and switch back and forth between the original identify
	 * and the impersonated one.
	 * @return a pointer to a CImpersonate instance.
	 */
    CImpersonate* GetTlsValue();

	/** Set the impersonation object used by this thread in thread local storage.
	 * This allows any code executing on the same thread to retrieve the
	 * impersonation object and switch back and forth between the original identify
	 * and the impersonated one.
	 * For DEBUG builds, this call will assert if nested calls to SetTlsValue are
	 * made. The problem with doing this is that it typically means when the nested
	 * call completes the impersonation it will result in a call to RevertToSelf even
	 * though the outer call still thinks it is impersonating. To avoid this assert
	 * you must explicitly call CTlsImpersonate::ClearTlsValue(void).
	 * @param oImpersonate a instance of a CImpersonate which contains the identify
	 * of the user to be impersonated.
	 * @return true if the call succeeded in storing the impersonation object in TLS;
	 * otherwise false.
	 * @see CTlsImpersonate::ClearTlsValue(void)
	 */
    bool SetTlsValue(CImpersonate& oImpersonate);

	/** Clears a previously impersonation object that was previously stored by calling
	 * CTlsImpersonate::SetTlsValue(void). This call will allow you to subsequently call
	 * CTlsImpersonate::SetTlsValue(void) without asserting.
	 * CAUTION: This call should typically never need to be used. It does not need
	 * to be called as part of any cleanup code. It only needs to be called prior to
	 * over-writing a previously set impersonation object. However, doing so has potentially
	 * unexpected side effects such as a premature call to RevertToSelf.
	* @return true if the call succeeded in storing the impersonation object in TLS;
	* otherwise false.
	* @see CTlsImpersonate::SetTlsValue(void)
	 */
    bool ClearTlsValue(void);

    //create TLS slot
    static bool CreateSlot();

    //delete TLS slot
    static void DestroySlot();

private:
    static unsigned int m_uiTlsSlot;
    bool m_bClearTlsSlot;
    bool m_bTlsSet;
};