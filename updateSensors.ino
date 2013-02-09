void updateSensors() {
  DHTerrorHandler(myDHT22.readData());
  TEMPvalue = getTemperatureC(); // C
  HUMvalue = getHumidity(); // %
  COvalue = getMICS(MICS_5525, 4000); //ppm
  NO2value = getMICS(MICS_2710, 100); //ppm
  LIGHTvalue = getLight(); //mV
  NOISEvalue = getNoise(); //mV
  BATvalue = getBattery(); //%
  PANELvalue = getPanel();  // %
}








