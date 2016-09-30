========================================================================
       WIN32 APPLICATION : editopts
========================================================================

This is a patch tool designed for the Espana patch which updates all versions of NAV 2002 to r8.07.

It will add add the extension SPW to the extension list and add the new email option introduced in Espana 
"NAVPROXY:ShowProgressOut" to navopts.dat, navopts.def, and navopts.bak.  It will also add the option
"SCRIPTBLOCKING:Installed" to all the options files with a value of 1 if SB is installed and 0 if SB
is not installed.