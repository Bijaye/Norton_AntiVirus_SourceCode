// AVISTransactionExportSignature.cpp: implementation of the CAVISTransactionExportSignature class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVISTransactionExportSignature.h"
#include "Signature.h"
#include "AVISTransactionExportSignatureException.h"
#include "AttributeKeys.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SIGNATURE_READ_BUFFER_SIZE (1024 * 256)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// new constructor!
CAVISTransactionExportSignature::CAVISTransactionExportSignature(
	LPCSTR pszAgentName,
	LPCSTR pszGatewayURL,
	UINT uiTimeoutTransactionConnected,
	UINT uiTimeoutTransactionCompleted,
	CTransactionTimeoutMonitorThread* pthreadTransactionTimeoutMonitor,
	BOOL boUseSSL)
  : CAVISTransaction(
		pszAgentName,
		pszGatewayURL,
		CHttpConnection::HTTP_VERB_POST,
		uiTimeoutTransactionConnected,
		uiTimeoutTransactionCompleted,
		pthreadTransactionTimeoutMonitor,
		EXPORT_SIGNATURE_SERVLET_NAME,
		boUseSSL),
	m_cbSignatureFile(0)
{
	CAVISACSignatureCommApp::s_tc.enter("Entering CAVISTransactionExportSignature constructor...");

	std::string strSignatureStoragePathBase;
	m_strSignatureStoragePathBase = CAVISProfile::GetStringValue(SIGNATURE_STORAGE_PATH_KEYNAME);

	if ("" == m_strSignatureStoragePathBase)
	{
		// This entry is required!
		throw new CAVISTransactionExportSignatureException(
			CAVISTransactionExportSignatureException::noSignatureStoragePathBase,
			"The profile entry for the signature storage path was not present, and is required.");
	}

	m_strSignatureStoragePathBase += "\\";

	CAVISACSignatureCommApp::s_tc.exit("Leaving CAVISTransactionExportSignature contstructor.");
}
/*****/
CAVISTransactionExportSignature::~CAVISTransactionExportSignature()
{
	CAVISACSignatureCommApp::s_tc.enter("Entering CAVISTransactionExportSignature destructor...");
	CAVISACSignatureCommApp::s_tc.exit("Leaving CAVISTransactionExportSignature destructor.");
}
/*****/
DWORD CAVISTransactionExportSignature::InitRequest()
{
	EmptyRequestHeaders();
	// add required headers for "postSampleStatus" transaction
	AddRequestHeader(EXPORT_SIGNATURE_TX_REQUIRED_HEADERS);

	// *** must eventually get the signature sequence number from the database ***
	Signature sig(m_uiSignatureSeqNum);

	// ensure that the signature entry for this sequence number actually exists
	if (sig.IsNull())
	{
		CString strMsg;
		strMsg.Format("A signature file with SeqNum=%d was marked for export, but this signature did not have a database entry.", m_uiSignatureSeqNum);
		throw new CAVISTransactionExportSignatureException(
			CAVISTransactionExportSignatureException::signatureNotFoundInDB,
			strMsg);
	}

	CString strHeader;
	DateTime dt;
	std::string strDateTime;

	// add "signature sequence number" header
	AddRequestHeader(AttributeKeys::SignatureSequence(), m_uiSignatureSeqNum);

	// determine which type of package is stored in the signature directory

	// construct FatAlbert-format signature filename
	CString strSignatureFilename;
	CString strSignaturePath;
	CFileStatus fs;

	// check if the file has a FatAlbert-type filename
	strSignatureFilename.Format("%08u." FILENAME_EXTENSION_FATALBERT, m_uiSignatureSeqNum);
	strSignaturePath = m_strSignatureStoragePathBase.c_str() + strSignatureFilename;
	if (!CFile::GetStatus(strSignaturePath, fs))
	{
		// assume that GetStatus failed because the file does not exist.
		// construct VDB-format signature filename
		strSignatureFilename.Format("%08u." FILENAME_EXTENSION_VDB, m_uiSignatureSeqNum);
		strSignaturePath = m_strSignatureStoragePathBase.c_str() + strSignatureFilename;

		// check if the file has a VDB-type filename
		if (!CFile::GetStatus(strSignaturePath, fs))
		{
			CString strMsg;
			strMsg.Format("An entry for a signature with sequence number=%u exists in the database, but the file is not present in the directory %s.", m_uiSignatureSeqNum, m_strSignatureStoragePathBase.c_str());
			throw new CAVISTransactionExportSignatureException(
				CAVISTransactionExportSignatureException::signatureFileNotFound,
				strMsg);
		}
		else
		{
			CAVISACSignatureCommApp::s_tc.info("Definition file is of the VDB type.");
			AddRequestHeader(AttributeKeys::ContentType(), EXPORT_SIGNATURE_CONTENT_TYPE_HEADER_VALUE_VDB);
		}
	}
	else
	{
		CAVISACSignatureCommApp::s_tc.info("Definition file is of the FatAlbert type.");
		AddRequestHeader(AttributeKeys::ContentType(), EXPORT_SIGNATURE_CONTENT_TYPE_HEADER_VALUE_FATALBERT);
	}

	// add "signatures name" header
	AddRequestHeader(AttributeKeys::SignatureName(), (LPCSTR )strSignatureFilename);

	// add "blessed date" if it exists
	if (sig.IsBlessed())
	{
		dt = sig.Blessed();
		dt.AsHttpString(strDateTime);
		AddRequestHeader(AttributeKeys::DateBlessed(), strDateTime);
	}

	// add "date produced" header
	dt = sig.Produced();
	dt.AsHttpString(strDateTime);
	AddRequestHeader(AttributeKeys::DateProduced(), strDateTime);

	// add "date published" header
	dt = sig.Published();
	dt.AsHttpString(strDateTime);
//	AddRequestHeader(AttributeKeys::DatePublished(), strDateTime);
	AddRequestHeader(SIGNATURE_DATE_PUBLISHED_HEADER_NAME, strDateTime);

	// add "signature checksum" and "content checksum" headers
	AddRequestHeader(SIGNATURE_CHECKSUM_HEADER_NAME, sig.CheckSum());
	AddRequestHeader(AttributeKeys::ContentCheckSum(), sig.CheckSum());

	CFileException exceptFile;

	if (!m_fileSignature.Open(
		m_strSignatureStoragePathBase.c_str() + strSignatureFilename, 
		CFile::modeRead |
		CFile::shareDenyNone,
		&exceptFile))
	{
		CString strMsg;
		strMsg.Format("An entry for a signature with filename=\"%s\" exists in the database, but the file is not present in the directory %s.", strSignatureFilename, m_strSignatureStoragePathBase.c_str());
		throw new CAVISTransactionExportSignatureException(
			CAVISTransactionExportSignatureException::signatureFileNotFound,
			strMsg);
	}

	m_cbSignatureFile = fs.m_size;

	return AVISTX_SUCCESS;
}
/*****/
int CAVISTransactionExportSignature::StartRequest(DWORD, PVOID)
{
	return SendRequestEx(m_cbSignatureFile, SIGNATURE_READ_BUFFER_SIZE);
}
/*****/
void CAVISTransactionExportSignature::EndRequest()
{
	if (m_fileSignature.m_hFile != CFile::hFileNull)
		m_fileSignature.Close();

	CAVISTransaction::EndRequest();
}
/*****/
int CAVISTransactionExportSignature::GetNextRequestContentBlock(PVOID pvRequestContentBlock)
{
	TRY
	{
		return m_fileSignature.Read(pvRequestContentBlock, SIGNATURE_READ_BUFFER_SIZE);
	}
	CATCH(CFileException, pExcept)
	{
		char szFileErrMsg[500];
		pExcept->GetErrorMessage(szFileErrMsg, sizeof(szFileErrMsg));
		pExcept->Delete();

		CString strMsg;
		strMsg.Format("An error occurred (\"%s\") while reading the signature file from disk", szFileErrMsg);
		throw new CAVISTransactionExportSignatureException(
			CAVISTransactionExportSignatureException::errorReadingSignatureFile,
			strMsg);
	}
	END_CATCH
}
/*****/
void CAVISTransactionExportSignature::SetSignatureSeqNum(UINT uiSignatureSeqNum)
{
	m_uiSignatureSeqNum = uiSignatureSeqNum;
}
