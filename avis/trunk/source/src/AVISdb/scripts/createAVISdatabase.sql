--
-- createAVISdatabase.sql
--
-- Author:	Andy Klapper
-- Date:	8/14/98
-- 
-- Summary:	This file creates a new AVIS Database.
--
--	NOTE:	To run this script open up a db2 command window and type the
--			following on the command line
--			db2 -f createAVISdatabase.sql
--
--	NOTE:	This script deletes any database called AVISData.  Only run this
--			script if you are sure this is what you want to.
--
-- $Log: /AVISdb/ImmuneSystem/Database scripts/createAVISdatabase.sql $
--	
--	2     8/24/98 6:43p Andytk
--	Finished Cleaning up database.  Added cascading deletes and triggers.
--
--	1     8/19/98 9:11p Andytk
--	Initial check in
--
--


START DATABASE MANAGER
DROP DATABASE AVISData
CREATE DATABASE AVISData
CONNECT TO AVISDATA

GRANT CONNECT ON DATABASE TO PUBLIC

CREATE TABLE AttributeKeys								\
(														\
	AttributeKey	SMALLINT	NOT NULL PRIMARY KEY,	\
	Text			VARCHAR(32)	NOT NULL UNIQUE			\
)
--	DontPrune		SMALLINT	NOT NULL				\



CREATE TABLE AnalysisStateInfo							\
(														\
	AnalysisStateID	SMALLINT	NOT NULL PRIMARY KEY,	\
	Name			VARCHAR(32)	NOT NULL UNIQUE			\
)
--CREATE TABLE SampleTypeInfo							\
--(														\
--	SampleType	SMALLINT	NOT NULL PRIMARY KEY,		\		
--	USEnglish	VARCHAR(32)	NOT NULL UNIQUE,	\
--)
--INSERT INTO SampleTypeInfo VALUES ( 0, 'UnknownSampleType' )
--INSERT INTO SampleTypeInfo VALUES ( 1, 'Executable' )
--INSERT INTO SampleTypeInfo VALUES ( 2, 'Macro' )
--INSERT INTO SampleTypeInfo VALUES ( 3, 'Boot' )
--INSERT INTO SampleTypeInfo VALUES ( 4, 'Memory' )


--CREATE TABLE MinuteStats												\
--(																		\
--	SequenceNum		INTEGER		NOT NULL PRIMARY KEY ON DELETE CASCADE,	\
--	When			TIMESTAMP	NOT NULL,								\
--	TestRequests	SMALLINT	NOT NULL DEFAULT 0,						\
--	TestResolved	SMALLINT	NOT NULL DEFAULT 0,						\
--	StatusRequests	SMALLINT	NOT NULL DEFAULT 0,						\
--	SigRequests		SMALLINT	NOT NULL DEFAULT 0,						\
--	AveFileSize		INTEGER		NOT NULL DEFAULT 0,						\
--	AveCPUUser		SMALLINT	NOT NULL,								\
--	AveCPUSystem	SMALLINT	NOT NULL,								\
--	FreeMemory		SMALLINT	NOT NULL,								\
--	PageSwaps		INTEGER		NOT NULL,								\
--	FreeDiskSpace	INTEGER		NOT NULL								\
--)

--CREATE TABLE StateCounts												\
--(																		\
--	SequenceNum	INTEGER		NOT NULL REFERENCES MinuteStats(SequenceNum),\
--	AnalysisState SMALLINT	NOT NULL REFERENCES AnalysisStateInfo(AnalysisState),\
--	Count		SMALLINT	NOT NULL,									\
--	PRIMARY KEY(SequenceNum, AnalysisState)								\
--)

--CREATE TABLE SampleTypeCounts											\
--(																		\
--	SequenceNum	INTEGER		NOT NULL,									\
--	SampleType	SMALLINT	NOT NULL REFERENCES SampleTypeInfo(SampleType),	\
--	Count		SMALLINT	NOT NULL DEFAULT 1,							\
--	PRIMARY KEY (SequenceNum, SampleType)								\
--)

CREATE TABLE Servers													\
(																		\
	ServerID	SMALLINT	NOT NULL PRIMARY KEY,						\
--	NickName	VARCHAR(16)	NOT NULL UNIQUE,							\
	URL			VARCHAR(128)	NOT NULL UNIQUE,						\
	Active		SMALLINT	NOT NULL DEFAULT 1							\
)


CREATE TABLE Submittors													\
(																		\
	SubmittorID		VARCHAR(64)	NOT NULL PRIMARY KEY,					\
	UsageToday		INTEGER		NOT NULL DEFAULT 1,						\
	UsageThisWeek	INTEGER		NOT NULL DEFAULT 1,						\
	UsageThisMonth	INTEGER		NOT NULL DEFAULT 1,						\
	UsageThisYear	INTEGER		NOT NULL DEFAULT 1						\
)
--	Name			VARCHAR(64)	NOT NULL,								\
--	ContactInfo		VARCHAR(128)	NOT NULL,							\

--CREATE TABLE Platforms												\
--(																		\
--	HWCorrelator	VARCHAR(34)	NOT NULL PRIMARY KEY,					\
--	UsageToday		INTEGER		NOT NULL DEFAULT 1,						\
--	UsageThisWeek	INTEGER		NOT NULL DEFAULT 1,						\
--	UsageThisMonth	INTEGER		NOT NULL DEFAULT 1,						\
--	UsageThisYear	INTEGER		NOT NULL DEFAULT 1						\
--)

CREATE TABLE Signatures													\
(																		\
	SignatureSeqNum	INTEGER		NOT NULL PRIMARY KEY,					\
	Blessed			TIMESTAMP,											\
	Produced		TIMESTAMP	NOT NULL,								\
	Published		TIMESTAMP	NOT NULL,								\
	CheckSum		VARCHAR(34)	NOT NULL								\
)

CREATE TABLE SigsToBeExported											\
(																		\
	ServerID		SMALLINT	NOT NULL REFERENCES Servers(ServerID) ON DELETE CASCADE, \
	SignatureSeqNum	INTEGER		NOT NULL REFERENCES Signatures(SignatureSeqNum)	ON DELETE CASCADE, \
	PRIMARY KEY(ServerID, SignatureSeqNum)								\
)

-- Added on Mar/21/2000. This table will store the ServerId and Checksum values
-- for which we need to report the status.
CREATE TABLE StatusToBeReported											\
(																		\
	ServerID		SMALLINT	NOT NULL REFERENCES Servers(ServerID) ON DELETE CASCADE, \
	CheckSum		VARCHAR(34)	NOT NULL 								\
)


CREATE TABLE AnalysisResults											\
(																		\
	CheckSum		VARCHAR(34)	NOT NULL PRIMARY KEY,					\
-- Mikemod.  Removing Signatures constraint.
--	SignatureSeqNum	INTEGER,	\	REFERENCES Signatures(SignatureSeqNum),	\
	SignatureSeqNum	INTEGER,	\	 
	AnalysisStateID	SMALLINT	NOT NULL REFERENCES AnalysisStateInfo(AnalysisStateID)	\
)

CREATE TABLE Globals													\
(																		\
	AlwaysZero			SMALLINT	NOT NULL PRIMARY KEY DEFAULT 0,		\
	ThisServerID		SMALLINT	NOT NULL REFERENCES Servers(ServerID),	\
	TypeOfServer		CHAR		FOR BIT DATA NOT NULL CHECK(TypeOfServer IN (X'00', X'01', X'02', X'03')) DEFAULT X'00',	\
	NextLocalCookie		INTEGER		NOT NULL DEFAULT 1,					\
	NewestSeqNum		INTEGER		NOT NULL REFERENCES Signatures(SignatureSeqNum),	\
	NewestBlssdSeqNum	INTEGER		NOT NULL REFERENCES Signatures(SignatureSeqNum),	\
	DatabaseVersion		SMALLINT	NOT NULL DEFAULT 2,					\
	BackOfficeLock		CHAR		FOR BIT DATA NOT NULL CHECK(BackOfficeLock IN (X'00', X'01')) DEFAULT X'00'	\
)
--	CollectStats		SMALLINT	NOT NULL DEFAULT 0,					\
--	NextMinuteSeqNum	INTEGER		NOT NULL DEFAULT 1,					\


CREATE TABLE AnalysisRequest											\
(																		\
	LocalCookie		INTEGER		NOT NULL PRIMARY KEY,					\
	CheckSum		VARCHAR(34)	NOT NULL UNIQUE,								\
	Priority		SMALLINT	NOT NULL CHECK( Priority BETWEEN 0 AND 1000 ) DEFAULT 0,	\
	Imported		TIMESTAMP,											\
	Informed		TIMESTAMP,											\
	Finished		TIMESTAMP,											\
	LastStateChange	TIMESTAMP	NOT NULL DEFAULT CURRENT TIMESTAMP,		\
	AnalysisStateID	SMALLINT	NOT NULL REFERENCES AnalysisStateInfo(AnalysisStateID),	\
	NotifyClient	CHAR		FOR BIT DATA NOT NULL CHECK(NotifyClient IN (X'00', X'01')) DEFAULT X'00',		\
	NotifySymantec	CHAR		FOR BIT DATA NOT NULL CHECK(NotifySymantec IN (X'00', X'01')) DEFAULT X'01',	\
	SubmittorID		VARCHAR(64)	NOT NULL REFERENCES Submittors(SubmittorID),	\
--	HWCorrelator	VARCHAR(34)	NOT NULL REFERENCES Platforms(HWCorrelator),	\
	RemoteServerID	SMALLINT	REFERENCES Servers(ServerID),			\
	RemoteCookie	INTEGER												\
--	CONSTRAINT unqRemoteIDs UNIQUE(RemoteServerID, RemoteCookie)		\
)
CREATE INDEX idxCheckSum ON AnalysisRequest(CheckSum)
CREATE INDEX idxPriority ON AnalysisRequest(Priority)
CREATE INDEX idxImported ON AnalysisRequest(Imported)
CREATE INDEX idxStateID  ON AnalysisRequest(AnalysisStateID)
CREATE INDEX idxRemoteCookie   ON AnalysisRequest(RemoteCookie)
CREATE INDEX idxPriAndState ON AnalysisRequest (AnalysisStateId, Priority)

--	FileSize		INTEGER		NOT NULL,								\
--	SampleType		SMALLINT	NOT NULL REFERENCES SampleTypeInfo(SampelType),\
--	FinalCheck		SMALLINT	DEFAULT 0,								\



--CREATE TABLE Attributes													\
--(																		\
--	LocalCookie		INTEGER		NOT NULL REFERENCES AnalysisRequest(LocalCookie) ON DELETE CASCADE,	\
--	AttributeKey	SMALLINT	NOT NULL REFERENCES AttributeKeys(AttributeKey),	\
--	Value			VARCHAR(256)	NOT NULL,							\
--	PRIMARY KEY(LocalCookie, AttributeKey)								\
--)


--CREATE TABLE AnalysisTimes											\
--(																		\
--	LocalCookie		INTEGER		NOT NULL PRIMARY KEY REFERENCES AnalysisRequest(LocalCookie) ON DELETE CASCADE,	\
--	AnalysisState	SMALLINT	NOT NULL REFERENCES AnalysisStateInfo(AnalysisStateID),	\
--	Time			INTEGER		NOT NULL								\
--)


CREATE TABLE RemoteMustBeUnique											\
(																		\
--	RemoteCookie	INTEGER	NOT NULL REFERENCES AnalysisRequest(RemoteCookie) ON DELETE CASCADE,	\
--	RemoteServerID	INTEGER NOT NULL REFERENCES AnalysisRequest(RemoteServerID) ON DELETE CASCADE,	\
	RemoteCookie	INTEGER NOT NULL,									\
	RemoteServerID	SMALLINT NOT NULL,									\
	PRIMARY KEY(RemoteCookie, RemoteServerID)							\
)

DISCONNECT ALL
