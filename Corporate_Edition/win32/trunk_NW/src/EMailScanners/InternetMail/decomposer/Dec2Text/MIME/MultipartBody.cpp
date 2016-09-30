// MultipartBody.cpp : MULTIPARTBODY states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

inline int CMIMEParser::State_MULTIPARTBODY1(void)
{
	if (m_iTokenID == MIME_TOKEN_EOF)
	{
		EMIT_ID = MIME_TOKEN_EOF;
		DO_EMIT;
		m_ParseState = MIME_PARSER_STATE_HEADERFIELD1;
		return (PARSER_DONE);
	}

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

	EMIT_ID = MIME_TOKEN_BODY;
	DO_EMIT;  // Emit BODY
	m_ParseState = MIME_PARSER_STATE_MULTIPARTBODY2;
	return (PARSER_CHANGE_STATE);
}


inline int CMIMEParser::State_MULTIPARTBODY2(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// Here we are simply looking for another boundary.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_ATOM:
		{
			if (m_nStringLength >= 2 &&
				m_pString[0] == ASC_CHR_MINUS &&
				m_pString[1] == ASC_CHR_MINUS)
			{
				// Initialize the boundary test string.
				if (!SetBoundaryTest(&m_pString[2]))
				{
					rc = PARSER_ERROR;
					break;
				}

				m_bWantBytes = true;  // Use single-byte lexing to get the rest of the possible boundary marker.
				m_nBoundaryStart = m_nOffset-(m_pLexer->GetSizeOfLineTerminator());

				if ( m_nBoundaryStart < m_lastEmit.nTokenOffset )
				{
					m_nBoundaryStart += m_pLexer->GetSizeOfLineTerminator();
				}

				m_ParseState = MIME_PARSER_STATE_MULTIPARTBODY3;
			}
			else
			{
				m_bWantBytes = false;  // Go back to the faster buffer-based routine.
			}

			break;
		}

		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;
			EMIT_ID = MIME_TOKEN_EOF;
			DO_EMIT;
			m_ParseState = MIME_PARSER_STATE_MULTIPART1;
			break;
		}

		case MIME_TOKEN_CRLF:
		{
			// Stay with byte-based lexer to see if next two bytes
			// are an atom consisting of "--".
			m_bWantBytes = true;
			break;
		}

		default:
		{
			m_bWantBytes = false;  // Go back to the faster buffer-based routine.
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_MULTIPARTBODY3(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// Here we are looking for another boundary match.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			if (m_pString[0] != ASC_CHR_TAB)
			{
				// Copy the string into the boundary test buffer.
				if (!AppendBoundaryTest(m_pString))
					rc = PARSER_ERROR;
			}
			break;
		}

		case MIME_TOKEN_CRLF:
		{
			int		iBoundary;

			// See if we have a boundary match.  If so, close the body
			// and begin parsing the multipart header.
			iBoundary = CheckForBoundary();
			if (iBoundary != MIME_NOT_BOUNDARY)
			{
				EMIT_OFFSET = m_nBoundaryStart;
				EMIT_ID = MIME_TOKEN_END_MARKER;
				DO_EMIT;  // Emit END_MARKER
				m_iEncoding = MIME_TOKEN_NONE;
				m_iEncryption = 0;
				m_bInMultipartHeader = true;
				m_nMultipartCount++;
				EMIT_ID = MIME_TOKEN_BOUNDARY_START;
				EMIT_OFFSET = m_nBoundaryStart;
				DO_EMIT;
				EMIT_ID = MIME_TOKEN_END_MARKER;
				EMIT_OFFSET = m_nOffset;
				DO_EMIT;

				if (iBoundary == MIME_BOUNDARY_END)
				{
					m_ParseState = MIME_PARSER_STATE_MULTIPART1;
				}
				else
				{
					m_bInHeader = true;  // We are now going to start parsing header lines.
					m_ParseState = MIME_PARSER_STATE_MULTIPARTHEADER1;
				}
			}
			else
			{
				m_ParseState = MIME_PARSER_STATE_MULTIPARTBODY2;
			}
			break;
		}

		case MIME_TOKEN_EOF:
		{
			int		iBoundary;

			// See if we have a boundary match.  If so, close the body.
			iBoundary = CheckForBoundary();
			if (iBoundary != MIME_NOT_BOUNDARY)
			{
				EMIT_OFFSET = m_nBoundaryStart;
				EMIT_ID = MIME_TOKEN_END_MARKER;
				DO_EMIT;  // Emit END_MARKER
				m_iEncoding = MIME_TOKEN_NONE;
				m_iEncryption = 0;
				m_bInMultipartHeader = true;
				m_nMultipartCount++;
				EMIT_ID = MIME_TOKEN_BOUNDARY_START;
				EMIT_OFFSET = m_nBoundaryStart;
				DO_EMIT;
				EMIT_ID = MIME_TOKEN_END_MARKER;
				EMIT_OFFSET = m_nOffset;
				DO_EMIT;

				if (iBoundary == MIME_BOUNDARY_END)
				{
					m_ParseState = MIME_PARSER_STATE_MULTIPART1;
				}
				else
				{
					m_bInHeader = true;  // We are now going to start parsing header lines.
					m_ParseState = MIME_PARSER_STATE_MULTIPARTHEADER1;
				}
			}
			else
			{
				m_ParseState = MIME_PARSER_STATE_MULTIPARTBODY2;
			}

			if (m_iTokenID == MIME_TOKEN_EOF)
			{
				EMIT_ID = MIME_TOKEN_END_MARKER;
				DO_EMIT;
				EMIT_ID = MIME_TOKEN_EOF;
				DO_EMIT;
				m_ParseState = MIME_PARSER_STATE_MULTIPART1;
			}
			break;
		}

		default:
		{
			// Copy everything else into the boundary test buffer.
			if (!AppendBoundaryTest(m_pString))
				rc = PARSER_ERROR;
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_MULTIPARTBODY4(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	switch (m_iTokenID)
	{
		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;
			EMIT_ID = MIME_TOKEN_EOF;
			DO_EMIT;
			m_ParseState = MIME_PARSER_STATE_MULTIPART1;
			break;
		}

		default:
		{
			break;
		}
	}

	return (rc);
}
