// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef __MANAGED_PRODUCTS_H_
#define __MANAGED_PRODUCTS_H_

#ifdef SERVER

// SCF configuration info

typedef struct _SCFMGMTDATA
{
	BOOL		m_is_managed;
	BOOL		m_is_enabled;
	SCFVERSION	m_version;
	VTIME		m_current_policy;
	DWORD		m_last_policy_import_ret_code;
	VTIME		m_last_policy_fail;
	DWORD		m_last_policy_fail_count;
	DWORD		m_last_policy_fail_count_max;
	char		m_current_policy_file_name[MAX_PATH];

} SCFMGMTDATA;

// Public functions from this module

void SCF_GetSCFManagementData( SCFMGMTDATA* _scf_data );

void SCF_GetSCFPongData( PONGDATA* _pong );

void SCF_CheckForPolicyUpdate();

DWORD SCF_FindLocallyCachedPolicy( VTIME _policy_id,
								   char* _policy_file_name,
								   DWORD _policy_file_name_buf_len,
								   BOOL _get_from_parent );

DWORD SCF_EnsurePolicyExistsLocally( VTIME _scf_policy_id );

void SCF_CleanUpPolicyCache();

void ManagedProducts_ManageWatchThreads();

#endif // SERVER

#endif // __MANAGED_PRODUCTS_H_

