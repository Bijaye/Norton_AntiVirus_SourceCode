// Body.cpp : BODY states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

inline int CMIMEParser::State_BODY1(void)
{
	if (m_iTokenID == MIME_TOKEN_EOF)
	{
		EMIT_ID = MIME_TOKEN_EOF;
		DO_EMIT;
		m_ParseState = MIME_PARSER_STATE_HEADERFIELD1;
		if (m_bIdentified)
			return (PARSER_IDENTIFIED);

		return (PARSER_DONE);
	}

	// Always emit the message-body-start token.
	EMIT_ID = MIME_TOKEN_BODY;
	switch (m_iEncoding)
	{
		case MIME_TOKEN_NONE:
			EMIT_FLAGS |= ENCODE_NONE;
			break;

		case MIME_TOKEN_BASE64:
			EMIT_FLAGS |= ENCODE_BASE64;
			break;

		case MIME_TOKEN_UUENCODE:
			EMIT_FLAGS |= ENCODE_UUE;
			break;

		case MIME_TOKEN_7BIT:
			EMIT_FLAGS |= ENCODE_7BIT;
			break;

		case MIME_TOKEN_8BIT:
			EMIT_FLAGS |= ENCODE_8BIT;
			break;

		case MIME_TOKEN_QUOTED_PRINTABLE:
			EMIT_FLAGS |= ENCODE_QP;
			break;

		case MIME_TOKEN_BINARY:
			EMIT_FLAGS |= ENCODE_BINARY;
			break;

		default:
			break;
	}

	if (m_iEncryption)
		EMIT_FLAGS |= ENCRYPTED;

	// Emit BODY
	DO_EMIT;

	// In single-part messages, we don't need to parse any further
	// since the remainder of the message is just the "attachment"
	// which we will deal with during extraction.
	// Now all we need to do is wait for the EOF marker.
	m_ParseState = MIME_PARSER_STATE_BODY2;
	return (PARSER_DONE);
}


inline int CMIMEParser::State_BODY2(void)
{
	int		rc = PARSER_OK;	// Parsing is finished

	// In single-part messages, we only need to look for EOF.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;
			EMIT_ID = MIME_TOKEN_EOF;
			DO_EMIT;
			break;
		}

		default:
		{
			// Skip everything else.
			break;
		}
	}

	return (rc);
}
