//
//	IBM AntiVirus Immune System
//
//	File Name:	SignatureImporterTransaction.cpp
//	Author:		Milosz Muszynski
//
//	This class is based on the CAVISTransaction class
//	and overrides some of its virtual functions in order
//	to perform transaction to import a signature set
//	from a gateway
//
//	$Log: $
//


// ------ Microsoft headers -------------------------------------------
#include "base.h"

//------ pragmas ------------------------------------------------------
#pragma warning ( disable : 4786 ) // identifier truncated

// ------ standard C++ headers ----------------------------------------
#include <string>
#include <map>
#include <fstream.h>

// ------ transaction headers -----------------------------------------
#include "SignatureImporterTransaction.h"
#include "AttributeKeys.h"
#include "Config.h"

// ------ unpacking headers -------------------------------------------
#if 0 /* inw 2000-02-16 removed FatAlbert support */
#include "FatAlbertUnpack.h"
#endif /* 0 */
#include "VDBUnpacker.h"
 
// ------ utility headers ---------------------------------------------
#include "StrStrMap.h"
#include "StrUtil.h"
#include "Util.h"

// ------ external variables ------------------------------------------
extern	int			s_unpackingTimeout;
extern	std::string s_unpackingTempDir;




// ------ SignatureImporterTransaction --------------------------------

//
// Constructor
//
SignatureImporterTransaction::SignatureImporterTransaction(
	const std::string&				agentName,
	const std::string&				gatewayURL,
	const std::string&				targetDirectory,
	bool							withUnpacking,
	RawEvent&						eventStop, 
	MyTraceClient&					traceClient,
	bool							useSSL,
  const std::string&        target,
  const HttpConnection::connectionVerb    verb)
  : TransactionExecutor(
		agentName,
		gatewayURL,
		verb,
		traceClient,
		eventStop, 
		target,
		useSSL),
	_signatureStoragePath( targetDirectory ),
	_withUnpacking( withUnpacking),
	_headersRetrieved( false ),
	_packageType( FatAlbert )
{
	_tc.enter("Entering SignatureImporterTransaction constructor...");

	if ( _signatureStoragePath.length() == 0 )
	{
		_tc.debug("signature storage path was missing");
	}

	StrUtil::makeSureEndsWith( _signatureStoragePath, '\\' );

	_tc.exit("Leaving SignatureImporterTransaction constructor.");
}


//
// Destructor
//
SignatureImporterTransaction::~SignatureImporterTransaction()
{
	_tc.enter("Entering SignatureImporterTransaction destructor...");
	_tc.exit("Leaving SignatureImporterTransaction destructor.");
}


//
// DetermineFileName
//

IcePackAgentComm::ErrorCode SignatureImporterTransaction::DetermineFileNameAndType( std::string& fileName )
{
	StrStrMap headers( _responseHeaders );

	fileName = headers.getString( AttributeKeys::SignatureSequence() );
	std::string packageType = headers.getString( "Content-Type" );

  /*
   * This is a hack to ensure that the filename of the signatures is always
   * precisely eight characters long.  This makes me feel ill, but it's
   * necessary -- occasionally, the gateway has been seen returning
   * signature file names without leading zeros.
   */
  while (8 > fileName.size())
  {
    fileName = "0" + fileName;
  }

	if ( StrUtil::containsAnyCase( packageType, "FatAlbert" ) )
	{
		_packageType = FatAlbert;
		_tc.debug( "package type set to \"FatAlbert\"" );
	}
	else if ( StrUtil::containsAnyCase( packageType, "VDB" ) )
	{
		_packageType = VDB;
		_tc.debug( "package type set to \"VDB\"" );
	}
	else
	{
		_packageType = FatAlbert;	// the default is FatAlbert
		_tc.error( "unknown package type: %s", packageType.c_str() );
		_tc.error( "FatAlbert assumed as default" );
	}

	if ( fileName.length() == 0 )
	{
		_tc.critical( "missing signature sequence" );
		return IcePackAgentComm::MissingSignatureSequenceError;
	}

	std::string sigSequence = fileName;

	if ( _packageType == VDB )
		fileName += CFG_signature_file_vdb_postfix;
	else
		fileName += CFG_signature_file_postfix;

	fileName = _signatureStoragePath + fileName;

	StrUtil::makeSureEndsWith( _signatureStoragePath, '\\' );
	_signatureStoragePath += sigSequence; // add a signature sequence without postfix to a storage path

	return IcePackAgentComm::NoError;
}



//
// ReceiveContentIntoFile
//

IcePackAgentComm::ErrorCode SignatureImporterTransaction::ReceiveContentIntoFile  ( const std::string& fileName )
{
	ofstream of;

	of.open( fileName.c_str() );
	of.setmode( filebuf::binary );

	if ( !of )
	{
		_tc.critical( "problem with opening the file: %s", fileName.c_str() );
		return IcePackAgentComm::FileOpenError;
	}

	// read response content

	DWORD responseContentLength = GetResponseContentLength();
	_tc.debug( "response content length = %d", responseContentLength );

	char	bufResponseContent[ ReadResponseBlockSize ];
	_tc.debug( "block size for reading content = %d", ReadResponseBlockSize );

	SetReadBufferSize( 0 );

	int		bytesRead = 0;
	DWORD	bytesLeft = responseContentLength;

	do
	{
		if ( _eventStop.isSet() )
		{
			_eventStop.reset();
			_tc.critical( "terminated by caller while reading response content" );
			return IcePackAgentComm::TerminatedByCaller;
		}
		bytesLeft -= bytesRead;
		//_tc.debug( "bytes left %d", bytesLeft );
		bytesRead = 0;
		if ( bytesLeft > 0 )
			bytesRead = ReadNextResponseContentBlock( bufResponseContent, ReadResponseBlockSize < bytesLeft ? ReadResponseBlockSize : bytesLeft );
		if ( bytesRead > 0 )
			of.write( bufResponseContent, bytesRead );
		else
			break;
		if ( of.fail() )
			break;
	}
	while( bytesRead > 0 );

	if ( bytesRead < 0 )
	{
		_tc.critical( "problem with reading data from the network" );
		return IcePackAgentComm::NetworkError;
	}


	if ( of.fail() )
	{
		_tc.critical( "problem with writing to the file: %s", fileName.c_str() );
		return IcePackAgentComm::FileWriteError;
	}

	of.close();

	return IcePackAgentComm::NoError;
}

#if 0 /* inw 2000-02-16 removed FatAlbert support */
//
// FatAlbertUnpack
//

IcePackAgentComm::ErrorCode SignatureImporterTransaction::FatAlbertUnpack( const std::string& fileName )
{
	//HANDLE mutex = CreateMutex( NULL, FALSE, NULL ); // I need a wrapper class for that
	
	IcePackAgentComm::ErrorCode errorCode = IcePackAgentComm::NoError;

	_tc.msg( "unpacking FatAlbertPackage %s into %s (timeout=%d, temporary directory=%s)", 
			 fileName.c_str(), _signatureStoragePath.c_str(), s_unpackingTimeout, s_unpackingTempDir.c_str() );

	FatAlbertPackage package;
	package.Unpack( fileName, _signatureStoragePath, s_unpackingTimeout, NULL, s_unpackingTempDir, &_tc );

	std::string fatError;
	switch ( package.Error() )
	{
		case FatAlbertPackage::NoError:				errorCode = IcePackAgentComm::NoError					; fatError = "NoError";				break;
		case FatAlbertPackage::UnknownError:		errorCode = IcePackAgentComm::UnknownError				; fatError = "UnknownError";		break;
		case FatAlbertPackage::PackageNotFoundError:errorCode = IcePackAgentComm::PackageNotFoundError		; fatError = "PackageNotFound";		break;
		case FatAlbertPackage::TempDirError:		errorCode = IcePackAgentComm::TempDirError				; fatError = "TempDirError";		break;
		case FatAlbertPackage::TargetDirError:		errorCode = IcePackAgentComm::TargetDirError			; fatError = "TargetDirError";		break;
		case FatAlbertPackage::TargetDOSDirError:	errorCode = IcePackAgentComm::TargetDOSDirError			; fatError = "TargetDOSDirError";	break;
		case FatAlbertPackage::FileCopyError:		errorCode = IcePackAgentComm::FileCopyError				; fatError = "FileCopyError";		break;
		case FatAlbertPackage::NoFilesFoundError:	errorCode = IcePackAgentComm::NoFilesFoundError			; fatError = "NoFilesFoundError";	break;
		case FatAlbertPackage::DiskFullError:		errorCode = IcePackAgentComm::DiskFullError				; fatError = "DiskFull";			break;
		case FatAlbertPackage::VersionError:		errorCode = IcePackAgentComm::VersionError				; fatError = "VersionError";		break;
		case FatAlbertPackage::SystemException:		errorCode = IcePackAgentComm::FatAlbertSystemException 	; fatError = "SystemException";		break;
		case FatAlbertPackage::ProcessCreationError:	errorCode = IcePackAgentComm::ProcessCreationError 	; fatError = "ProcessCreationError";break;
		case FatAlbertPackage::ProcessTimedOutError:	errorCode = IcePackAgentComm::ProcessTimedOutError 	; fatError = "ProcessTimedOutError";break;
		case FatAlbertPackage::ProcessTerminationError:	errorCode = IcePackAgentComm::ProcessTerminationError 	; fatError = "ProcessTerminationError";	break;
		case FatAlbertPackage::ProcessSyncError:		errorCode = IcePackAgentComm::ProcessSyncError	 	; fatError = "ProcessSyncError";	break;
		default:									errorCode = IcePackAgentComm::UnknownError				; fatError = "UnknownError";		break;
	};

	_tc.msg( "FatAlbert unpacking completed, returned \"%s\"", fatError.c_str() );

	return errorCode;
}
#endif /* 0 */

//
// VDBUnpack
//

IcePackAgentComm::ErrorCode SignatureImporterTransaction::VDBUnpack( const std::string& fileName )
{
	IcePackAgentComm::ErrorCode errorCode = IcePackAgentComm::NoError;

	_tc.msg( "unpacking VDBPackage %s into %s", fileName.c_str(), _signatureStoragePath.c_str() );

	VDBPackage package;
	package.Unpack( fileName, _signatureStoragePath, NULL, static_cast<void*>(&_tc) );

	std::string vdbError;
	switch ( package.Error() )
	{
		case VDBPackage::NoError:				errorCode = IcePackAgentComm::NoError				; vdbError = "NoError";				break;
		case VDBPackage::PackageNotFoundError:	errorCode = IcePackAgentComm::PackageNotFoundError	; vdbError = "PackageNotFound";		break;
		case VDBPackage::TargetDirError:		errorCode = IcePackAgentComm::TargetDirError		; vdbError = "TargetDirError";		break;
		case VDBPackage::NoFilesFoundError:		errorCode = IcePackAgentComm::NoFilesFoundError		; vdbError = "NoFilesFoundError";	break;
		case VDBPackage::DiskFullError:			errorCode = IcePackAgentComm::DiskFullError			; vdbError = "DiskFull";			break;
		case VDBPackage::VersionError:			errorCode = IcePackAgentComm::VersionError			; vdbError = "VersionError";		break;
		case VDBPackage::SystemException:		errorCode = IcePackAgentComm::VDBSystemException 	; vdbError = "SystemException";		break;
		case VDBPackage::ZipArchiveError:		errorCode = IcePackAgentComm::VDBZipArchiveError	; vdbError = "VDBZipArchiveError";	break;
		case VDBPackage::MemoryError:			errorCode = IcePackAgentComm::VDBMemoryError		; vdbError = "MemoryError";			break;
		case VDBPackage::ParameterError:		errorCode = IcePackAgentComm::VDBParameterError 	; vdbError = "ParameterError";		break;
		case VDBPackage::UnexpectedEOFError:	errorCode = IcePackAgentComm::VDBUnexpectedEOFError	; vdbError = "UnexpectedEOFError";	break;
		case VDBPackage::ZipUnknownError:		
		default:								errorCode = IcePackAgentComm::VDBUnknownError		; vdbError = "UnknownError";		break;
	};

	_tc.msg( "VDB unpacking completed, returned \"%s\"", vdbError.c_str() );

	return errorCode;
}


//
// ReadResponseContent
//

int SignatureImporterTransaction::ReadResponseContent()
{
	// we get to this point if the request completes successfully and there is content

  std::string                 sigFileName;
	IcePackAgentComm::ErrorCode errorCode = IcePackAgentComm::NoError;

	_tc.debug("starting to read content...");

	RetrieveResponseHeaders();

	_headersRetrieved = true;

	errorCode = DetermineFileNameAndType(sigFileName);
	if (errorCode != IcePackAgentComm::NoError)
		return (int)errorCode;

	DirUtil::make(_signatureStoragePath); 

	errorCode = ReceiveContentIntoFile(sigFileName);
  if (errorCode == IcePackAgentComm::NoError)
  {
  	if (_withUnpacking)
  	{
	  	if (_packageType == VDB)
		  	errorCode = VDBUnpack(sigFileName);
#if 0 /* inw 2000-02-16 removed FatAlbert support */
		  else if (_packageType == FatAlbert)
  			errorCode = FatAlbertUnpack(sigFileName ;
#endif /* 0 */
	  	else
  		{
			  errorCode = IcePackAgentComm::UnknownSignatureTypeError;
			  _tc.error("unrecognized package type: %d", static_cast<int>(_packageType));
		  }
    }
  }

  if (errorCode)
  {
    /*
     * If there was an error, then attempt to remove the package.
     */
    _tc.info("Removing corrupt/invalid package...");
      
    /*
     * First remove the file.
     */
    if (FileUtil::isFile(sigFileName))
    {
      if (FileUtil::remove(sigFileName))
        _tc.info("Removal of corrupt/invalid package apparently successful");
      else
        _tc.error("Removal of corrupt/invalid package apparently FAILED");
    }
    else
      _tc.error("Allegedly corrupt/invalid package does not appear to exist on disk");

    /*
     * Now remove the directory into which we may have partially unpacked.
     */
    if (DirUtil::isDir(_signatureStoragePath))
    {
      if (DirUtil::prune(_signatureStoragePath))
        _tc.info("Removal of partially unpacked corrupt/invalid package apparently successful");
      else
        _tc.error("Removal of partially unpacked corrupt/invalid package apparently FAILED");
    }
    else
      _tc.info("No unpacked version of corrupt/invalid package found");
  }

	return errorCode;
}



bool SignatureImporterTransaction::SignatureWasAvailableAtGateway()
{
	// for this transaction, the gateway sends 'ok' when there is a signature available
	return GetResponseStatusCode() == HTTP_STATUS_OK;
}



//
// StartRequest
//

int SignatureImporterTransaction::StartRequest(	DWORD	cbSendContent,		// how much data to send
												PVOID	pvSendContent	)	// data to send
{
	_rc = SendRequest(cbSendContent, pvSendContent);
	return _rc;
}

//
// EndRequest
//

void SignatureImporterTransaction::EndRequest()
{
	if ( !_headersRetrieved && _rc == AVISTX_SUCCESS )
	{
		RetrieveResponseHeaders();
	}
}






