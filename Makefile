compile:
	sdcc --use-non-free -p16f88 -mpic14 keyer.c

up: compile
	pk2cmd -P -M -Fkeyer.hex

upX:compile
	pk2cmd -P PIC16F88 -X -M -Fkeyer.hex

flush:
	pk2cmd -P PIC16F88 -X -E

sim: compile
	gpsim -p p16f88 keyer.hex
