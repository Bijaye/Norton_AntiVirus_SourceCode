========================================================================
       WIN32 APPLICATION : UnRegCmd - Tool created by Wesley Jordan
				      to integrate MicroDefs 2.0 in
				      the Espana (NAV 2002 r8.07) inline
				      patch.
========================================================================


// If this executable is run with a -i commandline parameter it will
// remove the MicroDefs 1.0 commandlines from the LU product catalog
// and update the registry commandlines with the MicroDefs 2.0 data.
//
// This tool will also give Everyone full access to the alert.dat file
// so that DefAlert will be able to function properly for all user
// accounts.
//
// If it is not run with a -i commandline it will just copy itself to
// the NAV directory and create a run key entry to run on the next reboot
// with the -i parameter
