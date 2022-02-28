
#SELECT HARDWARE (ONE OF)
#DRY
#SIM
#COMEDI
#USPACE
#IIO (TODO)

HARDWARE=DRY
cmake -S ./src/ -B build-lib -G "Unix Makefiles" -DHW=$HARDWARE \
-DCMAKE_EXPORT_COMPILE_COMMANDS=1 

cmake -S ./tst/ -B build-tst -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=1

