#if sensorEnabled
  void updateSensors() {
    if (sck.dhtRead())
    {
      TEMPvalue = sck.getTemperatureC(); // C
      HUMvalue = sck.getHumidity(); // %
    }
    sck.getMICS(4000, 30000);
    COvalue = sck.getCO(); //ppm
    NO2value = sck.getNO2(); //ppm
    LIGHTvalue = sck.getLight(); //mV
    NOISEvalue = sck.getNoise(); //mV
    BATvalue = sck.getBattery(); //%
    PANELvalue = sck.getPanel();  // %
  }
#endif







