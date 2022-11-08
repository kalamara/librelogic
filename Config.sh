#SELECT HARDWARE (ONE OF)

HARDWARE=DRY
#HARDWARE=SIM
#HARDWARE=COMEDI
#HARDWARE=USPACE
#HARDWARE=GPIOD
#HARDWARE=IIO (TODO)
#HARDWARE=GPIOD

cmake -S ./src/ -B build-lib -G "Ninja" -DHW=$HARDWARE \
-DCMAKE_EXPORT_COMPILE_COMMANDS=1 

cmake -S ./tst/ -B build-tst -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=1

