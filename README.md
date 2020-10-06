# MKS-DLP-alter
 
**Alternative firmware for MKS DLP board v1.1**

Using IAR 8.32 with ST HAL library.

**Screenshots**
![Main menu](https://raw.githubusercontent.com/Andy-Big/MKS-DLP-alter/master/_screenshots/main_menu.jpg)
![Service menu](https://raw.githubusercontent.com/Andy-Big/MKS-DLP-alter/master/_screenshots/service_menu.jpg)
![MoveZ menu](https://raw.githubusercontent.com/Andy-Big/MKS-DLP-alter/master/_screenshots/movez_menu.jpg)
![Info screen](https://raw.githubusercontent.com/Andy-Big/MKS-DLP-alter/master/_screenshots/info_screen.jpg)
![Files screen icons](https://raw.githubusercontent.com/Andy-Big/MKS-DLP-alter/master/_screenshots/files_icons.jpg)
![Files screen list](https://raw.githubusercontent.com/Andy-Big/MKS-DLP-alter/master/_screenshots/files_list.jpg)
![File preview](https://raw.githubusercontent.com/Andy-Big/MKS-DLP-alter/master/_screenshots/file_preview.jpg)
![Printing screen](https://raw.githubusercontent.com/Andy-Big/MKS-DLP-alter/master/_screenshots/printing_screen.jpg)

**Demo video**
https://www.youtube.com/watch?v=ob9bVc12w_o

## 22.07.2020
+ UI display works (ili9488).
+ The touch panel works.
+ USB works, tested on flash drives up to 32 GB, average read speed ~ 700 KB / sec.
+ Finished rendering library of proportional fonts with support for texts in UTF-8 encoding.
+ Working file system for USB mass storage.

## 23.07.2020
+ Added images for splash, main and service screens. I'm a designer like a sumo ballerina, so it sucks. But I can't get it any better :(

## 30.07.2020
+ Completed the basic user interface
+ The output of background images is optimized
+ Added multilanguage support
+ Laid the foundation for a new bootloader

## 01.08.2020
+ Fully started the stepper motor. The procedures for working with it are taken from Marlin 1.1.8. All unnecessary was completely cut out, only work with one axis was left. Two classes are used - the scheduler (planner.cpp) and directly working with the motor (stepper.cpp).

## 15.09.2020 First print version!
**Already done:**
+ Printing .pws files (no anti-aliasing yet).
+ Two languages - Russian and English, the ability to add other languages.
+ Clock and calendar in the main menu. To keep the time from getting lost when turning off the printer, a 3-volt CR2016 / CR2032 battery must be soldered to the board.
+ Manual Z-axis movement in steps of 30, 10, 1, 0.2 and 0.05 mm.
+ Zeroing of the Z axis along the upper or lower limit switch.
+ Reduced travel speed below 30 mm - to prevent resin splashing.
+ Settings for acceleration and speed of movement separately for free movement and for print mode, as well as setting other standard parameters of the Z axis.
+ Adjustment of the lower and upper limit switches.
+ Setting the software limitation of the minimum and maximum platform lifting height.
+ Adjustment of the platform lifting height to pause printing.
+ Adjustment of the operating current of the motor up to 1000 mA.
+ Mode to reduce the motor current in hold mode.
+ Motor current settings for hold mode.
+ Setting the time after the last movement of the motor to activate the hold mode.
+ Setting the duration of the buzzer for button presses and for messages.
+ Adjust the screen flip by 180 degrees.
+ All settings can be loaded from the configuration file via the file browser and they are applied immediately. No printer reboot required.
+ File browser with two preview modes - icons with a preview of the model or a list with the date and time of file modification.
+ Work of the file browser with directories.
+ Work of the file browser with cyrillic names of files and directories.
+ In the preview of the file before printing, detailed information about the printing modes is displayed.
+ Good accuracy in determining the print time of the model.
+ Runtime counters of the display, illumination, fan and the printer as a whole.
+ Through the interface in the settings screen, you can configure:
	1. Height of rise to pause printing.
	1. Enable / disable buzzer.
+ Lock against accidental pressing of the on-screen buttons in print mode.

## 18.09.2020 Updates
![Printing screen](https://raw.githubusercontent.com/Andy-Big/MKS-DLP-alter/master/_screenshots/screensaver_clock.jpg)
**Added/changed:**
+ Setting the time and date.
+ Screensaver with a LCD-like digital clock.
+ Motor disable after inactivity timeout.
+ Parameter in config file for setting the time for the screensaver to turn on.
+ Parameter in config file for setting the time for the motor disable.

## 21.09.2020 Updates
**Added/changed:**
+ Added print progress to screensaver.
+ The process for printing with antialiasing has been prepared.
* Fixed initialization of 2k display.

## 22.09.2020 Updates
**Added/changed:**
+ Complete antialiasing support.
+ Added processing of individual layer parameters.
+ UV test function completed.

## 22.09.2020 Updates
**Added/changed:**
+ Add current height in printing progress.
+ Automatic exit from screensaver mode at the end of printing.
* Fixed UV-LCD initialization.
* Now, when switching to a screensaver, power consumption is reduced as much as possible, to less than 0.8 watts.

## 01.10.2020 Updates
**Added/changed:**
+ Added support for .photon and .cbddlp files.
+ Added settings for the platform lifting height at the end of printing (in Settings menu and in config file).
+ Added file deleting in file preview screen.

## 02.10.2020 Updates
**Added/changed:**
+ Added changing parameters during printing - exposure time, elevation and lifting speed, etc.

## 03.10.2020 Updates
**Added/changed:**
+ Added total printer on time counter.
* Fixed several errors that appeared when compiling with maximum optimization.
* Fixed a bug due to which the acceleration settings were not applied without rebooting.
* Fixed bug in implementation of low-level functions for FatFS in SPI-flash.
* Fixed work of the firmware together with the original bootloader.
* Fixed reading of configuration files when they start with an encoding identifier (UTF-8 Big Endian or Little Endian).
* Runtime counters are saved every 60 minutes.

## 05.10.2020 Updates
**Added/changed:**
* Fixed errors with antialiasing in .cbddlp and .photon files with version 3.

## 05.10.2020 Updates
# v0.02
**Added/changed:**
+ Added the ability to connect an external clock DS3231. The firmware uses this clock to get the current time and date. You can enable or disable the use of an external clock in the configuration file.
![Printing screen](https://raw.githubusercontent.com/Andy-Big/MKS-DLP-alter/master/_screenshots/ds3231_connect.jpg)

**To do in the near future:**
1. Upgradeable bootloader and firmware.
1. Touch panel calibration.
1. Runtime counters reset.
1. Lifting platform request after Z=0 setting.

