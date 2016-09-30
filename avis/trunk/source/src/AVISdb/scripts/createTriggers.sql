CONNECT TO AVISDATA


--
--	AnalysisRequestTR1
--
--	This trigger updates the submittor usage counts.
--	The trigger on the submittor table will fail if the maximum
--	usage values are exceeded.

CREATE TRIGGER AnalysisRequestTR1										\
AFTER INSERT ON AnalysisRequest											\
REFERENCING NEW AS new													\
FOR EACH ROW MODE DB2SQL												\
BEGIN ATOMIC															\
	UPDATE Submittors SET UsageToday = UsageToday + 1					\
						WHERE SubmittorID = new.SubmittorID;			\ 
END


--
--	AnalysisRequestTR2
--
--	This trigger updates the NotifyClient, NotifySymantec, and LastState
--	Changecolumns whenever the AnalysisStateID field is updated.

CREATE TRIGGER AnalysisRequestTR2										\
AFTER UPDATE OF AnalysisStateID ON AnalysisRequest						\
REFERENCING NEW AS new	OLD AS old										\
FOR EACH ROW MODE DB2SQL												\
-- AnalysisStateId of 44 means 'imported' and 45 means 'rescan'.
-- AnalysisStateId of 30 means 'receiving'.
-- Trigger will be effective only for the following conditions.
-- 1) For a new analysis state id.
-- 2) For a non-zero imported date.
-- 3) For a new analysisstateid that is not equal to 45 ('rescan').
-- 4) For a new analysisstateid that is not equal to 30 ('receiving').
-- 5) For a new analysisstateid that is not moving from 45 ('rescan') to 44 ('imported').
WHEN (new.AnalysisStateID <> old.AnalysisStateID AND new.Imported IS NOT NULL AND new.AnalysisStateID <> 30 AND new.AnalysisStateID <> 45 AND NOT (old.AnalysisStateID = 45 AND new.AnalysisStateID = 44)) \
BEGIN ATOMIC \
--	UPDATE AnalysisRequest												\
-- We don't need to set the NotifyClient any more (Mar/21/2000).
--			SET NotifyClient	= X'01',								\
	UPDATE AnalysisRequest SET											\
				NotifySymantec 	= X'01',								\
				LastStateChange = CURRENT TIMESTAMP						\
			WHERE LocalCookie = new.LocalCookie AND Imported IS NOT NULL; \
-- Any new state change will add new rows for every active server to which we have to report status.
    INSERT INTO StatusToBeReported (ServerID, CheckSum) SELECT ServerID, new.CheckSum FROM Servers WHERE Active = 1; \
END


--
--	AnalysisRequestTR10
--
--	This trigger updates the platform usage counts.
--	The trigger on the platform table will fail if the maximum
--	usage values are exceeded.

--CREATE TRIGGER AnalysisRequstTR10										\
--AFTER INSERT ON AnalysisRequest											\
--REFERENCING NEW AS new													\
--FOR EACH ROW MODE DB2SQL												\
--BEGIN ATOMIC															\
--	UPDATE Platforms SET UsageToday = UsageToday + 1					\
--						WHERE HWCorrelator = new.HWCorrelator;			\ 
--END


-- AnalysisRequestTRA (TR10 breaks the 18 character DB2 symbol size
-- limit)
-- 
-- This trigger is responsible for making sure the "NextLocalCookie"
-- is always unique. (By changing the available value every time one
-- is inserted into the AnalysisRequest table

CREATE TRIGGER AnalysisRequestTRA                                 \
AFTER INSERT ON AnalysisRequest                                   \
FOR EACH ROW MODE DB2SQL                                          \
BEGIN ATOMIC                                                      \
      UPDATE GLOBALS SET NextLocalCookie = NextLocalCookie + 1;   \
END


--
--	RemoteMustBeUnique
--
--	This table exists exclusively to prevent duplicate
--	RemoteCookie/RemoteServerID pairs for existing in the AnalysisRequest
--	table.  Why not use a Unique constraint on the AnalysisRequest table
--	you ask?  Well that is not possible because both of these values can
--	also be NULL and DB2 does not allow keys to be both NULL and Unique.
--
--	My solution is to keep a table that only lists the non null values
--	stored in the AnalysisRequest table.  Through the use of triggers this
--	new table will be automatically updated when records are added, updated,
--	or deleted.


--
--	AnalysisRequestTR4
--
--	This trigger will insert the RemoteCookie/RemoteServerID pair into the 
--	RemoteMustBeUnique table when they are not null.

CREATE TRIGGER AnalysisRequestTR4										\
AFTER INSERT ON AnalysisRequest											\
REFERENCING NEW AS new													\
FOR EACH ROW MODE DB2SQL												\
WHEN (new.RemoteCookie IS NOT NULL AND new.RemoteServerID IS NOT NULL)	\
	INSERT INTO RemoteMustBeUnique VALUES (new.RemoteCookie, new.RemoteServerID)

--
--	AnalysisRequestTR5
--
--	This trigger will remove RemoteCookie/RemoteServerID pairs from the
--	RemoteMustBeUnique table when they are not null


CREATE TRIGGER AnalysisRequestTR5										\
AFTER UPDATE OF RemoteCookie, RemoteServerID ON AnalysisRequest			\
REFERENCING OLD AS old													\
FOR EACH ROW MODE DB2SQL												\
WHEN (old.RemoteCookie IS NOT NULL AND old.RemoteServerID IS NOT NULL)	\
	DELETE  FROM	RemoteMustBeUnique									\
			WHERE	RemoteCookie = old.RemoteCookie AND					\
					RemoteServerID = old.RemoteServerID

--
--	AnalysisRequestTR6
--
--	This trigger will add RemoteCookie/RemoteServerID pairs to the
--	RemoteMustBeUnique table when they are not null on update

CREATE TRIGGER AnalysisRequestTR6										\
AFTER UPDATE OF RemoteCookie, RemoteServerID ON AnalysisRequest			\
REFERENCING NEW AS new													\
			OLD AS old													\
FOR EACH ROW MODE DB2SQL												\
WHEN (new.RemoteCookie IS NOT NULL AND new.RemoteServerID IS NOT NULL AND	\
		(new.RemoteCookie <> old.RemoteCookie OR						\
			 new.RemoteServerID <> old.RemoteServerID)	)				\
	INSERT INTO RemoteMustBeUnique VALUES (new.RemoteCookie, new.RemoteServerID)


--
--	AnalysisRequestTR7
--
--	This trigger will remove RemoteCookie/RemoteServerID pairs from the 
--	RemoteMustBeUnique table when they are not null on deletetion


CREATE TRIGGER AnalysisRequestTR7										\
AFTER DELETE ON AnalysisRequest											\
REFERENCING OLD AS old													\
FOR EACH ROW MODE DB2SQL												\
WHEN (old.RemoteCookie IS NOT NULL AND old.RemoteServerID IS NOT NULL)	\
	DELETE FROM		RemoteMustBeUnique									\
		   WHERE	RemoteCookie = old.RemoteCookie AND					\
					RemoteServerID = old.RemoteServerID


--
--	AnalysisRequestTR8
--
--	This trigger will ensure that RemoteCookie and RemoteServerID are
--	either both NULL or both NOT NULL on insertion

CREATE TRIGGER AnalysisRequestTR8										\
AFTER INSERT ON AnalysisRequest											\
REFERENCING NEW AS new													\
FOR EACH ROW MODE DB2SQL												\
WHEN ((new.RemoteCookie IS NULL AND new.RemoteServerID IS NOT NULL) OR	\
	  (new.RemoteCookie IS NOT NULL AND new.RemoteServerID IS NULL))	\
	SIGNAL SQLSTATE '75000' (											\
	'RemoteCookie and RemoteServerID must be either both NULL or both NOT NULL' )

--
--	AnalysisRequestTR9
--
--	This trigger will ensure that RemoteCookie and RemoteServerID are
--	either both NULL or both NOT NULL on update

CREATE TRIGGER AnalysisRequestTR9										\
AFTER UPDATE OF RemoteCookie, RemoteServerID ON AnalysisRequest			\
REFERENCING NEW AS new													\
FOR EACH ROW MODE DB2SQL												\
WHEN ((new.RemoteCookie IS NULL AND new.RemoteServerID IS NOT NULL) OR	\
	  (new.RemoteCookie IS NOT NULL AND new.RemoteServerID IS NULL))	\
	SIGNAL SQLSTATE '75000' (											\
	'RemoteCookie and RemoteServerID must be either both NULL or both NOT NULL' )

--
--	SignaturesTR1
--
--	Update Globals::NewestSeqNum if > Globals::NewestSeqNum
--	Update Globals::NewestBlssdSeqNum if > Globals::NewestBlssdSeqNum

CREATE TRIGGER SignaturesTR1a											\
AFTER INSERT ON Signatures												\
REFERENCING NEW AS new													\
FOR EACH ROW MODE DB2SQL												\
BEGIN ATOMIC															\
	UPDATE Globals SET NewestSeqNum = new.SignatureSeqNum				\
		WHERE NewestSeqNum < new.SignatureSeqNum;						\
--	UPDATE Globals SET NewestBlssdSeqNum = new.SignatureSeqNum			\
--		WHERE new.Blessed IS NOT NULL AND NewestBlssdSeqNum < new.SignatureSeqNum;	\
END


CREATE TRIGGER SignaturesTR1b											\
AFTER INSERT ON Signatures												\
REFERENCING NEW AS new													\
FOR EACH ROW MODE DB2SQL												\
BEGIN ATOMIC															\
--	UPDATE Globals SET NewestSeqNum = new.SignatureSeqNum				\
--		WHERE NewestSeqNum < new.SignatureSeqNum;						\
	UPDATE Globals SET NewestBlssdSeqNum = new.SignatureSeqNum			\
		WHERE new.Blessed IS NOT NULL AND NewestBlssdSeqNum < new.SignatureSeqNum;	\
END

--
--	SignaturesTR2
--
--	Update Globals::NewestBlssdSeqNum if Blessed == true and if > Globals::NewestBlssdSeqNum
--	Update Globals::NewestBlssdSeqNum if Blessed was true and is being set to false

CREATE TRIGGER SignaturesTR2											\
AFTER UPDATE OF Blessed ON Signatures									\
REFERENCING NEW AS new													\
			OLD AS old													\
FOR EACH ROW MODE DB2SQL												\
BEGIN ATOMIC															\
	UPDATE Globals SET NewestBlssdSeqNum = new.SignatureSeqNum			\
		WHERE new.Blessed IS NOT NULL AND NewestBlssdSeqNum < new.SignatureSeqNum;	\
	UPDATE Globals SET NewestBlssdSeqNum =								\
			(SELECT MAX(SignatureSeqNum) FROM Signatures				\
			 WHERE Blessed IS NOT NULL )								\
		WHERE old.Blessed IS NOT NULL AND new.Blessed IS NULL;			\
END


--
--	NOTE:	This tracking solution will only work if submittors use the
--			same Server every time.

--
--	SubmittorsTR1
--
--	This trigger increases the size of the usage counts.

CREATE TRIGGER SubmittorsTR1											\
AFTER UPDATE OF UsageToday On Submittors								\
REFERENCING NEW AS new	OLD AS old										\
FOR EACH ROW MODE DB2SQL												\
BEGIN ATOMIC															\
	UPDATE Submittors													\
			SET UsageThisWeek = (old.UsageThisWeek	+ new.UsageToday - old.UsageToday),	\
				UsageThisMonth= (old.UsageThisMonth	+ new.UsageToday - old.UsageToday),	\
				UsageThisYear = (old.UsageThisYear	+ new.UsageToday - old.UsageToday)	\
	WHERE SubmittorID = new.SubmittorID;								\
END


--
--	PlatformsTR1
--
--	This trigger increases the size of the usage counts.

--CREATE TRIGGER PlatformsTR1												\
--AFTER UPDATE OF UsageToday On Platforms									\
--REFERENCING NEW AS new	OLD AS old										\
--FOR EACH ROW MODE DB2SQL												\
--BEGIN ATOMIC															\
--	UPDATE Platforms													\
--			SET UsageThisWeek = (old.UsageThisWeek	+ new.UsageToday - old.UsageToday),	\
--				UsageThisMonth= (old.UsageThisMonth	+ new.UsageToday - old.UsageToday),	\
--				UsageThisYear = (old.UsageThisYear	+ new.UsageToday - old.UsageToday)	\
--	WHERE HWCorrelator = new.HWCorrelator;								\
--END


CREATE TRIGGER GlobalsTR1												\
AFTER INSERT ON Globals													\
FOR EACH ROW MODE DB2SQL												\
	SIGNAL SQLSTATE '75000' ( 'Only one row is allowed in the Globals table ' )



CREATE TRIGGER AttributeKeysTR1											\
AFTER DELETE ON AttributeKeys											\
FOR EACH ROW MODE DB2SQL												\
	SIGNAL SQLSTATE '75000' ( 'AttributeKeys records cannot be deleted' )



CREATE TRIGGER AnalysisStInfTR1										\
AFTER DELETE ON AnalysisStateInfo										\
FOR EACH ROW MODE DB2SQL												\
	SIGNAL SQLSTATE '75000' ( 'AnalysisStateInfo records cannot be deleted' )



CREATE TRIGGER GlobalsTR2												\
AFTER DELETE ON Globals													\
FOR EACH ROW MODE DB2SQL												\
	SIGNAL SQLSTATE '75000' ( 'Globals record cannot be deleted' )



CREATE TRIGGER ServersTR1												\
AFTER DELETE ON Servers												\
FOR EACH ROW MODE DB2SQL												\
	SIGNAL SQLSTATE '75000' ( 'Servers records cannot be deleted' )



CREATE TRIGGER AnalysisRequestTR3										\
AFTER UPDATE OF LocalCookie, CheckSum, SubmittorID ON AnalysisRequest	\
FOR EACH ROW MODE DB2SQL												\
	SIGNAL SQLSTATE '75000' (											\
	'LocalCookie, CheckSum, and SubmittorID fields in the AnalysisRequest table are not changable' )

--CREATE TRIGGER AttributesTR1											\
--AFTER UPDATE OF LocalCookie, AttributeKey ON Attributes					\
--FOR EACH ROW MODE DB2SQL												\
--	SIGNAL SQLSTATE '75000' (											\
--	'LocalCookie and AttributeKey fields in the Attributes table are not changable' )


CREATE TRIGGER AttributeKeysTR2											\
AFTER UPDATE OF AttributeKey, Text ON AttributeKeys						\
FOR EACH ROW MODE DB2SQL												\
	SIGNAL SQLSTATE '75000' (											\
	'AttributeKey and Text fields in the AttributeKeys table are not changable' )

CREATE TRIGGER AnalysisStInfTR2											\
AFTER UPDATE OF AnalysisStateID, Name ON AnalysisStateInfo				\
FOR EACH ROW MODE DB2SQL												\
	SIGNAL SQLSTATE '75000' (											\
	'AnalysisStateID and Name fields in the AnalysisStateInfo table are not changable' )

CREATE TRIGGER ServersTR2												\
AFTER UPDATE OF ServerID ON Servers										\
FOR EACH ROW MODE DB2SQL												\
	SIGNAL SQLSTATE '75000' (											\
	'ServerID field in the Servers table is not changable' )


CREATE TRIGGER SubmittorsTR3											\
AFTER UPDATE OF SubmittorID ON Submittors								\
FOR EACH ROW MODE DB2SQL												\
	SIGNAL SQLSTATE '75000' (											\
	'SubmittorID field in the Submittors table is not changable' )

--CREATE TRIGGER PlatformsTR3												\
--AFTER UPDATE OF HWCorrelator ON Platforms								\
--FOR EACH ROW MODE DB2SQL												\
--	SIGNAL SQLSTATE '75000' (											\
--	'HWCorrelator field in the Platforms table is not changable' )


CREATE TRIGGER AnalysisResultsTR3										\
AFTER UPDATE OF CheckSum ON AnalysisResults								\
FOR EACH ROW MODE DB2SQL												\
	SIGNAL SQLSTATE '75000' (											\
	'CheckSum field in the AnalysisResults table is not changable' )


DISCONNECT ALL
