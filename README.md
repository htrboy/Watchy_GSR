**NOTE:**
This branch has made some changes to allow encapsulating a watchface inside a header file and making use of functionality in the main code from inside the watchface file.  To add a watchface, fonts and icons can be added to 'myResources.h', alter the 'main.cpp/.ino' file per GuruSR's instructions, and add the 'drawMyWatchFace()' function definition to 'Watchy_GSR.h' in the public list.


**GURU_SR's notes follow:**

This repo's format:

Bin folder contains the current OTA Bin that you can use a tool to upload with or an OTA Upgrade capable firmware to install the current OTA Bin to the Watchy.

src folder contains a universal (Arduino & PlatformIO) setup for compilation, instructions on how to get it to compile are in the src folder.

Needed libraries:  Arduino Libraries, ArduinoOTA (included in ESP32 2.0.2+), SmallRTC (1.6 or greater, GuruSR), SmallNTP (GuruSR), StableBMA (GuruSR), Olson2POSIX (GuruSR) AND Watchy (1.3.3 or greater) base.

**NOTES:**
- Watchy uses version 1.0.6 or 2.0.2 (or higher) of the ESP32 libraries, 2.0.0 won't compile, remove them manually and revert back to version 1.0.6 or use 2.0.2 or higher (which seems to have been fixed).

For instructions on usage of the firmware, see "Usage".

Below is a layout of the Watchy Connect & the OTA Website (used while online your network) that gives you options to various things.


![OTA Website](https://github.com/GuruSR/Watchy_GSR/blob/main/Images/Server-Help.png)

OTA update is **ONLY** possible via an OTA compatible firmware which are:

`Watchy GSR` and those overriding it.

**If you have an OTA upload function in your firmware, ask for it to be added here.**

For those wanting to override the looks of this Watchy face, look at the Override Information.
