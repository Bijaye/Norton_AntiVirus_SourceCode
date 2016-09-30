// ContentDisposition.cpp : CONTENTDISPOSITION states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

inline int CMIMEParser::State_CONTENTDISPOSITION1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is expecting an atom (often "attachment").
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			// Ignore any whitespace here.
			break;
		}

		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;
			EMIT_ID = MIME_TOKEN_EOF;
			DO_EMIT;
			break;
		}

		case MIME_TOKEN_ATOM:
		{
			int iID;

			iID = IdentifyAtom(MIME_ATOM_GROUP_DISPOSITION);
			if (iID != MIME_TOKEN_LAST)
			{
				if (iID == MIME_TOKEN_FILENAME)
				{
					m_bScanForPairs = true;	// Begin scanning for pairs.
					m_ParseState = MIME_PARSER_STATE_CONTENTDISPOSITION3;
				}

//                if (iID == MIME_TOKEN_ATTACHMENT)
//                    m_bMultipart = true;

				if (iID == MIME_TOKEN_ENCRYPTION ||
					iID == MIME_TOKEN_ENCRYPTION_FLAGS)
				{
					m_iEncryption = 1;
				}

				// We found a recognized parameter.  Emit its token.
				EMIT_ID = iID;
				DO_EMIT;
			}

			break;
		}

		case MIME_TOKEN_SPECIAL:
		{
			// If this is a semi-colon, then we'll assume this is
			// the separator.  Just skip it and stay in this same state.
			if (m_pString[0] == ASC_CHR_EQ)
			{
				m_bScanForPairs = true;	// Begin scanning for pairs.
				m_ParseState = MIME_PARSER_STATE_CONTENTDISPOSITION2;
			}

			break;
		}

		case MIME_TOKEN_CRLF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;
			m_ParseState = m_bInMultipartHeader ? MIME_PARSER_STATE_MULTIPARTHEADER1:
												 MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_CONTENT_DISPOSITION);
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_CONTENTDISPOSITION2(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// Here we are just looking for the next parameter separator (which should be a SPECIAL semi-colon token).
	switch (m_iTokenID)
	{
		case MIME_TOKEN_CRLF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;
			m_bScanForPairs = false;  // Stop scanning for pairs.
			m_ParseState = m_bInMultipartHeader ? MIME_PARSER_STATE_MULTIPARTHEADER1:
												 MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}

		case MIME_TOKEN_LWSP:
		{
			// Ignore any whitespace here.
			break;
		}

		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;
			EMIT_ID = MIME_TOKEN_EOF;
			DO_EMIT;
			break;
		}

		case MIME_TOKEN_SPECIAL:
		{
			if (m_pString[0] == ASC_CHR_SEMI)
			{
				m_bScanForPairs = false;  // Stop scanning for pairs.
				m_ParseState = MIME_PARSER_STATE_CONTENTDISPOSITION1;
			}

			break;
		}

		default:
		{
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_CONTENTDISPOSITION3(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for a SPECIAL token following the "filename"
	// keyword.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			break;	// Ignore LWSP.
		}

		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			break;
		}

		case MIME_TOKEN_SPECIAL:
		{
			if (m_pString[0] == ASC_CHR_EQ)
			{
				m_bScanForPairs = true;	// Begin scanning for pairs.
				m_ParseState = MIME_PARSER_STATE_CONTENTDISPOSITION4;
				break;
			}

			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_SEPARATOR, MIME_TOKEN_CONTENT_DISPOSITION);
			m_bScanForPairs = true;	// Begin scanning for pairs.
			m_ParseState = MIME_PARSER_STATE_CONTENTDISPOSITION4;
			break;
		}

		case MIME_TOKEN_QUOTED_STRING:
		{
			m_ParseState = MIME_PARSER_STATE_CONTENTDISPOSITION4;
			rc = PARSER_CHANGE_STATE;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_CONTENT_DISPOSITION);
			m_bScanForPairs = true;	// Begin scanning for pairs.
			m_ParseState = MIME_PARSER_STATE_CONTENTDISPOSITION4;
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_CONTENTDISPOSITION4(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for whatever follows "filename=".
	// *** NOTE ***
	// You can not leave this state without calling EmitName.
	// If EmitName does not get called at some point before we
	// change to a different state, the token file will be corrupt
	// because a name *must* follow the MIME_TOKEN_FILENAME token.
	// ************
	switch (m_iTokenID)
	{
		case MIME_TOKEN_CRLF:
		{
			// Emit whatever name we found so far.
			rc = EmitName();

			// The END_OF_NAME_MARKER needs to be positioned at the last character of the
			// name.  So, back up one character since we are currently positioned on a
			// CRLF token that immediately follows the name.
			EMIT_ID = MIME_TOKEN_END_OF_NAME_MARKER;
			EMIT_OFFSET = EMIT_OFFSET - 1;
			DO_EMIT;
			EMIT_ID = MIME_TOKEN_END_MARKER;
			EMIT_OFFSET = EMIT_OFFSET + 1;
			DO_EMIT;
			m_bScanForPairs = false;  // Stop scanning for pairs.
			m_ParseState = m_bInMultipartHeader ? MIME_PARSER_STATE_MULTIPARTHEADER1:
												 MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}

		case MIME_TOKEN_LWSP:
		{
			break;	// Ignore LWSP.
		}

		case MIME_TOKEN_EOF:
		{
			// Emit whatever name we found so far.
			rc = EmitName();

			// The END_OF_NAME_MARKER needs to be positioned at the last character of the
			// name.  So, back up one character since we are currently positioned on an
			// EOF token that immediately follows the name.
			EMIT_ID = MIME_TOKEN_END_OF_NAME_MARKER;
			EMIT_OFFSET = EMIT_OFFSET - 1;
			DO_EMIT;
			EMIT_ID = MIME_TOKEN_END_MARKER;
			EMIT_OFFSET = EMIT_OFFSET + 1;
			DO_EMIT;  // Emit END_MARKER
			break;
		}

		case MIME_TOKEN_QUOTED_STRING:
		case MIME_TOKEN_ATOM:
		case MIME_TOKEN_SPECIAL:
		{
			size_t	nLen;
			char	*ptr1;
			char	*ptr2;
			char	ch;

			// Check for special separator character.
			if (m_pString[0] == ASC_CHR_SEMI)
			{
				// Emit whatever name we found so far.
				rc = EmitName();

				// The END_OF_NAME_MARKER needs to be positioned at the last character of the
				// name.  So, back up one character since we are currently positioned on a
				// CRLF token that immediately follows the name.
				EMIT_ID = MIME_TOKEN_END_OF_NAME_MARKER;
				EMIT_OFFSET = EMIT_OFFSET - 1;
				DO_EMIT;
				m_bScanForPairs = false;  // Stop scanning for pairs.
				m_ParseState = MIME_PARSER_STATE_CONTENTDISPOSITION1;
				break;
			}

			// These tokens should be part of the name following "filename=".
			nLen = m_nStringLength;
			if (!m_pszName)
			{
				m_pszName = (char *)malloc(nLen + 1);
				if (!m_pszName)
				{
					rc = PARSER_ERROR;
					break;
				}

				m_pszName[0] = 0;
			}
			else
			{
				size_t	nNewLen;

				// The name buffer has already been allocated.
				// Re-allocate it here.
				nNewLen = strlen(m_pszName) + nLen + 1;
				if (nNewLen > MAX_FILENAME_LEN)
				{
					// This name is extremely large.  We set this limit
					// to prevent the size of this buffer from running away.
					EmitWarning(MIME_WARNING_MAX_FILENAME_LENGTH, MIME_TOKEN_CONTENT_DISPOSITION);
					//break; // crn- this might not be needed, since the files are stored in truncated form anyway.
				}

				{
					char* pszRealloced = (char *)realloc(m_pszName, nNewLen);
					if (!pszRealloced)
					{
						free(m_pszName);
						m_pszName = NULL;
						rc = PARSER_ERROR;
						break;
					}
					m_pszName = pszRealloced;
				}
			}

			if (m_iTokenID != MIME_TOKEN_QUOTED_STRING)
			{
				// The string is not quoted.  Copy it into m_pszName, but
				// do so one character at a time so that we can translate any
				// NULL characters as needed.
				ptr1 = m_pString;
				ptr2 = &m_pszName[strlen(m_pszName)];
				while (nLen)
				{
					ch = *ptr1++;
					if (ch == 0 && m_chTranslateNULL)
						*ptr2++ = m_chTranslateNULL;
					else
						*ptr2++ = ch;
					nLen--;
				}

				// NULL-terminate the string.
				*ptr2 = 0;
			}
			else
			{
				// The string is quoted and may contain CRLF pairs and TABs.
				// Strip off the quotes and any CRLFs and any TABs.
				ptr1 = &m_pString[1];
				ptr2 = &m_pszName[strlen(m_pszName)];
				nLen--;
				while (nLen)
				{
					ch = *ptr1++;
					if (ch == 0 && m_chTranslateNULL)
						*ptr2++ = m_chTranslateNULL;
					else if (ch == ASC_CHR_DQUOTE && nLen == 1)
						*ptr2++ = 0;  // Strip off any closing quote.
					else if (ch != ASC_CHR_CR && ch != ASC_CHR_LF && ch != ASC_CHR_TAB)
						*ptr2++ = ch;
					nLen--;
				}

				// NULL-terminate the string.
				*ptr2 = 0;
			}
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_CONTENT_DISPOSITION);

			// Emit whatever name we found so far.
			rc = EmitName();

			// The END_OF_NAME_MARKER needs to be positioned at the last character of the
			// name.  So, back up one character since we are currently positioned on a
			// token that immediately follows the name.
			EMIT_ID = MIME_TOKEN_END_OF_NAME_MARKER;
			EMIT_OFFSET = EMIT_OFFSET - 1;
			DO_EMIT;
			m_bScanForPairs = false;  // Stop scanning for pairs.
			m_ParseState = MIME_PARSER_STATE_CONTENTDISPOSITION1;
			break;
		}
	}

	return (rc);
}
