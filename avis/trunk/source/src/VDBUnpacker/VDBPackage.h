//
//	IBM AntiVirus Immune System
//
//	File Name:	VDBPackage.h
//	Author:		Milosz Muszynski
//
//	VDB Package Extraction Utility Class
//
//	$Log:  $
//

#ifndef _VDBPACKAGE_H__621DC470_A806_11d3_8ADB_60D2C7000000
#define _VDBPACKAGE_H__621DC470_A806_11d3_8ADB_60D2C7000000

class VDBUNPACKER_API VDBPackage
{
public:
	VDBPackage();
	virtual ~VDBPackage(){}

	bool					Unpack		(	const std::string&	package, 
											const std::string&	targetDirectory,
											HANDLE				mutex = NULL,
											void *				traceClient = NULL );

	bool					Prune		(	const std::string&	package, 
											const std::string&	targetDirectory,
											HANDLE				mutex = NULL,
											void *				traceClient = NULL );

	enum					ErrorCode	{ 
											NoError = 0, PackageNotFoundError, 
											TargetDirError, NoFilesFoundError, DiskFullError, 
											VersionError, SystemException, ZipArchiveError, 
											MemoryError, ParameterError, ZipNoFilesFoundError, 
											UnexpectedEOFError, ZipUnknownError, 
											FileRemovingError, DirPruningError
										};

	// Error() returns NoError in case of successful unpacking
	virtual	ErrorCode		Error(void);
	// meaningful when Error() returns SystemException
	virtual	unsigned long	ExceptionCode();	

protected:
	ErrorCode				_errorCode;
	unsigned int			_systemExceptionCode;
};

#endif _VDBPACKAGE_H___621DC470_A806_11d3_8ADB_60D2C7000000
