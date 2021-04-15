erase:
	pio run --target erase
install:
	platformio run -t upload
bin:
	platformio run ; cp .pio/build/d1_mini/firmware.bin sccc_VER.bin