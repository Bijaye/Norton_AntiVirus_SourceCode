// DeliveryMonitor.cpp: implementation of the DeliveryMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeliveryMonitor.h"

#include <atlbase.h>

#include <list>
#include <algorithm>

#include <AttributeKeys.h>
#include <CMclAutoLock.h>
#include "EventLog.h"

#include <NscQueryNavInfo.h>
#include <dddsinterface.h>

#include "Sample.h"
#include "GlobalData.h"
#include <Logger.h>
//#include "IcePackMonitor.h"

using namespace std;

class DDDSInfo
{
public:
	DDDSInfo(string d, string s, string n, string g) :
				domainName(d), serverName(s), clientName(n), clientGUID(g)
	{
	}

	string	domainName, serverName, clientName, clientGUID;
};

class DDDSInfoComp
{
public:
	DDDSInfoComp(DDDSInfo& info) : dddsInfo(info) {};

	operator()(DDDSInfo& info)
	{ return dddsInfo.domainName == info.domainName && 
			 dddsInfo.serverName == info.serverName &&
			 dddsInfo.clientName == info.clientName &&
			 dddsInfo.clientGUID == info.clientGUID;
	}

	DDDSInfo	dddsInfo;
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DeliveryMonitor::DeliveryMonitor() : CMclThreadHandler()
{
}

DeliveryMonitor::~DeliveryMonitor()
{
}

static inline void AppendError(int value, int minV, int maxV, const char* name, string& msg)
{
	if (value < minV || value > maxV)
	{
		char	buffer[64];
		sprintf(buffer, " %s should be between %d and %d, ", name, minV, maxV);
		msg	+= buffer;
	}
}

//
//	DeliveryMonitor
//
//	Overview:
//		This thread takes samples in the "distributed" state and checks to see if
//		the definition file has been installed yet.  After verifying that the
//		definition file was successfully installed the thread sets the sample's
//		state to "installed".
//		definition files to the client machine that submitted the sample.  After
//		successfully sending the definition files to the client machine the thread
//		sets the sample's state to "distributed" and deletes the sample from the
//		master sample list.
//
//	Algorithm:
//		(Simplified version, real code is slightly different)
//		Do
//			Make a list of samples in the "distributed" state from the global sample list
//			Sort the list in decending priority
//				For each element in the list
//				Get the domain name, server name, client name, and client GUID from
//					the sample attributes
//				Query the Topology API for information on the currently installed
//					definitions
//				If the currently installed definitions meet or exceed the requirements
//					to fix the sample then set the sample status to "installed" and 
//					remove from the global samples list.
//
//	Globals static methods/members Used
//		stop					bool		if true exit this thread.  Set by SCM when service is
//											told to stop or the system is shutting down.
//		critSamplesList			CMclCritSec	used to serialize access to the samplesList
//		samplesList				std::list	List of samples that IcePack might deal with (samples not
//											already in a final state or submitted via Scan and Deliver)

unsigned DeliveryMonitor::ThreadHandlerProc()
{
	EntryExit		entryExit(Logger::LogIcePackDeliveryMonitor, "ThreadHandlerProc");
//	EnterExitThread	enterExitThread(IcePackMonitor::deliveryMonitor);

	list<Sample>	listCopy;
	Sample			sample;
	list<Sample>::iterator	i, j;
	string			newStatus;
	string			attrs;				// attributes
	bool			changed;

	int				dddsRC;
	list<DDDSInfo>	delivered, pending;
	list<DDDSInfo>	alreadyDeleted;
	list<DDDSInfo>::iterator	k, l, r;

	if (GlobalData::stop)
		return -1;

	if (S_OK != NSCInitialize())
	{
		Logger::Log(Logger::LogCriticalError, Logger::LogIcePackDeliveryMonitor,
					"NSCInitialize() failed!");
		EventLog::Error(FACILITY_DELIVERYMONITOR, IPREG_TOPOLOGY_INIT_STOP);

		GlobalData::Stop();
	}

	do
	{
		try
		{
			Logger::Log(Logger::LogDebug, Logger::LogIcePackDeliveryMonitor,
							"About to check if any samples are in \"distributed\" state");

			changed			= false;

			// make copy of samples list
			listCopy.clear();
			{
				CMclAutoLock	lock(GlobalData::critSamplesList);
				i = GlobalData::samplesList.begin();

				for (; i != GlobalData::samplesList.end(); i++)
				{
					if (SampleStatus::distributed == i->Status())
					{
						listCopy.push_back(*i);
						listCopy.back().Mark(Sample::sUnchanged);
					}
				}
			}

			string		msg;
			HRESULT		rc;
			for (i = listCopy.begin(); i != listCopy.end() && !GlobalData::stop; i++)
			{
				if (!i->IsNull() && SampleStatus::distributed == i->Status())
				{
					USES_CONVERSION;

					string			domain, server, computer, clientGUID;
					AVMACHINEINFO	info;
					DWORD			flags = 0;

					i->GetAttribute(AttributeKeys::PlatformDomain(),		domain);
					i->GetAttribute(AttributeKeys::PlatformDistributor(),	server);
					i->GetAttribute(AttributeKeys::PlatformComputer(),		computer);
					i->GetAttribute(AttributeKeys::PlatformGUID(),			clientGUID);

					if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackDeliveryMonitor))
					{
						msg	= "Checking if signatures have been installed on ";
						msg	+= computer;
						msg	+= " [domain = ";
						if (0 == domain.size())
							msg	+= "NULL";
						else
							msg += domain;
						msg	+= ", server = ";
						if (0 == server.size())
							msg	+= "NULL";
						else
							msg	+= server;
						msg	+= ", flags = 0)";

						Logger::Log(Logger::LogDebug, Logger::LogIcePackDeliveryMonitor, msg.c_str());
					}

					try
					{
						rc = NscQueryNavInfo(0 == domain.size() ? NULL   : T2OLE(domain.c_str()),
											 0 == server.size() ? NULL   : T2OLE(server.c_str()),
											 0 == computer.size() ? NULL : T2OLE(computer.c_str()),
											 flags,
											 &info);

						if (S_OK == rc &&
							!((0 == info.dtLastScanTime.wYear || 1999 == info.dtLastScanTime.wYear ||
																 2000 == info.dtLastScanTime.wYear) &&
							  (info.dtLastScanTime.wMonth >= 0 && info.dtLastScanTime.wMonth <= 12) &&
							  (info.dtLastScanTime.wDay   >= 0 && info.dtLastScanTime.wDay   <= 31) &&
							  (info.dtLastScanTime.wHour  >= 0 && info.dtLastScanTime.wHour  <= 23) &&
							  (info.dtLastScanTime.wMinute>= 0 && info.dtLastScanTime.wMinute<= 59) &&
							  (info.dtLastScanTime.wSecond>= 0 && info.dtLastScanTime.wSecond<= 59) &&

							  (0 == info.dtLastVirusTime.wYear || 1999 == info.dtLastVirusTime.wYear ||
																  2000 == info.dtLastVirusTime.wYear) &&
							  (info.dtLastVirusTime.wMonth >= 0 && info.dtLastVirusTime.wMonth <= 12) &&
							  (info.dtLastVirusTime.wDay   >= 0 && info.dtLastVirusTime.wDay   <= 31) &&
							  (info.dtLastVirusTime.wHour  >= 0 && info.dtLastVirusTime.wHour  <= 23) &&
							  (info.dtLastVirusTime.wMinute>= 0 && info.dtLastVirusTime.wMinute<= 59) &&
							  (info.dtLastVirusTime.wSecond>= 0 && info.dtLastVirusTime.wSecond<= 59) &&

							  (info.dwSequence >= 0 && info.dwSequence <  5000)		&&
							  (1999 == info.wYearDefs || 2000 == info.wYearDefs)	&&
							  (info.wMonthDefs >= 1 && info.wMonthDefs <= 12)		&&
							  (info.wDayDefs   >= 1 && info.wDayDefs   <= 31)		&&
							  (info.dwRevDefs  >= 1 && info.dwRevDefs  <= 99)		))
						{
							rc = 2261;

							char	buffer[512];
							string	message("Topology API returned invalid information (");
							if (!(0 == info.dtLastScanTime.wYear || 1999 == info.dtLastScanTime.wYear ||
																 2000 == info.dtLastScanTime.wYear))
							{
								sprintf(buffer, "lastScanTime should be 0, 1999, or 2000, is %d,",
												info.dtLastScanTime.wYear);
								message	+= buffer;
							}
							AppendError(info.dtLastScanTime.wMonth, 0, 12, "lastScanTime.wMonth", message);
							AppendError(info.dtLastScanTime.wDay, 0, 31, "lastScanTime.wDay", message);
							AppendError(info.dtLastScanTime.wHour, 0, 23, "lastScanTime.wHour", message);
							AppendError(info.dtLastScanTime.wMinute, 0, 59, "lastScanTime.wMinute", message);
							AppendError(info.dtLastScanTime.wSecond, 0, 59, "lastScanTime.wSecond", message);

							AppendError(info.dwSequence, 0, 4999, "dwSequence", message);

							if (!(1999 == info.wYearDefs || 2000 == info.wYearDefs))
							{
								sprintf(buffer, "wYearDefs should be 1999, or 2000, is %d,",
												info.wYearDefs);
								message	+= buffer;
							}
							AppendError(info.wMonthDefs, 1, 12, "wMonthDefs", message);
							AppendError(info.wDayDefs, 1, 31, "wDayDefs", message);
							AppendError(info.dwRevDefs, 1, 99, "dwRevDefs", message);

							message	+= ")";
							Logger::Log(Logger::LogError, Logger::LogIcePackDeliveryMonitor, message.c_str());
						}
					}
					catch (...)
					{
						Logger::Log(Logger::LogDebug, Logger::LogIcePackDeliveryMonitor,
									"NscQueryNavInfo threw an exception"); //, IcePack is shutting down as a result");

//						EventLog::Error(FACILITY_DELIVERYMONITOR, IPREG_TOPOLOGY_REQUEST_FAILED);
						
//						GlobalData::Stop();

//						return 0;

						rc	= 2260;
					}

					if (S_OK == rc)
					{
						if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackDeliveryMonitor))
						{
							char	infoMsg[128];
							sprintf(infoMsg, "Topology call succeeded, info[sequence = %ld, daily version(yyyy.mm.dd.rrr) = %04d.%02d.%02d.%03ld]",
												info.dwSequence, info.wYearDefs, info.wMonthDefs, info.wDayDefs,
												info.dwRevDefs);

							Logger::Log(Logger::LogDebug, Logger::LogIcePackDeliveryMonitor, infoMsg);
						}

						if (info.dwSequence >= i->NeededSeqNum())
						{
							if (Logger::WillLog(Logger::LogDebug, Logger::LogIcePackDeliveryMonitor))
							{
								msg	= "Signature installed on client ";
								msg	+= computer.c_str();
	
								Logger::Log(Logger::LogInfo, Logger::LogIcePackDeliveryMonitor,
												msg.c_str());
							}

							i->Status(SampleStatus::installed);
							changed	= true;
							i->Mark(Sample::sDelete);
							GlobalData::UpdateMasterSampleList(*i);

							delivered.push_back(DDDSInfo(domain, server, computer, clientGUID));
						}
						else
						{
							pending.push_back(DDDSInfo(domain, server, computer, clientGUID));

							Logger::Log(Logger::LogDebug, Logger::LogIcePackDeliveryMonitor,
										"Topology api returned success, but returned a sequence number less than that needed");
						}
					}
					else if (S_FALSE == rc)
					{
						string	msg("Symantec Topology Service unable to find client machine ");
						msg	+= computer;
						msg += " IcePack unable to verify delivery of virus definition files";

						Logger::Log(Logger::LogDebug, Logger::LogIcePackDeliveryMonitor, msg.c_str());
						i->SetAttribute(AttributeKeys::Attention(), msg);
					}
					else
					{
						string	errMsg;

						if (E_FAIL == rc)
							errMsg	= "Topology request failed, general failure";
						else if (E_POINTER == rc)
							errMsg	= "Topology request failed, pavData parameter was NULL, not possiable!";
						else if (E_INVALIDARG == rc)
							errMsg	= "Topology request failed, bad input parameter";
						else if (2260 == rc)
							errMsg	= "Topology request failed due to topology api mis-match (caused topology api to throw an exception)";
						else if (2261 == rc)
							errMsg	= "Topology request failed due to topology api mis-match (caused topology api to return invalid data)";
						else
						{
							errMsg	= "Topology request failed, with unknown code (";
							char	errBuff[20];
							sprintf(errBuff, "%d", rc);
							errMsg	= errBuff;
							errMsg	= ")";
						}

						Logger::Log(Logger::LogError, Logger::LogIcePackDeliveryMonitor, errMsg.c_str());

						i->SetAttribute(AttributeKeys::Attention(), errMsg);
						i->Status(SampleStatus::attention);
						i->Mark(Sample::sDelete);
						GlobalData::UpdateMasterSampleList(*i);
					}
					
					i->DeActivate();
				}
			}
/*
			if (changed && !GlobalData::stop)
			{
				Logger::Log(Logger::LogDebug, Logger::LogIcePackDeliveryMonitor,
								"Change in Sample made, changing master list to match");

				CompSampleByKeyOnly	comp(0L);
				CMclAutoLock		lock(GlobalData::critSamplesList);

				for (i = listCopy.begin(); i != listCopy.end(); i++)
				{
					if (!i->IsNull() && Sample::sChanged == i->Mark())
					{
						comp.Key(i->SampleKey());
						j = find_if(GlobalData::samplesList.begin(),
									GlobalData::samplesList.end(),
									comp);
						if (j != GlobalData::samplesList.end())
							*j = *i;
					}
					else if (Sample::sDelete == i->Mark())
					{
						GlobalData::samplesList.remove(*i);
					}
				}
			}
*/
			//
			//	Get rid of unneeded DDDS files
			if (delivered.size() > 0)
			{
				if (pending.size() > 0)
				{
					for (k = pending.begin(); k != pending.end(); k++)
					{
						r = remove_if(delivered.begin(), delivered.end(), DDDSInfoComp(*k));
						if (delivered.end() != r)
							delivered.erase(r, delivered.end());
					}
				}

				for (k = delivered.begin(); k != delivered.end(); k++)
				{
					if (alreadyDeleted.end() == find_if(alreadyDeleted.begin(),
														alreadyDeleted.end(),
														DDDSInfoComp(*k)))
					{
						dddsRC = DDDSDistributeDefinitions(
								0 == k->domainName.size() ? NULL : k->domainName.c_str(),
								0 == k->serverName.size() ? NULL : k->serverName.c_str(),
								0 == k->clientName.size() ? NULL : k->clientName.c_str(),
								0 == k->clientGUID.size() ? NULL : k->clientGUID.c_str(),
								0);
						string	clientDescription("[domain = ");
						msg	+= k->domainName;
						msg	+= ", server = ";
						msg	+= k->serverName;
						msg	+= ", clientName = ";
						msg	+= k->clientName;
						msg	+= ", guid = ";
						msg	+= k->clientGUID;
						msg	+= " ]";
						if (0 == dddsRC)
						{
							string	msg("Successfully removed ddds file for ");
							msg	+= clientDescription;

							Logger::Log(Logger::LogInfo, Logger::LogIcePackDeliveryMonitor, 
										msg.c_str());
						}
						else
						{
							string	msg("Attempt to remove ddds file after confirmed installation of defintion set failed because");
							if (1 == dddsRC)
								msg += " an argument was invalid.  ";
							else if (2 == dddsRC)
								msg += " an error prevented it from completing.  ";
							else
								msg += " returned an unknown return code.  ";
							msg	+= clientDescription;
							Logger::Log(Logger::LogInfo, Logger::LogIcePackDeliveryMonitor,
										msg.c_str());
						}
						alreadyDeleted.push_back(*k);
					}
				}
			}
		}

		catch (...)
		{
			GlobalData::HandleException("DeliveryMonitor::ThreadHandlerProc()",
										Logger::LogIcePackDeliveryMonitor);
		}

		
		try
		{
			delivered.clear();
			pending.clear();
			alreadyDeleted.clear();

			listCopy.clear();
		}
		catch (...)
		{
		}

		Logger::Log(Logger::LogDebug, Logger::LogIcePackDeliveryMonitor, "Pausing for next check interval");

		//
		//	This method will wake up when the interval has passed or IcePack is shutting
		//	down.  It also handles the issue of configuration changes while asleep.
		GlobalData::SleepInSegments(GlobalData::TopologyCheckInterval,
									Logger::LogIcePackDeliveryMonitor,
									"Normal pause between checks for definition file installation");

	} while (!GlobalData::stop);

	NSCUnInitialize();

	return 0;
}
