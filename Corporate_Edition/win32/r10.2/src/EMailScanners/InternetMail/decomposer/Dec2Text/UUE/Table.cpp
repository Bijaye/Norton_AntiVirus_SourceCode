// Table.cpp : TABLE states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

int CUUEParser::State_TABLE1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state just keeps reading in tokens until we see a new-line (or EOF).
	switch (m_iTokenID)
	{
		case UUE_TOKEN_NEWLINE:
		case UUE_TOKEN_EOF:
		{
			// The next line should contain the first set of 32 characters of the table.
			m_bNewLine = true;
			m_nTableIndex = 0;
			m_ParseState = UUE_PARSER_STATE_TABLE2;
			break;
		}

		default:
		{
			m_ParseState = UUE_PARSER_STATE_HEADERLINE1;
			break;
		}
	}

	return (rc);
}


int CUUEParser::State_TABLE2(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state just keeps reading in tokens until we see a new-line (or EOF).
	switch (m_iTokenID)
	{
		case UUE_TOKEN_NEWLINE:
		case UUE_TOKEN_EOF:
		{
			// The next line should contain the second set of 32 characters of the table.
			m_bNewLine = true;
			m_ParseState = UUE_PARSER_STATE_TABLE3;
			break;
		}

		default:
		{
			size_t	nIndex = 1;

			// Copy the first set of 32 characters into m_uulookup.
			while (m_nTableIndex < 64)
			{
				if (nIndex > m_nStringLength)
					break;

				m_uulookup[32 + m_nTableIndex] = m_pString[nIndex - 1] - 32;
				m_nTableIndex++;
				nIndex++;
			}

			break;
		}
	}

	return (rc);
}


int CUUEParser::State_TABLE3(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state just keeps reading in tokens until we see a new-line (or EOF).
	switch (m_iTokenID)
	{
		case UUE_TOKEN_NEWLINE:
		case UUE_TOKEN_EOF:
		{
			if (m_nTableIndex > 0 && m_nTableIndex != 64)
			{
				// The table is corrupt (either too many or too few table
				// characters were found).  Overwrite the table with the
				// default table and keep going.
				for (int i = 0; i < sizeof(g_uulookup); i++)
					m_uulookup[i] = g_uulookup[i];
			}

			// Go back to looking for the header line.
			m_bNewLine = true;
			m_ParseState = UUE_PARSER_STATE_HEADERLINE2;
			break;
		}

		default:
		{
			size_t	nIndex = 1;

			// Copy the second set of 32 characters into m_uulookup.
			while (m_nTableIndex < 64)
			{
				if (nIndex > m_nStringLength)
					break;

				m_uulookup[32 + m_nTableIndex] = m_pString[nIndex - 1] - 32;
				m_nTableIndex++;
				nIndex++;
			}

			break;
		}
	}

	return (rc);
}
