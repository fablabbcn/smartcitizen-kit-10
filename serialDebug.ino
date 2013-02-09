
void serialPrint() {
  Serial.print("Temp ");
  Serial.print(TEMPvalue); 
  Serial.print("C | "); 
  Serial.print("Hum "); 
  Serial.print(HUMvalue); 
  Serial.print("% | "); 
  Serial.print("CO "); 
  Serial.print(COvalue); // average Earth 3 less than 20 check 
  Serial.print("ppm | "); 
  Serial.print("NO2 "); 
  Serial.print(NO2value); // average Earth 390 ppm
  Serial.print("ppm | ");  
  Serial.print("Light "); 
  Serial.print(LIGHTvalue); 
  Serial.print("% | "); 
  Serial.print("Audio "); 
  Serial.print(NOISEvalue); 
  Serial.print("% | "); 
  Serial.print("Panel ");
  Serial.print(PANELvalue); 
  Serial.print("% | "); 
  Serial.print("Bat "); 
  Serial.print(BATvalue); 
  Serial.print("%"); 
  Serial.println(); 
}

void fecha ()
{
  DateTime now = RTC.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}
