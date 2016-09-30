// ContentTransferEncoding.cpp : CONTENTTRANSFERENCODING states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

inline int CMIMEParser::State_CONTENTTRANSFERENCODING1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// We are looking for an atom.
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

		case MIME_TOKEN_ATOM:
		{
			int iID;

			iID = IdentifyAtom(MIME_ATOM_GROUP_ENCODING);
			if (iID != MIME_TOKEN_LAST)
			{
				// We found a recognized encoding type. 
				// We are only going to set the encoding type
				// if one has not already been set.  If one
				// has already been set then emit that token.
				// This means that the FIRST content-transfer-encoding
				// in a MIME header wins.  Subsequent content-transfer-encodings
				// contained in the header will NOT change the encoding.
				if ( m_iEncoding == MIME_TOKEN_NONE )
				{
					m_iEncoding = iID;
				}
				// if multiple encoding entries are found and are different then
				// we need to emit a warning and set the malformed flag.
				else if ( m_iEncoding != iID )
				{
					EmitWarning(MIME_WARNING_MULTIPLE_DIFFERENT_TRANSFER_ENCODINGS, m_iTokenID);
				}

				EMIT_ID = iID;
				DO_EMIT;
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

