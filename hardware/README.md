Smart Citizen Kit
=================

###Smart Citizen Kit Ambient Sensor Board versions


| Smart Citizen Kit |           | SCK 1.0        | SCK 1.1        |
|:-----------|:---------:|:---------------------------:|:---------------------------:|
| **Data Board**        |           |                                                                                  |                                                                                     |
| **MCU**               |           | ATMEGA32U4                                                                       | ATMEGA32U4                                                                          |
| **Clock**             |           | 16Mhz                                                                            | 8Mhz                                                                                |
| **Firmware**         |           | https://github.com/fablabbcn/Smart-Citizen-Kit                                   | https://github.com/fablabbcn/Smart-Citizen-Kit                                      |
| **Design files**      |           | https://github.com/fablabbcn/Smart-Citizen-Kit/tree/master/hardware/Goteo/v1.01  | https://github.com/fablabbcn/Smart-Citizen-Kit/tree/master/hardware/Kickstarter     |
| **Ambient Board**     |           |                                                                                  |                                                                                     |
| **Light**             | *Part*      | PVD-P8001                                                                        | BH1730FVC                                                                           |
|                   | *Type*      | LDR Analog Light Sensor                                                          | Digital Ambient Light Sensor                                                        |
|                   | *Units*     | %                                                                                | Lux                                                                                 |
|                   | *Datasheet* | https://github.com/fablabbcn/Smart-Citizen-Kit/wiki/Datasheets/PDV-P8001.pdf     | https://github.com/fablabbcn/Smart-Citizen-Kit/wiki/Datasheets/BH-1730FCV.pdf       |
|                   | Firmware  | `SCKAmbient::getLight()                                                           | `SCKAmbient::getLight()                                                              |
| **Temp**              | *Part*      | DHT22                                                                            | HPP828E031 (SHT21)                                                                  |
|                   | *Type*      | Digital Temperature and Relative Humidity Sensor                                 | Digital Temperature and Relative Humidity Sensor                                    |
|                   | *Units*     | ºC                                                                               | ºC                                                                                  |
|                   | *Datasheet* | https://github.com/fablabbcn/Smart-Citizen-Kit/wiki/Datasheets/DHT22.pdf         | https://github.com/fablabbcn/Smart-Citizen-Kit/wiki/Datasheets/HTU-21D.pdf          |
|                   | Firmware  | `SCKAmbient::getDHT22();` `SCKAmbient::getHumidity();`                               | `SCKAmbient::getSHT21();` `SCKAmbient::getTemperature();`                               |
| **Humidity**          | *Part*      | DHT22                                                                            | HPP828E031 (SHT21)                                                                  |
|                   | *Type*      | Digital Temperature and Relative Humidity Sensor                                 | Digital Temperature and Relative Humidity Sensor                                    |
|                   | *Units*     | % Rel                                                                            | % Rel                                                                               |
|                   | *Datasheet* | https://github.com/fablabbcn/Smart-Citizen-Kit/wiki/Datasheets/DHT22.pdf         | https://github.com/fablabbcn/Smart-Citizen-Kit/wiki/Datasheets/HTU-21D.pdf          |
|                   | Firmware  | `SCKAmbient::getDHT22();` `SCKAmbient::getHumidity();`                               | `SCKAmbient::getSHT21();` `SCKAmbient::getHumidity();`                                  |
| **Noise**             | *Part*      | POM-3044P-R                                                                      | POM-3044P-R                                                                         |
|                   | *Type*      | Electret microphone with envelop follower sound pressure sensor                  | Electret microphone with envelop follower sound pressure sensor                     |
|                   | *Units*     | dB                                                                               | dB                                                                                  |
|                   | *Datasheet* | https://github.com/fablabbcn/Smart-Citizen-Kit/wiki/Datasheets/POM-3044P-R.pdf   | https://github.com/fablabbcn/Smart-Citizen-Kit/wiki/Datasheets/POM-3044P-R.pdf      |
|                   | *Firmware*  | `SCKAmbient::getNoise();`                                                          | `SCKAmbient::getNoise();`                                                             |
| **CO**                | *Part*      | MICS-5525                                                                        | MiCS-4514                                                                           |
|                   | *Type*      | MOS CO gas sensor                                                                | MOS CO and NO2 gas sensor                                                           |
|                   | *Units*     | kOhm (ppm)                                                                       | kOhm (ppm)                                                                          |
|                   | *Datasheet* | https://github.com/fablabbcn/Smart-Citizen-Kit/wiki/Datasheets/MICS-5525_CO.pdf  | https://github.com/fablabbcn/Smart-Citizen-Kit/wiki/Datasheets/MiCS-4514_CO_NO2.pdf |
|                   | *Firmware*  | `SCKAmbient::getMICS();`                                                           | `SCKAmbient::getMICS();`                                                              |
| **NO2**               | *Part*      | MICS-2710                                                                        | MiCS-4514                                                                           |
|                   | *Type*      | MOS NO2 gas sensor                                                               | MOS CO and NO2 gas sensor                                                           |
|                   | *Units*     | kOhm (ppm)                                                                       | kOhm (ppm)                                                                          |
|                   | *Datasheet* | https://github.com/fablabbcn/Smart-Citizen-Kit/wiki/Datasheets/MICS-2710_NO2.pdf | https://github.com/fablabbcn/Smart-Citizen-Kit/wiki/Datasheets/MiCS-4514_CO_NO2.pdf |
|                   | *Firmware*  | `SCKAmbient::getMICS();`                                                           | `SCKAmbient::getMICS();`                                                              |