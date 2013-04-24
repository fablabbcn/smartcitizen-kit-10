#if SDEnabled
  void updateSensors() {
    if (sck.dhtRead())
    {
      SENSORvalue[0] = sck.getTemperatureC(); // C
      SENSORvalue[1] = sck.getHumidity(); // %
    }
    sck.getMICS(4000, 30000);
    SENSORvalue[2] = sck.getCO(); //Ohm
    SENSORvalue[3] = sck.getNO2(); //Ohm
    SENSORvalue[4] = sck.getLight(); // %
    SENSORvalue[5] = sck.getNoise(); //dB
    SENSORvalue[6] = sck.getBattery(); //%
    SENSORvalue[7] = sck.getPanel();  // %
  }
#endif








