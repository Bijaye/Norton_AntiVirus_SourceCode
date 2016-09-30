#########################################################################
#
#
# $Header:   S:/SETUPBLD/VCS/setupbld.mav   1.8   18 Sep 1998 18:56:26   VGLOSTE  $
#
#########################################################################
# $Log:   S:/SETUPBLD/VCS/setupbld.mav  $ #
#  
#     Rev 1.8   18 Sep 1998 18:56:26   VGLOSTE
#  Changed the /D item for custome OEM builds
#  
#     Rev 1.7   18 Sep 1998 17:11:14   VGLOSTE
#  Added build commands for the files cRos95.ins cRosNT.ins cTesla.ins and
#  cTeslaA.ins.
#  
#     Rev 1.6   19 Feb 1998 16:03:52   mdunn
#  Added NEC versions.
#  
#     Rev 1.5   24 Oct 1997 20:23:54   jtaylor
#  -- Added the oem install builders
#
#     Rev 1.4   17 Jul 1997 19:57:54   JBRENNA
#  Define PMODE_OVERRIDE in the header files instead of in the MAK file.
#
#     Rev 1.3   11 Jul 1997 20:11:14   JBRENNA
#  Fulling integrate Access database and Trial versions.
#
#     Rev 1.2   11 Jul 1997 10:54:40   JBRENNA
#  Add a target for TeslaAxp.INS.
#
#     Rev 1.1   08 Jul 1997 20:00:34   JBRENNA
#  Add RosNT.ins and Tesla.ins to the build.
#
#     Rev 1.0   08 Jul 1997 11:36:30   JBRENNA
#  Initial revision.
#
#########################################################################

All: oRos95.ins oRosNt.ins oTesla.ins oTeslaA.ins Ros95.ins RosNt.ins  \
     Tesla.ins TeslaAxp.ins Ros95Tr.ins RosNtTr.ins TeslaTr.ins TeslaATr.ins \
     Ros95NEC.ins RosNTNEC.ins TeslaNEC.ins \
     Ros95NECTr.ins RosNTNECTr.ins TeslaNECTr.ins \
	 cRos95.ins cRosNt.ins cTesla.ins cTeslaA.ins 

Ros95NEC.ins : Ros95NEC.h setup.rul setupbld.h NavCom.h
    if exist Ros95NEC.ins del Ros95NEC.ins
    if exist setup.ins del setup.ins
    compile /DROS95NEC setup
    move setup.ins Ros95NEC.ins

RosNTNEC.ins : RosNTNEC.h setup.rul setupbld.h NavCom.h
    if exist RosNTNEC.ins del RosNTNEC.ins
    if exist setup.ins del setup.ins
    compile /DROSNTNEC setup
    move setup.ins RosNTNEC.ins

TeslaNEC.ins : TeslaNEC.h setup.rul setupbld.h NavCom.h
    if exist TeslaNEC.ins del TeslaNEC.ins
    if exist setup.ins del setup.ins
    compile /DTESLANEC setup
    move setup.ins TeslaNEC.ins

Ros95NECTr.ins : Ros95NECTr.h setup.rul setupbld.h NavCom.h
    if exist Ros95NECTr.ins del Ros95NECTr.ins
    if exist setup.ins del setup.ins
    compile /DROS95NEC_TRIAL setup
    move setup.ins Ros95NECTr.ins

RosNTNECTr.ins : RosNTNECTr.h setup.rul setupbld.h NavCom.h
    if exist RosNTNECTr.ins del RosNTNECTr.ins
    if exist setup.ins del setup.ins
    compile /DROSNTNEC_TRIAL setup
    move setup.ins RosNTNECTr.ins

TeslaNECTr.ins : TeslaNECTr.h setup.rul setupbld.h NavCom.h
    if exist TeslaNECTr.ins del TeslaNECTr.ins
    if exist setup.ins del setup.ins
    compile /DTESLANEC_TRIAL setup
    move setup.ins TeslaNECTr.ins

oRos95.ins:  oRos95.h   setup.rul  setupbld.h NavCom.h
    if exist oRos95.ins del oRos95.ins
    if exist setup.ins del setup.ins
    compile /DOROS95 setup
    move setup.ins oRos95.ins

oRosNt.ins:  oRosNt.h   setup.rul  setupbld.h NavCom.h
    if exist oRosNt.ins del oRosNt.ins
    if exist setup.ins del setup.ins
    compile /DOROSNT setup
    move setup.ins oRosNt.ins

oTesla.ins:  oTesla.h   setup.rul  setupbld.h NavCom.h
    if exist oTesla.ins del oTesla.ins
    if exist setup.ins del setup.ins
    compile /DOTESLA setup
    move setup.ins oTesla.ins

oTeslaA.ins:  oTeslaA.h   setup.rul  setupbld.h NavCom.h
    if exist oTeslaA.ins del oTeslaA.ins
    if exist setup.ins del setup.ins
    compile /DOTESLA_AXP setup
    move setup.ins oTeslaA.ins

Ros95.ins:  Ros95.h   setup.rul  setupbld.h NavCom.h
    if exist Ros95.ins del Ros95.ins
    if exist setup.ins del setup.ins
    compile /DROS95 setup
    move setup.ins Ros95.ins

RosNt.ins:  RosNt.h   setup.rul  setupbld.h NavCom.h
    if exist RosNt.ins del RosNt.ins
    if exist setup.ins del setup.ins
    compile /DROSNT setup
    move setup.ins RosNt.ins

Tesla.ins:  Tesla.h   setup.rul  setupbld.h NavCom.h
    if exist Tesla.ins del Tesla.ins
    if exist setup.ins del setup.ins
    compile /DTESLA setup
    move setup.ins Tesla.ins

TeslaAxp.ins:  TeslaAxp.h   setup.rul  setupbld.h NavCom.h
    if exist TeslaAxp.ins del TeslaAxp.ins
    if exist setup.ins del setup.ins
    compile /DTESLA_AXP setup
    move setup.ins TeslaAxp.ins

Ros95Tr.ins:  Ros95Tr.h   setup.rul  setupbld.h NavCom.h
    if exist Ros95Tr.ins del Ros95Tr.ins
    if exist setup.ins del setup.ins
    compile /DROS95_TRIAL setup
    move setup.ins Ros95Tr.ins

RosNtTr.ins:  RosNtTr.h   setup.rul  setupbld.h NavCom.h
    if exist RosNtTr.ins del RosNtTr.ins
    if exist setup.ins del setup.ins
    compile /DROSNT_TRIAL setup
    move setup.ins RosNtTr.ins

TeslaTr.ins:  TeslaTr.h   setup.rul  setupbld.h NavCom.h
    if exist TeslaTr.ins del TeslaTr.ins
    if exist setup.ins del setup.ins
    compile /DTESLA_TRIAL setup
    move setup.ins TeslaTr.ins

TeslaATr.ins:  TeslaATr.h   setup.rul  setupbld.h NavCom.h
    if exist TeslaATr.ins del TeslaATr.ins
    if exist setup.ins del setup.ins
    compile /DTESLA_AXP_TRIAL setup
    move setup.ins TeslaATr.ins

cRos95.ins:  cRos95.h   setup.rul  setupbld.h NavCom.h
    if exist cRos95.ins del cRos95.ins
    if exist setup.ins del setup.ins
    compile /DCROS95 setup
    move setup.ins cRos95.ins

cRosNt.ins:  cRosNt.h   setup.rul  setupbld.h NavCom.h
    if exist cRosNt.ins del cRosNt.ins
    if exist setup.ins del setup.ins
    compile /DCROSNT setup
    move setup.ins cRosNt.ins

cTesla.ins:  cTesla.h   setup.rul  setupbld.h NavCom.h
    if exist cTesla.ins del cTesla.ins
    if exist setup.ins del setup.ins
    compile /DCTESLA setup
    move setup.ins cTesla.ins

cTeslaA.ins:  cTeslaA.h   setup.rul  setupbld.h NavCom.h
    if exist cTeslaA.ins del cTeslaA.ins
    if exist setup.ins del setup.ins
    compile /DCTESLA_AXP setup
    move setup.ins cTeslaA.ins

