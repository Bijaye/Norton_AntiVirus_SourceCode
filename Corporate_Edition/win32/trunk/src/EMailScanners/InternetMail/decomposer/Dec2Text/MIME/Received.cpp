// Received.cpp : RECEIVED states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

inline int CMIMEParser::State_RECEIVED1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			// Skip over any whitespace in the Received: line.
			break;
		}

		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_ParseState = MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}

		case MIME_TOKEN_CRLF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_ParseState = m_bInMultipartHeader ? MIME_PARSER_STATE_MULTIPARTHEADER1:
												 MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}

		case MIME_TOKEN_SPECIAL:
		{
			// OK, we found a SPECIAL token.  Move to the RECEIVED4 state to
			// examine the date-time portion of the Received: field.
			if (m_pString[0] == ASC_CHR_SEMI)
				m_ParseState = MIME_PARSER_STATE_RECEIVED4;
			break;
		}

		case MIME_TOKEN_ATOM:
		{
			// Identify the atom.
			m_iFieldID = IdentifyAtom(MIME_ATOM_GROUP_RECEIVED);
			switch (m_iFieldID)
			{
				case MIME_TOKEN_WITH:
				case MIME_TOKEN_VIA:
					// Move to state to look for single atom.
					m_ParseState = MIME_PARSER_STATE_RECEIVED5;
					break;

				case MIME_TOKEN_FROM:
				case MIME_TOKEN_BY:
					// Move to state to look for domain.
					m_ParseState = MIME_PARSER_STATE_RECEIVED7;
					break;

				case MIME_TOKEN_ID:
					break;

				case MIME_TOKEN_FOR:
					break;

				default:
					// Did not identify the atom so just skip it.
					m_iFieldID = MIME_TOKEN_ATOM;
					break;
			}

			EMIT_ID = m_iFieldID;
			DO_EMIT;  // Emit the identified atom
			break;
		}

		case MIME_TOKEN_COMMENT:
			// Skip over any comments.
			break;

		case MIME_TOKEN_QUOTED_STRING:
		case MIME_TOKEN_DOMAIN_LITERAL:
		case MIME_TOKEN_ROUTE_ADDR:
		case MIME_TOKEN_QUOTED_PAIR:
		{
			// Skip over these.
			break;
		}

		default:
		{
			// This is an unexpected token.  We recover by going back to
			// looking for header lines.
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_RECEIVED);
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_ParseState = MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_RECEIVED2(void)
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


inline int CMIMEParser::State_RECEIVED3(void)
{
	int		rc = PARSER_ERROR;

	// This state is unused...
	return (rc);
}


inline int CMIMEParser::State_RECEIVED4(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// Here we are looking for a date-time sequence of the form:
	// [ day "," ] date time
	// For example:
	// Tue, 28 Mar 2000 16:53:44 -0500
	switch (m_iTokenID)
	{
		case MIME_TOKEN_CRLF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_ParseState = m_bInMultipartHeader ? MIME_PARSER_STATE_MULTIPARTHEADER1:
												 MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}

		default:
		{
			// Skip over any other tokens.
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_RECEIVED5(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for the next keyword.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_ParseState = MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}

		case MIME_TOKEN_SPECIAL:
		{
			// OK, we found a SPECIAL token.  Move to the RECEIVED4 state to
			// examine the date-time portion of the Received: field.
			if (m_pString[0] == ASC_CHR_SEMI)
				m_ParseState = MIME_PARSER_STATE_RECEIVED4;
			break;
		}

		case MIME_TOKEN_ATOM:
		{
			// Identify the atom.
			m_iFieldID = IdentifyAtom(MIME_ATOM_GROUP_RECEIVED);
			switch (m_iFieldID)
			{
				case MIME_TOKEN_WITH:
				case MIME_TOKEN_VIA:
					// Continue looking for the next keyword.
					break;

				case MIME_TOKEN_FROM:
				case MIME_TOKEN_BY:
					// Move to state to look for domain.
					m_ParseState = MIME_PARSER_STATE_RECEIVED7;
					break;

				case MIME_TOKEN_ID:
					// Continue looking for the next keyword.
					break;

				case MIME_TOKEN_FOR:
					// Continue looking for the next keyword.
					break;

				default:
					// Did not identify the atom so just skip it.
					// Continue looking for the next keyword.
					break;
			}

			EMIT_ID = m_iFieldID;
			DO_EMIT;  // Emit the identified atom
			break;
		}

		case MIME_TOKEN_CRLF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_ParseState = m_bInMultipartHeader ? MIME_PARSER_STATE_MULTIPARTHEADER1:
												 MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}

		case MIME_TOKEN_LWSP:
		{
			// Skip over any whitespace in front of the atom.
			break;
		}

		default:
		{
			// Consider any other token to be equivalent to an atom.
			// Continue looking for the next keyword.
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_RECEIVED6(void)
{
	int		rc = PARSER_ERROR;

	// This state is unused...
	return (rc);
}


inline int CMIMEParser::State_RECEIVED7(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// Here we are looking for the beginning of a domain name.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_ParseState = MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}

		case MIME_TOKEN_CRLF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_ParseState = m_bInMultipartHeader ? MIME_PARSER_STATE_MULTIPARTHEADER1:
												 MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}

		case MIME_TOKEN_LWSP:
		{
			// Skip over any whitespace in front of the domain name.
			break;
		}

		case MIME_TOKEN_SPECIAL:
		{
			// OK, we found a SPECIAL token.  Move to the RECEIVED4 state to
			// examine the date-time portion of the Received: field.
			if (m_pString[0] == ASC_CHR_SEMI)
				m_ParseState = MIME_PARSER_STATE_RECEIVED4;
			break;
		}

		case MIME_TOKEN_ATOM:
		case MIME_TOKEN_QUOTED_STRING:
		case MIME_TOKEN_DOMAIN_LITERAL:
		case MIME_TOKEN_COMMENT:
		case MIME_TOKEN_ROUTE_ADDR:
		case MIME_TOKEN_QUOTED_PAIR:
		{
			EMIT_ID = MIME_TOKEN_DOMAIN;
			DO_EMIT;  // Emit DOMAIN
			m_ParseState = MIME_PARSER_STATE_RECEIVED8;
			break;
		}

		default:
		{
			// This is an unexpected token.  We recover by going back to
			// looking for header lines.
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_RECEIVED);
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_ParseState = MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_RECEIVED8(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// Here we are looking for the continuation of a domain name.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_ParseState = MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}

		case MIME_TOKEN_CRLF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_ParseState = m_bInMultipartHeader ? MIME_PARSER_STATE_MULTIPARTHEADER1:
												 MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}

		case MIME_TOKEN_LWSP:
		{
			// This should mark the end of the domain name.
			m_ParseState = MIME_PARSER_STATE_RECEIVED1;
			break;
		}

		case MIME_TOKEN_SPECIAL:
		{
			// OK, we found a SPECIAL token.  Move to the RECEIVED4 state to
			// examine the date-time portion of the Received: field.
			if (m_pString[0] == ASC_CHR_SEMI)
				m_ParseState = MIME_PARSER_STATE_RECEIVED4;
			break;
		}

		case MIME_TOKEN_ATOM:
		case MIME_TOKEN_QUOTED_STRING:
		case MIME_TOKEN_DOMAIN_LITERAL:
		case MIME_TOKEN_COMMENT:
		case MIME_TOKEN_ROUTE_ADDR:
		case MIME_TOKEN_QUOTED_PAIR:
		{
			// These tokens should be part of the domain name.
			break;
		}

		default:
		{
			// This is an unexpected token.  We recover by going back to
			// looking for header lines.
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_RECEIVED);
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_ParseState = MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}
	}

	return (rc);
}
