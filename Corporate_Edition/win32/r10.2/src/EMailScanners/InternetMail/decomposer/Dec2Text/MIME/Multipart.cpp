// Multipart.cpp : MULTIPART states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

inline int CMIMEParser::State_MULTIPART1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	m_nBoundaryStart = 0;

	// Here we are looking for the multipart boundary prefix (CRLF "--").
	switch (m_iTokenID)
	{
		case MIME_TOKEN_ATOM:
		{
			// According to RFC 1521, all multipart boundaries *must*
			// be preceeded by a CRLF sequence.  We can't really ignore
			// this rule because if we do, we will get lots of false-positive
			// boundary detections.
			if (m_iPreviousTokenID == MIME_TOKEN_CRLF &&
				m_nStringLength >= 2 &&
				m_pString[0] == ASC_CHR_MINUS &&
				m_pString[1] == ASC_CHR_MINUS)
			{
				// Initialize the boundary test string.
				if (!SetBoundaryTest(&m_pString[2]))
				{
					rc = PARSER_ERROR;
					break;
				}

				m_nBoundaryStart = m_nOffset;
				m_ParseState = MIME_PARSER_STATE_MULTIPART2;
			}
			break;
		}

		case MIME_TOKEN_EOF:
		{
			if (m_bInPreamble)
			{
				// Emit the END_MARKER for the preamble.
				EMIT_ID = MIME_TOKEN_END_MARKER;
				DO_EMIT;  // Emit END_MARKER
				m_bInPreamble = false;
			}

			EMIT_ID = MIME_TOKEN_EOF;
			DO_EMIT;
			break;
		}

		default:
		{
			break;
		}
	}

	return (rc);
}


bool CMIMEParser::SetBoundaryTest(char *pszTest)
{
	dec_assert(pszTest);

	FreeBoundaryTest();

	m_pszBoundaryTest = (char *)malloc(strlen(pszTest) + 1);
	if (!m_pszBoundaryTest)
		return false;  // Failed to allocate memory for boundary test buffer.

	strcpy(m_pszBoundaryTest, pszTest);
	return true;
}


bool CMIMEParser::AppendBoundaryTest(char *pszTest)
{
	size_t	nLength;

	dec_assert(pszTest);
	dec_assert(m_pszBoundaryTest);

	// Here I am picking an arbitrary size limit for the maximum length
	// of a boundary test string.  This prevents us from growing this
	// buffer out of control.  According to RFC 1521, a boundary should
	// never be more than 70 characters in length, but we'll allow more
	// so that we can handle malformities up to a reasonable point.
	nLength = strlen(m_pszBoundaryTest) + strlen(pszTest) + 1;
	if (nLength > 32 * 1024)
		return true;  // Return true here - we just skip the append.

	{
		char* pszRealloced = (char *)realloc(m_pszBoundaryTest, nLength);
		if (!pszRealloced)
		{
			free(m_pszBoundaryTest);
			m_pszBoundaryTest = NULL;
			return false;  // Failed to re-allocate memory for boundary test buffer.
		}
		m_pszBoundaryTest = pszRealloced;
	}

	strcat(m_pszBoundaryTest, pszTest);
	return true;
}


void CMIMEParser::FreeBoundaryTest(void)
{
	if (m_pszBoundaryTest)
		free(m_pszBoundaryTest);
	m_pszBoundaryTest = NULL;
}


inline int CMIMEParser::State_MULTIPART2(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// We have seen a multipart prefix ("--").  Now we need to take
	// everything from here to the end of the line to be a possible
	// boundary.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_CRLF:
		{
			int		iBoundary;

			// See if we have a boundary match.  If so, begin parsing
			// the multipart header.
			iBoundary = CheckForBoundary();
			if (iBoundary != MIME_NOT_BOUNDARY)
			{
				if (m_bInPreamble)
				{
					// Emit the END_MARKER for the preamble.
					EMIT_OFFSET = m_nBoundaryStart;
					EMIT_ID = MIME_TOKEN_END_MARKER;
					DO_EMIT;  // Emit END_MARKER
					m_bInPreamble = false;
				}

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
				m_ParseState = MIME_PARSER_STATE_MULTIPART1;
			}
			break;
		}

		case MIME_TOKEN_EOF:
		{
			if (m_bInPreamble)
			{
				// Emit the END_MARKER for the preamble.
				EMIT_ID = MIME_TOKEN_END_MARKER;
				DO_EMIT;  // Emit END_MARKER
				m_bInPreamble = false;
			}

			EMIT_ID = MIME_TOKEN_EOF;
			DO_EMIT;
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


inline int CMIMEParser::State_MULTIPART3(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// We need to emit whatever token we have in our buffer.
	DO_EMIT;
	return (rc);
}


inline int CMIMEParser::CheckForBoundary(void)
{
	size_t	index;
	size_t	nLength;
	bool	bEnd = false;
	int		rc = MIME_NOT_BOUNDARY;

	if (!m_pszBoundaryTest)
		return (rc);

	nLength = strlen(m_pszBoundaryTest);

	// At this point we have a test boundary.  The Boundary
	// can be one of the following:
	//
	//	START BOUNDARY:
	//	END BOUNDARY:
	//	NOT A BOUNDARY:
	//
	// We need to first scan the boundary list looking for
	// an exact match.  Since we will only find an exact match
	// on starting boundaries we don't have to deal with the
	// end boundary case at this point.  We cannot have an 
	// end boundary in our list of valid boundaries since the
	// end boundary has an additional two dashes at the end of
	// the string.

	for (index = 0; index < m_nBoundaryCount; index++)
	{
		if (	m_pszBoundary[index].m_nBoundaryStrLen == nLength 
			&&	m_pszBoundary[index].m_pszBoundary != NULL
			&&	!memcmp(m_pszBoundary[index].m_pszBoundary, m_pszBoundaryTest, nLength))
		{
			rc = MIME_BOUNDARY_START;
			break;
		}
	}

	// If this is not a start boundary we need to see if it is 
	// a possible stop boundary.  We test this by removing two
	// trailing dashes, and run through our list again.

	if ( rc != MIME_BOUNDARY_START )
	{
		if (nLength > 2 &&
			m_pszBoundaryTest[nLength - 2] == ASC_CHR_MINUS &&
			m_pszBoundaryTest[nLength - 1] == ASC_CHR_MINUS)
		{
			nLength -= 2;

			// Search our list of boundary markers looking for 
			// the boundary marker at the correct nested level.

			for (index = 0; index < m_nBoundaryCount; index++)
			{
				if (	m_pszBoundary[index].m_nBoundaryStrLen == nLength
					&&	m_pszBoundary[index].m_nNestedLevel == m_nBoundaryNestLevel
					&&	m_pszBoundary[index].m_pszBoundary != NULL
					&&  !memcmp(m_pszBoundary[index].m_pszBoundary, m_pszBoundaryTest, nLength)
				   )
				{
					if (m_nBoundaryNestLevel)
						m_nBoundaryNestLevel--;

					m_pszBoundary[index].m_fClosed = true;

					rc = MIME_BOUNDARY_END;
					break;
				}
			}

			// If we did not find the ending boundary at the correct
			// nested level then look for the boundary marker ignoring
			// the nested level.  If we find the boundary we will use it
			// but we will issue a WARNING.

			if ( rc != MIME_BOUNDARY_END )
			{
				for (index = 0; index < m_nBoundaryCount; index++)
				{
					if (	m_pszBoundary[index].m_nBoundaryStrLen == nLength
						&&	m_pszBoundary[index].m_pszBoundary != NULL
						&&  !memcmp(m_pszBoundary[index].m_pszBoundary, m_pszBoundaryTest, nLength))
					{
						// This is not right.  We are closing a different
						// nesting level than the one we should be closing.
						// Emit a warning for this.

						// If this boundary has already been closed, we have a duplicate closing boundary.
						// Do not warn on outermost duplicates.

						// ( m_pszBoundary[index].m_fClosed && m_nBoundaryNestLevel == 0 ) Don't warn
						if( !m_pszBoundary[index].m_fClosed || m_nBoundaryNestLevel != 0 )
						{
							EmitWarning(MIME_WARNING_UNEXPECTED_BOUNDARY_CLOSURE, MIME_TOKEN_BOUNDARY_START);
						}

						// Attempt to re-synchronize the boundary level by
						// setting the current nesting level to the level
						// that is being closed.  Note that this will not
						// always succeed in re-synchronizing, but is a reasonable
						// recovery approach.
						m_nBoundaryNestLevel = index + 1;

						if (m_nBoundaryNestLevel)
							m_nBoundaryNestLevel--;

						rc = MIME_BOUNDARY_END;
						break;
					}
				}
			}
		}
	}
	return (rc);
}
