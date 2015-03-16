Smart Citizen Kit firmware for SD
=================================

**Smart Citizen Kit SD firmware version for the Ambient Sensor Board.**

This firmware is aimed to people wanting to log data in off-line mode, without internet connectivity, storing data on the built-in micro SD.


##Steps

1. **microSD**: We recomend formating **micro SD** cards using the official SD tool you can download [here](https://www.sdcard.org/downloads/formatter_4/).

2. **RTC**: Place a **CR1220** cell battery on your kit. Before uploading the SD firmware use the on-line configuration tool at [smartcitizen.me](https://smartcitizen.me) to set your Wi-Fi credentials. Your kit will connect to the internet and sync its the internal clock with our remote servers.

3. You can now install the **SD firmware** using the Arduino IDE. Ready!


##Installation


**The firmware is totally compatible with Arduino. You can upload the firmware using the [Arduino IDE](http://arduino.cc/en/main/software).**

####Dependencies: 

In order to compile and upload the firmware you will need to install the **SdFat** library for Arduino.

Download the library [here](https://github.com/greiman/SdFat) and install the library following the [instructions](http://arduino.cc/en/Guide/Libraries).

####Boards:

* For SmartCitizen Kit version 1.0 select `Tools/Boards/Arduino Leonardo` on the Arduino IDE (ATmega 32U4 at 16Mhz) 

* For SmartCitizen Kit version 1.1 select `Tools/Boards/Lylipad Arduino USB` on the Arduino IDE (ATmega 32U4 at 8Mhz) 

##SD data format


When using the **SD firmware** on the SCK, data is stored as **CSV** (comma separated) file on the SD card.

This is an example of the output file once opened on a spreadsheet application:

| Temperature | Humidity| Light   | Battery | Solar Panel | CO     | NO2  | Noise| UTC			  	    | 
|-------------|---------|---------|---------|-------------|--------|------|------|----------------------|
| 2821.20     | 4072.00 | 4413.10 | 96.40   | 0.00        | 94.67  | 0.65 | 5.23 | 2000-01-01  00:00:02 |
| 2784.40     | 4236.80 | 5936.70 | 96.10   | 0.00        | 278.27 | 1.05 | 2.39 | 2000-01-01  00:00:02 |

###Data Conversions

The data stored is automatically converted to the proper units in firmware.

If you prefer to do the conversions manually set `DataRaw       false` in the `Constants.h` file and apply the formulas on the following table:


| ID  | Sensor       | Units | Conversion Formula                                 
|-----|--------------|-------|---------------------------------------------|
| 0   | Temperature  | ºC    |  T = -53 + 175.72 / 65536.0 * ( Traw * 10 ) |
| 1   | Humidity     | %Rel  |  H =   7 + 125.0  / 65536.0 * ( Hraw * 10 ) |
| 2   | Light        | Lux   |  L = Lraw / 10                              |
| 3   | Battery      | %     |  Not required                               |
| 4   | Panel        | mV    |  Not required                               |                                             
| 5   | CO      	  | kOhm  |  Not required                               |                                            
| 6   | NO2          | kOhm  |  Not required                               |                               
| 7   | Noise        | dB    |  Apply the conversion table from mV to dB: [CSV](https://gist.github.com/pral2a/d767cc45874361fd38bf) 
| 8   | Date         | DD:MM:YY |  Not required                            |                             
| 9   | Time         | hh:mm:ss |  Not required                            |                            






