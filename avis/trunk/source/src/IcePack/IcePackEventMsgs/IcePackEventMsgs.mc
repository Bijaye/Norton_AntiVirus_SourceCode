MessageIdTypedef=DWORD
SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
				Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
				Warning=0x2:STATUS_SEVERITY_WARNING
				Error=0x3:STATUS_SEVERITY_ERROR
				)

FacilityNames=(System=0x0:FACILITY_SYSTEM
				General=0x101:FACILITY_GENERAL
				Sample=0x102:FACILITY_SAMPLE
				Submission=0x103:FACILITY_SUBMISSION
				QSMonitor=0x104:FACILITY_QSMONITOR
				RegMonitor=0x105:FACILITY_REGMONITOR
				StatusMonitor=0x106:FACILITY_STATUSMONITOR
				BlessedMonitor=0x107:FACILITY_BLESSEDMONITOR
				SigDownload=0x108:FACILITY_SIGDOWNLOAD
				Scan=0x109:FACILITY_SCAN
				DDS=0x10a:FACILITY_DDS
				DeliveryMonitor=0x10b:FACILITY_DELIVERYMONITOR
				Communications=0x10c:FACILITY_COMMUNICATIONS
				Other=0x10d:FACILITY_OTHER)

LanguageNames=(English=0x409:MSG00409)

MessageId=0x1
Severity=Informational
Facility=General
SymbolicName=REG_BLANK_INFO
Language=English
%1
.

MessageId=0x02
Severity=Error
Facility=General
SymbolicName=REG_BLANK_ERROR
Language=English
%1
.

MessageId=0x03
Severity=Informational
Facility=General
SymbolicName=REG_SERVICE_START
Language=English
Immune System Interface Agent (IcePack) service started
.

MessageId=0x04
Severity=Informational
Facility=General
SymbolicName=REG_SERVICE_STOP
Language=English
Immune System Interface Agent (IcePack) service stopped
.

MessageId=0x05
Severity=Error
Facility=General
SymbolicName=REG_EXCEPTION_CAUGHT
Language=English
Exception caught in %1 [%2]
.

MessageId=0x06
Severity=Error
Facility=General
SymbolicName=REG_EXCEPTION_CAUGHT_STOP
Language=English
Exception caught in %1 [%2]. IcePack shutting down
.

MessageId=0x07
Severity=Error
Facility=General
SymbolicName=REG_UNKNOWN_EXCEPTION_CAUGHT
Language=English
Exception of unknown type caught by %1
.

MessageId=0x08
Severity=Error
Facility=General
SymbolicName=REG_UNKNOWN_EXCEPTION_CAUGHT_STOP
Language=English
Exception of unknown type caught by %1. IcePack shutting down
.

MessageId=
Severity=Error
Facility=General
SymbolicName=IPREG_COMM_ERROR_STOP
Language=English
Communications error with Immune System [%1].  IcePack shutting down
.

MessageId=
Severity=Error
Facility=DeliveryMonitor
SymbolicName=IPREG_TOPOLOGY_INIT_STOP
Language=English
Initialization of link with Topology service failed.  IcePack shutting down
.

MessageId=
Severity=Error
Facility=DeliveryMonitor
SymbolicName=IPREG_TOPOLOGY_REQUEST_FAILED
Language=English
NscQueryNavInfo threw an exception, IcePack is shutting down as a result
.

MessageId=
Severity=Error
Facility=SigDownload
SymbolicName=IPREG_SIG_DOWNLOAD_NO_XSIGSEQ
Language=English
Successful signature download did not return an X-Signature-Sequence attribute
.

MessageId=
Severity=Error
Facility=Communications
SymbolicName=IPREG_COMM_UNPACK_FAILURE
Language=English
Unable to unpack new definition set in a way that will effect all future attempts [%1].  IcePack shutting down
.

MessageId=
Severity=Error
Facility=Communications
SymbolicName=IPREG_COMM_SYS_EXCEPTION
Language=English
System exception intercepted (%1).  IcePack is shutting down as a result.
.

MessageId=
Severity=Error
Facility=Communications
SymbolicName=IPREG_COMM_LIB_EXCEPTION
Language=English
Library exception intercepted (%1).  IcePack is shutting down as a result.
.

MessageId=
Severity=Error
Facility=Communications
SymbolicName=IPREG_COMM_FILE_IO
Language=English
Unable to read, write, or open files for communications to continue (%1).  IcePack is shutting down as a result.
.

MessageId=
Severity=Error
Facility=Communications
SymbolicName=IPREG_COMM_UNKWN_SIG_TYPE
Language=English
Internal IcePack error, UnknownSignatureTypeError
.

MessageId=
Severity=Error
Facility=Communications
SymbolicName=IPREG_COMM_MISSING_SEQ_NUM
Language=English
Internal IcePack error, Response from Immune System signature request missing sequence number
.

MessageId=
Severity=Error
Facility=Communications
SymbolicName=IPREG_COMM_THREAD_NO_START
Language=English
Internal IcePack error, unable to start communications thread
.

MessageId=
Severity=Error
Facility=Communications
SymbolicName=IPREG_COMM_UNKNOWN
Language=English
Internal IcePack error, unknown error from communications sub system
.

MessageId=
Severity=Error
Facility=Communications
SymbolicName=IPREG_COMM_OUT_OF_MEM
Language=English
Internal IcePack error, out of memory
.

MessageId=
Severity=Error
Facility=Communications
SymbolicName=IPREG_COMM_BAD_MSG
Language=English
Internal IcePack error, unable to form a proper message to the gateway
.

MessageId=
Severity=Error
Facility=Communications
SymbolicName=IPREG_COMM_BAD_CHECKSUM
Language=English
Internal IcePack error, unable to generate a proper checksum for file to be submitted
.

MessageId=
Severity=Error
Facility=Communications
SymbolicName=IPREG_COMM_UNDERRUN
Language=English
Internal IcePack error, bad communications error (underrun)
.
