// HeaderLine.cpp : HEADERLINE states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

inline int CUUEParser::State_HEADERLINE1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	switch (m_iTokenID)
	{
		case UUE_TOKEN_NEWLINE:
		{
			// When we hit a return then move to our header line check.
			// Hearder lines are: begin, end, sum, and table.
			m_ParseState = UUE_PARSER_STATE_HEADERLINE2;
			break;
		}
        default:
        {	// Just keep coming back here until we hit a newline.
            break;
        }
	}

	return rc;
}

inline int CUUEParser::State_HEADERLINE2(void)
{
    int     rc = PARSER_OK;  // Assume parsing should continue

    switch (m_iTokenID)
    {
		case UUE_TOKEN_BEGIN:
		{
			m_ParseState = UUE_PARSER_STATE_BEGIN1;
			break;
		}

		case UUE_TOKEN_END:
		{
			m_ParseState = UUE_PARSER_STATE_END1;
			break;
		}

		case UUE_TOKEN_SECTION:
		{
			m_ParseState = UUE_PARSER_STATE_SECTION1;
			break;
		}

		case UUE_TOKEN_SUM:
		{
			m_ParseState = UUE_PARSER_STATE_SUM1;
			break;
		}

		case UUE_TOKEN_TABLE:
		{
			m_ParseState = UUE_PARSER_STATE_TABLE1;
			break;
		}

		case UUE_TOKEN_NEWLINE:
		{
			// Skip over CR, LF, and CRLF.
			break;
		}

		case UUE_TOKEN_LWSP:
		case UUE_TOKEN_ATOM:
		{
			// Skip over whitespace.
			// Also ignore sequences that the lexer doesn't recognize.
			m_ParseState = UUE_PARSER_STATE_HEADERLINE1;
			break;
		}

		case UUE_TOKEN_EOF:
		{
			rc = PARSER_DONE;
			break;
		}

		default:
		{
			// Should not get here...
			dec_assert(0);
			break;
		}
	}

	return (rc);
}
