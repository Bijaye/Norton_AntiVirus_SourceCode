-- **************************************************************
-- ConfigureAVISDatabase.sql
--
-- The default configuration values for DB2 seem to be not
-- sufficient for the Immune System requirements.
-- This file can be used to set the initial configuration
-- values for the AVISDATA database. These configuration
-- parameters are essential for the optimal performance of
-- the Gateway and the Analysis center databases under severe
-- load conditions. The values assigned in this script also
-- helps for the long term stability of the programs that use
-- the AVISDATA database when the number of records in the
-- database grows. This script has to be run whenever the 
-- AVISDATA database is created. It is recommended to run this
-- script at the end of the createDB.bat. 
--
-- DB2 version 5.2 default configuration values are shown at the
-- bottom of this file.
--
-- Authors: Senthil Velayudham, Srikant Jalan
-- Creation date: Feb/10/2000
-- Last modified: Apr/06/2000
-- **************************************************************
--
-------- Configuration parameters for the DB2 MANAGER --------
--
update database manager configuration using MAXAGENTS 1500
update database manager configuration using NUM_POOLAGENTS 1500
update database manager configuration using MAX_COORDAGENTS 1500
update database manager configuration using MAXCAGENTS 1500
-- You have to STOP and START the database manager now --
STOP DATABASE MANAGER
START DATABASE MANAGER
--
CONNECT TO AVISDATA
--
-------- Configuration parameters under the Performance tab. --------
--
alter bufferpool IBMDEFAULTBP size -1
update database configuration for avisdata using DBHEAP 12000
update database configuration for avisdata using CATALOGCACHE_SZ 4096
update database configuration for avisdata using LOGBUFSZ 64
update database configuration for avisdata using BUFFPAGE 30000
update database configuration for avisdata using LOCKLIST 4096
update database configuration for avisdata using SORTHEAP 4096
update database configuration for avisdata using STMTHEAP 16384
update database configuration for avisdata using APPLHEAPSZ 512
update database configuration for avisdata using APP_CTL_HEAP_SZ 1024
update database configuration for avisdata using NUM_IOCLEANERS 64
--
-------- Configuration parameters under the Logs tab. --------
--
update database configuration for avisdata using LOGFILSIZ 2500
update database configuration for avisdata using LOGPRIMARY 10
update database configuration for avisdata using LOGSECOND 118
--
-------- Configuration parameters under the Applications tab. --------
--
update database configuration for avisdata using MAXAPPLS 5000
update database configuration for avisdata using AVG_APPLS 2500
update database configuration for avisdata using MAXFILOP 512
update database configuration for avisdata using DLCHKTIME 30000
update database configuration for avisdata using MAXLOCKS 100
--
DISCONNECT ALL
-- Make the 1st database connection after applying the performance
-- tuning values. Because of the new configuration, the very first
-- DB2 connection will take about 50 seconds. If we don't do this here and 
-- when the AVISDATA related applications (SampleImporter, SigExporter) start, 
-- they get database timeout exception on the very first DB2 connect call.
CONNECT TO AVISDATA
DISCONNECT ALL
--
-- **************************************************************
-- DB2 5.2 default configuration values are as shown below.
--
-- Application tab
-- MAXAPPLS 40 [1 - 5000] COUNTER
-- AVG_APPLS 1 [1 - MAXAPPLS] COUNTER
-- MAXFILOP 64 [2 - 32768] COUNTER
-- DLCHKTIME 10000 [1000 - 6000000] MILLISECONDS
-- MAXLOCKS 22 [1 - 100] PERCENTAGE OF LOCKLIST
-- LOCKTIMEOUT -1 [-1;0 - 30000] SECONDS
--
-- Performance tab
-- DBHEAP 600 [32 - 60000] PAGES 4KB
-- CATALOGCACHE_SZ 32 [1 - DBHEAP] PAGES 4KB
-- LOGBUFSZ 8 [4 - 128] PAGES 4KB
-- UTIL_HEAP_SZ 5000 [16 - 524288] PAGES 4KB
-- BUFFPAGE 250 [2*MAXAPPLS - 524288] PAGES 4KB
-- PCKCACHESZ -1 [32 - 64000] PAGES 4KB
-- LOCKLIST 50 [4 - 60000] PAGES 4KB
-- ESTORE_SEG_SZ 16000 [0 - 1048575] PAGES 4KB
-- NUM_ESTORE_SEGS 0 [0 - SQL_MAXINTVAL] PAGES 4KB
-- SORTHEAP 256 [16 - 524288] PAGES 4KB
-- STMTHEAP 2048 [128 - 60000] PAGES 4KB
-- APPLHEAPSZ 64 [16 - 60000] PAGES 4KB
-- STAT_HEAP_SZ 4384 [1096 - 524288] PAGES 4KB
-- APP_CTL_HEAP_SZ 512 [1 - 64000] PAGES 4KB
-- CHNGPGS_THRESH 60 [5 - 80] PERCENT
-- NUM_IOCLEANERS 1 [0 - 255] COUNTER
-- NUM_IOSERVERS 3 [1 - 255] COUNTER
-- SEQDETECT 1 [0 - 1] BOOL
-- DFT_PREFETCH_SZ 16 [0 - 32767] PAGES
-- INDEXSORT 1 [0 - 1] BOOL
-- NUMSEGS 1 
-- DFT_EXTENT_SZ 32 [2 - 256] PAGES 4KB
-- MULTIPAGE_ALLOC 0 [0 - 1] BOOL
--
-- Logs tab
-- LOGFILSIZ 250 [4 - 4095] PAGES 4KB
-- LOGPRIMARY 3 [2 - 128] COUNTER
-- LOGSECOND 2 [2 - 128] COUNTER
-- LOGPATH C:\DB2MPP\NODE0000\SQL00001\SQLOGDIR
-- NEWLOGPATH
-- NEXTACTIVE
-- LOGHEAD
-- MINCOMMIT 1 [1 - 25] COUNTER
-- SOFTMAX 100 [1 - 100*LOGPRIMARY] PERCENTAGE
-- LOGRETAIN 0 [0 - 1] BOOL
-- USEREXIT 0 [0 - 1] BOOL
-- **************************************************************
