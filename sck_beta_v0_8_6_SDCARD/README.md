SD data format
==============

When using the **SD firmware** on the SCK data is stored as **CSV** (comma separated) file.

This is an example of the output file:

ID | Temp   | ID | Hum | ID | Light | ID | Panel | ID |CO | ID | NO2 | ID | Noise | ID | Date  | Time | 
-- | ------- | - | ------- | - | ------- | - | ----- | - | ---- | - | ------ | - | ---- | - | ---- | ---------- | ------- |
0  | 2821.20 | 1 | 4072.00 | 2 | 4413.10 | 3 | 96.40 | 4 | 0.00 | 5 | 94.67  | 6 | 0.65 | 7 | 5.23 | 2000-01-01 | 00:00:02|
0  | 2784.40 | 1 | 4236.80 | 2 | 5936.70 | 3 | 96.10 | 4 | 0.00 | 5 | 278.27 | 6 | 1.05 | 7 | 2.39 | 2000-01-01 | 00:00:02|


The data stored requires some conversions as specified on the table:

| ID  | Sensor       | Units | Conversion Formula                                 
|-----|--------------|-------|---------------------------------------------|
| 0   | Temperature  | ºC    |  T = -53 + 175.72 / 65536.0 * ( Traw * 10 ) |
| 1   | Humidity     | %Rel  |  H =   7 + 125.0  / 65536.0 * ( Hraw * 10 ) |
| 2   | Light        | Lux   |  L = Lraw / 10                              |
| 3   | Battery      | %     |  Not required                               |
| 4   | Panel        | mV    |  Not required                               |                                             
| 5   | CO      	  | kOhm  |  Not required                               |                                            
| 6   | NO2          | kOhm  |  Not required                               |                               
| 7   | Noise        | mV    |  Apply the conversion table below          |                
| 8   | Date         | DD:MM:YY |  Not required                            |                             
| 9   | Time         | hh:mm:ss |  Not required                            |                            


Noise (dB) converison table:

raw*100 | dB
- | -
0 | 1
5 | 45
10 | 55
15 | 63
20 | 65
30 | 67
40 | 69
50 | 70
60 | 71
80 | 72
90 | 73
100 | 74
130 | 75
160 | 76
190 | 77
220 | 78
260 | 79
300 | 80
350 | 81
410 | 82
450 | 83
550 | 84
600 | 85
650 | 86
750 | 87
850 | 88
950 | 89
1100 | 90
1250 | 91
1375 | 92
1500 | 93
1650 | 94
1800 | 95
1900 | 96
2000 | 97
2125 | 98
2250 | 99
2300 | 100
2400 | 101
2525 | 102
2650 | 103


Noise (dB) conversion in [CSV](https://gist.github.com/pral2a/d767cc45874361fd38bf) 


