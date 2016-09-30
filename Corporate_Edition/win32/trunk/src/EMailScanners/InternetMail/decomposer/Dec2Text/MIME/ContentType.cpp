// ContentType.cpp : CONTENTTYPE states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

inline int CMIMEParser::State_CONTENTTYPE1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is expecting an atom (often "multipart").
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

			// Assume we next move to the state that looks for the '/' delimeter.
			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE3;

			iID = IdentifyAtom(MIME_ATOM_GROUP_TYPE);
			if (iID != MIME_TOKEN_LAST)
			{
				if (iID == MIME_TOKEN_BOUNDARY)
				{
					if( !m_fX_pkcs7_mime )
					{
						m_bMultipart = true;

						// We found a recognized parameter.  Emit its token.
						EMIT_ID = iID;
						DO_EMIT;

					}
					m_ParseState = MIME_PARSER_STATE_CONTENTTYPE5;
				}
				else if (iID == MIME_TOKEN_NAME)
				{
					m_ParseState = MIME_PARSER_STATE_CONTENTTYPE7;
					// We found a recognized parameter.  Emit its token.
					EMIT_ID = iID;
					DO_EMIT;
				}
				else if (iID == MIME_TOKEN_MULTIPART)
				{
					m_bMultipart = true;
					// We found a recognized parameter.  Emit its token.
					EMIT_ID = iID;
					DO_EMIT;
				}
				else if (iID == MIME_TOKEN_CHARSET)
				{
					m_ParseState = MIME_PARSER_STATE_CONTENTTYPE9;
					// We found a recognized parameter.  Emit its token.
					EMIT_ID = iID;
					DO_EMIT;
				}
				else
				{
					// We found a recognized parameter.  Emit its token.
					EMIT_ID = iID;
					DO_EMIT;
				}


			}

			break;
		}

		case MIME_TOKEN_SPECIAL:
		{
			// If this is a semi-colon, then we'll assume this is
			// the separator.  Just skip it and stay in this same state.
// *** REMOVED *** Fix for defect #1-6IB43.  This was too strict since there can be
//                 other TSPECIALs in or around atoms in the Content-Type line.
//            if (m_pString[0] != ASC_CHR_SEMI &&)
//            {
//                // Note that this is not RFC 822 compliant.
//                EmitWarning(MIME_WARNING_UNEXPECTED_SEPARATOR, MIME_TOKEN_CONTENT_TYPE);
//            }

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
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_CONTENT_TYPE);
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_CONTENTTYPE2(void)
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
				m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
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


inline int CMIMEParser::State_CONTENTTYPE3(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// Here we are looking for a SPECIAL token that separates the
	// primary type from its sub-type (e.g. "multipart/mixed").
	// We might also find a semi-colon delimiter.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_CRLF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;
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
			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;

			if (m_pString[0] == ASC_CHR_FSLASH)
			{

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
				m_pMIMEAttributes->SetContentTypeSlashSeparatorOffset(m_nOffset);
#endif //*** End AS400 Attribute API Removal *** 

				m_ParseState = MIME_PARSER_STATE_CONTENTTYPE4;
			}
			else if (m_pString[0] == ASC_CHR_EQ)
			{
				m_bScanForPairs = true;	// Begin scanning for pairs.
				m_ParseState = MIME_PARSER_STATE_CONTENTTYPE2;
			}
// *** REMOVED *** Fix for defect #1-6IB43.  This was too strict since there can be
//                 other TSPECIALs in or around atoms in the Content-Type line.
//            else if (m_pString[0] != ASC_CHR_SEMI)
//            {
//                // Note that this is not RFC 822 compliant.
//                EmitWarning(MIME_WARNING_UNEXPECTED_SEPARATOR, MIME_TOKEN_CONTENT_TYPE);
//            }

			break;
		}

		default:
		{
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_CONTENTTYPE4(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for an ATOM (the content sub-type).
	// If we see anything else (other than LWSP) then the field
	// is malformed.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			break;	// Ignore LWSP.
		}

		case MIME_TOKEN_SPECIAL:
		{
			if (m_pString[0] != ASC_CHR_SEMI)
			{
				// Note that this is not RFC 822 compliant.
				EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_CONTENT_TYPE);
			}

			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
			break;
		}

		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			break;
		}

		case MIME_TOKEN_ATOM:
		{
			int iID;

			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;

			iID = IdentifyAtom(MIME_ATOM_GROUP_SIGNED_DATA);

			if( iID == MIME_TOKEN_X_PKSC7_MIME )
			{
				m_fX_pkcs7_mime = true;

			}
			else
			{
				iID = IdentifyAtom(MIME_ATOM_GROUP_TYPE);

				if (iID != MIME_TOKEN_LAST)
				{
					// We found a recognized parameter.  Emit its token.
					EMIT_ID = iID;
					DO_EMIT;
				}
			}
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_CONTENT_TYPE);
			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_CONTENTTYPE5(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for a SPECIAL token following the "boundary"
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
				if( !m_fX_pkcs7_mime )
				{
					// When we reach the maximum number of nested boundaries
					// that we support, we just won't see them anymore.
					// Generally this means that we will treat everything nested
					// below this level to be a block of plain-text.
					if (m_nBoundaryCount >= MAX_BOUNDARIES - 1)
					{
						// Track this as a warning...
						EmitWarning(MIME_WARNING_MAX_BOUNDARY_NESTING, MIME_TOKEN_CONTENT_TYPE);
					}

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
					m_pMIMEAttributes->SetContentTypeBoundaryOffset(m_nOffset);
#endif //*** End AS400 Attribute API Removal ***
				

					m_nBoundaryCount++;
					m_nBoundaryNestLevel++;
				}

				m_bScanForPairs = true;	// Begin scanning for pairs.
				m_ParseState = MIME_PARSER_STATE_CONTENTTYPE6;
				break;
			}

			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_SEPARATOR, MIME_TOKEN_CONTENT_TYPE);
			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_CONTENT_TYPE);
			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_CONTENTTYPE6(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for whatever follows "boundary=".
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			break;	// Ignore LWSP.
		}

		case MIME_TOKEN_CRLF:
		{
			// If we get a CRLF here then we have a boundary in the form
			//	boundary='CRLF'
			// This is an empty boundary so we need to construct an empty
			// boundary entry in our boundary table
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;
			m_bScanForPairs = false;  // Stop scanning for pairs.
			m_ParseState = m_bInMultipartHeader ? MIME_PARSER_STATE_MULTIPARTHEADER1:
												 MIME_PARSER_STATE_HEADERFIELD1;

			// new the memory
			char* ptr = (char *)malloc(1);
			if (!ptr)
				return (PARSER_ERROR);
			
			// Create an empty 'C' string for the entry boundary
			*ptr = 0;

			// Save the address of the new memory in the nested boundary array.
			m_pszBoundary[m_nBoundaryCount - 1].m_pszBoundary = ptr;
			m_pszBoundary[m_nBoundaryCount - 1].m_nNestedLevel = m_nBoundaryNestLevel;

			EmitWarning(MIME_WARNING_EMPTY_BOUNDARY_SPECIFIED, MIME_TOKEN_BOUNDARY_START);
			break;
		}

		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			break;
		}

		case MIME_TOKEN_SPECIAL:
		case MIME_TOKEN_QUOTED_STRING:
		case MIME_TOKEN_ATOM:
		{
			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE11;
			rc = PARSER_CHANGE_STATE;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_CONTENT_TYPE);
			m_bScanForPairs = false;  // Stop scanning for pairs.
			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_CONTENTTYPE7(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for a SPECIAL token following the "name"
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
				m_ParseState = MIME_PARSER_STATE_CONTENTTYPE8;
				break;
			}

			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_SEPARATOR, MIME_TOKEN_CONTENT_TYPE);

			// Emit whatever name we found so far.
			rc = EmitName();

			// The END_OF_NAME_MARKER needs to be positioned at the last character of the
			// name.  So, back up one character since we are currently positioned one character
			// beyond "name".  Note that we are emitting an empty name here because we need to
			// complete the MIME_TOKEN_NAME sequence.  Otherwise we'll throw off the token reader.
			EMIT_ID = MIME_TOKEN_END_OF_NAME_MARKER;
			EMIT_OFFSET = EMIT_OFFSET - 1;
			DO_EMIT;
			EMIT_ID = MIME_TOKEN_END_MARKER;
			EMIT_OFFSET = EMIT_OFFSET + 1;
			DO_EMIT;
			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_CONTENT_TYPE);

			// Emit whatever name we found so far.
			rc = EmitName();

			// The END_OF_NAME_MARKER needs to be positioned at the last character of the
			// name.  So, back up one character since we are currently positioned one character
			// beyond "name".  Note that we are emitting an empty name here because we need to
			// complete the MIME_TOKEN_NAME sequence.  Otherwise we'll throw off the token reader.
			EMIT_ID = MIME_TOKEN_END_OF_NAME_MARKER;
			EMIT_OFFSET = EMIT_OFFSET - 1;
			DO_EMIT;
			EMIT_ID = MIME_TOKEN_END_MARKER;
			EMIT_OFFSET = EMIT_OFFSET + 1;
			DO_EMIT;
			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_CONTENTTYPE8(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for whatever follows "name=".
	// *** NOTE ***
	// You can not leave this state without calling EmitName.
	// If EmitName does not get called at some point before we
	// change to a different state, the token file will be corrupt
	// because a name *must* follow the MIME_TOKEN_NAME token.
	// ************
	switch (m_iTokenID)
	{
		case MIME_TOKEN_CRLF:
		{
			// Emit whatever name we found so far.
			rc = EmitName();

			// The END_OF_NAME_MARKER needs to be positioned at the last character of the
			// name.  So, back up one character since we are currently positioned on a CRLF token
			// that immediately follows the name.
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
			// name.  So, back up one character since we are currently positioned on an EOF token
			// that immediately follows the name.
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
				// special semi-colon token that immediately follows the name.
				EMIT_ID = MIME_TOKEN_END_OF_NAME_MARKER;
				EMIT_OFFSET = EMIT_OFFSET - 1;
				DO_EMIT;
				m_bScanForPairs = false;  // Stop scanning for pairs.
				m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
				break;
			}

			// These tokens should be part of the name following "name=".
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
					EmitWarning(MIME_WARNING_MAX_FILENAME_LENGTH, MIME_TOKEN_CONTENT_TYPE);
					//break; commented out due to malformed B64 data being generated.
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
			DWORD	dwLen = 0;

			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_CONTENT_TYPE);

			// Emit whatever name we found so far.
			rc = EmitName();

			// The END_OF_NAME_MARKER needs to be positioned at the last character of the
			// name.  So, back up one character since we are currently positioned on a
			// token that immediately follows the name.
			EMIT_ID = MIME_TOKEN_END_OF_NAME_MARKER;
			EMIT_OFFSET = EMIT_OFFSET - 1;
			DO_EMIT;
			m_bScanForPairs = false;  // Stop scanning for pairs.
			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_CONTENTTYPE9(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for a SPECIAL token following the "charset"
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
				m_ParseState = MIME_PARSER_STATE_CONTENTTYPE10;
				break;
			}

			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_SEPARATOR, MIME_TOKEN_CONTENT_TYPE);
			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_CONTENT_TYPE);
			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_CONTENTTYPE10(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	bool	bSetCharsetAttribute = false;

	// This state is looking for whatever follows "charset=".
	switch (m_iTokenID)
	{
		case MIME_TOKEN_CRLF:
		{
			bSetCharsetAttribute = true;

			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;
			m_bScanForPairs = false;  // Stop scanning for pairs.
			m_ParseState = m_bInMultipartHeader ? MIME_PARSER_STATE_MULTIPARTHEADER1:
												 MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}

		case MIME_TOKEN_SPECIAL:
		{
			if (m_pString[0] == ASC_CHR_SEMI)
			{

				bSetCharsetAttribute = true;
				m_bScanForPairs = false;  // Stop scanning for pairs.
				m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
				break;
			}
			else if (	m_pString[0] == ASC_CHR_LPAREN
					 ||	m_pString[0] == ASC_CHR_RPAREN
					 ||	m_pString[0] == ASC_CHR_LT
					 ||	m_pString[0] == ASC_CHR_GT
					 ||	m_pString[0] == ASC_CHR_AT
					 ||	m_pString[0] == ASC_CHR_COMMA
					 ||	m_pString[0] == ASC_CHR_COLON
					 ||	m_pString[0] == ASC_CHR_BSLASH
					 ||	m_pString[0] == ASC_CHR_DQUOTE
					 ||	m_pString[0] == ASC_CHR_FSLASH
					 ||	m_pString[0] == ASC_CHR_LBRACK
					 ||	m_pString[0] == ASC_CHR_RBRACK
					 ||	m_pString[0] == ASC_CHR_QUESTION
					 ||	m_pString[0] == ASC_CHR_EQ	)
			{
				EmitWarning(MIME_WARNING_UNEXPECTED_SEPARATOR, MIME_TOKEN_CONTENT_TYPE);
			}

			int tempTokenID = m_iTokenID;
			m_iTokenID = MIME_TOKEN_ATOM;
			State_CONTENTTYPE10();
			m_iTokenID = tempTokenID;
			break;
		}

		case MIME_TOKEN_LWSP:
		{
			break;	// Ignore LWSP.
		}

		case MIME_TOKEN_EOF:
		{
			bSetCharsetAttribute = true;
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			break;
		}

		case MIME_TOKEN_QUOTED_STRING:
		case MIME_TOKEN_ATOM:
		{
			size_t	nLen;

			// These tokens should be part of the character set following "charset=".
			nLen = strlen(m_pString);
			
			// Remember the value offset of the charset field.  This
			// is for the attribute code.

			if ( nLen > 0 && m_nCharsetValueOffset == 0 )
			{
				m_nCharsetValueOffset = m_nOffset;
			}

			if (strlen(m_szCharset) + nLen + 1 <= MAX_CHARSET_NAME_LEN)
			{
				if (m_iTokenID == MIME_TOKEN_ATOM)
				{
					strcat(m_szCharset, m_pString);
				}
				else
				{
					// The string is quoted and may contain CRLF pairs and TABs.
					// Strip off the quotes and any CRLFs and any TABs.
					char	*ptr1 = &m_pString[1];
					char	*ptr2 = &m_szCharset[strlen(m_szCharset)];
					char	ch;

					while (true)
					{
						ch = *ptr1++;
						if (ch != ASC_CHR_CR && ch != ASC_CHR_LF && ch != ASC_CHR_TAB)
							*ptr2++ = ch;
						if (ch == 0)
							break;
					}

				  // If a closing quote is present, strip it. It is possible
				  // that a missing dblquote will be encountered, in which case
				  // a email client might eat it anyway, so we have to as well.
				  nLen = strlen(m_szCharset);
				  if (nLen > 0 && m_szCharset[nLen-1] == ASC_CHR_DQUOTE)
					  m_szCharset[nLen - 1] = 0;
				}
			}
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			bSetCharsetAttribute = true;
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_CONTENT_TYPE);
			m_bScanForPairs = false;  // Stop scanning for pairs.
			m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
		}
	}

	// Save off the charset information for later consumption
	if ( bSetCharsetAttribute && m_nCharsetValueOffset > 0 )
	{

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
		m_pMIMEAttributes->SetContentTypeCharset(m_nCharsetValueOffset,&m_szCharset[0],strlen(m_szCharset));
#endif //*** End AS400 Attribute API Removal *** 

		m_nCharsetValueOffset = 0;
		m_szCharset[0] = 0;
	}

	return (rc);
}


inline int CMIMEParser::State_CONTENTTYPE11(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state just keeps appending everything up to the end of the line
	// as the boundary marker (following the "boundary=" keyword).
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

		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			break;
		}

		default:
		{
			char	*ptr = NULL;

			if( !m_fX_pkcs7_mime )
			{
				dec_assert(m_nBoundaryCount);
			}

			if (m_pString[0] == ASC_CHR_SEMI)
			{
				m_bScanForPairs = false;  // Stop scanning for pairs.
				m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;

				// Before we move on.  Let's see if this is an empty
				// boundary case.  If the boundary ptr is NULL in the
				// boundary record then we have found a 'boundary= ;' 
				// case and need to handle it.  We are going to issue
				// a warning in this case and create an empty boundary.

				if ( m_pszBoundary[m_nBoundaryCount - 1].m_pszBoundary == NULL )
				{
					// new the memory
					char* ptr = (char *)malloc(1);
					if (!ptr)
						return (PARSER_ERROR);
					
					// Create an empty 'C' string for the entry boundary
					*ptr = 0;

					// Save the address of the new memory in the nested boundary array.
					m_pszBoundary[m_nBoundaryCount - 1].m_pszBoundary = ptr;
					m_pszBoundary[m_nBoundaryCount - 1].m_nNestedLevel = m_nBoundaryNestLevel;

					EmitWarning(MIME_WARNING_EMPTY_BOUNDARY_SPECIFIED, MIME_TOKEN_BOUNDARY_START);
				}
				break;
			}

			if (m_pString[0] == ASC_CHR_COLON)
			{
				// Note that this is not RFC 822 compliant.
				EmitWarning(MIME_WARNING_UNEXPECTED_SEPARATOR, MIME_TOKEN_CONTENT_TYPE);
			}

			if( !m_fX_pkcs7_mime )
			{
				ptr = m_pszBoundary[m_nBoundaryCount - 1].m_pszBoundary;

				// Do we have memory allocated in this index of the nested boundary array?
				if (!ptr)
				{
					// new the memory
					ptr = (char *)malloc(m_nStringLength + 1);
					if (!ptr)
						return (PARSER_ERROR);

					// Save the address of the new memory in the nested boundary array.
					m_pszBoundary[m_nBoundaryCount - 1].m_pszBoundary = ptr;
					m_pszBoundary[m_nBoundaryCount - 1].m_nNestedLevel = m_nBoundaryNestLevel;
				}
				else
				{
					char	*pNewBoundaryString;
					size_t	nLen;

					nLen = strlen(ptr);
					pNewBoundaryString = (char *)malloc(nLen + m_nStringLength + 1);
					if (!pNewBoundaryString)
						return (PARSER_ERROR);

					strcpy(pNewBoundaryString, ptr);
					free(ptr);
					m_pszBoundary[m_nBoundaryCount - 1].m_pszBoundary = pNewBoundaryString;
					m_pszBoundary[m_nBoundaryCount - 1].m_nNestedLevel = m_nBoundaryNestLevel;
					ptr = &pNewBoundaryString[nLen];
				}
			}

			if (m_iTokenID == MIME_TOKEN_QUOTED_STRING)
			{
				size_t nLen      = 0;
				DWORD  dwCharset = 0;
				bool   bRFC2231  = (EMIT_FLAGS & RFC2231) ? true : false;

				// Decode the boundary if necessary
				bool fFound;

				char* pszTemp = (char*)malloc(m_nStringLength+1);
				if (!pszTemp)
					return (PARSER_ERROR);

				pszTemp[m_nStringLength] = 0;

				// Save the string
				memcpy( pszTemp, m_pString, m_nStringLength);  

				DecodeName( m_pString, bRFC2231, &dwCharset, &fFound);
				if(!fFound)
				{
					// There is a value here but it isn't encoded. Restore the previous value.
					memcpy( m_pString, pszTemp, m_nStringLength );
				}
				else
				{
					// Reset the string length since the boundary was encoded.
					m_nStringLength = strlen( m_pString );
				}
				
				free(pszTemp);

				nLen = m_nStringLength;

				if( !m_fX_pkcs7_mime )
				{
					if (nLen > 2)
					{
						// Remove the first and last characters. Note: this address still points to
						// the boundary index array also.
						memcpy(ptr, &m_pString[1], m_nStringLength - 2);
						ptr[m_nStringLength - 2] = 0;
						m_pszBoundary[m_nBoundaryCount - 1].m_nBoundaryStrLen = strlen(ptr);
					}
					else
					{
						ptr[0] = m_pString[1];
						ptr[1] = 0;
						m_pszBoundary[m_nBoundaryCount - 1].m_nBoundaryStrLen = 1;
					}
				}
			}
			else
			{
				if( !m_fX_pkcs7_mime )
				{
					memcpy(ptr, m_pString, m_nStringLength);
					ptr[m_nStringLength] = 0;
					m_pszBoundary[m_nBoundaryCount - 1].m_nBoundaryStrLen = strlen(m_pszBoundary[m_nBoundaryCount - 1].m_pszBoundary);
				}
			}

			m_fX_pkcs7_mime = false;

			if (m_iTokenID == MIME_TOKEN_QUOTED_STRING)
			{
				m_bScanForPairs = false;  // Stop scanning for pairs.
				m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
			}

			break;
		}
	}

	return (rc);
}

