// Date.cpp : DATE states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

// Military timezone designations
static int g_timezone[26] =
{
	-1,		// A
	-2,		// B
	-3,		// C
	-4,		// D
	-5,		// E
	-6,		// F
	-7,		// G
	-8,		// H
	-9,		// I
	0,		// J
	-10,	// K
	-11,	// L
	-12,	// M
	1,		// N
	2,		// O
	3,		// P
	4,		// Q
	5,		// R
	6,		// S
	7,		// T
	8,		// U
	9,		// V
	10,		// W
	11,		// X
	12,		// Y
	0		// Z
};


inline int CMIMEParser::State_DATE1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for the first atom which should be either the
	// day of the week (e.g. "Mon", "Tue", etc.) or the day of the month
	// (e.g. "15").
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			// Ignore any whitespace here.
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
			// Identify the day of the week.
			m_pString[3] = 0; // Truncate the atom to 3 characters for the comparison.
			m_iFieldID = IdentifyAtom(MIME_ATOM_GROUP_DAYOFWEEK);
			if (m_iFieldID != MIME_TOKEN_LAST)
			{
				EMIT_ID = m_iFieldID;
				DO_EMIT;  // Emit the identified field.
				m_ParseState = MIME_PARSER_STATE_DATE2;
				break;
			}

			// If the first atom is not a recognized day of the week, it should
			// be the day of the month (e.g. "15").
			// This should be a 1 or 2 digit integer.
			m_pString[m_nStringLength] = 0;
			m_dwDay = (unsigned long)ascii_atoi(m_pString);
			if (m_dwDay > 31)
			{
				m_dwDay = 1;

				// Note that this is not RFC 822 compliant.
				EmitWarning(MIME_WARNING_INVALID_DAYOFMONTH, MIME_TOKEN_DATE);
			    m_ParseState = MIME_PARSER_STATE_DATE3;  // Now look for the month.
				break;
			}

			if (m_dwDay < 1)
			{
				// Now check to see if the atom is a valid month.
				m_ParseState = MIME_PARSER_STATE_DATE3;
				rc = PARSER_CHANGE_STATE;
				break;
			}

			// Now look for the next atom to be the month.
			m_ParseState = MIME_PARSER_STATE_DATE3;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_DATE);
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_DATE2(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for an atom which should be the
	// day of the month.  This state will also accept LWSP and a ",".
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			// Ignore any whitespace here.
			break;
		}

		case MIME_TOKEN_SPECIAL:
		{
			if (m_pString[0] == ASC_CHR_SLASH)
			{
				// We'll assume the date is in the form mm/dd/yyyy.
				// Just skip this token.
				break;
			}

			// Ignore any delimiters here.
			if (m_pString[0] != ASC_CHR_COMMA)
			{
				// Note that this is not RFC 822 compliant.
				EmitWarning(MIME_WARNING_UNEXPECTED_SEPARATOR, MIME_TOKEN_DATE);
			}
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
			// This should be a 1 or 2 digit integer.
			m_dwDay = (unsigned long)ascii_atoi(m_pString);
			if (m_dwDay > 31)
			{
				m_dwDay = 1;

				// Note that this is not RFC 822 compliant.
				EmitWarning(MIME_WARNING_INVALID_DAYOFMONTH, MIME_TOKEN_DATE);
			    m_ParseState = MIME_PARSER_STATE_DATE3;  // Now look for the month.
				break;
			}

			if (m_dwDay < 1)
			{
				// Now check to see if the atom is a valid month.
				m_ParseState = MIME_PARSER_STATE_DATE3;
				rc = PARSER_CHANGE_STATE;
				break;
			}

			m_ParseState = MIME_PARSER_STATE_DATE3;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_DATE);
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_DATE3(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for an atom which should be the month.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_SPECIAL:
		{
			if (m_pString[0] == ASC_CHR_SLASH)
			{
				// We'll assume the date is in the form mm/dd/yyyy.
				// Right now the month is in the m_dwDay field, so move it.
				m_dwMonth = m_dwDay;
				m_ParseState = MIME_PARSER_STATE_DATE11;
			}

			break;
		}

		case MIME_TOKEN_LWSP:
		{
			// Ignore any whitespace here.
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
			// Identify the month.
			m_pString[3] = 0; // Truncate the atom the 3 characters for the comparison.
			m_iFieldID = IdentifyAtom(MIME_ATOM_GROUP_MONTH);
			if (m_iFieldID != MIME_TOKEN_LAST)
			{
				m_dwMonth = (m_iFieldID - MIME_TOKEN_JANUARY) + 1;
				EMIT_ID = m_iFieldID;
				DO_EMIT;  // Emit the identified field.
			}
			else
			{
				// We did not identify the field.
				// Note that this is not RFC 822 compliant.
				EmitWarning(MIME_WARNING_INVALID_MONTH, MIME_TOKEN_DATE);
			}

			m_ParseState = MIME_PARSER_STATE_DATE4;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_DATE);
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_DATE4(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for an atom which should be the year.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		case MIME_TOKEN_SPECIAL:
		{
			// Ignore any whitespace here.
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
			// This should be a 1 or 2 digit integer.
			m_dwYear = (unsigned long)ascii_atoi(m_pString);
			if (m_dwYear < 32)
			{
				// This looks like it's actually a day of the month.
				if (m_dwDay == 0)
				{
					m_dwDay = m_dwYear;
					m_dwYear = 1980;
					break;  // Stay in this state - the next atom should be the year then.
				}

				// We have already seen what should be the day of the month.
				// This is probably the hour.
				m_dwYear = 1980;
				m_ParseState = MIME_PARSER_STATE_DATE5;
				rc = PARSER_CHANGE_STATE;
				break;
			}
			else if (m_dwYear < 1980)
			{
				m_dwYear += 1900;
			}
			
			if (m_dwYear < 1980 || m_dwYear > 9999)
			{
				m_dwYear = 1980;

				// Note that this is not RFC 822 compliant.
				EmitWarning(MIME_WARNING_INVALID_YEAR, MIME_TOKEN_DATE);
			}

			m_ParseState = MIME_PARSER_STATE_DATE5;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_DATE);
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_DATE5(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for an atom which should be the hour.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			// Ignore any whitespace here.
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
			// This should be a 1 or 2 digit integer.
			m_dwHour = (unsigned long)ascii_atoi(m_pString);
			if (m_dwHour > 23)
			{
				m_dwHour = 0;

				// Note that this is not RFC 822 compliant.
				EmitWarning(MIME_WARNING_INVALID_HOUR, MIME_TOKEN_DATE);
			}

			m_ParseState = MIME_PARSER_STATE_DATE6;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_DATE);
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_DATE6(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for a special which should be a ":".
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			// Ignore any whitespace here.
			break;
		}

		case MIME_TOKEN_SPECIAL:
		{
			if (m_pString[0] != ASC_CHR_COLON)
			{
				// Note that this is not RFC 822 compliant.
				EmitWarning(MIME_WARNING_UNEXPECTED_SEPARATOR, MIME_TOKEN_DATE);
			}
			m_ParseState = MIME_PARSER_STATE_DATE7;
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
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_DATE);
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_DATE7(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for an atom which should be the minute.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			// Ignore any whitespace here.
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
			// This should be a 1 or 2 digit integer.
			m_dwMinute = (unsigned long)ascii_atoi(m_pString);
			if (m_dwMinute > 59)
			{
				m_dwMinute = 0;

				// Note that this is not RFC 822 compliant.
				EmitWarning(MIME_WARNING_INVALID_MINUTE, MIME_TOKEN_DATE);
			}

			m_ParseState = MIME_PARSER_STATE_DATE8;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_DATE);
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_DATE8(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for either a special which should be a ":" or
	// an atom which should be the timezone.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			// Ignore any whitespace here.
			break;
		}

		case MIME_TOKEN_SPECIAL:
		{
			if (m_pString[0] != ASC_CHR_COLON)
			{
				// Note that this is not RFC 822 compliant.
				EmitWarning(MIME_WARNING_UNEXPECTED_SEPARATOR, MIME_TOKEN_DATE);
			}
			m_ParseState = MIME_PARSER_STATE_DATE10;
			break;
		}

		case MIME_TOKEN_ATOM:
		{
			m_iFieldID = IdentifyAtom(MIME_ATOM_GROUP_TIMEZONE);
			if (m_iFieldID != MIME_TOKEN_LAST)
			{
				switch (m_iFieldID)
				{
					case MIME_TOKEN_UT:
					case MIME_TOKEN_GMT:
						m_iTimezoneDifferential = 0;
						break;

					case MIME_TOKEN_EST:
						m_iTimezoneDifferential = -5;
						break;

					case MIME_TOKEN_EDT:
						m_iTimezoneDifferential = -4;
						break;

					case MIME_TOKEN_CST:
						m_iTimezoneDifferential = -6;
						break;

					case MIME_TOKEN_CDT:
						m_iTimezoneDifferential = -5;
						break;

					case MIME_TOKEN_MST:
						m_iTimezoneDifferential = -7;
						break;

					case MIME_TOKEN_MDT:
						m_iTimezoneDifferential = -6;
						break;

					case MIME_TOKEN_PST:
						m_iTimezoneDifferential = -8;
						break;

					case MIME_TOKEN_PDT:
						m_iTimezoneDifferential = -7;
						break;
				}

				m_ParseState = MIME_PARSER_STATE_DATE9;
				break;
			}

			char	ch;

			ch = m_pString[0];
			if (ch >= ASC_CHR_A && ch <= ASC_CHR_Z)
			{
				m_iTimezoneDifferential = g_timezone[ch - ASC_CHR_A];
				m_ParseState = MIME_PARSER_STATE_DATE9;
				break;
			}

			if (ch >= ASC_CHR_a && ch <= ASC_CHR_z)
			{
				m_iTimezoneDifferential = g_timezone[ch - ASC_CHR_a];
				m_ParseState = MIME_PARSER_STATE_DATE9;
				break;
			}

			// Here we run through a simple state machine to parse the
			// differential string (("+" / "-") HHMM).
			// State 0: Look for + or -.
			// State 1: Get the number of hours (HH).
			// State 2: Get the number of minutes (MM).
			int		iState = 0;
			bool	bMinus = false;
			int		iHH = 0;
			int		iMM = 0;
			char	*ptr;
			char	szValue[3];

			ptr = m_pString;
			if (ch != ASC_CHR_MINUS && ch != ASC_CHR_PLUS)
			{
				// This is probably a year, not a timezone differential.
				// If we have not set a year yet, use whatever we see here.
				if (m_dwYear == 1980)
				{
					m_dwYear = (unsigned long)ascii_atoi(m_pString);
					if (m_dwYear < 1980)
					{
						m_dwYear += 1900;
					}
					
					if (m_dwYear < 1980 || m_dwYear > 9999)
					{
						m_dwYear = 1980;

						// Note that this is not RFC 822 compliant.
						EmitWarning(MIME_WARNING_INVALID_YEAR, MIME_TOKEN_DATE);
					}

					break;  // Just stay in this state.
				}
			}

			while (*ptr != 0)
			{
				if (iState == 0)
				{
					if (*ptr == ASC_CHR_MINUS)
					{
						bMinus = true;
						ptr++;
					}

					if (*ptr == ASC_CHR_PLUS)
					{
						ptr++;
					}

					iState = 1;
				}
				else if (iState == 1)
				{
					szValue[0] = *ptr++;
					szValue[1] = *ptr;
					szValue[2] = 0;
					iHH = ascii_atoi(szValue);
					if (*ptr != 0)
						ptr++;
					iState = 2;
				}
				else if (iState == 2)
				{
					szValue[0] = *ptr++;
					szValue[1] = *ptr;
					szValue[2] = 0;
					iMM = ascii_atoi(szValue);
					break;
				}
			}

			m_iTimezoneDifferential = (iHH * 60) + iMM;
			if (bMinus)
				m_iTimezoneDifferential = 0 - m_iTimezoneDifferential;
			m_ParseState = MIME_PARSER_STATE_DATE9;
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
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_DATE);
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_DATE9(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state just keeps reading in tokens until we see a CRLF.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			// Ignore any whitespace here.
			break;
		}

		case MIME_TOKEN_CRLF:
		{
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER

			// ********************************************************************************
			// 7-12-2004 THURTT - The extraction of a extra long date header is being removed
			// due to problems it is causing with the Attribute API. We now emit a 
			// MIME_WARNING_LONG_DATE warning in this scenario. Further information 
			// about these problems are documented in defect 1-2FONDE.
			// ********************************************************************************
			// If the line length exceeds 80 characters total, emit the
			// token for a possibly malicious Date: line.
			if (m_nOffset - m_nLineOffset > 80)
			{
				EmitWarning(MIME_WARNING_LONG_DATE, MIME_TOKEN_DATE);
				EMIT_ID = MIME_TOKEN_END_MARKER;
				DO_EMIT;  // Emit END_MARKER
			}

			m_ParseState = m_bInMultipartHeader ? MIME_PARSER_STATE_MULTIPARTHEADER1:
												 MIME_PARSER_STATE_HEADERFIELD1;
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

		default:
		{
			// Note that this is not RFC 822 compliant.
			//EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_DATE);
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_DATE10(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for an atom which should be the second.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		{
			// Ignore any whitespace here.
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
			// This should be a 1 or 2 digit integer.
			m_dwSecond = (unsigned long)ascii_atoi(m_pString);
			if (m_dwSecond > 59)
			{
				m_dwSecond = 0;

				// Note that this is not RFC 822 compliant.
				EmitWarning(MIME_WARNING_INVALID_SECOND, MIME_TOKEN_DATE);
			}

			m_ParseState = MIME_PARSER_STATE_DATE8;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_DATE);
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_DATE11(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state is looking for an atom which should be the
	// day of the month.
	// This state expects that we are in the middle of a date following the
	// form mm/dd/yyyy.
	switch (m_iTokenID)
	{
		case MIME_TOKEN_LWSP:
		case MIME_TOKEN_SPECIAL:
		{
			// Ignore any whitespace or special tokens here.
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
			// This should be a 1 or 2 digit integer.
			m_dwDay = (unsigned long)ascii_atoi(m_pString);
			if (m_dwDay < 1 || m_dwDay > 31)
			{
				m_dwDay = 1;

				// Note that this is not RFC 822 compliant.
				EmitWarning(MIME_WARNING_INVALID_DAYOFMONTH, MIME_TOKEN_DATE);
			    m_ParseState = MIME_PARSER_STATE_DATE4;  // Now look for the year.
				break;
			}

			m_ParseState = MIME_PARSER_STATE_DATE4;
			break;
		}

		default:
		{
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_DATE);
			break;
		}
	}

	return (rc);
}
