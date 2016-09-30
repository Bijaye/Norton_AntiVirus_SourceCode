// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef PREPEND_TEXT_H_INCLUDED
#define PREPEND_TEXT_H_INCLUDED

//------------------------------------------------------------------------
// Prepend the szFont text onto the beginning of szBack. The result is
// written to strOut.
//
// Parameters:
//   pszOut     [out]  Pointer to a string. This function allocates the
//                     string. The caller is responsible for "delete[]"-ing
//                     it.
//   szFront    [in]   New text to prepend onto the front of szBack
//   szBack     [in]   Existing text to prepend szFront onto.
//
// Returns:
//   NO_ERROR (which = 0)    - success
//   ERROR_NOT_ENOUGH_MEMORY - out of memory
//------------------------------------------------------------------------
DWORD PrependPlainText (TCHAR **pszOut, const TCHAR *szFront, const TCHAR *szBack);

//------------------------------------------------------------------------
// Insert szMsg right after the <BODY> tag found in szHtml. Note that
// szMsg is translated during the copy. For instance, the '<'
// characters translate to "&lt;".
//
// Parameters:
//   pszOut     [out]  Pointer to a string. This function allocates the
//                     string. The caller is responsible for "delete[]"-ing
//                     it. This contains the resulting HTML text.
//   szMsg      [in]   Message to insert into the HTML
//   szHtml     [in]   HTML text to insert szMsg onto.
//   wCodePage  [in]   The code page szMsg needs to be translated to.
//
// Returns:
//   NO_ERROR (which = 0)    - success
//   ERROR_NOT_ENOUGH_MEMORY - out of memory
//   ERROR_INVALID_DATA      - HTML in szBack is missing a <BODY> tag.
//------------------------------------------------------------------------
DWORD InsertHtmlText (TCHAR **pszOut, const TCHAR *szMsg, const TCHAR *szHtml, WORD wCodePage);

#endif // PREPEND_TEXT_H_INCLUDED