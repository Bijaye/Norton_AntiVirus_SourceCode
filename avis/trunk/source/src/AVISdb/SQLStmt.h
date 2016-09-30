// 
//	IBM AntiVirus Immune System
//
//	File Name:	SQLStmt.h
//	Author:		Andy Klapper
//
//	This class represents an SQL statment.
//
//	$Log: /AVISdb/AVISdb/SQLStmt.h $
//	
//	1     8/19/98 9:10p Andytk
//	Initial check in.
//	
//

#if !defined(AFX_SQLSTMT_H__0E9911E8_361C_11D2_88A8_00A0C9DB9E9C__INCLUDED_)
#define AFX_SQLSTMT_H__0E9911E8_361C_11D2_88A8_00A0C9DB9E9C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "avisdbdll.h"
#include "avis.h"
#include "DBSession.h"
#include "DateTime.h"
#include "AVISDBException.h"

typedef unsigned int	uint;

class AVISDB_API SQLStmt  
{
public:
  explicit SQLStmt(DBSession& session, const char* where, AVISDBException::TypeOfException eType);

  virtual ~SQLStmt();
  bool	Valid()		{ return validHandle; };
  bool	Prepared()	{ return validPrep; };

  enum RetType{ Error = 0, Okay = 1, Warning = 2 };

  enum IOType { Input = SQL_PARAM_INPUT,
                Output= SQL_PARAM_OUTPUT,
                InputAndOutput = SQL_PARAM_INPUT_OUTPUT };

  enum ErrorType { UnknownDBError, NoError, CausedDuplicate };

  RetType	ExecDirect(std::string& stmt, bool logWarnings = true, bool throwExceptions = true);
  RetType Prepare(std::string& stmt, bool logWarnings = true, bool throwExceptions = true);

  RetType	BindParameter(uint paramIndex, IOType ioType, short* arg);
  RetType	BindParameter(uint paramIndex, IOType ioType, ushort* arg);
  RetType	BindParameter(uint paramIndex, IOType ioType, int* arg);
  RetType	BindParameter(uint paramIndex, IOType ioType, uint* arg);
  RetType	BindParameter(uint paramIndex, IOType ioType, char* arg, uint byteLength);
  RetType	BindParameter(uint paramIndex, IOType ioType, char* arg);
  RetType	BindParameter(uint paramIndex, IOType ioType, DateTime& dateTime);
  //RetType	BindParameter(uint paramIndex, IOType ioType, std::string& string);

  RetType	BindColumn(uint column, short*	arg, SQLINTEGER* retLength);
  RetType	BindColumn(uint column, ushort*	arg, SQLINTEGER* retLength);
  RetType	BindColumn(uint column, int*	arg, SQLINTEGER* retLength);
  RetType	BindColumn(uint column, uint*	arg, SQLINTEGER* retLength);
  RetType	BindColumn(uint column, char*	arg, uint byteLength, SQLINTEGER* retLength);
  RetType	BindColumn(uint column, char*	arg, SQLINTEGER* retLength);
  RetType	BindColumn(uint column, DateTime&	arg, SQLINTEGER* retLength);
  //	bool	BindColumn(uint column, std::string&	arg, SQLINTEGER* retLength);


  RetType	Execute(bool logWarnings = true, bool throwExceptions = true);
  RetType	Fetch(bool logWarnings = true, bool throwExceptions = true);

  RetType	FreeCursor(bool logWarnings = true, bool throwExceptions = true);

  RETCODE		LastRetCode(void)	{ return lastRetCode; }
  bool		GetErrorMessage(std::string& error);
  ErrorType	GetErrorType(void);

  void		ThrowException(const char *prefix)
  { throw AVISDBException(exceptType, PackageMsg(prefix)); }


  //
  //	This method changes the input string if it contains characters
  //	like ' that would cause problems if they were passed as a 
  //	string to be executed or parsed by the database.  DO NOT call this
  //	method on strings that are passed as parameters to the database!
  static bool	PrepString(std::string& str);
  /*
    static bool ProcessInit();
    static bool ProcessExit();
    static bool	ThreadInit();
    static bool ThreadExit();
  */

private:
  bool		validHandle;
  bool		validPrep;
  SQLHANDLE	sqlStatement;
  RETCODE		lastRetCode;

  // debugging/tracing attributes
  std::string							stmtAsStr;
  std::string							where;
  AVISDBException::TypeOfException	exceptType;


  //	static	RETCODE		*RetCode();

  RetType		RetCodeToRetType()
  { if (SQL_SUCCESS == lastRetCode)					return Okay;
  else if (SQL_SUCCESS_WITH_INFO == lastRetCode)	return Warning;
  else												return Error;
  }

	
  void		HandleCheck(const char* where)
  {
    if (!validHandle)
      {
        std::string	msg(where);
        msg	+= ", handle is invalid";

        throw AVISDBException(sqlExceptType, msg);
      }
  }

  void		PrepCheck(const char* where)
  {
    if (!validPrep)
      {
        std::string	msg(where);
        msg	+= ", statement not prepared";

        throw AVISDBException(sqlExceptType, msg);
      }
  }

  std::string	PackageMsg(const char* prefix)
  {
    char			buff[32];
    std::string		details, msg(prefix);
    msg	+= " ( ";
    msg	+= where;
    msg	+= " ) rc = ";
    sprintf(buff, "%d", lastRetCode);
    msg	+= buff;
    msg	+= " [ ";
		
    GetErrorMessage(details);

    msg	+= details;
    msg	+= " ] { ";
    msg	+= stmtAsStr;
    msg	+= " }";

    return msg;
  }

  static const AVISDBException::TypeOfException	sqlExceptType;
};

#endif // !defined(AFX_SQLSTMT_H__0E9911E8_361C_11D2_88A8_00A0C9DB9E9C__INCLUDED_)
