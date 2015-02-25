SD data format
==============

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






