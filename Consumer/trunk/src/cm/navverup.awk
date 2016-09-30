###
### navverupd.awk
### This script is called from update_nav_headers.btm
###
BEGIN {
    bCM=0; # Default value if not passed in       
    CM_BUILD_NUMBER=0;
    MonthNo=1;   # Default value if not passed in
    DayNo=1;     # Default value if not passed in
    YearNo=2003; # Default value if not passed in
}

$1 ~ /\#define/ && $2 ~/VER_STR_BUILDNUMBER/ {
    QBUILD_NUM=$3;
    gsub("\"","",QBUILD_NUM);
    OLD_CM_BUILD_NUM=QBUILD_NUM;
    gsub(OLD_CM_BUILD_NUM,CM_BUILD_NUMBER,$0);
    print $0;
    next;
}

$1 ~ /\#define/ && $2 ~/VER_NUM_BUILDNUMBER/ {
    BUILD_NUM=$3;
    OLD_CM_BUILD_NUM=BUILD_NUM;
    gsub(OLD_CM_BUILD_NUM,CM_BUILD_NUMBER,$0);
    print $0;
    next;
}

$1 ~ /\#define/ && $2 ~/VER_NUM_BUILDMONTH/ {
    OLDBUILDMONTH=$3;
    gsub(OLDBUILDMONTH,MonthNo,$0);
    print $0;
    next;
}

$1 ~ /\#define/ && $2 ~/VER_NUM_BUILDDAY/ {
    OLDBUILDDAY=$3;
    gsub(OLDBUILDDAY,DayNo,$0);
    print $0;
    next;
}

$1 ~ /\#define/ && $2 ~/VER_NUM_BUILDYEAR/ {
    OLDBUILDYEAR=$3;
    gsub(OLDBUILDYEAR,YearNo,$0);
    print $0;
    next;
}

$1 ~ /\#define/ && $2 ~/VER_STR_BUILDDATETUPLE/ {
    DATE_STRING=$3;                      ## Set variable
    gsub("\"","",DATE_STRING);           ## Remove the quotation marks
    split(DATE_STRING,DateParts,"/");    ## Split up by /
    OLDMONTH=DateParts[1];
    OLDDAY=DateParts[2];
    OLDYEAR=DateParts[3];
    OLD_DATE_STRING=sprintf("%s/%s/%s", OLDMONTH, OLDDAY, OLDYEAR);
    NEW_DATE_STRING=sprintf("%s/%s/%s", MonthNo, DayNo, YearNo);
    gsub(OLD_DATE_STRING,NEW_DATE_STRING,$0);
    print $0;
    next;
}

$1 ~ /PublicRevision=/ {
    FULL_STRING1=$1;                       ## Set variable
    split(FULL_STRING1,String1Parts,".");  ## Split up by .  The last part contains the build #.
    OLDSUB1=String1Parts[1];
    OLDSUB2=String1Parts[2];
    OLDSUB3=String1Parts[3];    
    OLDSUB4=String1Parts[4];    
    OLD_PUBLICREVISION_STRING=sprintf("%s.%s.%s.%s", OLDSUB1, OLDSUB2, OLDSUB3, OLDSUB4);
    NEW_PUBLICREVISION_STRING=sprintf("%s.%s.%s.%s", OLDSUB1, OLDSUB2, OLDSUB3, CM_BUILD_NUMBER);
    gsub(OLD_PUBLICREVISION_STRING,NEW_PUBLICREVISION_STRING,$0);
    print $0;
    next;
}

$1 ~ /InternalRevision=/ {
    FULL_STRING2=$1;                       ## Set variable
    split(FULL_STRING2,String2Parts,".");  ## Split up by .  The last part contains the build #.
    OLDSUB1=String2Parts[1];
    OLDSUB2=String2Parts[2];
    OLDSUB3=String2Parts[3];    
    OLDSUB4=String2Parts[4];    
    OLD_INTERNALREVISION_STRING=sprintf("%s.%s.%s.%s", OLDSUB1, OLDSUB2, OLDSUB3, OLDSUB4);
    NEW_INTERNALREVISION_STRING=sprintf("%s.%s.%s.%s", OLDSUB1, OLDSUB2, OLDSUB3, CM_BUILD_NUMBER);
    gsub(OLD_INTERNALREVISION_STRING,NEW_INTERNALREVISION_STRING,$0);
    print $0;
    next;
}


{
    print $0;
}



