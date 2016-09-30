// MultipartHeader.cpp : MULTIPARTHEADER states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

inline int CMIMEParser::State_MULTIPARTHEADER1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// Always reset the field ID here so that we don't accidentally
	// emit a field ID.
	m_iFieldID = MIME_TOKEN_LAST;

	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			// Skip over any whitespace that appears before the header.
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_INVALID_MULTIPART_HEADER, m_iTokenID);
			break;
		}

		case MIME_TOKEN_SPECIAL:
		{
			// OK, we found a SPECIAL token.  Move to HEADERFIELD3 state.
			if (m_pString[0] == ASC_CHR_COLON)
			{
				if ( m_AtomCandidate.getPosition() == 0 )
				{
					EmitWarning(MIME_WARNING_INVALID_HEADER, m_iTokenID);

				}
				else
				{
					if ( (rc = ProcessHeaderAtom()) == PARSER_ERROR )
					{ return rc; }

					// At this point we have found the sequence: KEY:
					// We may or may not recognize the key but it sure
					// looks like MIME at this point.  

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
					if ( m_nStartOffsetOfHeader == -1 ) 
					{
						m_nStartOffsetOfHeader = m_nAtomCandidateOffset;
						m_pMIMEAttributes->SetHeaderBaseOffset(m_nStartOffsetOfHeader);
					}

					// Save the offset to the byte past the colon
					m_nValueStartOffset = m_nOffset+1;
#endif //*** End AS400 Attribute API Removal *** 

					if (m_iFieldID != MIME_TOKEN_UNKNOWN_FIELD)
					{
						m_nHeaderLinesIDed++;

						if (m_pArchive->m_dwEnhancedID & TEXT_OPTION_LOOSE_MIME_HEADER_ID)
						{
							// This is a special trigger for processing malformed messages.
							// If we have identified at least 1 well-formed header line,
							// we will start ignoring malformed header lines.
							m_bFuzzyHeader = true;
						}
					}

					m_bScanForPairs = true;	// Begin scanning for pairs.
					m_ParseState = MIME_PARSER_STATE_MULTIPARTHEADER3;
				}
			} 
			else
			{
				m_iTokenID = MIME_TOKEN_ATOM;
				State_MULTIPARTHEADER1();
			}

			break;
		}
		case MIME_TOKEN_CRLF:
		{
			size_t atom_size = 0;
			
			// There should be no atom when we hit the CRLF

			if ( (atom_size = m_AtomCandidate.getPosition()) != 0 )
			{
				EmitWarning(MIME_WARNING_INVALID_HEADER, m_iTokenID);

			}
			
			if ( (rc = ProcessHeaderAtom()) == PARSER_ERROR )
			{ return rc; }

			// m_bBoundarySeparatorFoundInHeader is set by ProcessHeaderAtom.  This
			// value gets set to true when a boundary separator is found inside the MIME
			// header.
			if ( m_bBoundarySeparatorFoundInHeader == false && atom_size == 0)
			{

				
#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
				if ( m_nStartOffsetOfHeader == -1 ) 
				{
					// See if this multipart section contains any headers
					// at all.  If there are no headers then the atom
					// offset will preceed the boundary.  If this is 
					// the case then use m_nOffset instead of the atom
					// candidate
					if ( m_nAtomCandidateOffset < m_nBoundaryStart )
					{
						if ( m_nOffset > m_nBoundaryStart )
						{
							m_nStartOffsetOfHeader = m_nOffset;
						}
						else
						{
							m_nStartOffsetOfHeader = m_nBoundaryStart;
						}
					}
					else
					{
						m_nStartOffsetOfHeader = m_nAtomCandidateOffset;
					}
				}
#endif //*** End AS400 Attribute API Removal *** 


				// This is a blank header line.  Since there is no requirement
				// that a multipart section have even one header field, we move
				// to the next state which will process the multipart body.


				EMIT_ID = MIME_TOKEN_HEADER_SEPARATOR;
				EMIT_DATA = m_nStringLength;
				DO_EMIT;

				m_bInHeader = false;  // We are no longer parsing header lines.

				EMIT_OFFSET = EMIT_OFFSET + m_nStringLength;  // Move marker forward to account for CRLF
				EMIT_ID = MIME_TOKEN_END_MARKER;
				DO_EMIT;
				m_ParseState = MIME_PARSER_STATE_MULTIPARTBODY1;
			}
			break;
		}

		case MIME_TOKEN_EOF:
		{
			if ( (rc = ProcessHeaderAtom()) == PARSER_ERROR )
			{ return rc; }

			EMIT_ID = MIME_TOKEN_EOF;
			DO_EMIT;
			m_ParseState = MIME_PARSER_STATE_MULTIPARTBODY1;
			break;
		}

		case MIME_TOKEN_ATOM:
		{
			try 
			{
				if ( m_iAtomCandidateBufSize == 0 )
				{
					m_nAtomCandidateOffset = m_nOffset;
					m_nAtomCandidatePreviousToken = m_iPreviousTokenID;
				}
				m_AtomCandidate.setAt(m_iAtomCandidateBufSize,m_pString,m_nStringLength);
				m_iAtomCandidateBufSize += m_nStringLength;
				m_AtomCandidate[m_iAtomCandidateBufSize] = 0;
			} 
			catch ( const char*  )
			{
				m_AtomCandidate.reset();
				rc = PARSER_ERROR;
			}
			m_ParseState = MIME_PARSER_STATE_MULTIPARTHEADER1;
			break;
		}

		default:
		{
			// Skip over any tokens that appear before the header.
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_INVALID_MULTIPART_HEADER, m_iTokenID);
			break;
		}
	}

	return (rc);
}




inline int CMIMEParser::State_MULTIPARTHEADER3(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			// Skip over any whitespace following the ":" separator.
			break;
		}

		case MIME_TOKEN_CRLF:
		case MIME_TOKEN_EOF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_bScanForPairs = false;  // Stop scanning for pairs.
			m_ParseState = MIME_PARSER_STATE_MULTIPARTHEADER1;
			break;
		}

		case MIME_TOKEN_SPECIAL:
		case MIME_TOKEN_ATOM:
		case MIME_TOKEN_QUOTED_STRING:
		case MIME_TOKEN_DOMAIN_LITERAL:
		case MIME_TOKEN_COMMENT:
		case MIME_TOKEN_ROUTE_ADDR:
		case MIME_TOKEN_QUOTED_PAIR:
		{
			// At this point we have identified a sequence that follows
			// the form: fieldID [LWSP] ":" [LWSP] text
			m_bScanForPairs = false;  // Stop scanning for pairs.
			
#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
			m_nKeyID = m_iFieldID;
			m_nKeyOffset = m_nAtomCandidateOffset;
#endif //*** End AS400 Attribute API Removal *** 

			switch (m_iFieldID)
			{
				case MIME_TOKEN_CONTENT_TYPE:
					m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
					break;

				case MIME_TOKEN_CONTENT_DISPOSITION:
					m_ParseState = MIME_PARSER_STATE_CONTENTDISPOSITION1;
					break;

				case MIME_TOKEN_CONTENT_TRANSFER_ENCODING:
					m_ParseState = MIME_PARSER_STATE_CONTENTTRANSFERENCODING1;
					break;

				case MIME_TOKEN_UNKNOWN_FIELD:
					// We did not identify the field so just keep reading in
					// tokens until we hit a CRLF, then look for a new header
					// field.
					m_ParseState = MIME_PARSER_STATE_MULTIPARTHEADER4;
					break;

				default:
					m_ParseState = MIME_PARSER_STATE_MULTIPARTHEADER4;
					break;
			}

			rc = PARSER_CHANGE_STATE;
			break;
		}

		default:
		{
			// This is an unexpected token.  We recover by going back to
			// looking for header lines.
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_INVALID_MULTIPART_HEADER, m_iTokenID);
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_bScanForPairs = false;  // Stop scanning for pairs.
			m_ParseState = MIME_PARSER_STATE_MULTIPARTHEADER1;
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_MULTIPARTHEADER4(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state just keeps reading in tokens until we see a CRLF.
	if (m_iTokenID == MIME_TOKEN_CRLF ||
		m_iTokenID == MIME_TOKEN_EOF)
	{
		EMIT_ID = MIME_TOKEN_END_MARKER;
		DO_EMIT;  // Emit END_MARKER
		m_ParseState = MIME_PARSER_STATE_MULTIPARTHEADER1;
	}

	return (rc);
}
