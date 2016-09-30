// Begin.cpp : BEGIN states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

int CUUEParser::State_BEGIN1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	//	begin 'all other variations'	<--- Weak header. data validation required.
	m_ParseState = UUE_PARSER_STATE_FORMATERROR;

	// We don't yet have a well formed header
	m_fStrongHeader = false;

	switch (m_iTokenID)
	{
		case UUE_TOKEN_LWSP:
		{
			if( m_nStringLength == 1 )
			{
				// Only allow spaces i.e. no tabs
				if( m_pString[0] == ASC_CHR_SPACE )
				{
					m_ParseState = UUE_PARSER_STATE_SINGLESPACE1;
				}
			}
			else if( m_nStringLength == 2 )
			{
				if( m_pString[0] == ASC_CHR_SPACE && m_pString[1] == ASC_CHR_SPACE )
				{
					m_ParseState = UUE_PARSER_STATE_DOUBLESPACE;
				}
			}
			break;
		}
		case UUE_TOKEN_NEWLINE:
		{
			m_bNewLine = true;
			m_nEncodedLineLength = 0;
			m_nQuadIndex = 0;
			m_ParseState = UUE_PARSER_STATE_BODY1;
			break;
		}
		case UUE_TOKEN_EOF:
		{
			m_ParseState = UUE_PARSER_STATE_HEADERLINE1;
			rc = PARSER_DONE;
			break;
		}
		default:
		{
			// Skip over everything else.
			break;
		}
	}

	return (rc);
}

int CUUEParser::State_SINGLESPACE1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	char*	ptr;
	int		iOctalLen = 0;

	//	begin 'all other variations'	<--- Weak header. data validation required.
	m_ParseState = UUE_PARSER_STATE_FORMATERROR;

	switch (m_iTokenID)
	{
		case UUE_TOKEN_ATOM:
		{
			// Now capture the octal mode
			ptr = m_pString;
			while( IsValidOctalChar( *ptr ) )
			{
				iOctalLen++;
				ptr++;
			}

			// There is octal data
			if ( iOctalLen > 0 )
			{
				m_ParseState = UUE_PARSER_STATE_OCTALNUMBER;
			}

			break;
		}
		case UUE_TOKEN_NEWLINE:
		{
			// Weak header
			m_fStrongHeader = false;

			m_bNewLine = true;
			m_nEncodedLineLength = 0;
			m_nQuadIndex = 0;
			m_ParseState = UUE_PARSER_STATE_BODY1;
			break;
		}
		case UUE_TOKEN_EOF:
		{
			m_ParseState = UUE_PARSER_STATE_HEADERLINE1;
			rc = PARSER_DONE;
			break;
		}
		default:
		{
			break;
		}
	}

	return (rc);
}

int CUUEParser::State_DOUBLESPACE(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	//	begin 'all other variations'	<--- Weak header. data validation required.
	m_ParseState = UUE_PARSER_STATE_FORMATERROR;

	//	begin  filename					<--- Well formed header.  No data validation
	//		 ^^  Note the required double space if no mode number specified.

	switch (m_iTokenID)
	{
		case UUE_TOKEN_ATOM:
		{
			if( m_nStringLength > 0 )
			{
				m_ParseState = UUE_PARSER_STATE_FILENAME;
			}
			break;
		}
		case UUE_TOKEN_NEWLINE:
		{
			// Weak header
			m_fStrongHeader = false;

			m_bNewLine = true;
			m_nEncodedLineLength = 0;
			m_nQuadIndex = 0;
			m_ParseState = UUE_PARSER_STATE_BODY1;
			break;
		}
		case UUE_TOKEN_EOF:
		{
			m_ParseState = UUE_PARSER_STATE_HEADERLINE1;
			rc = PARSER_DONE;
			break;
		}
		default:
		{
			break;
		}
	}

	return (rc);
}

int CUUEParser::State_OCTALNUMBER(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	//	begin 'all other variations'	<--- Weak header. data validation required.
	m_ParseState = UUE_PARSER_STATE_FORMATERROR;

	//	begin 'octal number' filename	<--- Well formed header.  No data validation
	//	begin 'octal number' 			<--- Well formed header.  No data validation
	//						^ Note the required space after the octal number

	switch (m_iTokenID)
	{
		case UUE_TOKEN_LWSP:
		{
			// Do we have a single space character
			if( m_nStringLength == 1 )
			{
				m_ParseState = UUE_PARSER_STATE_SINGLESPACE2;
			}
			break;
		}
		case UUE_TOKEN_NEWLINE:
		{
			// Well formed header
			m_fStrongHeader = true;

			m_bNewLine = true;
			m_nEncodedLineLength = 0;
			m_nQuadIndex = 0;
			m_ParseState = UUE_PARSER_STATE_BODY1;
			break;
		}
		case UUE_TOKEN_EOF:
		{
			m_ParseState = UUE_PARSER_STATE_HEADERLINE1;
			rc = PARSER_DONE;
			break;
		}
		default:
		{
			break;
		}
	}

	return (rc);
}

int CUUEParser::State_SINGLESPACE2(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	//	begin 'all other variations'	<--- Weak header. data validation required.
	m_ParseState = UUE_PARSER_STATE_FORMATERROR;

	//	begin 'octal number' filename	<--- Well formed header.  No data validation
	//	begin 'octal number' 			<--- Well formed header.  No data validation
	//						^ Note the required space after the octal number

	switch (m_iTokenID)
	{
		case UUE_TOKEN_ATOM:
		{
			// Do we have a file name
			if( m_nStringLength > 0 )
			{
				m_ParseState = UUE_PARSER_STATE_FILENAME;
			}
			break;
		}
		case UUE_TOKEN_NEWLINE:
		{
			// Well formed header
			m_fStrongHeader = true;

			m_bNewLine = true;
			m_nEncodedLineLength = 0;
			m_nQuadIndex = 0;
			m_ParseState = UUE_PARSER_STATE_BODY1;
			break;
		}
		case UUE_TOKEN_EOF:
		{
			m_ParseState = UUE_PARSER_STATE_HEADERLINE1;
			rc = PARSER_DONE;
			break;
		}
		default:
		{
			break;
		}
	}

	return (rc);
}

int CUUEParser::State_FILENAME(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	//	begin 'all other variations'	<--- Weak header. data validation required.
	m_ParseState = UUE_PARSER_STATE_FORMATERROR;

	switch (m_iTokenID)
	{
		case UUE_TOKEN_NEWLINE:
		{
			// Well formed header
			m_fStrongHeader = true;

			m_bNewLine = true;
			m_nEncodedLineLength = 0;
			m_nQuadIndex = 0;
			m_ParseState = UUE_PARSER_STATE_BODY1;
			break;
		}
		case UUE_TOKEN_LWSP:
		{
			m_ParseState = UUE_PARSER_STATE_FILENAME;
			break;
		}
		case UUE_TOKEN_ATOM:
		{
			m_ParseState = UUE_PARSER_STATE_FILENAME;
			break;
		}
		case UUE_TOKEN_EOF:
		{
			m_ParseState = UUE_PARSER_STATE_HEADERLINE1;
			rc = PARSER_DONE;
			break;
		}
		default:
		{
			break;
		}
	}

	return (rc);
}

int CUUEParser::State_FORMATERROR(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	//	begin 'all other variations'	<--- Weak header. data validation required.
	m_ParseState = UUE_PARSER_STATE_FORMATERROR;

	// We don't have a well formed header
	m_fStrongHeader = false;

	switch (m_iTokenID)
	{
		case UUE_TOKEN_NEWLINE:
		{
			m_bNewLine = true;
			m_nEncodedLineLength = 0;
			m_nQuadIndex = 0;
			m_ParseState = UUE_PARSER_STATE_BODY1;
			break;
		}
		case UUE_TOKEN_EOF:
		{
			m_ParseState = UUE_PARSER_STATE_HEADERLINE1;
			rc = PARSER_DONE;
			break;
		}
		default:
		{
			// Skip over everything else.
			break;
		}
	}

	return (rc);
}

int CUUEParser::State_BEGIN2(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state just keeps reading in tokens until we see a CRLF.
	if (m_iTokenID == UUE_TOKEN_NEWLINE)
	{
		m_ParseState = UUE_PARSER_STATE_HEADERLINE1;
	}

	return (rc);
}

int CUUEParser::State_BEGIN3(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state just keeps reading in tokens until we see a CRLF.
	if (m_iTokenID == UUE_TOKEN_NEWLINE)
	{
		m_ParseState = UUE_PARSER_STATE_HEADERLINE1;
	}

	return (rc);
}


int CUUEParser::State_BEGIN4(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state just keeps reading in tokens until we see an END token (or EOF).
	switch (m_iTokenID)
	{
		case UUE_TOKEN_END:
		case UUE_TOKEN_EOF:
		{
			m_ParseState = UUE_PARSER_STATE_HEADERLINE1;

			// This is too loose of an identification...
			m_bIdentified = true;
			rc = PARSER_IDENTIFIED;
			break;
		}

		default:
		{
			// Skip over everything else.
			break;
		}
	}

	return (rc);
}
