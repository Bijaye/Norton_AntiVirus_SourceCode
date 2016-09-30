// ReplyTo.cpp : REPLYTO states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

inline int CMIMEParser::State_REPLYTO1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state just keeps reading in tokens until we see a CRLF.
	if (m_iTokenID == MIME_TOKEN_CRLF)
	{
		EMIT_ID = MIME_TOKEN_END_MARKER;
		DO_EMIT;  // Emit END_MARKER
		m_ParseState = m_bInMultipartHeader ? MIME_PARSER_STATE_MULTIPARTHEADER1:
											 MIME_PARSER_STATE_HEADERFIELD1;
	}

	return (rc);
}
