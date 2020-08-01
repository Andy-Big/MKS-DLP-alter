# MKS-DLP-alter
 
Alternative firmware for MKS DLP board v1.1

Using IAR 8.32 with ST HAL library.


22.07.2020
- UI display works (ili9488)
- the touch panel works
- USB works, tested on flash drives up to 32 GB, average read speed ~ 700 KB / sec
- finished rendering library of proportional fonts with support for texts in UTF-8 encoding
- working file system for USB mass storage

23.07.2020
- Added images for splash, main and service screens. I'm a designer like a sumo ballerina, so it sucks. But I can't get it any better :(

30.07.2020
- Completed the basic user interface
- The output of background images is optimized
- Added multilanguage support
- Laid the foundation for a new bootloader