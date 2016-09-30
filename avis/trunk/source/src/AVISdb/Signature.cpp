// Signiture.cpp: implementation of the Signiture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Signature.h"
#include "Internal.h"
#include "Globals.h"

#include "CheckSum.h"

using namespace std;

const AVISDBException::TypeOfException	Signature::exceptType =
													AVISDBException::etSignature;

//
//	Add a new signature record to the database
//	DateTime fields that are invalid will be inserted as nulls.

static void AddDateTime(string& str, DateTime& dateTime)
{
	if (!dateTime.IsNull())
	{
		string	dtAsStr;
		if (dateTime.AsDBString(dtAsStr))
		{
			str += "'";
			str += dtAsStr;
			str += "'";
		}
		else
			throw AVISDBException(AVISDBException::etSignature,
										"AddDateTime, AsDBString returned false");
	}
	else
		str += "NULL";
}

bool Signature::AddNew(uint sigSeqNum, DateTime& blessed,
						DateTime& produced, DateTime& published,
						string& checkSum)
{
	if (0 == sigSeqNum)
		throw AVISDBException(exceptType, "AddNew, 0 == sigSeqNum");

	bool		rc = false;
	string	dateTime;
	string	addSig("INSERT INTO Signatures VALUES ( ");
	char		seqNumAsChar[1024];
        memset(seqNumAsChar,0,1024);
	sprintf(seqNumAsChar, "%d", sigSeqNum);
	addSig += seqNumAsChar;
	addSig += " , ";
	AddDateTime(addSig, blessed);
	addSig += " , ";
	AddDateTime(addSig, produced);
	addSig += " , ";
	AddDateTime(addSig, published);
	addSig += " , '";
	CheckSum::ToLower(checkSum);
	addSig += checkSum;
	addSig += "' )";

	SQLStmt		sql(Internal::DefaultSession(), "AddNew", exceptType);
	if (sql.ExecDirect(addSig, true, false))
	{
		rc = true;
	}
	else if (SQLStmt::CausedDuplicate == sql.GetErrorType())
	{
		rc = false;
	}
	else
		sql.ThrowException("");

	return rc;
}

bool Signature::AddNewOrReplaceExisting(
						uint sigSeqNum, DateTime& blessed,
						DateTime& produced, DateTime& published,
						string& checkSum,
						bool& replacedOld)
{
	if (0 == sigSeqNum)
		throw AVISDBException(exceptType, "AddNewOrReplaceExisting, 0 == sigSeqNum");

	replacedOld	= false;
	if (!AddNew(sigSeqNum, blessed, produced, published, checkSum))
	{
		replacedOld = true;

		string	dateTime;
		string	addSig("UPDATE Signatures SET Blessed = ");
		AddDateTime(addSig, blessed);
		addSig += " , Produced = ";
		AddDateTime(addSig, produced);
		addSig += " , Published = ";
		AddDateTime(addSig, published);
		addSig += " , CheckSum = '";
		CheckSum::ToLower(checkSum);
		addSig += checkSum;
		addSig += "' WHERE SignatureSeqNum = ";
		char		seqNumAsChar[1024];
                memset(seqNumAsChar,0,1024);
		sprintf(seqNumAsChar, "%d", sigSeqNum);
		addSig += seqNumAsChar;

		SQLStmt		sql(Internal::DefaultSession(), "AddNewOrReplaceExisting", exceptType);
		sql.ExecDirect(addSig);
	}

	return true;
}

//
//	Get the latest/greatest signature (the most recently produced
//	signature record)
bool Signature::Latest(Signature& sig)
{
	return Globals::NewestSignature(sig);
}

//
//	Get the latest/greatest blessed signature (the most recently
//	blessed signature record)
bool Signature::LatestBlessed(Signature& sig)
{
	return Globals::NewestBlessedSignature(sig);
}


Signature::Signature() : null(true), seqNum(0)
{
	blessed.Null(true);
	produced.Null(true);
	published.Null(true);
}

//
//	Get a Signature object based on signature seq number
Signature::Signature(uint sigSeqNum) : null(true), seqNum(0)
{
	if (0 != sigSeqNum)
	{
		char		seqNumAsChar[1024];
                memset(seqNumAsChar,0,1024);
		sprintf(seqNumAsChar, "%d", sigSeqNum);
		string	select("SELECT SignatureSeqNum, Blessed, Produced, Published, CheckSum ");
		select += "FROM Signatures ";
		select += "WHERE SignatureSeqNum = ";
		select += seqNumAsChar;

		Get(select, *this, "Construct from sequence number");
	}
}

//
//	Deconstruct a signature object
Signature::~Signature()
{
	null	= true;
}


//
//	Modify methods
bool Signature::Blessed(DateTime& when)
{
	NullCheck("Blessed");

	string	update("UPDATE Signatures SET Blessed = ");
	AddDateTime(update, when);
	update += " WHERE SignatureSeqNum = ";
	char	seqNumAsChar[1024];
        memset(seqNumAsChar,0,1024);
	sprintf(seqNumAsChar, "%d", seqNum);
	update += seqNumAsChar;

	SQLStmt	sql(Internal::DefaultSession(), "Blessed", exceptType);
	sql.ExecDirect(update);

	return true;
}

bool Signature::Get(string& select, Signature& sig, const char* where)
{
	bool	rc	= false;

	sig.null	= true;
	sig.blessed.Null(true);
	sig.produced.Null(true);
	sig.published.Null(true);

	SQLINTEGER	sigNumRetLength, blessedRetLength, producedRetLength;
	SQLINTEGER	publishedRetLength, checkSumRetLength;
	uint		checkSumMaxLength	= 34;
	char		charCheckSum[34];

	SQLStmt		sql(Internal::DefaultSession(), where, exceptType);
	if (sql.ExecDirect(select)									&&
		sql.BindColumn(1, &sig.seqNum,	&sigNumRetLength)		&&
		sql.BindColumn(2, sig.blessed,	&blessedRetLength)		&&
		sql.BindColumn(3, sig.produced, &producedRetLength)		&&
		sql.BindColumn(4, sig.published,&publishedRetLength)	&&
		sql.BindColumn(5, charCheckSum, checkSumMaxLength, &checkSumRetLength)	&&
		sql.Fetch(true, false)									)
	{
		if (blessedRetLength > 0)
			sig.blessed.Null(false);
		if (producedRetLength > 0)
			sig.produced.Null(false);
		if (publishedRetLength > 0)
			sig.published.Null(false);

		charCheckSum[checkSumRetLength]	= '\0';
		sig.checkSum	= charCheckSum;

		sig.null = false;

		rc	= true;
	}
	else if (100 == sql.LastRetCode())
	{
		rc	= false;
	}
	else
		sql.ThrowException("");

	return rc;
}

string Signature::ToDebugString()	// returns a string representation of the
{										// object suitable for debugging messages
using namespace std;

	string	dbStr("Signature{\n");

	if (null)
	{
		dbStr	+= "Is Null";
	}
	else
	{
		char	asChar[1024];
		bool	okay;
                memset(asChar,0,1024);

		sprintf(asChar, "seqNum = %d\nblessed = ", seqNum);
		dbStr	+= asChar;
		if (blessed.IsNull())
			dbStr	+= "Null";
		else
			dbStr	+= blessed.AsHttpString(okay);
		dbStr	+= "\nproduced = ";
		if (produced.IsNull())
			dbStr	+= "Null";
		else
			dbStr	+= produced.AsHttpString(okay);
		dbStr	+= "\npublished = ";
		if (published.IsNull())
			dbStr	+= "Null";
		else
			dbStr	+= published.AsHttpString(okay);
		dbStr	+= "\ncheckSum = ";
		dbStr	+= checkSum;
	}

	dbStr	+= "}\n";


	return dbStr;
}
