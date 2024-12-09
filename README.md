# wheelchair-HUD
Development environment for the wheelchair display. Designed around the ESP32-2432S022 CYD board.

Version: Beta 1.0.2A

GETTING STARTED:

This codebase is intended to be used in PlatformIO. All directories excluding the readme and misc will replace
folders in a esp32 configuration 1:1. 
lv_conf, found in the misc folder, should be placed inside the .pio/libdeps/esp32-2432S022N directory that should form
if the .ini has been properly scanned by the PlatformIO instance.
The other misc arduino files are to generate test I2C outputs.

I2C communications on the ESP32-2432S022 CYD require a 6 x 0.5mm ribbon cable, along with a breakout board for anyone that is sane.
The pins are configured from closest to edge to farthest:
Ground
SDA
SCL
Unused
Unused
Unused
One of the Unuseds is an additonal ground if I recall correctly.
