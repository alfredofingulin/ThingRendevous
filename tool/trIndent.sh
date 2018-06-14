#!/bin/sh
indent -linux -l120 -i4 -nut inc/trAPI.h
# indent screws up -linux -l120 -i4 -nut inc/trAPIEI.h
indent -linux -l120 -i4 -nut inc/trAPIRet.h
indent -linux -l120 -i4 -nut src/trAPI.c
indent -linux -l120 -i4 -nut src/trB.h
indent -linux -l120 -i4 -nut src/trBidt.c
indent -linux -l120 -i4 -nut src/trBidt.h
indent -linux -l120 -i4 -nut src/trDump.c
indent -linux -l120 -i4 -nut src/trEvt.c
indent -linux -l120 -i4 -nut src/trEvt.h
indent -linux -l120 -i4 -nut src/trEvtP.c
indent -linux -l120 -i4 -nut src/trEvtP.h
indent -linux -l120 -i4 -nut src/trKhf.c
indent -linux -l120 -i4 -nut src/trKhf.h
indent -linux -l120 -i4 -nut src/trKIDS.h
indent -linux -l120 -i4 -nut src/trM.c
indent -linux -l120 -i4 -nut src/trM.h
indent -linux -l120 -i4 -nut src/trMsg.c
indent -linux -l120 -i4 -nut src/trMsg.h
indent -linux -l120 -i4 -nut src/trMsgP.c
indent -linux -l120 -i4 -nut src/trMsgP.h
indent -linux -l120 -i4 -nut src/trPl.c
indent -linux -l120 -i4 -nut src/trPl.h
indent -linux -l120 -i4 -nut src/trSClk.c
indent -linux -l120 -i4 -nut src/trSClk.h
indent -linux -l120 -i4 -nut src/trSR.c
indent -linux -l120 -i4 -nut src/trSR.h
# indent screws up -linux -l120 -i4 -nut src/trStart.c
indent -linux -l120 -i4 -nut src/trStart.h
indent -linux -l120 -i4 -nut src/trStrm.c
indent -linux -l120 -i4 -nut src/trStrm.h
indent -linux -l120 -i4 -nut src/trStrmP.c
indent -linux -l120 -i4 -nut src/trStrmP.h
indent -linux -l120 -i4 -nut src/trTei.c
indent -linux -l120 -i4 -nut src/trTei.h
indent -linux -l120 -i4 -nut src/trT.c
indent -linux -l120 -i4 -nut src/trT.h
# trTi.c created by trtgen
indent -linux -l120 -i4 -nut src/trTi.h
indent -linux -l120 -i4 -nut src/trTP.c
indent -linux -l120 -i4 -nut src/trTP.h
indent -linux -l120 -i4 -nut src/trTS.h
indent -linux -l120 -i4 -nut src/trTstc.c
indent -linux -l120 -i4 -nut src/trTstc.h
# indent screws up -linux -l120 -i4 -nut src/trTti.c
indent -linux -l120 -i4 -nut src/trTti.h
# indent screws up -linux -l120 -i4 -nut src/truPACM4.c
# indent screws up -linux -l120 -i4 -nut src/truPACM4.h
indent -linux -l120 -i4 -nut src/trTti.h
# ----
# indent no vendor supplied files -linux -l120 -i4 -nut ext/STM32F429Nucleo144/Drivers/...
# ----
# indent no source binary recovery -linux -l120 -i4 -nut ext/STM32F429Nucleo144/HandyStuff/...
# ----
# indent no leave alone for easy compare to existing files -linux -l120 -i4 -nut ext/STM32F429Nucleo144/Goodies/...
# ----
# indent no leave alone for easy compare to existing files -linux -l120 -i4 -nut ext/STM32F429Nucleo144/trDrivers/...
# ----
indent -linux -l120 -i4 -nut test/STM32F429Nucleo144/bmTst00/main.c
indent -linux -l120 -i4 -nut test/STM32F429Nucleo144/bmTst01/main.c
indent -linux -l120 -i4 -nut test/STM32F429Nucleo144/trTst00/main.c
indent -linux -l120 -i4 -nut test/STM32F429Nucleo144/trTst01/main.c
indent -linux -l120 -i4 -nut test/STM32F429Nucleo144/trTst02/main.c
indent -linux -l120 -i4 -nut test/STM32F429Nucleo144/trTst03/main.c
indent -linux -l120 -i4 -nut test/STM32F429Nucleo144/trTst04/main.c
indent -linux -l120 -i4 -nut test/STM32F429Nucleo144/trTst05/main.c
indent -linux -l120 -i4 -nut test/STM32F429Nucleo144/trTst06/main.c
indent -linux -l120 -i4 -nut test/STM32F429Nucleo144/trTst07/main.c
