
#ifndef XANALYSISSTATE_H
#define XANALYSISSTATE_H

#include "AVISCommon.h"

class AVISCOMMON_API XAnalysisState
{
public:
	//
	//	Officially sanctioned values

	//
	//	Samples that have been finsihed are in one of these final state
	//	All nodes in the Immune System use the terminal states.  After
	//	samples have been placed in a terminal state, its state will not
	//	change again.  The X-Date-Analyzed attribute is set when a sample
	//	is placed into a terminal state; its presence means that the value
	//	of X-Analysis-State is terminal


	static const char * UnInfectable()			{ return "uninfectable"; }
	static const char * UnSubmitable()			{ return "unsubmitable"; };
	static const char * Declined()				{ return "declined"; };
	static const char * NoDetect()				{ return "nodetect"; };
	static const char * Infected()				{ return "infected"; };
	static const char * UnInfected()			{ return "uninfected"; };
	static const char * Misfired()				{ return "misfired"; };
	static const char * Abort()					{ return "abort"; }
	static const char * Broken()				{ return "broken"; }

	//
	//	Samples that have not yet reached the anlaysis center are in one of the
	//	transit states.  Only components outside the analysis center use the
	//	transit states.  A sample may remain in a pending state indefinitely 
	//	before moving to another state.

	static const char * Receiving()				{ return "receiving"; };
	static const char * Accepted()				{ return "accepted"; };
	static const char * Importing()				{ return "importing"; };


	//
	// Samples that are awaiting analysis within the analysis center are in
	// one of the pending states.  Only componenets within the analysis center
	// use the pending states.  A sample may remain in a pending state 
	// indefinitely before moving to another state.

	static const char * Defer()					{ return "defer"; };
	static const char * Deferred()				{ return "deferred"; };
	static const char * Deferring()				{ return "deferring"; };
	static const char * Imported()				{ return "imported"; };
	static const char * Rescan()				{ return "rescan"; }

	//	Sample sthat are being analyzed within the analysis center are in
	//	one of the active states.  Only the dataflow component within the
	//	analysis center uses the active states.  A sample may remain in an
	//	active state for only a few seconds or for many minutes before
	//	moving to another state.


	static const char * Classifying()			{ return "classifying"; }
	static const char * Binary()				{ return "binary"; }
	static const char * BinaryControlling()		{ return "binaryControlling"; }
	static const char * BinaryWait()			{ return "binaryWait"; }
	static const char * BinaryReplicating()		{ return "binaryReplicating"; }
	static const char * BinaryScoring()			{ return "binaryScoring"; }
	static const char * Macro()					{ return "macro"; }
	static const char * MacroConrolling()		{ return "macroControlling"; }
	static const char * MacroWait()				{ return "macroWait"; }
	static const char * MacroReplicating()		{ return "macroReplicating"; }
	static const char * MacroScoring()			{ return "macroScoring"; }
	static const char * Signatures()			{ return "signatures"; }
	static const char * Locking()				{ return "locking"; }
	static const char * IncrBuilding()			{ return "incrBuilding"; }
	static const char * IncrUnitTesting()		{ return "incrUnitTesting"; }
	static const char * FullBuilding()			{ return "fullBuilding"; }
	static const char * FullUnitTesting()		{ return "fullUnitTesting"; }
	static const char * Unlocking()				{ return "unlocking"; }
	static const char * Archiving()				{ return "archiving"; }



/*	states that where until really recently valid
	static const char * ArchiveSample()			{ return "archive_sample"; }
	static const char * BinaryAnalysis()		{ return "binary_analysis"; };
	static const char * BinaryController()		{ return "binary_controller"; };
	static const char * BinaryReplicator()		{ return "binary_replicator"; }
	static const char * Building()				{ return "building"; };
	static const char * Built()					{ return "built"; };
	static const char * Classifier()			{ return "classifier"; };
	static const char * Cloned()				{ return "cloned"; };
	static const char * CriticalError()			{ return "critical_error"; };
	static const char * Deferrer()				{ return "deferrer"; };
	static const char * Exectuting()			{ return "exectuting"; };
	static const char * Extracted()				{ return "extracted"; };
	static const char * Extracting()			{ return "extracting"; };
	static const char * FullDefBuilder()		{ return "full_def_builder"; };
	static const char * FullUnitTest()			{ return "full_unit_test"; };
	static const char * IncrementalDefBuilder()	{ return "incremental_def_builder"; };
	static const char * IncrementalUnitTest()	{ return "incremental_unit_test"; };
	static const char * MacroAnalysis()			{ return "macro_analysis"; };
	static const char * MacroController()		{ return "macro_controler"; };
	static const char * MacroReplicator()		{ return "macro_replicator"; };
	static const char * Pong()					{ return "pong"; };
	static const char * Released()				{ return "released"; };
	static const char * Replicated()			{ return "replicated"; };
	static const char * Scored()				{ return "scored"; };
	static const char * Scoring()				{ return "scoring"; };
	static const char * Segragated()			{ return "segragated"; };
	static const char * Segregating()			{ return "segregating"; };
	static const char * SerializeBuild()		{ return "serializeBuild"; };
	static const char * Testing()				{ return "testing"; };
	static const char * UnDeferrer()			{ return "undeferrer"; };
	static const char * UnSerializeBuild()		{ return "unserialize_build"; };
*/

	//
	//	Not offical states used by IcePack

	static const char * Submitting()			{ return "submitting"; };
	static const char * Captured()				{ return "captured"; };

	//
	// Not (yet?) officially sanctioned values
/* */
	static const char * UnknownSampleState()	{ return "unknown_sample_state"; };
	static const char * SampleCheckError()		{ return "sample_check_error"; };
	static const char * Corrupted()				{ return "corrupted"; };
	static const char * Malformed()				{ return "malformed"; };
	static const char * Lost()					{ return "lost"; };
	static const char * CRCSampleFailure()		{ return "crc_sample_failure"; };
	static const char * CRCContentFailure()		{ return "crc_content_failure"; };
	static const char * BadSampleCheckSum()		{ return "bad_sample_checksum"; };
	static const char * BadContentCheckSum()	{ return "bad_content_checksum"; };
	static const char * UnScrambleFailure()		{ return "unscramble_failure"; };
	static const char * UncompressFailure()		{ return "uncompress_failure"; };
	static const char * MissingAttributes()		{ return "missing_attributes"; };
	static const char * BadSubmittorID()		{ return "bad_submittor_id"; };
	static const char * BadHWCorrelator()		{ return "bad_hw_correlator"; };
	static const char * DataOverflow()			{ return "data_overflow"; };
	static const char * PausedFilter()			{ return "paused_filter"; };
	static const char * ScanError()				{ return "scan_error"; };
	static const char * PausedScanning()		{ return "paused_scanning"; };
	static const char * UnknownSampleType()		{ return "unknown_sample_type"; };
	static const char * UnpackageFailure()		{ return "unpackage_failure"; };
	static const char * PausedFilterUnknown()	{ return "paused_filter_unknown"; };
	static const char * ExportForAnalysis()		{ return "export_for_analysis"; };
	static const char * BufferError()			{ return "buffer_error"; };
	static const char * InternalError()			{ return "internal_error"; };
	static const char * NoRepair()				{ return "norepair"; };
	static const char * UnRepairable()			{ return "unrepairable"; };
	static const char * BadRepair()				{ return "badrepair"; };
/* */

private:
	XAnalysisState();
	~XAnalysisState();
};


#endif
