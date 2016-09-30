CONNECT TO AVISDATA

INSERT INTO AttributeKeys VALUES ( 1, 'Accept' )
INSERT INTO AttributeKeys VALUES ( 2, 'Accept-Ranges' )
INSERT INTO AttributeKeys VALUES ( 3, 'Connection' )
INSERT INTO AttributeKeys VALUES ( 4, 'Content-Length' )
INSERT INTO AttributeKeys VALUES ( 5, 'Content-Range' )
INSERT INTO AttributeKeys VALUES ( 6, 'Content-Type' )
INSERT INTO AttributeKeys VALUES ( 7, 'Date' )
INSERT INTO AttributeKeys VALUES ( 8, 'From' )
INSERT INTO AttributeKeys VALUES ( 9, 'Location' )
INSERT INTO AttributeKeys VALUES ( 10, 'Pragma' )
INSERT INTO AttributeKeys VALUES ( 11, 'Range' )
INSERT INTO AttributeKeys VALUES ( 12, 'Server' )
INSERT INTO AttributeKeys VALUES ( 13, 'User-Agent' )

INSERT INTO AttributeKeys VALUES ( 20, 'X-Analysis-Cookie' )
INSERT INTO AttributeKeys VALUES ( 21, 'X-Analysis-Issue' )
INSERT INTO AttributeKeys VALUES ( 22, 'X-Analysis-Service' )
INSERT INTO AttributeKeys VALUES ( 23, 'X-Analysis-State' )
INSERT INTO AttributeKeys VALUES ( 24, 'X-Analysis-Virus-Identifier' )
INSERT INTO AttributeKeys VALUES ( 25, 'X-Analysis-Virus-Name')

INSERT INTO AttributeKeys VALUES ( 30, 'X-Checksum-Method' )

INSERT INTO AttributeKeys VALUES ( 40, 'X-Content-Checksum' )
INSERT INTO AttributeKeys VALUES ( 41, 'X-Content-Compression' )
INSERT INTO AttributeKeys VALUES ( 42, 'X-Content-Encoding' )
INSERT INTO AttributeKeys VALUES ( 43, 'X-Content-Scrambling' )
INSERT INTO AttributeKeys VALUES ( 44, 'X-Content-Strip' )

INSERT INTO AttributeKeys VALUES ( 50, 'X-Customer-Contact-Name' )
INSERT INTO AttributeKeys VALUES ( 51, 'X-Customer-Contact-Telephone' )
INSERT INTO AttributeKeys VALUES ( 52, 'X-Customer-Contact-Email' )
INSERT INTO AttributeKeys VALUES ( 53, 'X-Customer-Credentials' )
INSERT INTO AttributeKeys VALUES ( 54, 'X-Customer-Identifier' )
INSERT INTO AttributeKeys VALUES ( 55, 'X-Customer-Name' )

INSERT INTO AttributeKeys VALUES ( 60, 'X-Date-Accessed' )
INSERT INTO AttributeKeys VALUES ( 61, 'X-Date-Analyzed' )
INSERT INTO AttributeKeys VALUES ( 62, 'X-Date-Captured' )
INSERT INTO AttributeKeys VALUES ( 63, 'X-Date-Completed' )
INSERT INTO AttributeKeys VALUES ( 64, 'X-Date-Created' )
INSERT INTO AttributeKeys VALUES ( 65, 'X-Date-Delivered' )
INSERT INTO AttributeKeys VALUES ( 66, 'X-Date-Distributed' )
INSERT INTO AttributeKeys VALUES ( 67, 'X-Date-Forwarded' )
INSERT INTO AttributeKeys VALUES ( 68, 'X-Date-Installed' )
INSERT INTO AttributeKeys VALUES ( 69, 'X-Date-Modified' )
INSERT INTO AttributeKeys VALUES ( 70, 'X-Date-Produced' )
INSERT INTO AttributeKeys VALUES ( 71, 'X-Date-Published' )

INSERT INTO AttributeKeys VALUES ( 80, 'X-Error' )
INSERT INTO AttributeKeys VALUES ( 81, 'X-Error-Import' )
INSERT INTO AttributeKeys VALUES ( 82, 'X-Error-IcePack' )

INSERT INTO AttributeKeys VALUES ( 91, 'X-Platform-Address' )
INSERT INTO AttributeKeys VALUES ( 92, 'X-Platform-Computer' )
INSERT INTO AttributeKeys VALUES ( 93, 'X-Platform-Correlator' )
INSERT INTO AttributeKeys VALUES ( 94, 'X-Platform-Dstributor' )
INSERT INTO AttributeKeys VALUES ( 95, 'X-Platform-Domain' )
INSERT INTO AttributeKeys VALUES ( 96, 'X-Platform-GUID' )
INSERT INTO AttributeKeys VALUES ( 97, 'X-Platform-Host' )
INSERT INTO AttributeKeys VALUES ( 98, 'X-Platform-Language' )
INSERT INTO AttributeKeys VALUES ( 99, 'X-Platform-Owner' )
INSERT INTO AttributeKeys VALUES ( 100, 'X-Platform-Processor' )
INSERT INTO AttributeKeys VALUES ( 101, 'X-Platform-Scanner' )
INSERT INTO AttributeKeys VALUES ( 102, 'X-Platform-System' )
INSERT INTO AttributeKeys VALUES ( 103, 'X-Platform-User' )

INSERT INTO AttributeKeys VALUES ( 110, 'X-Sample-Category' )
INSERT INTO AttributeKeys VALUES ( 111, 'X-Sample-Checksum' )
INSERT INTO AttributeKeys VALUES ( 112, 'X-Sample-Checkup' )
INSERT INTO AttributeKeys VALUES ( 113, 'X-Sample-File' )
INSERT INTO AttributeKeys VALUES ( 114, 'X-Sample-Geometry' )
INSERT INTO AttributeKeys VALUES ( 115, 'X-Sample-Priority' )
INSERT INTO AttributeKeys VALUES ( 116, 'X-Sample-Sector' )
INSERT INTO AttributeKeys VALUES ( 117, 'X-Sample-Service' )
INSERT INTO AttributeKeys VALUES ( 118, 'X-Sample-Size' )
INSERT INTO AttributeKeys VALUES ( 119, 'X-Sample-Switches' )
INSERT INTO AttributeKeys VALUES ( 120, 'X-Sample-Type' )

INSERT INTO AttributeKeys VALUES ( 131, 'X-Scan-Name' )
INSERT INTO AttributeKeys VALUES ( 132, 'X-Scan-Result' )
INSERT INTO AttributeKeys VALUES ( 133, 'X-Scan-Sequence' )
INSERT INTO AttributeKeys VALUES ( 134, 'X-Scan-Virus' )
INSERT INTO AttributeKeys VALUES ( 135, 'X-Signatures-Name' )
INSERT INTO AttributeKeys VALUES ( 136, 'X-Signatures-Sequence' )
INSERT INTO AttributeKeys VALUES ( 137, 'X-Sample-Reason' )



INSERT INTO AnalysisStateInfo VALUES ( 0, 'UnknownSampleState' )
INSERT INTO AnalysisStateInfo VALUES ( 1, 'SampleCheckError' )

INSERT INTO AnalysisStateInfo VALUES ( 10, 'uninfectable' )
INSERT INTO AnalysisStateInfo VALUES ( 11, 'unsubmitable' )
INSERT INTO AnalysisStateInfo VALUES ( 12, 'declined' )
INSERT INTO AnalysisStateInfo VALUES ( 13, 'nodetect' )
INSERT INTO AnalysisStateInfo VALUES ( 14, 'infected' )
INSERT INTO AnalysisStateInfo VALUES ( 15, 'uninfected' )
INSERT INTO AnalysisStateInfo VALUES ( 16, 'misfired' )
INSERT INTO AnalysisStateInfo VALUES ( 17, 'abort' )
INSERT INTO AnalysisStateInfo VALUES ( 18, 'broken' )

INSERT INTO AnalysisStateInfo VALUES ( 30, 'receiving' )
INSERT INTO AnalysisStateInfo VALUES ( 31, 'accepted' )
INSERT INTO AnalysisStateInfo VALUES ( 32, 'importing' )

INSERT INTO AnalysisStateInfo VALUES ( 41, 'defer' )
INSERT INTO AnalysisStateInfo VALUES ( 42, 'deferred' )
INSERT INTO AnalysisStateInfo VALUES ( 43, 'deferring' )
INSERT INTO AnalysisStateInfo VALUES ( 44, 'imported' )
INSERT INTO AnalysisStateInfo VALUES ( 45, 'rescan' )

INSERT INTO AnalysisStateInfo VALUES ( 50, 'classifying' )
INSERT INTO AnalysisStateInfo VALUES ( 51, 'binary' )
INSERT INTO AnalysisStateInfo VALUES ( 52, 'binaryControlling' )
INSERT INTO AnalysisStateInfo VALUES ( 53, 'binaryWait' )
INSERT INTO AnalysisStateInfo VALUES ( 54, 'binaryReplicating' )
INSERT INTO AnalysisStateInfo VALUES ( 55, 'binaryScoring' )
INSERT INTO AnalysisStateInfo VALUES ( 56, 'macro' )
INSERT INTO AnalysisStateInfo VALUES ( 57, 'macroControlling' )
INSERT INTO AnalysisStateInfo VALUES ( 58, 'macroWait' )
INSERT INTO AnalysisStateInfo VALUES ( 59, 'macroReplicating' )
INSERT INTO AnalysisStateInfo VALUES ( 60, 'macroScoring' )
INSERT INTO AnalysisStateInfo VALUES ( 61, 'signatures' )
INSERT INTO AnalysisStateInfo VALUES ( 62, 'locking' )
INSERT INTO AnalysisStateInfo VALUES ( 63, 'incrBuilding' )
INSERT INTO AnalysisStateInfo VALUES ( 64, 'incrUnitTesting' )
INSERT INTO AnalysisStateInfo VALUES ( 65, 'fullBuilding' )
INSERT INTO AnalysisStateInfo VALUES ( 66, 'fullUnitTesting' )
INSERT INTO AnalysisStateInfo VALUES ( 67, 'unlocking' )
INSERT INTO AnalysisStateInfo VALUES ( 68, 'archive' )
INSERT INTO AnalysisStateInfo VALUES ( 69, 'archiving' )
INSERT INTO AnalysisStateInfo VALUES ( 70, 'error' )


--
--	Add a couple of records that are needed for the operation of 
--	the database.  These are needed so we can create the one and
--	only Globals row.  After that row is created a trigger is 
--	defined to prevent any additional rows from being added to the
--	globals table.

INSERT INTO Servers VALUES ( 0, 'http://www.Odin.com', 0 )

INSERT INTO Signatures VALUES ( 1, '1998-8-24-20.21.00',				\
								'1998-8-24-20.21.00',					\
								'1998-8-24-18.00.00',					\
								'Not A Valid Signature')
INSERT INTO Signatures VALUES ( 2, NULL,								\
								'1998-8-24-19.21.00',					\
								'1998-8-24-17.00.00',					\
								'Not A Valid Signature')

INSERT INTO Globals VALUES ( 0, 0, X'00', 1, 2, 1, 0, X'00' )

DISCONNECT ALL
