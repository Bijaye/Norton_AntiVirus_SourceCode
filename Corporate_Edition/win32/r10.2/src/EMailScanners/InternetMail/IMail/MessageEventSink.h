// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// MessageEventSink.h: interface for the CMessageEventSink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGEEVENTSINK_H__C384477E_F0B0_4C7E_BE9C_E9DF78845237__INCLUDED_)
#define AFX_MESSAGEEVENTSINK_H__C384477E_F0B0_4C7E_BE9C_E9DF78845237__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMessageEventSink : public CDecEventSink  
{
public:
	CMessageEventSink();
	virtual ~CMessageEventSink();

public:
    virtual DECRESULT OnObjectBegin(IDecObject *pObject, WORD *pwResult, char *pszNewDataFile, bool *pbContinue);
    virtual DECRESULT OnDiskFileNeeded(IDecContainerObject *pObject, char *pszdiskfile, int iBufferSize);
    virtual DECRESULT OnContainerIdentified(IDecContainerObject *pObject, bool *pbContinue); 
	virtual DECRESULT OnChildFound(IDecContainerObject *pObject, DWORD dwType, const char *pszChildName, DWORD dwCharset, bool *pbContinue);

    void    SetDecObj(CDecInterface  *pDecObj);
    void    SetWarningMsg (const char *szMsg);
    bool    myCopyFile(const char *pszExistingFileName, const char *pszNewFileName, bool bFailIfExists);

protected:
    const CMessageEventSink &operator=(const CMessageEventSink &) { return *this; }

    // This is used to track the two body text sections of the MIME that
    // we need to insert our "you are infected" message in. We replace the
    // first text/plain and text/html sections.
    class CBodyCounters
    {
    public:
        CBodyCounters();
        void clear ();
        bool calc (IDecObject *pObject, CDecInterface  *pDecObj);

        // Fetch the body number associated with the specified eTextType
        // Returns -1 for nothing of that type.
        DWORD getTextBodyNum (enum MIMELexerTokens eTextType) const;

        // Fetch the Windows codepage corresponding to the text encoding used for
        // this body section. Returns CP_ACP if unknown.
        UINT getTextBodyCharset (enum MIMELexerTokens eTextType) const;

    private:
        const CBodyCounters &operator=(const CBodyCounters &) { return *this; }
        UINT getCharset ( IDecObject *pObject, CDecInterface  *pDecObj, size_t nCharsetStart, size_t nCharsetLength );

        // Add a new body / text info to our list as long as the eTextType
        // does not already exist in the array. If it does exist, this add()
        // request is ignored.
        bool addNotExist (DWORD dwBodyNum, enum MIMELexerTokens eTextType, UINT uCharset);

        typedef struct _BODY_COUNTER
        {
            DWORD                  dwBodyNum;
            enum MIMELexerTokens   eTextType;    // MIME_TOKEN_PLAIN or	MIME_TOKEN_HTML
            UINT                   uCharset;     // Windows codepage corresponding to text encoding for this body section
        } BODY_COUNTER;

        BODY_COUNTER m_arr[5];      // use 2 now, 3 for expansion.
        DWORD        m_dwNumElems;
        const DWORD  m_dwMaxElems;
    };


    DWORD   m_dwDepthLimit;

    bool    m_bProcessingBodySections;      // Set to true once we begin receiving body sections in OnChildFound() and OnObjectBegin()
    DWORD   m_dwCurrentBodySection;         // 0-based, current body section.

    CDecInterface   *m_pDecObj;
    CBodyCounters    m_objBodyCounters;
    const char      *m_szWarningMsg;

    bool modifyMessage( IDecObject *pObject, WORD *pwResult, char *pszNewDataFile, bool *pbContinue, enum MIMELexerTokens eTextType );
};


inline CMessageEventSink::CBodyCounters::CBodyCounters () :
    m_dwNumElems (0),
    m_dwMaxElems (sizeof (m_arr) / sizeof (m_arr[0]))
{
    clear ();
}

inline void CMessageEventSink::SetWarningMsg (const char *szMsg)
{
    m_szWarningMsg = szMsg;
}

#endif // !defined(AFX_MESSAGEEVENTSINK_H__C384477E_F0B0_4C7E_BE9C_E9DF78845237__INCLUDED_)
