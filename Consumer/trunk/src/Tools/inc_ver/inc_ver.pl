#/////////////////////////////////////////////////////////////////////////////
#/////// SCRIPT TO AUTOMATICALLY UPDATE BUILD NUMBER FOR NAV UPON BUILD
#/////// BY: COLLIN DAVIS
#/////////////////////////////////////////////////////////////////////////////



#/////////////////////////////////////////////////////////////////////////////
#/////// INCREMENT VERSION.DAT

$root = "$ARGV[0]";
$filename = "$root..\\..\\nobuilds\\version.dat";
$tempname = "$root..\\..\\nobuilds\\version.tmp";

open(FILE, "+<$filename");						#OPEN UP VERSION.DAT
open(NEW, ">$tempname");		  					#OPEN UP TEMP VERSION.DAT
	
while ($line = <FILE>)							#STEP THROUGH LINE BY LINE	
{
	if ($line =~ /^PublicRevision/)				#IF AT FIRST FIELD NEEDING CHANGES 	
	{
		($field, $value) = split(/=/, $line);	#PARSE LINE
		($a, $b, $c) = split(/\./, $value);
		$c = $c + 1;							#INCREMENT VALUE
		$line = "$field=$a.$b.$c\n";			#CREATE INCREMENTD LINE
	}

	if ($line =~ /^InternalRevision/)			#IF AT SECOND FIELD NEEDING CHANGES
	{
		($field, $value) = split(/=/, $line);	#PARSE LINE
		($a, $b, $c) = split(/\./, $value);
		$c = $c + 1;							#INCREMENT VALUE
		$line = "$field=$a.$b.$c\n";			#CREATE INCREMENTED LINE
	}
	
	print NEW $line;							#PRINT LINE TO THE TEMP FILE
}

close(NEW);										#CLOSE THE TEMP FILE
unlink(FILE);									#DELETE THE OLD FILE
close(FILE);									#CLOSE OLD FILE
rename($tempname,$filename);				 	#RENAME TMP FILE TO OLD FILE



#//////////////////////////////////////////////////////////////////////////////
#/////// INCREMENT NAVVER.H

$filename = "$root..\\..\\include\\build.h";
$tempname = "$root..\\..\\include\\build.tmp";

open(FILE, "+<$filename");							#OPEN UP build.h
open(NEW, ">$tempname");		  					#OPEN UP TEMP build.tmp


while ($line = <FILE>)								#STEP THROUGH LINE BY LINE
{		
	if ($line =~ /^#define VER_STR_BUILDNUMBER/)	#IF AT SECOND LINE NEEEDING CHANGES
	{
		($field, $value) = split(/\"/, $line);		#PARSE LINE
		$value = $value + 1;
		$line = "$field\"$value\"\n";				#CREATE INCREMENTED LINE
	}
	
	if ($line =~ /^#define VER_NUM_BUILDNUMBER/)	#IF AT FIRST LINE NEEDING CHANGES
	{
		($define, $field, $value) = split(/ /, $line);		#PARSE LINE
		$value = $value + 1;
		$line = "$define $field $value\n";					#CREATE INCREMENTED LINE
	}
		
	print NEW $line;								#COPY APPROPRIATE LINE INTO TEMP FILE			
}

close(NEW);											#CLOSE TEMP FILE
unlink(FILE);										#DELETE ORIGINAL FILE
close(FILE);										#CLOSE OLD FILE
rename($tempname,$filename);	#RENAME TEMP FILE TO ORIGINAL FILENAME


