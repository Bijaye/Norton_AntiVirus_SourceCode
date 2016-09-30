// Preamble.cpp : PREAMBLE states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

inline int CMIMEParser::State_PREAMBLE1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for a blank-line (CRLF) all by itself
	// which signifies the end of the preamble section.
	if (m_iTokenID == MIME_TOKEN_CRLF)
	{
		EMIT_ID = MIME_TOKEN_END_MARKER;
		DO_EMIT;  // Emit END_MARKER

		// Move to the state that begins processing multipart headers.
		m_ParseState = MIME_PARSER_STATE_MULTIPART1;
	}
	else
	{
		// This is a non-CRLF token so move to a state where we
		// wait for a CRLF before returning back to this state.
		m_ParseState = MIME_PARSER_STATE_PREAMBLE2;
	}

	return (rc);
}


inline int CMIMEParser::State_PREAMBLE2(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state just keeps reading in tokens until we see a CRLF.
	if (m_iTokenID == MIME_TOKEN_CRLF)
	{
		// Found a CRLF which should signify the end of a non-empty text line.
		// Go back to the first preamble state and look for a blank line.
		m_ParseState = MIME_PARSER_STATE_PREAMBLE1;
	}

	return (rc);
}
