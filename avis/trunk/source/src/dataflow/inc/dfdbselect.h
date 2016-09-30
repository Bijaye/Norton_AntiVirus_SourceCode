/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFDBSelect.h                                                    */
/* FUNCTION:  To interface with the status database                           */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES: The status database is a directory as for today                     */
/*----------------------------------------------------------------------------*/
/* HISTORY:  September 22, 1998  - SNK                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef DFDBSELECT_H
#define DFDBSELECT_H


class DFDBSelect : public CObject
{ 
public:
        enum Priority {NONE, HIGH, LOW};
		enum SampleStage { IMPORTED, AFTERDEFBUILDER, AFTERDEFQUEUE, RESULT};
		enum ConfigKeys { UNCPATH, CacheCheckInterval, DirectoryCacheExpiryTime, SampleSubmissionInterval, SampleArrivalCheckInterval, ConcurrentNumberOfSamplesToBeProcessed, BuildDefFilename, DefBaseDir, DatabaseRescanInterval, DefImportInterval, StatusUpdateInterval, DefImporterDir, UndeferrerInterval, AttributesInterval };
        DFDBSelect(DFManager *mgr){manager = mgr;}
		int DBRequest(int number, Priority prior, DFDBSelect::ConfigKeys key );	
        int SelectFirst(DFSample &sample);
		int IsFileFound(char *fileName );
		int ReadConfigFile(char *value, DFDBSelect::ConfigKeys key );
		void AppendPath(char *lpszFirst,char *lpszSec);
		void DFImportSamples(int number);
		int DFImportSamplesFromDB(int neededNumber, DFSample::Origin stage); 
		void GetCookiesInStateWithPriorityTest();
		static BOOL MarkSamplesInDB();


public:
	    std::ifstream ifile;
	  	DFArrivedList       arrivedList;                 // collection of arrived samples 

        DFManager           *manager;                    // manager 
private:
     DFDBSelect(const DFDBSelect &);
     DFDBSelect &operator =(const DFDBSelect &);

};


#endif 
