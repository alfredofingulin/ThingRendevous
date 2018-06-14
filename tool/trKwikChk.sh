#!/bin/sh
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trAPI.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trBidt.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trDump.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trEvt.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trEvtP.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trKhf.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trM.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trMsg.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trMsgP.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trPl.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trSClk.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trSR.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trStart.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trStrm.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trStrmP.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trT.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trTei.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trTP.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trTstc.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/trTti.c
gcc -c -Wall -I./ -I ./src -I ./inc ./src/truPACM4.c
ar cr lib_trKwikChk.a \
trAPI.o \
trBidt.o \
trDump.o \
trEvt.o \
trEvtP.o \
trKhf.o \
trM.o \
trMsg.o \
trMsgP.o \
trPl.o \
trSClk.o \
trSR.o \
trStart.o \
trStrm.o \
trStrmP.o \
trT.o \
trTei.o \
trTP.o \
trTstc.o \
trTti.o \
truPACM4.o
rm trKwikChk.map
ld -o trKwikChk.o \
-Map=trKwikChk.map \
-cref \
trX.o \
-L .\
-l lib_trKwikChk.a
rm *.o
rm lib_trKwikChk.a

