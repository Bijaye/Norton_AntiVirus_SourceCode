// SampleStream.cpp: implementation of the SampleStream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SampleStream.h"
#include "GlobalData.h"

#include <AttributeKeys.h>
#include <ToolFactory.h>
//#include <platform.h>

typedef LPVOID FAR      *LPLPVOID;
#include <StripDoc.h>

#include "MappedMemCallback.h"

#include "Logger.h"


using namespace std;


//
//	Callbacks for the OLE Document stripping api

BOOL	CBMemoryAlloc(LPVOID lpvExtraParam, DWORD dwNumBytes, LPLPVOID lplpvBuffer)
{
	*lplpvBuffer = new char[dwNumBytes];

	return NULL != *lplpvBuffer;
}

BOOL	CBMemoryFree(LPVOID lpExtraParam, LPVOID lpBuffer)
{
	delete [] lpBuffer;

	return true;
}

BOOL	CBFileOpen(LPVOID lpvExtraParam, LPVOID lpvFileName,
				 ESTRIP_DOC_OPEN_T eOpenMode, LPLPVOID lplpvFile)
{
	*lplpvFile = lpvExtraParam;
	return TRUE;
}

BOOL	CBFileClose(LPVOID lpvExtraParam, LPVOID lpvFile)
{
	return TRUE;
}

static MappedMemCallback::Whence DocSeekToWhence(ESTRIP_DOC_SEEK_T eWhence)
{
	if (eStripDocSeekSet == eWhence)
		return MappedMemCallback::mmcbBegining;
	else if (eStripDocSeekCur == eWhence)
		return MappedMemCallback::mmcbCurrent;
	else
		return MappedMemCallback::mmcbEnd;
}

BOOL	CBFileSeek(LPVOID lpvExtraParam, LPVOID lpvFile, long lOffset,
				 ESTRIP_DOC_SEEK_T eWhence, LPDWORD lpdwNewOffset)
{
	MappedMemCallback*	callback = static_cast<MappedMemCallback *>(lpvFile);

	return callback->Seek(lOffset, DocSeekToWhence(eWhence), *lpdwNewOffset);
}

BOOL	CBFileRead(LPVOID lpvExtraParam, LPVOID lpvFile, LPVOID lpvBuffer,
				 DWORD dwNumBytes, LPDWORD lpdwBytesRead)
{
	MappedMemCallback*	callback = static_cast<MappedMemCallback *>(lpvFile);

	return callback->Read(lpvBuffer, dwNumBytes, *lpdwBytesRead);
}

BOOL	CBFileWrite(LPVOID lpvExtraParam, LPVOID lpvFile, LPVOID lpvBuffer,
				  DWORD dwNumBytes, LPDWORD lpdwBytesWritten)
{
	MappedMemCallback*	callback = static_cast<MappedMemCallback *>(lpvFile);

	return callback->Write(lpvBuffer, dwNumBytes, *lpdwBytesWritten);
}

BOOL	CBFileTruncate(LPVOID lpvExtraParam, LPVOID lpvFile)
{
	MappedMemCallback*	callback = static_cast<MappedMemCallback *>(lpvFile);

	return callback->Truncate();
}

BOOL	CBProgress(LPVOID lpvExtraParam, int nPercent)
{
	return TRUE;
}


static bool OLEFile(const char* contents)
{
	return 0 == strncmp("\xD0\xCF\x11\xE0\xA1\xB1\x1A\xE1", contents, 8);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


SampleStream::~SampleStream()
{
	if (NULL != contents)
		delete [] contents;
}

static bool Scramble(Sample& sample, char *contents, uint& contEnd)
{
	EntryExit		entryExit(Logger::LogIcePackOther, "SampleStream..Scramble");

	SimpleScrambler	scrambler;
	char			buffer[SampleStream::BufferSize];

	sample.SetAttribute(AttributeKeys::ContentConcealment(), scrambler.Name());

	uint	outPos	= 0;
	uint	inPos	= 0;
	uint	len;
	uint	inLen, outLen;

	while (outPos < contEnd)
	{
		if (contEnd - outPos > SampleStream::BufferSize)
			len	= inLen = SampleStream::BufferSize;
		else
			len = inLen = contEnd - outPos;

		memcpy(buffer, &contents[outPos], inLen);
		outLen = contEnd - inPos;
		while (inLen > 0 &&
				scrambler.Process((const uchar *) buffer, inLen,
									(uchar *) &contents[inPos], outLen)	&&
				outLen > 0)
		{
			inPos	+= outLen;
			outPos	+= len;
		}
	}

	outLen = contEnd - inPos;
	while (scrambler.Done((uchar *) &contents[inPos], outLen))
	{
		inPos	+= outLen;
		outLen	= contEnd - inPos;
	}

	return true;
}

static bool Compress(Sample& sample, char *contents, uint& contEnd, uint& contSize)
{
	EntryExit	enterExit(Logger::LogIcePackOther, "SampleStream..Compress");

	Deflator	deflator;
	char		buffer[SampleStream::BufferSize];

	uint	outPos	= 0;
	uint	inPos	= 0;
	uint	len;
	uint	inLen, outLen;
	bool	okay	= true;

	while (okay && outPos < contEnd)
	{
		if (contEnd - outPos > SampleStream::BufferSize)
			len	= inLen = SampleStream::BufferSize;
		else
			len = inLen = contEnd - outPos;

		memcpy(buffer, &contents[outPos], inLen);
		outLen = contEnd - inPos;
		okay = deflator.Process((const uchar *) buffer, inLen,
									(uchar *) &contents[inPos], outLen);
		if (okay)
		{
			inPos	+= outLen;
			outPos	+= len;
		}
	}

	if (!deflator.Error())
	{
		outLen = contSize - inPos;
		while (deflator.Done((uchar *) &contents[inPos], outLen))
		{
			inPos	+= outLen;
			outLen	= contSize - inPos;
		}
		if (deflator.Error())
		{
			char	msg[80];
			sprintf(msg, "Constructor, deflator Done failed with error %d",
							deflator.Error());
			throw IcePackException(IcePackException::SampleStream, msg);
		}
		else if (inPos > contEnd)
		{
			Logger::Log(Logger::LogWarning, Logger::LogIcePackOther,
						"Sample compression resulted in a file larger than the orginal");

			return false;
		}
		else
		{
			contEnd = inPos;
			sample.SetAttribute(AttributeKeys::ContentCompression(), deflator.Name());
		}
	}
	else
	{
		char	msg[80];
		sprintf(msg, "Constructor, deflator Process failed with error %d",
						deflator.Error());
		throw IcePackException(IcePackException::SampleStream, msg);
	}

	return true;
}

bool SampleStream::Strip()
{
	static STRIP_DOC_CB_T	cb = { &CBMemoryAlloc, &CBMemoryFree,
									&CBFileOpen, &CBFileClose, &CBFileSeek, &CBFileRead,
									&CBFileWrite, &CBFileTruncate, &CBProgress };

	static	uint	stripCount	= 0;

	Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, "Stripping non-macro data from sample");

	stripped	= true;

	DWORD	newEnd;
	DWORD	dummy;
	bool	error	= true;
	char	tempName[32];
	sprintf(tempName, "oleStripTemp%d", ++stripCount);
	if (stripCount >= 0xfffffff0)
		stripCount	= 0;

	MappedMemCallback	mem(tempName, contEnd);

	mem.Write(contents, contEnd, newEnd);
	mem.Truncate();
	mem.Seek(0, MappedMemCallback::mmcbBegining, dummy);

	string				errMsg("Stripping user data from document failed, [");
	ESTRIP_DOC_ERR_T	rc = StripDocument(&cb, &mem, &mem);
	switch (rc)
	{
	case eStripDocErrNone:
		error	= false;
		contEnd	= mem.FileLength();
		mem.Seek(0, MappedMemCallback::mmcbBegining, dummy);
		mem.Read(contents, contEnd, dummy);
		sampleSize = contEnd = mem.FileLength();
		sample.SetAttribute(AttributeKeys::ContentStrip(), "By IcePack");
/*
{
static uint fileCount = 0;
HANDLE	fileHandle;
DWORD	bytesWritten;
char	sampleName[80];
++fileCount;
sprintf(sampleName, "c:\\temp\\sample%02d.stripped", fileCount);

fileHandle = CreateFile(sampleName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, 0);
WriteFile(fileHandle, contents, contEnd, &bytesWritten, NULL);
CloseHandle(fileHandle);
}
*/
		break;

	case	eStripDocErrDetermineFileType:
		errMsg	+= "encountered an error in the attempt to determine whether the file was an OLE file]";
		break;

	case eStripDocErrNotOLEFile:
		errMsg	+= "does not think that an ole file is an ole file]";
		break;

	case eStripDocErrGeneral:
		errMsg	+= "indicated a general error.  No specific information is available.]";
		break;

	case eStripDocErrCreateCopy:
		errMsg	+= "returned eStripDocErrCreateCopy, should not be possible.]";
		break;

	case eStripDocErrOpenOutputFile:
		errMsg	+= "returned eStripDocErrOpenOutputFile, should not be possible.]";
		break;

	case eStripDocErrCloseOutputFile:
		errMsg	+= "returned eStripDocErrCloseOutputFile, should not be possible.]";
		break;

	case eStripDocErrMalloc:
		errMsg	+= "allocating memory]";
		break;

	case eStripDocErrPasswordEncrypted:
		errMsg	+= "document data is password-protected/encrypted and the decryption key could not be determined]";
		break;

	case eStripDocErrStripExcel95:
		errMsg	+= "attempting to strip Excel 97 data.  No specific information is available.  The file may be corrupt.]";
		break;

	case eStripDocErrStripWord95:
		errMsg	+= "attempting to strip Word 95 data.  No specific information is available.  The file may be corrupt.]";
		break;

	case eStripDocErrStripWord97:
		errMsg	+= "attempting to strip Word 97 data.  No specific information is available.  The file may be corrupt.]";
		break;

	case eStripDocErrProgressFalse:
		errMsg	+= "finished prematurely because the Progress callback returned false.  This shold not be possible.]";
		break;

	default:
		errMsg	+= "unknown error code]";
	}

	if (error)
	{
		Logger::Log(Logger::LogError, Logger::LogIcePackOther, errMsg.c_str());
		sample.SetAttribute(AttributeKeys::Attention(), errMsg);
		sample.Status(SampleStatus::error);

		throw IcePackException(IcePackException::SampleStream,
								"Constructor, OLE stripping failed.");
	}

	return !error;
}

SampleStream::SampleStream(Sample& samp) : sample(samp), stripped(false),
											compressed(false), scrambled(false),
											sampleSize(Size(sample)),
											contSize(sampleSize + 1024), contEnd(0),
											readPos(0), contents(new char[contSize])
{
	EntryExit	entryExit(Logger::LogIcePackOther, "SampleStream::SampleStream");


	ulong	end;
	sample.Read(contSize, end, contents);
	sample.Close();
	contEnd	= end;
/*
{
static uint fileCount = 0;
HANDLE	fileHandle;
DWORD	bytesWritten;
char	sampleName[80];
++fileCount;
sprintf(sampleName, "c:\\temp\\sample%02d", fileCount);

fileHandle = CreateFile(sampleName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, 0);
WriteFile(fileHandle, contents, contEnd, &bytesWritten, NULL);
CloseHandle(fileHandle);
}
*/
	if (GlobalData::StripUserData() && OLEFile(contents))
	{
		stripped = Strip();
	}

  /*
   * Calculate the post-stripping checksum.
   */
	md5Sample.CRCCalculate((const uchar *) contents, contEnd);
	md5Sample.Done(md5SampleResult);

  /*
   * Store the post-stripping method and checksum.
   */
	sample.SetAttribute(AttributeKeys::CheckSumMethod(), md5Sample.Name());
  sample.SetAttribute(AttributeKeys::SampleChecksumSubmitted(), md5SampleResult.c_str());

	if (GlobalData::CompressContent())
	{
		Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, "Compressing sample");
		compressed = Compress(sample, contents, contEnd, contSize);
		if (!compressed)
		{
			Logger::Log(Logger::LogWarning, Logger::LogIcePackOther,
							"Compressing failed or resulted in a file larger than the original");
		
			sample.Open();
			sample.Read(contSize, end, contents);
			sample.Close();
			contEnd	= end;
			if (stripped)
				stripped = Strip();
		}
	}

	if (GlobalData::ScrambleContent())
	{
		Logger::Log(Logger::LogInfo, Logger::LogIcePackOther, "Scramble sample");
		scrambled = Scramble(sample, contents, contEnd);
	}

	string		crc;
	md5Content.CRCCalculate((const uchar *) contents, contEnd);
	md5Content.Done(crc);
	sample.SetAttribute(AttributeKeys::ContentCheckSum(), crc.c_str());
	char	asChar[20];
	sprintf(asChar, "%d", contEnd);
	sample.SetAttribute(AttributeKeys::ContentLength(), asChar);

	sample.Commit();
	sample.Reset();
}


uint SampleStream::Size(Sample& samp)
{
	ulong	size	= 0;

	if (samp.Open())
		samp.Size(size);

	return size;
}
