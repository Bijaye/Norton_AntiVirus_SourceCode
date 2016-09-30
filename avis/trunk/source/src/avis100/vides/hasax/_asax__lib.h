#ifndef __ASAX_LIB__
#define __ASAX_LIB__

// Here are the C functions that can be called from within asax rules
// The fields are:
//  1) the identifier as it appears in the rules
//  2) the C function name
//  3) the number of arguments the function takes (-1 means variable number)
//  4) the return type
//  5) the index in the parameter array (following this one) describing the types of the successive parameters


/*
   The functions have to be sorted in alphabetical order (for binary lookups).
   The functions that are correctly indented are the ones we're using for the moment.
   The other ones are pretty much useless and were never tested.
 */

#define tfp_descr_lg 86

fp_descr tfp_descr[tfp_descr_lg] = {
	"accept"			,accept          	,0	,integer   	,0,
	"append"        	,append          	,2	,bytestring	,0,
	"bitand"			,bitand           ,2 ,integer    ,10,
	"bitor"            ,bitor            ,2 ,integer    ,10,
	"bitxor"           ,bitxor           ,2 ,integer    ,10,
	"bootTriage"			,bootTriage			,0		,integer	,0,
	"byte2h"				,byte2h				,1		,bytestring	,6,
	"bytesToInt"			,bytesToInt			,1		,integer	,2,
	"cancel"				,cancel				,0		,undef		,0,
	"closeNADF"				,closeNADF			,1		,integer	,7,
	"creatNADF"				,creatNADF			,1		,integer	,8,
	"extractFDBootSector"	,extractFDBS		,0		,integer	,0,
	"extractHDMBR"			,extractHDMBR		,0		,integer	,0,
	"extractHDPBS"			,extractHDPBS		,0		,integer	,0,
	"extractfile"			,extractfile		,1		,integer	,0,
	"extractsec"			,extractsec			,6		,integer	,17,
	"getextoffile"			,getextoffile		,1		,bytestring	,0,
	"getnameoffile"			,getnameoffile		,1		,bytestring	,0,
	"getnextgoat"			,getnextgoat		,0		,bytestring	,0,
	"int2h"					,int2h				,1		,bytestring	,6,
	"match"					,match				,2		,integer	,3,
	"mem_attach_marker"		,mem_attach_marker	,3		,undef		,13,
	"mem_detach_marker"		,mem_detach_marker	,1		,undef		,13,
	"print"					,print				,-1		,undef		,0,
	"println"				,println			,-1		,undef		,0,
	"priority"				,priority			,1		,integer	,0,
	"readASCIIZ"			,readASCIIZ			,3		,bytestring	,13,
	"readASCIIdollar"		,readASCIIdollar	,3		,bytestring	,13,
	"readmemword"			,readmemword		,2		,integer	,13,
	"reg_attach_marker"		,reg_attach_marker	,2		,undef		,13,
	"reg_detach_marker"		,reg_detach_marker	,1		,undef		,13,
	"replaceFloppyA"		,replaceFloppyA		,0		,undef		,0,
	"rh"					,rH					,1		,integer	,5,
	"rl"					,rL					,1		,integer	,5,
	"rx"					,rX					,1		,integer	,5,
	"sendln"				,sendln				,1		,undef		,0,
	"sendscancode"			,sendscancode		,1		,undef		,6,
	"sendtxt"				,sendtxt			,1		,undef		,0,
	"setac"          ,setac           ,1 ,undef      ,15,
	"setaf"          ,setaf           ,1 ,undef      ,15,
	"setbrkpt"				,setbrkpt			,3		,undef		,13,
	"setcf"          ,setcf           ,1 ,undef      ,15,
	"setcs"          ,setcs           ,1 ,undef      ,15,
	"setdf"          ,setdf           ,1 ,undef      ,15,
	"setds"          ,setds           ,1 ,undef      ,15,
	"seteax"          ,seteax           ,1 ,undef      ,15,
	"setebp"          ,setebp           ,1 ,undef      ,15,
	"setebx"          ,setebx           ,1 ,undef      ,15,
	"setecx"          ,setecx           ,1 ,undef      ,15,
	"setedi"          ,setedi           ,1 ,undef      ,15,
	"setedx"          ,setedx           ,1 ,undef      ,15,
	"seteip"          ,seteip           ,1 ,undef      ,15,
	"setes"          ,setes           ,1 ,undef      ,15,
	"setesi"          ,setesi           ,1 ,undef      ,15,
	"setesp"          ,setesp           ,1 ,undef      ,15,
	"setfs"          ,setfs           ,1 ,undef      ,15,
	"setgs"          ,setgs           ,1 ,undef      ,15,
	"setif"          ,setif           ,1 ,undef      ,15,
	"setiopl"          ,setiopl           ,1 ,undef      ,15,
	"setnt"          ,setnt           ,1 ,undef      ,15,
	"setof"          ,setof           ,1 ,undef      ,15,
	"setpf"          ,setpf           ,1 ,undef      ,15,
	"setrf"          ,setrf           ,1 ,undef      ,15,
	"setsf"          ,setsf           ,1 ,undef      ,15,
	"setss"          ,setss           ,1 ,undef      ,15,
	"settf"          ,settf           ,1 ,undef      ,15,
	"setvm"          ,setvm           ,1 ,undef      ,15,
	"setzf"          ,setzf           ,1 ,undef      ,15,
	"start_watch_brkpt"		,start_watch_brkpt	,0		,undef		,0,
	"start_watch_func"		,start_watch_func	,3		,undef		,13,
	"start_watch_int"		,start_watch_int	,2		,undef		,13,
	"start_watch_io"		,start_watch_io		,2		,undef		,13,
	"start_watch_irq"		,start_watch_irq	,1		,undef		,13,
	"start_watch_mem"		,start_watch_mem	,0		,undef		,0,
	"stop_program"			,stop_program		,1		,undef		,0,
	"stop_watch_brkpt"		,stop_watch_brkpt	,0		,undef		,0,
	"stop_watch_int"		,stop_watch_int		,1		,undef		,13,
	"stop_watch_io"			,stop_watch_io		,2		,undef		,13,
	"stop_watch_irq"		,stop_watch_irq		,1		,undef		,13,
	"stop_watch_mem"		,stop_watch_mem		,0		,undef		,0,
	"str2h"					,str2h				,1		,bytestring	,9,
	"strToInt"				,strToInt			,1		,integer	,5,
	"strToU"				,strToU				,1		,integer	,5,
	"toggle_watch_ins"		,toggle_watch_ins	,2		,undef		,14,
	"word2h"				,word2h				,1		,bytestring	,6,
	"writeNADF"    	,writeNADF       	,1	,integer   	,6
	};

// The following array describes variable types and passing methods
// It is pointed to by the functions array (above)

// note: this array is far from optimal, it could be made less redundant
// but there are other, more urgent, things to do...

#define targ_descr_lg 23

arg_descr targ_descr[targ_descr_lg] = {
/*  0 */		value	,bytestring,
/*  1 */		value	,bytestring,
/*  2 */		value	,bytestring,
/*  3 */		value	,bytestring,
/*  4 */		value	,bytestring,
/*  5 */		value	,bytestring,
/*  6 */		value	,integer,
/*  7 */		value	,integer,
/*  8 */		value	,bytestring,
/*  9 */		value	,bytestring,
/* 10 */		value	,integer,
/* 11 */		value	,integer,
/* 12 */		value	,bytestring,
/* 13 */		value	,integer,
/* 14 */		value	,integer,
/* 15 */		value	,integer,
/* 16 */		ref		,bytestring,
/* 17 */		value	,integer,
/* 18 */	    value	,integer,
/* 19 */		value	,integer,
/* 20 */		value	,integer,
/* 21 */		value	,integer,
/* 22 */		value	,integer,

		};


#endif
