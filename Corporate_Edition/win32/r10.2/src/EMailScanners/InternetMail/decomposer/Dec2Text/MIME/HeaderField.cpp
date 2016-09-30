// HeaderField.cpp : HEADERFIELD states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.
#include "bstricmp.h"

inline int CMIMEParser::State_HEADERFIELD1(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// Always reset the field ID here so that we don't accidentally
	// emit a field ID.
	m_iFieldID = MIME_TOKEN_LAST;

	switch (m_iTokenID)
	{
		case MIME_TOKEN_SPECIAL:
		{
			// OK, we found a SPECIAL token.  Move to HEADERFIELD3 state.
			if (m_pString[0] == ASC_CHR_COLON)
			{

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
				// Save the offset to the byte past the colon
				m_nValueStartOffset = m_nOffset+1;
#endif //*** End AS400 Attribute API Removal *** 
				
				if ( m_AtomCandidate.getPosition() == 0 )
				{
					SetEmitWarning(MIME_WARNING_INVALID_HEADER, m_iTokenID);
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
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD3;
				}
			} 
			else
			{
				m_iTokenID = MIME_TOKEN_ATOM;
				State_HEADERFIELD1();
			}

			break;
		}
		case MIME_TOKEN_LWSP:
		{
			// Skip over any whitespace that appears before the header.
			// If we have already seen something that looks like a header
			// line, do not consider a new line with whitespace in front of
			// it to be malformed.
			if (!m_bIdentified)
			{
				// We have not identified anything yet, so this looks to be
				// malformed.
				// Note that this is not RFC 822 compliant.

				SetEmitWarning( MIME_WARNING_INVALID_HEADER, m_iTokenID );

			}
			break;
		}

		case MIME_TOKEN_CRLF:
		{
			if ( m_bIdentified )
			{
				// We have ID'ed the file already.
				// See if there is text remaining to process.
				// There should be not text in general since we 
				// transition to state3 when we find a colon

				if ( m_AtomCandidate.getPosition() != 0)
				{
					if ( (rc = ProcessHeaderAtom()) == PARSER_ERROR )
					{ return rc; }

					// It's possible to no longer be in the header after calling
					// processHeaderAtom.  There are some malformed cases that
					// can cause this to happen.

					if ( m_bBoundarySeparatorFoundInHeader == false )
					{
						// We allow two special atoms to exists in the file.
						// These two atoms are HELO and DATA.
						if ( (binary_strnicmp(&m_AtomCandidate[0],STR_helo,STR_HELO,sizeof(STR_HELO)-1) != 0) &&
							 (binary_strnicmp(&m_AtomCandidate[0],STR_data,STR_DATA,sizeof(STR_DATA)-1) != 0)
							)
						{

							// The atom is NOT HELO or DATA
							EmitWarning(MIME_WARNING_INVALID_HEADER, m_iTokenID);
							EMIT_ID = MIME_TOKEN_END_MARKER;
							DO_EMIT;  // Emit END_MARKER
						}
					}
					// else it is HELO or DATA so just continue
					// We stay in HeaderField1 and continue to process the message
					// If we have id'ed the file but have found a bad line.
					break;
				}
				// else this is a blank line and we let MIME_TOKEN_EOF handle it below.
			}
			else
			{
				// If we encounter an ATOM not followed by a ":" then we assume this
				// is not MIME.  We allow two special cases that address a virus outbreak.
				// The special cases words are: HELO and DATA.

				if ( m_bFuzzyHeader || m_eSigState > NONE )
				{
					// The caller wants us to continue to look ahead if the line is NOT
					// MIME yet. Break and return to HeaderState1
					m_AtomCandidate.reset();
					m_iAtomCandidateBufSize = 0;
					break;
				}
				else
				{
					if ( m_AtomCandidate.getPosition() != 0 )
					{
						if ( (binary_strnicmp(&m_AtomCandidate[0],STR_helo,STR_HELO,sizeof(STR_HELO)-1) != 0) &&
							 (binary_strnicmp(&m_AtomCandidate[0],STR_data,STR_DATA,sizeof(STR_DATA)-1) != 0)
						   )
						{
							// It's not MIME so far and we are not allowed to continue since
							// the client has not set the Fuzzy flag.
							m_AtomCandidate.reset();
							m_iAtomCandidateBufSize = 0;
							rc = PARSER_DONE;
						}
						// else if is HELO or DATA so continue in HeaderState1
						break;

					}
					else
					{
						// We have a blank line in front of the possible MIME header.
						// We continue on
						break;
					}
				}
			}
		}
		case MIME_TOKEN_EOF:
		{
			if ( (rc = ProcessHeaderAtom()) == PARSER_ERROR )
			{ return rc; }
			// This is a blank header line.  If we have already processed
			// enough header information to reasonably consider the data to
			// be in a MIME format, move on to the state that begins
			// processing the preamble (in a multi-part message) or encoded
			// data (in a single-part message).
			// Otherwise, fall back to looking for more header lines.
			if (m_bIdentified)
			{
				EMIT_ID = MIME_TOKEN_HEADER_SEPARATOR;
				EMIT_DATA = m_nStringLength;
				DO_EMIT;

				m_bInHeader = false;  // We are no longer parsing header lines.

				// Always consider an EOF token to be a single byte in size.
				if (m_iTokenID == MIME_TOKEN_EOF)
					m_nStringLength = 1;

				EMIT_OFFSET = EMIT_OFFSET + m_nStringLength;  // Move marker forward to account for CRLF
				EMIT_ID = MIME_TOKEN_END_MARKER;
				DO_EMIT;

				if (m_bMultipart)
				{
					EMIT_ID = MIME_TOKEN_PREAMBLE;
					DO_EMIT;  // Emit PREAMBLE
					m_bInPreamble = true;
					m_ParseState = MIME_PARSER_STATE_MULTIPART1;
				}
				else
				{
					m_ParseState = MIME_PARSER_STATE_BODY1;
				}

				rc = PARSER_IDENTIFIED;
			}
			else
			{
				m_ParseState = MIME_PARSER_STATE_HEADERFIELD1;
				rc = PARSER_OK;
			}

			break;
		}

		case MIME_TOKEN_ATOM:
		{
			
			// If we have a digital signature, we will not have a separator between it and our first
			// keyword. Search for a keyword.
			if( m_eSigState == SIGNED_DATA )
			{
				IsAtomInSignature();
			}

			try 
			{
				// The AtomCandidate is used to accumulate data
				// that will be later used to compare against the 
				// list of known MIME words.  Since the list of known
				// MIME words is fixed we do not need to accumulate the
				// data beyond a certain point since we are certain that
				// the atom will not match the known set.

				if ( m_iAtomCandidateBufSize <= MAX_ATOM_CANDIDATE_SIZE )
				{
					// We need to save the token offset on the
					// first atom found since we are accumulating atoms
					// and special characters.

					if ( m_iAtomCandidateBufSize == 0 )
					{
						m_nAtomCandidateOffset = m_nOffset;
						m_nAtomCandidatePreviousToken = m_iPreviousTokenID;
					}
					m_AtomCandidate.setAt(m_iAtomCandidateBufSize,m_pString,m_nStringLength);
					m_iAtomCandidateBufSize += m_nStringLength;
					m_AtomCandidate[m_iAtomCandidateBufSize] = 0;

					// char* szDebug = &m_AtomCandidate[0];
				}
			} 
			catch ( const char*  )
			{
				m_AtomCandidate.reset();
				rc = PARSER_ERROR;
			}

			IsValidWarningDelay();

			break;
		}

		default:
		{

			// Skip over any tokens that appear before the header.
			// Note that this is not RFC 822 compliant.
			SetEmitWarning(MIME_WARNING_UNEXPECTED_TOKEN, MIME_TOKEN_UNKNOWN_FIELD);
			break;
		}
	}

	TriggerWarning();

	return (rc);
}

#define KEYWORD_LEVEL_UNKNWON		2
#define KEYWORD_LEVEL_0				5
#define KEYWORD_LEVEL_1				10
#define KEYWORD_LEVEL_2				20
#define KEYWORD_LEVEL_3				30
#define KEYWORD_LEVEL_4				40
#define KEYWORD_LEVEL_5				50
#define KEYWORD_LEVEL_XDASHPREFIX	50
#define KEYWORD_LEVEL_6				60
#define KEYWORD_LEVEL_7				70
#define KEYWORD_LEVEL_8				80
#define KEYWORD_LEVEL_9				90
#define KEYWORD_LEVEL_10			100


inline int CMIMEParser::State_HEADERFIELD3(void)
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
			m_ParseState = MIME_PARSER_STATE_HEADERFIELD1;
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
			// This must be considered sufficient to identify the data
			// as being in a message format.
			if (m_nHeaderLinesIDed != 0)
				m_bIdentified = true;

			m_bScanForPairs = false;  // Stop scanning for pairs.

#if !defined(OS400) //*** Start AS400 Attribute API Removal *** 
			m_nKeyID = m_iFieldID;
			m_nKeyOffset = m_nAtomCandidateOffset;
#endif //*** End AS400 Attribute API Removal *** 

			switch (m_iFieldID)
			{
				case MIME_TOKEN_RETURN_PATH:
					if ( ++m_nReturn_Path == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_8;
					}
					m_ParseState = MIME_PARSER_STATE_RETURNPATH1;
					break;

				case MIME_TOKEN_RECEIVED:
					if ( ++m_nReceived == 1 ) 
					{	m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_3;
					}
					m_ParseState = MIME_PARSER_STATE_RECEIVED1;
					break;

				case MIME_TOKEN_REPLY_TO:
					if ( ++m_nReply_to == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_4;
					}
					m_ParseState = MIME_PARSER_STATE_REPLYTO1;
					break;

				case MIME_TOKEN_FROM:
					if ( ++m_nFrom == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_1;
					}
					m_ParseState = MIME_PARSER_STATE_FROM1;
					break;

				case MIME_TOKEN_SENDER:
					if ( ++m_nSender == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_2;
					}
					m_ParseState = MIME_PARSER_STATE_SENDER1;
					break;

				case MIME_TOKEN_DATE:
					if ( ++m_nDate == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_0;
					}
					m_ParseState = MIME_PARSER_STATE_DATE1;
					break;

				case MIME_TOKEN_TO:
					if ( ++m_nTo == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_1;
					}
					m_ParseState = MIME_PARSER_STATE_TO1;
					break;

				case MIME_TOKEN_CC:
					if ( ++m_nCc == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_2;
					}
					m_ParseState = MIME_PARSER_STATE_CC1;
					break;

				case MIME_TOKEN_BCC:
					if ( ++m_nBcc == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_2;
					}
					m_ParseState = MIME_PARSER_STATE_BCC1;
					break;

				case MIME_TOKEN_MESSAGE_ID:
					if ( ++m_nMessage_id == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_3;
					}
					m_ParseState = MIME_PARSER_STATE_MESSAGEID1;
					break;

				case MIME_TOKEN_IN_REPLY_TO:
					if ( ++m_nIn_reply_to == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_8;
					}
					m_ParseState = MIME_PARSER_STATE_INREPLYTO1;
					break;

				case MIME_TOKEN_REFERENCES:
					if ( ++m_nReferences == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_1;
					}
					m_ParseState = MIME_PARSER_STATE_REFERENCES1;
					break;

				case MIME_TOKEN_KEYWORDS:
					if ( ++m_nKeywords == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_1;
					}
					m_ParseState = MIME_PARSER_STATE_KEYWORDS1;
					break;

				case MIME_TOKEN_SUBJECT:
					if ( ++m_nSubject == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_1;
					}
					m_ParseState = MIME_PARSER_STATE_SUBJECT1;
					break;

				case MIME_TOKEN_COMMENTS:
					if ( ++m_nComments == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_1;
					}
					m_ParseState = MIME_PARSER_STATE_COMMENTS1;
					break;

				case MIME_TOKEN_ENCRYPTED:
					if ( ++m_nEncrypted == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_2;
					}
					m_ParseState = MIME_PARSER_STATE_ENCRYPTED1;
					break;

				case MIME_TOKEN_CONTENT_TYPE:
					if ( ++m_nContent_type == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_10;
					}
					m_ParseState = MIME_PARSER_STATE_CONTENTTYPE1;
					break;

				case MIME_TOKEN_CONTENT_DISPOSITION:
					if ( ++m_nContent_disposition == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_10;
					}
					m_ParseState = MIME_PARSER_STATE_CONTENTDISPOSITION1;
					break;

				case MIME_TOKEN_CONTENT_DESCRIPTION:
					if ( ++m_nContent_description == 1 ) 
					{
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_10;
					}
					m_ParseState = MIME_PARSER_STATE_CONTENTDESCRIPTION1;
					break;

				case MIME_TOKEN_CONTENT_TRANSFER_ENCODING:
					if ( ++m_nContent_transfer_encoding == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_10;
					}
					m_ParseState = MIME_PARSER_STATE_CONTENTTRANSFERENCODING1;
					break;

				case MIME_TOKEN_CONTENT_ID:
					if ( ++m_nContent_id == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_8;
					}
					m_ParseState = MIME_PARSER_STATE_CONTENTID1;
					break;

				case MIME_TOKEN_MIME_VERSION:
					if ( ++m_nMime_version == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_10;
					}
					m_ParseState = MIME_PARSER_STATE_MIMEVERSION1;
					break;

				case MIME_TOKEN_IMPORTANCE:
					if ( ++m_nImportance == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_1;
					}
					m_ParseState = MIME_PARSER_STATE_IMPORTANCE1;
					break;

				case MIME_TOKEN_ORGANIZATION:
					if ( ++m_nOrganization == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_1;
					}
					m_ParseState = MIME_PARSER_STATE_ORGANIZATION1;
					break;

				case MIME_TOKEN_AUTHOR:
					if ( ++m_nAuthor == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_1;
					}
					m_ParseState = MIME_PARSER_STATE_AUTHOR1;
					break;

				case MIME_TOKEN_PRIORITY:
					if ( ++m_nPriority == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_1;
					}
					m_ParseState = MIME_PARSER_STATE_PRIORITY1;
					break;

				case MIME_TOKEN_DELIVERED_TO:
					if ( ++m_nDelivered_to == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_2;
					}
					m_ParseState = MIME_PARSER_STATE_DELIVEREDTO1;
					break;

				case MIME_TOKEN_CONTENT_LOCATION:
					if ( ++m_nContent_location == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_10;
					}
					m_ParseState = MIME_PARSER_STATE_CONTENTLOCATION1;
					break;

				case MIME_TOKEN_THREAD_TOPIC:
					if ( ++m_nThread_topic == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_5;
					}
					m_ParseState = MIME_PARSER_STATE_THREADTOPIC1;
					break;

				case MIME_TOKEN_THREAD_INDEX:
					if ( ++m_nThread_index == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_5;
					}
					m_ParseState = MIME_PARSER_STATE_THREADINDEX1;
					break;

				case MIME_TOKEN_MAILER:
					if ( ++m_nMailer == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_5;
					}
					m_ParseState = MIME_PARSER_STATE_MAILER1;
					break;

				case MIME_TOKEN_CONTENT_LENGTH:
					if ( ++m_nContent_length == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_10;
					}
					m_ParseState = MIME_PARSER_STATE_CONTENTLENGTH1;
					break;

// *** Fix for W32.Chir@mm virus ***
                case MIME_TOKEN_HELO:
					if ( ++m_nHelo == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_10;
                    }
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
                    break;

                case MIME_TOKEN_DATA:
					if ( ++m_nData == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
 						m_nKeywordWeighting += KEYWORD_LEVEL_10;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
                    break;
// *********************************

				case MIME_TOKEN_ACCEPT_LANGUAGE:
					if ( ++m_nAccept_language == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_7;
					}
					m_ParseState = MIME_PARSER_STATE_CONTENTLENGTH1;
					break;

				case MIME_TOKEN_MSMAIL_PRIORITY:
					if ( ++m_nMsmail_priority == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_10;
					}
					m_ParseState = MIME_PARSER_STATE_CONTENTLENGTH1;
					break;

				case MIME_TOKEN_USER_AGENT:
					if ( ++m_nUser_agent == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_4;
					}
					m_ParseState = MIME_PARSER_STATE_CONTENTLENGTH1;
					break;

				case MIME_TOKEN_APPARENTLY_TO:
					if ( m_bHasXDashPrefix )
					{
						if ( ++m_nApparently_to == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_10;
						}
					}
					else
					{
						// This is not an X- word that was ID.  
						// We need to 'undo' the ID since the ID routine
						// strips the X- prefix (i.e. ID == X-ID)
						m_nKeywordWeighting += KEYWORD_LEVEL_UNKNWON;
						m_iFieldID = MIME_TOKEN_UNKNOWN_FIELD;

						// Just keep a running total of lines we did not identify.
						// This will be useful in reducing false positives.

						m_nUnidentifiedHeaderLines++;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_ID:
					if ( m_bHasXDashPrefix )
					{
						if ( ++m_nId == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_10;
						}
					}
					else
					{
						// This is not an X- word that was ID.  
						// We need to 'undo' the ID since the ID routine
						// strips the X- prefix (i.e. ID == X-ID)
						m_nKeywordWeighting += KEYWORD_LEVEL_UNKNWON;
						m_iFieldID = MIME_TOKEN_UNKNOWN_FIELD;

						// Just keep a running total of lines we did not identify.
						// This will be useful in reducing false positives.

						m_nUnidentifiedHeaderLines++;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_EGROUPS_RETURN:
					if ( m_bHasXDashPrefix )
					{
						if ( ++m_nEgroups_return == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_10;
						}
					}
					else
					{
						// This is not an X- word that was ID.  
						// We need to 'undo' the ID since the ID routine
						// strips the X- prefix (i.e. ID == X-ID)
						m_nKeywordWeighting += KEYWORD_LEVEL_UNKNWON;
						m_iFieldID = MIME_TOKEN_UNKNOWN_FIELD;

						// Just keep a running total of lines we did not identify.
						// This will be useful in reducing false positives.

						m_nUnidentifiedHeaderLines++;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_LIST_UNSUBSCRIBE:
					if ( m_bHasXDashPrefix )
					{
						if ( ++m_nXList_unsubscribe == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_10;
						}
					}
					else
					{
						if ( ++m_nList_unsubscribe == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_6;
						}
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_LIST_SUBSCRIBE:
					if ( m_bHasXDashPrefix )
					{
						if ( ++m_nXList_subscribe == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_10;
						}
					}
					else
					{
						if ( ++m_nList_subscribe == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_6;
						}
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_AUTO_SUBMITTED:
					if ( ++m_nAuto_submitted == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_4;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_ORIGINATING_IP:
					if ( m_bHasXDashPrefix )
					{
						if ( ++m_nOriginating_ip == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_10;
						}
					}
					else
					{
						// This is not an X- word that was ID.  
						// We need to 'undo' the ID since the ID routine
						// strips the X- prefix (i.e. ID == X-ID)
						m_nKeywordWeighting += KEYWORD_LEVEL_UNKNWON;
						m_iFieldID = MIME_TOKEN_UNKNOWN_FIELD;

						// Just keep a running total of lines we did not identify.
						// This will be useful in reducing false positives.

						m_nUnidentifiedHeaderLines++;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_MAILER_VERSION:
					if ( m_bHasXDashPrefix )
					{
						if ( ++m_nMailer_version == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_10;
						}
					}
					else
					{
						// This is not an X- word that was ID.  
						// We need to 'undo' the ID since the ID routine
						// strips the X- prefix (i.e. ID == X-ID)
						m_nKeywordWeighting += KEYWORD_LEVEL_UNKNWON;
						m_iFieldID = MIME_TOKEN_UNKNOWN_FIELD;

						// Just keep a running total of lines we did not identify.
						// This will be useful in reducing false positives.

						m_nUnidentifiedHeaderLines++;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_LIST_ID:
					if ( ++m_nList_id == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_5;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_LIST_POST:
					if ( ++m_nList_post == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_5;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_LIST_HELP:
					if ( ++m_nList_help == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_5;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_ERRORS_TO:
					if ( ++m_nErrors_to == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_7;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_MAILING_LIST:
					if ( ++m_nMailing_list == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_5;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_MS_HAS_ATTACH:
					if ( m_bHasXDashPrefix )
					{
						if ( ++m_nMs_has_attach == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_10;
						}
					}
					else
					{
						// This is not an X- word that was ID.  
						// We need to 'undo' the ID since the ID routine
						// strips the X- prefix (i.e. ID == X-ID)
						m_nKeywordWeighting += KEYWORD_LEVEL_UNKNWON;
						m_iFieldID = MIME_TOKEN_UNKNOWN_FIELD;

						// Just keep a running total of lines we did not identify.
						// This will be useful in reducing false positives.

						m_nUnidentifiedHeaderLines++;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_MS_TNEF_CORRELATOR:
					if ( m_bHasXDashPrefix )
					{
						if ( ++m_nMs_tnef_correlator == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_10;
						}
					}
					else
					{
						// This is not an X- word that was ID.  
						// We need to 'undo' the ID since the ID routine
						// strips the X- prefix (i.e. ID == X-ID)
						m_nKeywordWeighting += KEYWORD_LEVEL_UNKNWON;
						m_iFieldID = MIME_TOKEN_UNKNOWN_FIELD;

						// Just keep a running total of lines we did not identify.
						// This will be useful in reducing false positives.

						m_nUnidentifiedHeaderLines++;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_MIME_AUTOCONVERTED:
					if ( m_bHasXDashPrefix )
					{
						if ( ++m_nMime_autoconverted == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_10;
						}
					}
					else
					{
						// This is not an X- word that was ID.  
						// We need to 'undo' the ID since the ID routine
						// strips the X- prefix (i.e. ID == X-ID)
						m_nKeywordWeighting += KEYWORD_LEVEL_UNKNWON;
						m_iFieldID = MIME_TOKEN_UNKNOWN_FIELD;

						// Just keep a running total of lines we did not identify.
						// This will be useful in reducing false positives.

						m_nUnidentifiedHeaderLines++;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_CONTENT_CLASS:
					if ( ++m_nContent_class == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_10;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_PRECEDENCE:
					if ( ++m_nPrecedence == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_1;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_ORIGINALARRIVALTIME:
					if ( m_bHasXDashPrefix )
					{
						if ( ++m_nOriginalarrivaltime == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_10;
						}
					}
					else
					{
						// This is not an X- word that was ID.  
						// We need to 'undo' the ID since the ID routine
						// strips the X- prefix (i.e. ID == X-ID)
						m_nKeywordWeighting += KEYWORD_LEVEL_UNKNWON;
						m_iFieldID = MIME_TOKEN_UNKNOWN_FIELD;

						// Just keep a running total of lines we did not identify.
						// This will be useful in reducing false positives.

						m_nUnidentifiedHeaderLines++;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_MIMEOLE:
					if ( m_bHasXDashPrefix )
					{
						if ( ++m_nMimeole == 1 ) 
						{ 
							m_nIdentifiedHeaderLines++;
							m_nKeywordWeighting += KEYWORD_LEVEL_10;
						}
					}
					else
					{
						// This is not an X- word that was ID.  
						// We need to 'undo' the ID since the ID routine
						// strips the X- prefix (i.e. ID == X-ID)
						m_nKeywordWeighting += KEYWORD_LEVEL_UNKNWON;
						m_iFieldID = MIME_TOKEN_UNKNOWN_FIELD;

						// Just keep a running total of lines we did not identify.
						// This will be useful in reducing false positives.

						m_nUnidentifiedHeaderLines++;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_LIST_ARCHIVE:
					if ( ++m_nList_archive == 1 ) 
					{ 
						m_nIdentifiedHeaderLines++;
						m_nKeywordWeighting += KEYWORD_LEVEL_5;
					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				case MIME_TOKEN_UNKNOWN_FIELD:
					// The ID is unknown but it may be an X- style
					// keyword.  If so we give the word more weighting
					// since X- headers are common MIME headers.
					if ( m_bHasXDashPrefix )
					{
						m_nKeywordWeighting += KEYWORD_LEVEL_XDASHPREFIX;
						// If an unknown field begins with an X- we want
						// to treat the line as 'known' even though we
						// did not recognize the word following the X-.
						m_nIdentifiedHeaderLines++;
					}
					else
					{
						m_nKeywordWeighting += KEYWORD_LEVEL_UNKNWON;
						// Just keep a running total of lines we did not identify.
						// This will be useful in reducing false positives.

						m_nUnidentifiedHeaderLines++;

					}
					m_ParseState = MIME_PARSER_STATE_HEADERFIELD4;
					break;

				default:
					if ( m_iFieldID > MIME_TOKEN_LAST )
					{
						dec_assert(0);
					}
					return (rc);
				//	break;
			}

			rc = PARSER_CHANGE_STATE;
			break;
		}

		default:
		{
			// This is an unexpected token.  We recover by going back to
			// looking for header lines.
			// Note that this is not RFC 822 compliant.
			EmitWarning(MIME_WARNING_INVALID_HEADER, m_iTokenID);
			EMIT_ID = MIME_TOKEN_END_MARKER;
			DO_EMIT;  // Emit END_MARKER
			m_bScanForPairs = false;  // Stop scanning for pairs.
			m_ParseState = MIME_PARSER_STATE_HEADERFIELD1;
			break;
		}
	}

	return (rc);
}


inline int CMIMEParser::State_HEADERFIELD4(void)
{
	int		rc = PARSER_OK;	// Assume parsing should continue

	// This state just keeps reading in tokens until we see a CRLF.
	if (m_iTokenID == MIME_TOKEN_CRLF ||
		m_iTokenID == MIME_TOKEN_EOF)
	{
		EMIT_ID = MIME_TOKEN_END_MARKER;
		DO_EMIT;  // Emit END_MARKER
		m_ParseState = MIME_PARSER_STATE_HEADERFIELD1;
	}

	return (rc);
}

