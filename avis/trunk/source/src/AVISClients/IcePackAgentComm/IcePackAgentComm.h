//
//	IBM AntiVirus Immune System
//
//	File Name:	IcePackAgentComm.h
//	Author:		Milosz Muszynski
//
//	This file defines a namespace, classes and functions constituting an interface
//	with the IcePack Agent communications module
//
//	$Log: $
//

#ifndef __ICEPACKAGENTCOMM__H__B63EB190_FF56_11d2_8A99_CEFDEB000000__
#define __ICEPACKAGENTCOMM__H__B63EB190_FF56_11d2_8A99_CEFDEB000000__

#ifdef ICEPACKAGENTCOMM_EXPORTS
#define ICEPACKAGENTCOMM_API __declspec(dllexport)
#else
#define ICEPACKAGENTCOMM_API __declspec(dllimport)
#endif

namespace IcePackAgentComm
{
	using namespace std;

	class Stream
	{
	protected:
		Stream(){}
		virtual ~Stream(){}
	
	public:
		virtual bool init		( /*[out]	*/	unsigned long& 	errorCode ) = 0;
		virtual bool read		( /*[in]	*/	unsigned char*	inBuf, 
								  /*[in,out]*/	unsigned long& 	size, 
								  /*[out]   */	unsigned long& 	errorCode ) = 0;
		virtual bool end		( /*[out]   */	unsigned long& 	errorCode ) = 0;
		virtual long size		() = 0;
    virtual char *start () = NULL;
	};


	enum SignatureType 	{ Regular, Latest, LatestBlessed };
	enum ErrorCode		{ NoError = 0, PackageNotFoundError, TempDirError, TargetDirError, 
						  TargetDOSDirError, FileCopyError, NoFilesFoundError, DiskFullError, VersionError, 
						  SystemException, LibraryException, ProcessCreationError, ProcessTimedOutError, ProcessTerminationError, 
						  ProcessSyncError, NoSignaturesAvailable, FileWriteError, FileReadError, FileOpenError, 
						  UnknownSignatureTypeError, MissingSignatureSequenceError, NetworkError, NoHttpCodeError,
						  NetworkCriticalError, GatewayError, NoContentError, FatAlbertSystemException, 
						  ThreadCouldNotStartError, OutOfMemoryError, TerminatedByCaller, 
						  VDBSystemException, VDBZipArchiveError, VDBMemoryError, VDBParameterError,
						  VDBUnexpectedEOFError, VDBUnknownError, YouveBeenRedirected, InvalidRedirectError, UnknownError };

	
//	This class provides communication functionality
//	dealing with signature sets. Downloading the signature set and inquiring
//	for its availablility is implemented.
//	SignatureComm objects maintain their own threads for communication
//  purposes. It is recommended to keep the object in existence
//  between method calls so that the same thread gets used multiple times
	class ICEPACKAGENTCOMM_API SignatureComm
	{
	public:
		SignatureComm();
		virtual ~SignatureComm();

		bool			Download		(	/*[in]	*/	const string&	gatewayURL	  			, 
                            /*[in]	*/	const std::string&	headers	  	  	, 
											/*[in]	*/	const string& 	sequenceNumber		  	, 
											/*[in]	*/	const string& 	directory				, 
											/*[out]	*/	string&	 		attributeStrings  		, 
											/*[in]	*/	SignatureType	signatureType = Regular	,
											/*[in]	*/	bool 			withUnpacking = false	);

#if 0 /* inw 2000-02-16 removed FatAlbert support */
		bool			Install			(	/*[in]	*/	const string&	packagePath				,
											/*[in]	*/	const string&	commandLine				,
											/*[in]	*/	int				timeout		  = 90		);
#endif /* 0 */

		bool			GetLatest		(	/*[in]	*/	const string&	gatewayURL	  			, 
                            /*[in]  */  const string& headers,
								  			/*[out]	*/	string& 		sequenceNumber		  	);

		bool			GetLatestBlessed(	/*[in]	*/	const string&	gatewayURL	  			, 
                                /*[in]  */  const string& headers,
										 	/*[out]	*/	string& 		sequenceNumber		  	);

		ErrorCode		Error				() const;
		unsigned long	SystemExceptionCode	() const;
		bool			SetTraceClient		(	MyTraceClient&	traceClient,
												const string&	logFileName,
												const string&	traceFileName,
												const string&	agentName		);


	private:
		ErrorCode						_errorCode;
		unsigned long					_systemExceptionCode;
		string							_logFileName;
		string							_traceFileName;
		string							_agentName;
		MyTraceClient *					_p_tc;			// no auto_ptr needed since we don't own the trace client
														// can't use reference because this pointer might change its value after
														// executing SetTraceClient
		void *							_si;
		void *							_sq;
		void *							_m;
	};


//	This class provides communication functionality
//	dealing with samples. Uploading the sample and inquiring
//	for its status is implemented.
//	SampleComm objects maintain their own threads for communication
//  purposes. It is recommended to keep the object in existence
//  between method calls so that the same thread gets used multiple times
	class ICEPACKAGENTCOMM_API SampleComm
	{
	public:
		SampleComm();
		virtual ~SampleComm();

		bool GetStatus( /*[in]		*/ const string& 	gatewayURL		,
                    /*[in]		*/ const string& 	headers		,
						/*[in]		*/ const string&	cookie			,
            /*[in]		*/ const string&	sampleChecksumSubmitted			,
						/*[out]		*/ string&			attributeStrings);

		bool Upload( 	/*[in]		*/ const string& 	gatewayURL		, 
						/*[in/out]	*/ string&			attributeStrings, 
						/*[in]		*/ Stream&  		stream			);

		ErrorCode		Error				() const;
		unsigned long	SystemExceptionCode	() const;
		bool			SetTraceClient		(	MyTraceClient&	traceClient,
												const string&	logFileName,
												const string&	traceFileName,
												const string&	agentName		);

	private:
		ErrorCode						_errorCode;
		unsigned long					_systemExceptionCode;
		string							_logFileName;
		string							_traceFileName;
		string							_agentName;
		MyTraceClient *					_p_tc;			// no auto_ptr needed since we don't own the trace client
														// can't use reference because this pointer might change its value after
														// executing SetTraceClient
		void *							_sr;
		void *							_su;
		void *							_m;
	};


	bool	ICEPACKAGENTCOMM_API SetCommParams	( 	
								const string&	firewallURL	  						= "", 
								const string&	proxyUserName						= "",
								const string&	proxyPassword						= "",
								int				connection_timeout					= 5,
								int				connection_retries					= 2,
								int				send_request_timeout				= 3,
								int				send_request_data_timeout			= 3,
								int				receive_response_timeout			= 3,
								int				receive_response_data_timeout		= 3,
								HANDLE			terminateEvent						= NULL,
								bool			ignore_SSL_name_mismatch			= false 	);

	bool	ICEPACKAGENTCOMM_API SetUnpackParams( 	
								int				timeout								= 90,
								const string&	temporaryDirectory					= "\\temp"	);

	void	ICEPACKAGENTCOMM_API GetErrorMessage( 
								ErrorCode		errorCode, 
								string&			msg );
};

#endif __ICEPACKAGENTCOMM__H__B63EB190_FF56_11d2_8A99_CEFDEB000000__
