
void DHTerrorHandler(DHT22_ERROR_t errorCode) {
  switch(errorCode) {
  case DHT_ERROR_NONE:
    DHT22ok = true;
    break;
  default:
    DHT22ok = false;
  }
}

float getTemperatureC(){
  if (DHT22ok) {
    float temperature = myDHT22.getTemperatureC();
    Serial.print("Temperature: ");
    Serial.println(temperature);
    //return myDHT22.getTemperatureC();
    return temperature;
  }
}

float getHumidity(){
  if (DHT22ok) {
    float humidity = myDHT22.getHumidity();
    Serial.print("Humidity: ");
    Serial.println(humidity);
    //return myDHT22.getHumidity();
    return humidity;
  }
}

float getPanel(){
  float value = mapfloat(average(PANEL), 409, 819, 0, 100); 
  if (value>0) {
    return value;  // %
  } 
  else { 
    return 0;  // %
  }
}

float getBattery() {
  
  float temp = average(BAT);
  Serial.print("Battery mV: ");
  Serial.println(temp);
  Serial.println("*******************");

  temp = mapfloat(temp, 613, 825, 0, 100);
  if (temp>100) temp=100;
  if (temp<0) temp=0;
  return temp; 
}

float getLight(){
  //float temp = mapfloat(average(S5), 370, 1021, 0, 100); 
  float temp = mapfloat(average(S5), 0, 1023, 0, 100);
  if (temp>100) temp=100;
  if (temp<0) temp=0;
  Serial.print("light mV: ");
  Serial.println(((float)((float)average(S5)/1023)*Vref));
  return temp;
  // return ((float)((float)average(S5)/1023)*Vref);
}

float getNoise() {
  unsigned long temp = 0;
  int temp_actual = 0;
  int max_temp = 0;
  float mVRaw = 0;
  float max_mVRaw = 0;
  float dB_temp = 0;
  float dB = 0;
  int n = 200;
  
  digitalWrite(IO1, HIGH); //VH_MICS2710
  delay(500); // LE DAMOS TIEMPO A LA FUENTE Y QUE DESAPAREZCA EL TRANSITORIO
//  
  for (int i=0; i<n; i++)
  {
    delay(1);
    //temp_actual = analogRead(S4);
    mVRaw = (float)((analogRead(S4))/1023.)*Vref;
    
    //dB_temp = 16.801*log( (mVRaw*200)/1000. ) + 9.872;
    dB_temp = 9.7*log( (mVRaw*200)/1000. ) + 40;  // calibracion para ruido rosa // energia constante por octava

    temp = temp + dB_temp;

  }
//  
  dB = temp/n;
  if(dB < 60) dB = 60; // minimo con la resolucion actual!
  
  //mVRaw = (float)((float)(average(S4))/1023)*Vref;
  
  Serial.print("nOISE mV = ");
  Serial.print(mVRaw);
  Serial.print("  -  nOISE dB = ");
  Serial.println(dB);
  
  digitalWrite(IO1, LOW); //VH_MICS2710
  
  //return max_mVRaw;
  return dB;
  
}


float Noise_test() {
  float mVRaw = 0;
  float dB = 0;
  digitalWrite(IO1, HIGH); //VH_MICS2710
  
  mVRaw = (float)((float)(average(S4))/1023)*Vref;
  dB = 9.7*log( (mVRaw*200)/1000. ) + 40;  // calibrated pink noise - 70 -> 100 dB
  
  Serial.print("nOISE = ");
  Serial.print(mVRaw);
  
  Serial.print("   dB = ");
  Serial.println(dB);
  
 
  return mVRaw;
  
}


