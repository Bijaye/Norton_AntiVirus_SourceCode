# Copyright 1992-1993 Symantec, Peter Norton Product Group
#########################################################################
#                                                                       #
# $Header:   S:/CERTLIB4/VCS/certlibe.mav   1.0   10 Dec 1996 16:41:16   MZAREMB  $                                                        #
#                                                                       #
# Description:                                                          #
#       This is a generic Make file for compiling certlib without       #
#       NAVEX enabled code.                                             #
#                                                                       #
#########################################################################
#
# $Log:   S:/CERTLIB4/VCS/certlibe.mav  $
#  
#     Rev 1.0   10 Dec 1996 16:41:16   MZAREMB
#  Initial revision.
#  
#     Rev 1.0.1.0   23 Oct 1995 15:51:32   DCHI
#  Branch base for QAK2.
#  
#     Rev 1.0   23 Oct 1995 15:50:46   DCHI
#  Initial revision.
#  
########################################################################

TARGET_NAME=CERTLIBE
PROGRAM_NAME=$(TARGET_NAME).LIB

.INCLUDE $(QUAKEINC)\MASM6.MAK
LIBS=
CVL=
CVA=
CVC=

%if "$[u,$(PLATFORM)]" != ".MIR"
MA=MLX
%else
MA=ML
%Endif

COMMONOBJS=\
    fcopy.obj  \
    fcopyh.obj \
    fcrc.obj   \
    fisat.obj  \
    fisro.obj  \
    fprot.obj  \
    ir_cmp35.obj \
    ir_cpbak.obj \
    ir_detmn.obj \
    ir_fnd35.obj \
    ir_main.obj  \
    fstart.obj \
    fstat.obj  \
    fkill.obj  \
    ffall.obj  \
    ffwild.obj \
    fsize.obj  \
    mcrc.obj   \
    mgeto.obj  \
    mseto.obj  \
    dwtoa.obj  \
    datow.obj  \
    twtoa.obj  \
    tatow.obj  \
    dbopen.obj \
    dbopcre.obj\
    dbclose.obj\
    dbcreat.obj\
    dbsrch.obj \
    dbdel.obj  \
    dbinst.obj \
    dbgetent.obj \
    swtoa.obj  \
    satow.obj  \
    sitoa.obj  \
    satoi.obj  \
    cdw.obj    \
    cup.obj    \
    strdw.obj  \
    strup.obj  \
    nmod.obj   \
    scn_del.obj\
    scn_alg.obj\
    getindat.obj\
    onehalf.obj\
    scn_boo.obj\
    scn_crc.obj\
    scn_me.obj \
    scn_mem.obj\
    scn_mte.obj\
    scn_mte2.obj \
    scn_malt.obj \
    scn_v2px.obj \
    scn_n20.obj\
    tsplit.obj \
    dsplit.obj \
    drlph.obj \
    dwlph.obj \
%If "$[u,$(PLATFORM)]" == ".MIR"
    mir_drwl.obj \
    mir_drwp.obj \
%Else
    drl.obj \
    dwl.obj \
    drph.obj \
    dwph.obj \
%EndIf
    strahh.obj\
    MEMREP.OBJ\
    MEMSA.OBJ\
    MEMSC.OBJ\
    MEMSET.OBJ\
    MEMSS.OBJ\
    STRCP.OBJ\
    STRCTO.OBJ\
    STRL.OBJ\
    STRLTO.OBJ\
    STRFORM.OBJ\
    scninit1.obj\
    scninit2.obj\
    scninit3.obj\
    scninit4.obj\
    scninit5.obj\
    scninit6.obj\
    scninit7.obj\
    scninit8.obj\
    scninit9.obj

DOSOBJS=dll1.obj \
    dll2.obj \
    dll3.obj

WINOBJS=


########################################################################
# DOS version of CERTLIBE
########################################################################
%If ("$(PLATFORM)" == ".DOS" || "$(PLATFORM)" == ".dos")

MODEL=M
ASMOPTS=/c /DDOS16 /DNONAVEX /WX

$(PROGRAM_NAME): $(COMMONOBJS)\
                 $(DOSOBJS)

doalways.dep:
        echo doalways.dep

scn_me.obj: doalways.dep

scn_alg.obj: doalways.dep

%EndIf



#UPDATE#

bbutt.obj:	CTS4.INC 
bclos.obj:	CTS4.INC 
bopen.obj:	CTS4.INC 
bprn.obj:	CTS4.INC 
bprnt.obj:	CTS4.INC 
cdw.obj:	CTS4.INC 
chrprn.obj:	CTS4.INC 
cmbc.obj:	CTS4.INC 
cmbd.obj:	CTS4.INC 
cmbl.obj:	CTS4.INC 
cmbo.obj:	CTS4.INC 
cmbr.obj:	CTS4.INC 
cmbw.obj:	CTS4.INC 
cmbx.obj:	CTS4.INC 
cup.obj:	CTS4.INC 
datow.obj:	CTS4.INC 
dbclose.obj:	CTS4.INC 
dbcreat.obj:	CTS4.INC 
dbdel.obj:	CTS4.INC 
dbinst.obj:	CTS4.INC 
dblquick.obj:	CTS4.INC 
dbopcre.obj:	CTS4.INC 
dbopen.obj:	CTS4.INC 
dbsrch.obj:	CTS4.INC 
dbsrq.obj:      CTS4.INC
dbgetent.obj:   CTS4.INC
dcvasc.obj:	CTS4.INC 
dcvasn.obj:	CTS4.INC 
dcvclus.obj:	CTS4.INC 
dcvlsn.obj:	CTS4.INC 
ddint13.obj:	CTS4.INC 
ddint25.obj:	CTS4.INC 
strcomp.obj:	CTS4.INC 
strcp.obj:	CTS4.INC 
strcs.obj:	CTS4.INC 
strcto.obj:	CTS4.INC 
strdw.obj:	CTS4.INC 
strend.obj:	CTS4.INC 
strform.obj:	CTS4.INC _TSR.INC 
stricomp.obj:	CTS4.INC 
strl.obj:	CTS4.INC 
strll.obj:	CTS4.INC 
strlto.obj:	CTS4.INC 
strpr.obj:	CTS4.INC 
strup.obj:	CTS4.INC 
stsrch.obj:	CTS4.INC 
swtoa.obj:	CTS4.INC 
sysenv.obj:	CTS4.INC 
sysfenv.obj:	CTS4.INC 
sysgv.obj:	CTS4.INC 
syshalt.obj:	CTS4.INC 
sysps2.obj:	CTS4.INC 
scn_alg.obj:    CTS4.INC
getindat.obj:   CTS4.INC repair.inc 1half.inc
onehalf.obj:   CTS4.INC repair.inc 1half.inc
scn_del.obj:    CTS4.INC
fwl.obj:	CTS4.INC 
syswhalt.obj:	CTS4.INC 
tatow.obj:	CTS4.INC 
tenc.obj:	CTS4.INC 
tsplit.obj:	CTS4.INC 
twtoa.obj:	CTS4.INC 
memreall.obj:	CTS4.INC 
keydelay.obj:	CTS4.INC 
keychar.obj:	CTS4.INC 
scn_crc.obj:	CTS4.INC 
swtoah.obj:	CTS4.INC 
disbpb.obj:	CTS4.INC 
rep4_com.obj:	CTS4.INC 
rep4_cal.obj:	CTS4.INC 
sdwcon.obj:	CTS4.INC 
rep4_exe.obj:	CTS4.INC 
rep4_fil.obj:	CTS4.INC 
rep4_gen.obj:	CTS4.INC 
scn_me.obj:	CTS4.INC 
scn_mte.obj:	CTS4.INC 
scninitb.obj:	CTS4.INC 
scninita.obj:	CTS4.INC 
fincache.obj:	CTS4.INC 
indata.obj:	CTS4.INC 
strcc.obj:	CTS4.INC 
dis4_mbr.obj:	CTS4.INC REPAIR.INC 
dll1.obj:	CTS4.INC 
dll2.obj:	CTS4.INC 
dll4.obj:	CTS4.INC 
dll5.obj:	CTS4.INC 
dll6.obj:	CTS4.INC 
dosmcb.obj:	CTS4.INC 
dosmyt.obj:	CTS4.INC 
dostsr.obj:	CTS4.INC 
dosunv.obj:	CTS4.INC 
dosvec.obj:	CTS4.INC 
dreset.obj:	CTS4.INC 
drl.obj:	CTS4.INC 
drlph.obj:	CTS4.INC 
drph.obj:	CTS4.INC 
dsplit.obj:	CTS4.INC 
dwint13.obj:	CTS4.INC 
dwint25.obj:	CTS4.INC 
dwint26.obj:	CTS4.INC 
dwl.obj:	CTS4.INC 
dwlph.obj:	CTS4.INC 
dwph.obj:	CTS4.INC 
dwsetio.obj:	CTS4.INC 
dwtoa.obj:	CTS4.INC 
fattr.obj:	CTS4.INC 
fcb4_clo.obj:	CTS4.INC 
discrrc.obj:	CTS4.INC 
sacon.obj:	CTS4.INC 
satoi.obj:	CTS4.INC 
fcc.obj:	CTS4.INC 
fcdir.obj:	CTS4.INC 
fcdta.obj:	CTS4.INC 
fchdir.obj:	CTS4.INC 
fclose.obj:	CTS4.INC 
fcopy.obj:	CTS4.INC 
fcrc.obj:	CTS4.INC 
othcl.obj:	CTS4.INC 
fdel.obj:	CTS4.INC 
fdir.obj:	CTS4.INC 
fdrive.obj:	CTS4.INC 
fdtime.obj:	CTS4.INC 
fdup.obj:	CTS4.INC 
fexists.obj:	CTS4.INC 
ffall.obj:	CTS4.INC 
ffindf.obj:	CTS4.INC 
fflush.obj:	CTS4.INC 
ffwild.obj:     CTS4.INC
fgdta.obj:	CTS4.INC 
fgetexd.obj:	CTS4.INC 
fgetexn.obj:	CTS4.INC 
fhton.obj:	CTS4.INC 
filog.obj:	CTS4.INC 
filst.obj:	CTS4.INC 
fisat.obj:	CTS4.INC 
fisro.obj:	CTS4.INC 
fkill.obj:	CTS4.INC 
fmarc.obj:	CTS4.INC 
fnext.obj:	CTS4.INC 
ddint26.obj:	CTS4.INC 
ddsetio.obj:	CTS4.INC 
denc.obj:	CTS4.INC 
dis4_boo.obj:	CTS4.INC REPAIR.INC 
memsc.obj:	CTS4.INC 
memset.obj:	CTS4.INC 
memsf.obj:	CTS4.INC 
memsl.obj:	CTS4.INC 
memsp.obj:	CTS4.INC 
memss.obj:	CTS4.INC 
memsu.obj:	CTS4.INC 
mgeto.obj:	CTS4.INC 
disbtst.obj:	CTS4.INC 
mseto.obj:	CTS4.INC 
mumba.obj:	CTS4.INC 
mumbf.obj:	CTS4.INC 
mwconva.obj:	CTS4.INC 
mwconvf.obj:	CTS4.INC 
mwstos.obj:	CTS4.INC 
mxmsa.obj:	CTS4.INC 
mxmsf.obj:	CTS4.INC 
mxmsl.obj:	CTS4.INC 
mxmsu.obj:	CTS4.INC 
ngeta.obj:	CTS4.INC 
ngetl.obj:	CTS4.INC 
ngtuc.obj:	CTS4.INC 
nmod.obj:	CTS4.INC 
novtest.obj:	CTS4.INC 
nsend.obj:	CTS4.INC 
nsendl.obj:	CTS4.INC 
fwlog.obj:	cts4.inc 
novuser.obj:	CTS4.INC 
dll3.obj:	CTS4.INC 
foc.obj:	CTS4.INC 
memfree.obj:	CTS4.INC 
memhma.obj:	CTS4.INC 
fopen.obj:	CTS4.INC 
fprot.obj:	CTS4.INC 
fred.obj:	CTS4.INC 
frename.obj:	CTS4.INC 
frlc.obj:	CTS4.INC 
fseek.obj:	CTS4.INC 
fsize.obj:	CTS4.INC 
fsp.obj:	CTS4.INC 
fstat.obj:	CTS4.INC 
memunlck.obj:	CTS4.INC 
fwrit.obj:	CTS4.INC 
syssda.obj:	CTS4.INC 
get2_13.obj:	CTS4.INC 
get13.obj:	CTS4.INC 
ginipi.obj:	CTS4.INC 
mhmaa.obj:	CTS4.INC 
keyshst.obj:	CTS4.INC 
mconva.obj:	CTS4.INC 
mconvf.obj:	CTS4.INC 
mcrc.obj:	CTS4.INC 
mdstos.obj:	CTS4.INC 
memconv.obj:	CTS4.INC 
fstart.obj:	CTS4.INC 
memcopy.obj:	CTS4.INC 
memctoc.obj:	CTS4.INC 
mememsa.obj:	CTS4.INC 
satow.obj:	CTS4.INC 
dislcbt.obj:	CTS4.INC 
fcb4_opn.obj:	CTS4.INC 
fcb4_rea.obj:	CTS4.INC 
fcb4_siz.obj:	CTS4.INC 
fcb4_wri.obj:	CTS4.INC 
rep4_sys.obj:	CTS4.INC 
saddl.obj:	CTS4.INC 
dishdmbr.obj:	CTS4.INC 
sblan.obj:	CTS4.INC 
scnget.obj:	CTS4.INC 
scninit1.obj:	CTS4.INC 
scninit2.obj:	CTS4.INC 
scninit3.obj:	CTS4.INC 
scninit4.obj:	CTS4.INC 
scninit5.obj:	CTS4.INC 
scninit6.obj:	CTS4.INC 
scninit7.obj:	CTS4.INC 
scninit8.obj:	CTS4.INC 
scninit9.obj:	CTS4.INC 
sysspec.obj:	CTS4.INC 
frl.obj:	CTS4.INC 
scn_n20.obj:	CTS4.INC 
scn_mem.obj:	CTS4.INC 
memdest.obj:	CTS4.INC 
meminit.obj:	CTS4.INC 
scrdisp.obj:	CTS4.INC 
scrgat.obj:	CTS4.INC 
scrgcu.obj:	CTS4.INC 
scrgpg.obj:	CTS4.INC 
sgatt.obj:	CTS4.INC 
sgvid.obj:	CTS4.INC 
sitoa.obj:	CTS4.INC 
stapp.obj:	CTS4.INC 
strahh.obj:	CTS4.INC 
memsa.obj:	CTS4.INC 
scn_boo.obj:	CTS4.INC 
dismodbt.obj:	CTS4.INC 
disisbt.obj:	CTS4.INC 
disdpb.obj:	CTS4.INC 
disbtnm.obj:	CTS4.INC 
dismvmbr.obj:	CTS4.INC 
disrbs.obj:	CTS4.INC 
disunbt.obj:	CTS4.INC 
disrmbr.obj:	CTS4.INC 
disroot.obj:	CTS4.INC 
bantest.obj:	CTS4.INC 
fcopyh.obj:	CTS4.INC 
memalloc.obj:	CTS4.INC 
memlock.obj:	CTS4.INC 
sysgn.obj:	CTS4.INC 
funmarc.obj:	CTS4.INC 
filcrcfl.obj:	CTS4.INC 
winips.obj:	CTS4.INC 
ginips.obj:	CTS4.INC 
sahtow.obj:	CTS4.INC 
memrep.obj:	CTS4.INC 
scn_malt.obj:	CTS4.INC 
scn_mte2.obj:	CTS4.INC 
scn_v2px.obj:	CTS4.INC 
disclab.obj:	CTS4.INC 
discllg.obj:	CTS4.INC 
dislgab.obj:	CTS4.INC 
dispart.obj:	CTS4.INC 
dislfat.obj:	CTS4.INC 
disunfat.obj:	CTS4.INC 
disgefat.obj:	CTS4.INC 
disrecit.obj:	CTS4.INC 
fgdfn.obj:	CTS4.INC 
fmedia.obj:	CTS4.INC 
ir_cmp35.obj:	CTS4.INC 
ir_cpbak.obj:	CTS4.INC 
ir_detmn.obj:	CTS4.INC 
ir_fnd35.obj:	CTS4.INC 
ir_main.obj:	CTS4.INC 
filap.obj:	CTS4.INC 
x1.obj:         CTS4.INC
keyintc.obj:	CTS4.INC

#ENDUPDATE#
