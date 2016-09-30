 //
//	IBM AntiVirus Immune System
//
//	File Name:	DBSession.h
//	Author:		Andy Klapper
//
//	This class represents a connection to the AntiVirus
//	Immune System database.  Each thread must have a separate
//	connection to the database.  In the future this class
//	could hide the details of some kind of permanent connection
//	sharing mechanism between threads.  But it is best to
//	pretend that this is a thread's connection to the database
//	and treat it accordingly.
//
//	$Log:  $
//

#ifndef DBSESSION_H
#define DBSESSION_H

#include "avisdbdll.h"
#include "avis.h"
#include "AVISDBException.h"

#include "DBService.h"


class AVISDB_API DBSession
{
public:
  DBSession(void);
  ~DBSession(void);

  SQLHANDLE	ConnectionHandle()
  {	SessionCheck("ConnectionHandle");	return sqlSession; };

  bool	Valid(void) { SessionCheck("Valid");	return !badSession; };
  bool    GetErrorMessage(std::string& errorMessage);


private:
  SQLHANDLE	sqlSession;
  bool		badSession;

  void		SessionCheck(const char* where)
  {
    if (badSession)
      {
        std::string	msg(where);
        msg	+= ", badSession";

        throw AVISDBException(exceptType, msg);
      }
  }
  static const AVISDBException::TypeOfException	exceptType;
};

#endif
