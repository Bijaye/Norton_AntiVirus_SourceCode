// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef AGENTS_DEFS
#define AGENTS_DEFS

/*	API to execute agents */

#ifdef __cplusplus
extern "C" {
#endif

typedef void far *HAGENT;
typedef void far *HAGENTCTX;

STATUS	LNPUBLIC AgentOpen (HANDLE hDB, NOTEID AgentNoteID, HAGENT far *rethAgent);
void	LNPUBLIC AgentClose (HAGENT hAgent);

STATUS	LNPUBLIC AgentCreateRunContext (HAGENT hAgent,
										 void far * pReserved,
										 DWORD dwFlags,
										 HAGENTCTX far *rethContext);
STATUS LNPUBLIC AgentSetDocumentContext(HAGENTCTX hAgentCtx, NOTEHANDLE hNote);

/* Definitions for stdout redirection types. This specifies where
	output from the LotusScript "print" statement will go */

#define AGENT_REDIR_NONE		0		/* goes to the bit bucket */
#define AGENT_REDIR_LOG			1		/* goes to the Notes log (default) */
#define AGENT_REDIR_MEMORY		2		/* goes to a memory buffer, cleared each AgentRun */
#define AGENT_REDIR_MEMAPPEND	3		/* goes to buffer, append mode for each agent */

STATUS 	LNPUBLIC AgentRedirectStdout(HAGENTCTX hAgentCtx, WORD redirType);
void 	LNPUBLIC AgentQueryStdoutBuffer(HAGENTCTX hAgentCtx, HANDLE FAR * retHdl, DWORD FAR * retSize);
void	LNPUBLIC AgentDestroyRunContext (HAGENTCTX hAgentCtx);
STATUS	LNPUBLIC AgentRun (HAGENT hAgent,
							HAGENTCTX hAgentCtx,
						    HANDLE hSelection,
							DWORD dwFlags);

#ifdef __cplusplus
}
#endif

#endif /* AGENTS_DEFS */


