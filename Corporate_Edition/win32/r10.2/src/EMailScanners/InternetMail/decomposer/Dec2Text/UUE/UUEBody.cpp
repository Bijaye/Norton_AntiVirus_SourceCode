// UUEBody.cpp : BODY states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

extern unsigned char g_uulookup[256];


int CUUEParser::State_BODY1(void)
{
	int   rc = PARSER_OK;	// Assume parsing should continue

	bool  IsValidUUE = false;

	size_t	nLineLength	= m_nStringLength;

	// This state just keeps reading in tokens until we see an END token (or EOF).
	switch (m_iTokenID)
	{
		case UUE_TOKEN_END:
		case UUE_TOKEN_EOF:
		{
			m_ParseState = UUE_PARSER_STATE_HEADERLINE1;
			break;
		}

		case UUE_TOKEN_NEWLINE:
		{
			m_bNewLine = true;
			break;
		}

		case UUE_TOKEN_LWSP:
		{
			// Skip any whitespace at the beginning of a line.
			if (m_bNewLine)
				break;

			// This is whitespace within an encoded line.  We need
			// to decode this so just fall through to the default
			// case below.
		}

		default:
		{
			char	*ptr;

			// This data should be part of the UU-encoded body.
			// We need to decode at least a portion of it to make sure
			// we are actually looking at UUE data.

			ptr = m_pString;

			if (m_bNewLine)
			{
				// This is the first non-whitespace on a new line.
				// The first character represents the length of the
				// encoded data that follows on this text line.
				if (m_pString[0] == ASC_CHR_GRAVE)
				{
					break;	// End of encoded data.
				}
				else if (m_pString[0] > ASC_CHR_SPACE)
				{
					m_nEncodedLineLength = m_pString[0] - ASC_CHR_SPACE;
					ptr++;
					m_nStringLength--;
				}
				else
				{
					break;
				}
			}

			m_bNewLine = false;

			IsValidUUE = IsValidUUEContainer( &m_bIdentified, m_fStrongHeader, (const unsigned char*)m_pString, nLineLength, &rc );

			break;
		}
	}

	return (rc);
}
