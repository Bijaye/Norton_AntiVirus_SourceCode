// MIMELexSt.cpp : Mime Lexer states
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

// Lexer state machine functions
inline int CMIMELexer::State_CRLF1(unsigned char uchByte)
{
	if (IsCR(uchByte))
	{
		// The byte is a carriage-return character.  Since we need
		// to also recognize the longer sequence CRLF, we can't emit
		// anything yet.  Move to the next state and wait for the
		// next input byte.
		m_State = MIME_LEXER_STATE_CRLF2;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a carriage-return character.
	// Move to the next start state.
	m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
	return LEXER_OK;
}


inline int CMIMELexer::State_CRLF2(unsigned char uchByte)
{	
	if (IsLF(uchByte) || IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a line-feed character or whitespace.  Move to the
		// next state and look for LWSP (to un-fold folded lines).
		m_State = MIME_LEXER_STATE_CRLF3;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a line-feed or whitespace character.
	// Remove the last byte added to our buffer by backing up one byte.
	m_nLexemeOffset--;

	// Emit a CRLF token.  Note that the CRLF token represents three
	// character combinations: CR, LF, and CRLF.
	int rc = EmitToken(MIME_TOKEN_CRLF);
	if (rc == LEXER_ERROR)
		return (rc);

	m_nStartOffset = m_nOffset;	// Set the next token's start location.

	// Now that our buffer has been reset, put this
	// new byte back.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Increment the current line number.
	m_nLineNumber++;

	// Make a mental note that we just emitted a new-line.
	m_bNewLine = true;

	// And, finally, go back and analyze the byte from scratch.
	m_iNextStartStateIndex = 1;
	return (rc);
}


inline int CMIMELexer::State_CRLF3(unsigned char uchByte)
{
	// Here we are looking for LWSP following a CR, LF, or CRLF sequence.
	// We translate this into LWSP to un-fold folded lines.
	if (!m_bNewLine &&
		(IsSPACE(uchByte) || IsHTAB(uchByte)))
	{
		// The byte is a whitespace character.
		// Increment the current line number.
		m_nLineNumber++;

		// Move to the LWSP2 state to continue looking for more
		// whitespace characters.
		m_State = MIME_LEXER_STATE_LWSP2;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a whitespace character.
	// Remove the last byte added to our buffer by backing up one byte.
	m_nLexemeOffset--;

	// Emit a CRLF token.  Note that the CRLF token represents three
	// character combinations: CR, LF, and CRLF.
	int rc = EmitToken(MIME_TOKEN_CRLF);
	if (rc == LEXER_ERROR)
		return (rc);

	m_nStartOffset = m_nOffset;	// Set the next token's start location.

	// Now that our buffer has been reset, put this
	// new byte back.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Increment the current line number.
	m_nLineNumber++;

	// Make a mental note that we just emitted a new-line.
	m_bNewLine = true;

	// And, finally, go back and analyze the byte from scratch.
	m_iNextStartStateIndex = 1;
	return (rc);
}


inline int CMIMELexer::State_LF1(unsigned char uchByte)
{
	if (IsLF(uchByte))
	{
		// The byte is a line-feed character.  Move to the
		// next state and look for LWSP (to un-fold folded lines).
		m_State = MIME_LEXER_STATE_CRLF3;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a line-feed character.
	// Move to the next start state.
	m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
	return LEXER_OK;
}


inline int CMIMELexer::State_LWSP1(unsigned char uchByte)
{
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  Since we need
		// to also recognize long sequences of whitespace, we can't emit
		// anything yet.  Move to the next state and wait for the
		// next input byte.
		m_State = MIME_LEXER_STATE_LWSP2;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a whitespace character.
	// Move to the next start state.
	m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
	return LEXER_OK;
}


inline int CMIMELexer::State_LWSP2(unsigned char uchByte)
{
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  We stay in this
		// same state until we find a non-whitespace character.
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a whitespace character.
	// Remove the last byte added to our buffer by backing up one byte.
	m_nLexemeOffset--;

	// Emit a LWSP token.
	int rc = EmitToken(MIME_TOKEN_LWSP);
	if (rc == LEXER_ERROR)
		return (rc);

	m_nStartOffset = m_nOffset;	// Set the next token's start location.

	// Now that our buffer has been reset, put this
	// new byte back.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// And, finally, go back and analyze the byte from scratch.
	m_iNextStartStateIndex = 1;
	return (rc);
}


inline int CMIMELexer::State_SPECIAL1(unsigned char uchByte)
{
	if (IsTSPECIAL(uchByte))
	{
		// Emit a SPECIAL token.
		int rc = EmitToken(MIME_TOKEN_SPECIAL);
		if (rc == LEXER_ERROR)
			return (rc);

		m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return (rc);
	}

	// The byte is not a special character.
	// Move to the next start state.
	m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
	return LEXER_OK;
}


inline int CMIMELexer::State_QUOTEDSTRING1(unsigned char uchByte)
{
	if (m_pParser->m_bScanForPairs && IsQUOTE(uchByte))
	{
		// The byte is a quote character.  Since we need to look
		// for the closing quote and everything else in-between,
		// we can't emit anything yet.  Move to the next state
		// and wait for the next input byte.
		m_State = MIME_LEXER_STATE_QUOTEDSTRING2;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a quote character.
	// Move to the next start state.
	m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
	return LEXER_OK;
}


inline int CMIMELexer::State_QUOTEDSTRING2(unsigned char uchByte)
{
	if (IsQUOTE(uchByte))
	{
		// The byte is a quote character.
		// Emit a QUOTED_STRING token.
		int rc = EmitToken(MIME_TOKEN_QUOTED_STRING);
		if (rc == LEXER_ERROR)
			return (rc);

		m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return (rc);
	}

	if (IsCR(uchByte))
	{
		// The byte is a CR character.  Move to state
		// QUOTEDSTRING4 to look for continuation lines.
		// Also save the offset of this character in the lexeme buffer
		// so that we can remove the continuation characters later.
		m_State = MIME_LEXER_STATE_QUOTEDSTRING4;
		m_nContinuationOffset = m_nLexemeOffset - 1;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	if (IsLF(uchByte))
	{
		// The byte is a LF character.  Move to state
		// QUOTEDSTRING5 to look for continuation lines.
		// Also save the offset of this character in the lexeme buffer
		// so that we can remove the continuation characters later.
		m_State = MIME_LEXER_STATE_QUOTEDSTRING5;
		m_nContinuationOffset = m_nLexemeOffset - 1;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	if (uchByte == ASC_CHR_BSLASH)
	{
		// The byte is the special quoted-pair character.
		// Since we need to get the next character to finish
		// the pair, we can't emit anything yet.  Move to the
		// next state and wait for the next input byte.
		m_State = MIME_LEXER_STATE_QUOTEDSTRING3;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is a CHAR.  We still need to locate the
	// closing quote character so stay in this state.
	m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
	return LEXER_OK;
}


inline int CMIMELexer::State_QUOTEDSTRING3(unsigned char uchByte)
{
	// The byte is a CHAR.
	// Copy the byte over the special quoted-pair character
	// in our buffer.
	m_nLexemeOffset--;
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Go back to the previous state where we are looking for
	// the closing quote.
	m_State = MIME_LEXER_STATE_QUOTEDSTRING2;
	m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
	return LEXER_OK;
}


inline int CMIMELexer::State_QUOTEDSTRING4(unsigned char uchByte)
{
	int rc = LEXER_OK;

	// Here we are looking for a continuation line.  So far all we
	// have is a CR character.
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  Now we have a valid
		// continuation-line sequence.  However, since we need
		// to also recognize long sequences of whitespace, we can't emit
		// anything yet.  Move to the next state and wait for the
		// next input byte.
		m_State = MIME_LEXER_STATE_QUOTEDSTRING6;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	if (IsLF(uchByte))
	{
		// The byte is a LF character.  We now have a CRLF sequence.
		// Move to state QUOTEDSTRING5 to look for continuation lines.
		m_State = MIME_LEXER_STATE_QUOTEDSTRING5;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a line-feed or whitespace character.
	// This is not a continuation line sequence.  We have a single CR
	// character.  This should be treated exactly the same as the
	// CRLF2 state (which also handles the single CR case).
	// Remove the last byte added to our buffer by backing up one byte.
	m_nLexemeOffset--;

	// Emit the suspected quoted string, since we know it was quoted to start
	//  and have abandon the notion of a possible continuation line.
	rc = EmitToken(MIME_TOKEN_QUOTED_STRING);
	if (rc == LEXER_ERROR)
	  return (rc);

	m_nStartOffset = m_nOffset;	// Set the next token's start location.

	// Now that our buffer has been reset, put this
	// new byte back.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Emit a CRLF token.  Note that the CRLF token represents three
	// character combinations: CR, LF, and CRLF.
	rc = EmitToken(MIME_TOKEN_CRLF);
	if (rc == LEXER_ERROR)
		return (rc);

	m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

	// Increment the current line number.
	m_nLineNumber++;

	// Make a mental note that we just emitted a new-line.
	m_bNewLine = true;

	// And, finally, go back and analyze the byte from scratch.
	m_State = g_iMIMELexerStartStates[0];
	m_iNextStartStateIndex = 1;
	return (rc);
}


inline int CMIMELexer::State_QUOTEDSTRING5(unsigned char uchByte)
{
	int rc = LEXER_OK;
	size_t nCount = 0;

	// Here we are looking for a continuation line.  So far all we
	// have is either a LF character or a CRLF sequence.
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  Now we have a valid
		// continuation-line sequence.  However, since we need
		// to also recognize long sequences of whitespace, we can't emit
		// anything yet.  Move to the next state and wait for the
		// next input byte.
		m_State = MIME_LEXER_STATE_QUOTEDSTRING6;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a whitespace character.
	// This is not a continuation line sequence.  We either have a
	// single LF character or a CRLF sequence.  In either case we
	// need to emit a CRLF token.
	// Remove the last byte added to our buffer by backing up one byte.
	m_nLexemeOffset--;

	// Back up past any CR or LF characters.
	nCount = 0;
	while (m_nLexemeOffset != 0 &&
		  (IsCR(m_byBuffer[m_nLexemeOffset - 1]) || IsLF(m_byBuffer[m_nLexemeOffset - 1])))
	{
		nCount++;
		m_nLexemeOffset--;
	}

	// Emit the suspected quoted string, since we know it was quoted to start
	//  and have abandon the notion of a possible continuation line.
	rc = EmitToken(MIME_TOKEN_QUOTED_STRING);
	if (rc == LEXER_ERROR)
	  return (rc);

	m_nStartOffset = m_nOffset - nCount;  // Set the next token's start location.

	// Now we need to put back the LF or CRLF characters.
	if (nCount == 2)
		m_byBuffer[m_nLexemeOffset++] = ASC_CHR_CR;

	m_byBuffer[m_nLexemeOffset++] = ASC_CHR_LF;

	// Emit a CRLF token.  Note that the CRLF token represents three
	// character combinations: CR, LF, and CRLF.
	rc = EmitToken(MIME_TOKEN_CRLF);
	if (rc == LEXER_ERROR)
		return (rc);

	m_nStartOffset = m_nOffset;	// Set the next token's start location.

	// Now that our buffer has been reset, put this
	// new byte back.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Increment the current line number.
	m_nLineNumber++;

	// Make a mental note that we just emitted a new-line.
	m_bNewLine = true;

	// And, finally, go back and analyze the byte from scratch.
	m_State = g_iMIMELexerStartStates[0];
	m_iNextStartStateIndex = 1;
	return (rc);
}


inline int CMIMELexer::State_QUOTEDSTRING6(unsigned char uchByte)
{
	// Here we are looking for additional whitespace on
	// a continuation line.  So far we have one of the following
	// sequences: CR+LWSP, LF+LWSP, CR+LF+LWSP.
	// Now all we need to do is skip any additional whitespace.
	// Once we hit something that is not whitespace, that character
	// must be the beginning of the continuation line.
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  This is still part
		// of the continuation-line sequence.  Stay in this state
		// to keep looking for more whitespace.
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a whitespace character.  This must be the
	// beginning of the continued line.  So, we need to fold the
	// previous line here by removing the continuation characters
	// that have been added to the buffer up to this point.  Since
	// we saved the offset (m_nContinuationOffset) of the first
	// lexeme in the continuation sequence, we simply need to back
	// up m_nLexemeOffset to that point and re-add this byte there.
	m_nLexemeOffset = m_nContinuationOffset;
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// And, finally, go back and analyze the byte from scratch.
	m_State = MIME_LEXER_STATE_QUOTEDSTRING2;
	m_iNextStartStateIndex = 1;
	return LEXER_OK;
}


inline int CMIMELexer::State_DOMAINLITERAL1(unsigned char uchByte)
{
	if (m_pParser->m_bScanForPairs && uchByte == ASC_CHR_LBRACK)
	{
		// The byte is a left-bracket.  Since we need to look
		// for the right-bracket and everything else in-between,
		// we can't emit anything yet.  Move to the next state
		// and wait for the next input byte.
		m_State = MIME_LEXER_STATE_DOMAINLITERAL2;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a left-bracket.
	// Move to the next start state.
	m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
	return LEXER_OK;
}  


inline int CMIMELexer::State_DOMAINLITERAL2(unsigned char uchByte)
{
	int rc = LEXER_OK;

	if (uchByte == ASC_CHR_RBRACK)
	{
		// The byte is a right-bracket character.
		// Emit a DOMAIN_LITERAL token.
		rc = EmitToken(MIME_TOKEN_DOMAIN_LITERAL);
		if (rc == LEXER_ERROR)
			return (rc);

		m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return (rc);
	}

	if (IsCR(uchByte))
	{
		// The byte is a CR character.  Move to state
		// DOMAINLITERAL4 to look for continuation lines.
		// Also save the offset of this character in the lexeme buffer
		// so that we can remove the continuation characters later.
		m_State = MIME_LEXER_STATE_DOMAINLITERAL4;
		m_nContinuationOffset = m_nLexemeOffset - 1;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	if (IsLF(uchByte))
	{
		// The byte is a LF character.  Move to state
		// DOMAINLITERAL5 to look for continuation lines.
		// Also save the offset of this character in the lexeme buffer
		// so that we can remove the continuation characters later.
		m_State = MIME_LEXER_STATE_DOMAINLITERAL5;
		m_nContinuationOffset = m_nLexemeOffset - 1;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	if (uchByte == ASC_CHR_BSLASH)
	{
		// The byte is the special quoted-pair character.
		// Since we need to get the next character to finish
		// the pair, we can't emit anything yet.  Move to the
		// next state and wait for the next input byte.
		m_State = MIME_LEXER_STATE_DOMAINLITERAL3;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is a CHAR.  We still need to locate the
	// right-bracket character so stay in this state.
	m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
	return LEXER_OK;
}  


inline int CMIMELexer::State_DOMAINLITERAL3(unsigned char uchByte)
{
	// The byte is a CHAR.
	// Copy the byte over the special quoted-pair character
	// in our buffer.
	m_nLexemeOffset--;
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Go back to the previous state where we are looking for
	// the right-bracket.
	m_State = MIME_LEXER_STATE_DOMAINLITERAL2;
	m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
	return LEXER_OK;
}  


inline int CMIMELexer::State_DOMAINLITERAL4(unsigned char uchByte)
{
	int rc = LEXER_OK;
	
	// Here we are looking for a continuation line.  So far all we
	// have is a CR character.
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  Now we have a valid
		// continuation-line sequence.  However, since we need
		// to also recognize long sequences of whitespace, we can't emit
		// anything yet.  Move to the next state and wait for the
		// next input byte.
		m_State = MIME_LEXER_STATE_DOMAINLITERAL6;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	if (IsLF(uchByte))
	{
		// The byte is a LF character.  We now have a CRLF sequence.
		// Move to state DOMAINLITERAL5 to look for continuation lines.
		m_State = MIME_LEXER_STATE_DOMAINLITERAL5;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a line-feed or whitespace character.
	// This is not a continuation line sequence.  We have a single CR
	// character.  This should be treated exactly the same as the
	// CRLF2 state (which also handles the single CR case).
	// Remove the last byte added to our buffer by backing up one byte.
	m_nLexemeOffset--;

	// Emit the suspected domain literal string, since we know it was
	// started and we have abandoned the notion of a possible continuation line.
	rc = EmitToken(MIME_TOKEN_DOMAIN_LITERAL);
	if (rc == LEXER_ERROR)
	  return (rc);

	m_nStartOffset = m_nOffset;	// Set the next token's start location.

	// Emit a CRLF token.  Note that the CRLF token represents three
	// character combinations: CR, LF, and CRLF.
	rc = EmitToken(MIME_TOKEN_CRLF);
	if (rc == LEXER_ERROR)
		return (rc);

	m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

	// Now that our buffer has been reset, put this
	// new byte back.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Increment the current line number.
	m_nLineNumber++;

	// Make a mental note that we just emitted a new-line.
	m_bNewLine = true;

	// And, finally, go back and analyze the byte from scratch.
	m_State = g_iMIMELexerStartStates[0];
	m_iNextStartStateIndex = 1;
	return (rc);
}  


inline int CMIMELexer::State_DOMAINLITERAL5(unsigned char uchByte)
{
	int rc = LEXER_OK;

	// Here we are looking for a continuation line.  So far all we
	// have is either a LF character or a CRLF sequence.
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  Now we have a valid
		// continuation-line sequence.  However, since we need
		// to also recognize long sequences of whitespace, we can't emit
		// anything yet.  Move to the next state and wait for the
		// next input byte.
		m_State = MIME_LEXER_STATE_DOMAINLITERAL6;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a whitespace character.
	// This is not a continuation line sequence.  We either have a
	// single LF character or a CRLF sequence.  In either case we
	// need to emit a CRLF token.
	// Remove the last byte added to our buffer by backing up one byte.
	m_nLexemeOffset--;

	// Emit the suspected domain literal string, since we know it was
	// started and we have abandoned the notion of a possible continuation line.
	rc = EmitToken(MIME_TOKEN_DOMAIN_LITERAL);
	if (rc == LEXER_ERROR)
	  return (rc);

	m_nStartOffset = m_nOffset;	// Set the next token's start location.

	// Emit a CRLF token.  Note that the CRLF token represents three
	// character combinations: CR, LF, and CRLF.
	rc = EmitToken(MIME_TOKEN_CRLF);
	if (rc == LEXER_ERROR)
		return (rc);

	m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

	// Now that our buffer has been reset, put this
	// new byte back.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Increment the current line number.
	m_nLineNumber++;

	// Make a mental note that we just emitted a new-line.
	m_bNewLine = true;

	// And, finally, go back and analyze the byte from scratch.
	m_State = g_iMIMELexerStartStates[0];
	m_iNextStartStateIndex = 1;
	return (rc);
}  


inline int CMIMELexer::State_DOMAINLITERAL6(unsigned char uchByte)
{
	// Here we are looking for additional whitespace on
	// a continuation line.  So far we have one of the following
	// sequences: CR+LWSP, LF+LWSP, CR+LF+LWSP.
	// Now all we need to do is skip any additional whitespace.
	// Once we hit something that is not whitespace, that character
	// must be the beginning of the continuation line.
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  This is still part
		// of the continuation-line sequence.  Stay in this state
		// to keep looking for more whitespace.
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a whitespace character.  This must be the
	// beginning of the continued line.  So, we need to fold the
	// previous line here by removing the continuation characters
	// that have been added to the buffer up to this point.  Since
	// we saved the offset (m_nContinuationOffset) of the first
	// lexeme in the continuation sequence, we simply need to back
	// up m_nLexemeOffset to that point and re-add this byte there.
	m_nLexemeOffset = m_nContinuationOffset;
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// And, finally, go back and analyze the byte from scratch.
	m_State = MIME_LEXER_STATE_DOMAINLITERAL2;
	m_iNextStartStateIndex = 1;
	return LEXER_OK;
}  


inline int CMIMELexer::State_COMMENT1(unsigned char uchByte)
{
	if (m_pParser->m_bScanForPairs && uchByte == ASC_CHR_LPAREN)
	{
		// The byte is an open-parenthesis.  Since we need to look
		// for the close-parenthesis and everything else in-between,
		// we can't emit anything yet.  Move to the next state
		// and wait for the next input byte.
		m_State = MIME_LEXER_STATE_COMMENT2;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not an open-parenthesis.
	// Move to the next start state.
	m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
	return LEXER_OK;
}  


inline int CMIMELexer::State_COMMENT2(unsigned char uchByte)
{
	int rc = LEXER_OK;

	if (uchByte == ASC_CHR_RPAREN)
	{
		// The byte is a close-parenthesis character.
		if (m_nCommentLevel)
			m_nCommentLevel--;

		if (m_nCommentLevel == 0)
		{
			// Emit a COMMENT token.
			rc = EmitToken(MIME_TOKEN_COMMENT);
			if (rc == LEXER_ERROR)
				return (rc);

			m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.
		}

		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return (rc);
	}

	if (IsCR(uchByte))
	{
		// The byte is a CR character.  Move to state
		// COMMENT4 to look for continuation lines.
		// Also save the offset of this character in the lexeme buffer
		// so that we can remove the continuation characters later.
		m_State = MIME_LEXER_STATE_COMMENT4;
		m_nContinuationOffset = m_nLexemeOffset - 1;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	if (IsLF(uchByte))
	{
		// The byte is a LF character.  Move to state
		// COMMENT5 to look for continuation lines.
		// Also save the offset of this character in the lexeme buffer
		// so that we can remove the continuation characters later.
		m_State = MIME_LEXER_STATE_COMMENT5;
		m_nContinuationOffset = m_nLexemeOffset - 1;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	if (uchByte == ASC_CHR_BSLASH)
	{
		// The byte is the special quoted-pair character.
		// Since we need to get the next character to finish
		// the pair, we can't emit anything yet.  Move to the
		// next state and wait for the next input byte.
		m_State = MIME_LEXER_STATE_COMMENT3;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is a CHAR.  We still need to locate the
	// close-parenthesis character so stay in this state.
	m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
	return LEXER_OK;
}  


inline int CMIMELexer::State_COMMENT3(unsigned char uchByte)
{
	// The byte is a CHAR.
	// Copy the byte over the special quoted-pair character
	// in our buffer.
	m_nLexemeOffset--;
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Go back to the previous state where we are looking for
	// the closing quote.
	m_State = MIME_LEXER_STATE_COMMENT2;
	m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
	return LEXER_OK;
}  


inline int CMIMELexer::State_COMMENT4(unsigned char uchByte)
{
	int rc = LEXER_OK;

	// Here we are looking for a continuation line.  So far all we
	// have is a CR character.
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  Now we have a valid
		// continuation-line sequence.  However, since we need
		// to also recognize long sequences of whitespace, we can't emit
		// anything yet.  Move to the next state and wait for the
		// next input byte.
		m_State = MIME_LEXER_STATE_COMMENT6;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	if (IsLF(uchByte))
	{
		// The byte is a LF character.  We now have a CRLF sequence.
		// Move to state COMMENT5 to look for continuation lines.
		m_State = MIME_LEXER_STATE_COMMENT5;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a line-feed or whitespace character.
	// This is not a continuation line sequence.  We have a single CR
	// character.  This should be treated exactly the same as the
	// CRLF2 state (which also handles the single CR case).
	// Remove the last byte added to our buffer by backing up one byte.
	m_nLexemeOffset--;

	// Emit the suspected comment string, since we know it was
	// started and we have abandoned the notion of a possible continuation line.
	rc = EmitToken(MIME_TOKEN_COMMENT);
	if (rc == LEXER_ERROR)
	  return (rc);

	m_nStartOffset = m_nOffset;	// Set the next token's start location.

	// Emit a CRLF token.  Note that the CRLF token represents three
	// character combinations: CR, LF, and CRLF.
	rc = EmitToken(MIME_TOKEN_CRLF);
	if (rc == LEXER_ERROR)
		return (rc);

	m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

	// Now that our buffer has been reset, put this
	// new byte back.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Increment the current line number.
	m_nLineNumber++;

	// Make a mental note that we just emitted a new-line.
	m_bNewLine = true;

	// And, finally, go back and analyze the byte from scratch.
	m_State = g_iMIMELexerStartStates[0];
	m_iNextStartStateIndex = 1;
	return (rc);
}  


inline int CMIMELexer::State_COMMENT5(unsigned char uchByte)
{
	int rc = LEXER_OK;

	// Here we are looking for a continuation line.  So far all we
	// have is either a LF character or a CRLF sequence.
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  Now we have a valid
		// continuation-line sequence.  However, since we need
		// to also recognize long sequences of whitespace, we can't emit
		// anything yet.  Move to the next state and wait for the
		// next input byte.
		m_State = MIME_LEXER_STATE_COMMENT6;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a whitespace character.
	// This is not a continuation line sequence.  We either have a
	// single LF character or a CRLF sequence.  In either case we
	// need to emit a CRLF token.
	// Remove the last byte added to our buffer by backing up one byte.
	m_nLexemeOffset--;

	// Emit the suspected comment string, since we know it was
	// started and we have abandoned the notion of a possible continuation line.
	rc = EmitToken(MIME_TOKEN_COMMENT);
	if (rc == LEXER_ERROR)
	  return (rc);

	m_nStartOffset = m_nOffset;	// Set the next token's start location.

	// Emit a CRLF token.  Note that the CRLF token represents three
	// character combinations: CR, LF, and CRLF.
	rc = EmitToken(MIME_TOKEN_CRLF);
	if (rc == LEXER_ERROR)
		return (rc);

	m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

	// Now that our buffer has been reset, put this
	// new byte back.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Increment the current line number.
	m_nLineNumber++;

	// Make a mental note that we just emitted a new-line.
	m_bNewLine = true;

	// And, finally, go back and analyze the byte from scratch.
	m_State = g_iMIMELexerStartStates[0];
	m_iNextStartStateIndex = 1;
	return (rc);
}  


inline int CMIMELexer::State_COMMENT6(unsigned char uchByte)
{
	// Here we are looking for additional whitespace on
	// a continuation line.  So far we have one of the following
	// sequences: CR+LWSP, LF+LWSP, CR+LF+LWSP.
	// Now all we need to do is skip any additional whitespace.
	// Once we hit something that is not whitespace, that character
	// must be the beginning of the continuation line.
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  This is still part
		// of the continuation-line sequence.  Stay in this state
		// to keep looking for more whitespace.
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a whitespace character.  This must be the
	// beginning of the continued line.  So, we need to fold the
	// previous line here by removing the continuation characters
	// that have been added to the buffer up to this point.  Since
	// we saved the offset (m_nContinuationOffset) of the first
	// lexeme in the continuation sequence, we simply need to back
	// up m_nLexemeOffset to that point and re-add this byte there.
	m_nLexemeOffset = m_nContinuationOffset;
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// And, finally, go back and analyze the byte from scratch.
	m_State = MIME_LEXER_STATE_COMMENT2;
	m_iNextStartStateIndex = 1;
	return LEXER_OK;
}  


inline int CMIMELexer::State_ROUTEADDR1(unsigned char uchByte)
{
	if (m_pParser->m_bScanForPairs && uchByte == ASC_CHR_LT)
	{
		// The byte is an less-than symbol.  Since we need to look
		// for the greater-than symbol and everything else in-between,
		// we can't emit anything yet.  Move to the next state
		// and wait for the next input byte.
		m_State = MIME_LEXER_STATE_ROUTEADDR2;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a less-than symbol.
	// Move to the next start state.
	m_State = g_iMIMELexerStartStates[m_iNextStartStateIndex++];
	return LEXER_OK;
}  

inline int CMIMELexer::State_ROUTEADDR2(unsigned char uchByte)
{
	int rc = LEXER_OK;

	if (uchByte == ASC_CHR_GT)
	{
		// The byte is a greater-than symbol character.
		// Emit a ROUTE_ADDR token.
		rc = EmitToken(MIME_TOKEN_ROUTE_ADDR);
		if (rc == LEXER_ERROR)
			return (rc);

		m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return (rc);
	}

	if (IsCR(uchByte))
	{
		// The byte is a CR character.  Move to state
		// ROUTEADDR4 to look for continuation lines.
		// Also save the offset of this character in the lexeme buffer
		// so that we can remove the continuation characters later.
		m_State = MIME_LEXER_STATE_ROUTEADDR4;
		m_nContinuationOffset = m_nLexemeOffset - 1;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	if (IsLF(uchByte))
	{
		// The byte is a LF character.  Move to state
		// ROUTEADDR5 to look for continuation lines.
		// Also save the offset of this character in the lexeme buffer
		// so that we can remove the continuation characters later.
		m_State = MIME_LEXER_STATE_ROUTEADDR5;
		m_nContinuationOffset = m_nLexemeOffset - 1;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	if (uchByte == ASC_CHR_BSLASH)
	{
		// The byte is the special quoted-pair character.
		// Since we need to get the next character to finish
		// the pair, we can't emit anything yet.  Move to the
		// next state and wait for the next input byte.
		m_State = MIME_LEXER_STATE_ROUTEADDR3;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is a CHAR.  We still need to locate the
	// greater-than symbol character so stay in this state.
	m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
	return LEXER_OK;
}  


inline int CMIMELexer::State_ROUTEADDR3(unsigned char uchByte)
{
	// The byte is a CHAR.
	// Copy the byte over the special quoted-pair character
	// in our buffer.
	m_nLexemeOffset--;
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Go back to the previous state where we are looking for
	// the closing quote.
	m_State = MIME_LEXER_STATE_ROUTEADDR2;
	m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
	return LEXER_OK;
}  


inline int CMIMELexer::State_ROUTEADDR4(unsigned char uchByte)
{
	int rc = LEXER_OK;

	// Here we are looking for a continuation line.  So far all we
	// have is a CR character.
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  Now we have a valid
		// continuation-line sequence.  However, since we need
		// to also recognize long sequences of whitespace, we can't emit
		// anything yet.  Move to the next state and wait for the
		// next input byte.
		m_State = MIME_LEXER_STATE_ROUTEADDR6;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	if (IsLF(uchByte))
	{
		// The byte is a LF character.  We now have a CRLF sequence.
		// Move to state ROUTEADDR5 to look for continuation lines.
		m_State = MIME_LEXER_STATE_ROUTEADDR5;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a line-feed or whitespace character.
	// This is not a continuation line sequence.  We have a single CR
	// character.  This should be treated exactly the same as the
	// CRLF2 state (which also handles the single CR case).
	// Remove the last byte added to our buffer by backing up one byte.
	m_nLexemeOffset--;

	// Emit the suspected route address string, since we know it was
	// started and we have abandoned the notion of a possible continuation line.
	rc = EmitToken(MIME_TOKEN_ROUTE_ADDR);
	if (rc == LEXER_ERROR)
	  return (rc);

	m_nStartOffset = m_nOffset;	// Set the next token's start location.

	// Emit a CRLF token.  Note that the CRLF token represents three
	// character combinations: CR, LF, and CRLF.
	rc = EmitToken(MIME_TOKEN_CRLF);
	if (rc == LEXER_ERROR)
		return (rc);

	m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

	// Now that our buffer has been reset, put this
	// new byte back.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Increment the current line number.
	m_nLineNumber++;

	// Make a mental note that we just emitted a new-line.
	m_bNewLine = true;

	// And, finally, go back and analyze the byte from scratch.
	m_State = g_iMIMELexerStartStates[0];
	m_iNextStartStateIndex = 1;
	return (rc);
}  


inline int CMIMELexer::State_ROUTEADDR5(unsigned char uchByte)
{
	int rc = LEXER_OK;

	// Here we are looking for a continuation line.  So far all we
	// have is either a LF character or a CRLF sequence.
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  Now we have a valid
		// continuation-line sequence.  However, since we need
		// to also recognize long sequences of whitespace, we can't emit
		// anything yet.  Move to the next state and wait for the
		// next input byte.
		m_State = MIME_LEXER_STATE_ROUTEADDR6;
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a whitespace character.
	// This is not a continuation line sequence.  We either have a
	// single LF character or a CRLF sequence.  In either case we
	// need to emit a CRLF token.
	// Remove the last byte added to our buffer by backing up one byte.
	m_nLexemeOffset--;

	// Emit the suspected route address string, since we know it was
	// started and we have abandoned the notion of a possible continuation line.
	rc = EmitToken(MIME_TOKEN_ROUTE_ADDR);
	if (rc == LEXER_ERROR)
	  return (rc);

	m_nStartOffset = m_nOffset;	// Set the next token's start location.

	// Emit a CRLF token.  Note that the CRLF token represents three
	// character combinations: CR, LF, and CRLF.
	rc = EmitToken(MIME_TOKEN_CRLF);
	if (rc == LEXER_ERROR)
		return (rc);

	m_nStartOffset = m_nOffset + 1;	// Set the next token's start location.

	// Now that our buffer has been reset, put this
	// new byte back.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// Increment the current line number.
	m_nLineNumber++;

	// Make a mental note that we just emitted a new-line.
	m_bNewLine = true;

	// And, finally, go back and analyze the byte from scratch.
	m_State = g_iMIMELexerStartStates[0];
	m_iNextStartStateIndex = 1;
	return (rc);
}  


inline int CMIMELexer::State_ROUTEADDR6(unsigned char uchByte)
{
	// Here we are looking for additional whitespace on
	// a continuation line.  So far we have one of the following
	// sequences: CR+LWSP, LF+LWSP, CR+LF+LWSP.
	// Now all we need to do is skip any additional whitespace.
	// Once we hit something that is not whitespace, that character
	// must be the beginning of the continuation line.
	if (IsSPACE(uchByte) || IsHTAB(uchByte))
	{
		// The byte is a whitespace character.  This is still part
		// of the continuation-line sequence.  Stay in this state
		// to keep looking for more whitespace.
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a whitespace character.  This must be the
	// beginning of the continued line.  So, we need to fold the
	// previous line here by removing the continuation characters
	// that have been added to the buffer up to this point.  Since
	// we saved the offset (m_nContinuationOffset) of the first
	// lexeme in the continuation sequence, we simply need to back
	// up m_nLexemeOffset to that point and re-add this byte there.
	m_nLexemeOffset = m_nContinuationOffset;
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// And, finally, go back and analyze the byte from scratch.
	m_State = MIME_LEXER_STATE_ROUTEADDR2;
	m_iNextStartStateIndex = 1;
	return LEXER_OK;
}  


inline int CMIMELexer::State_ATOM1(unsigned char uchByte)
{
	// If the character did not get recognized by any of the
	// previous start states, then we'll call it an atom.
	// Since we need to also recognize long sequences of atom
	// characters, we can't emit anything yet.  Move to the next
	// state and wait for the next input byte.
	m_State = MIME_LEXER_STATE_ATOM2;
	m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
	return LEXER_OK;
}  


inline int CMIMELexer::State_ATOM2(unsigned char uchByte)
{
	int rc = LEXER_OK;

	if (!IsTSPECIAL(uchByte) &&
		!IsSPACE(uchByte) &&
		!IsCTL(uchByte))
	{
		// The byte is a valid atom character.  We stay in this
		// same state until we find a non-atom character.
		m_iNextStartStateIndex = MIME_LEXER_START_RESET;  // Skip remaining start states
		return LEXER_OK;
	}

	// The byte is not a valid atom character.
	// Remove the last byte added to our buffer by backing up one byte.
	m_nLexemeOffset--;

	// Emit an ATOM token.
	rc = EmitToken(MIME_TOKEN_ATOM);
	if (rc == LEXER_ERROR)
		return (rc);

	m_nStartOffset = m_nOffset;	// Set the next token's start location.

	// Now that our buffer has been reset, put this
	// new byte back.
	m_byBuffer[m_nLexemeOffset++] = uchByte;

	// And, finally, go back and analyze the byte from scratch.
	m_iNextStartStateIndex = 1;
	return (rc);
}  
