// End.cpp : END states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

int CUUEParser::State_END1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	m_ParseState = UUE_PARSER_STATE_HEADERLINE1;

	return (rc);
}


int CUUEParser::State_END2(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state just keeps reading in tokens until we see a CRLF.
	if (m_iTokenID == UUE_TOKEN_NEWLINE)
	{
		m_ParseState = UUE_PARSER_STATE_HEADERLINE2;
	}

	return (rc);
}
