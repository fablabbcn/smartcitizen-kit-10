#define MCP1 0x2F    // Direcion del mcp1 MICS
#define MCP2 0x2E    // Direcion del mcp2 REGULADORES

#define R1 82  //Kohm
#define P1 100  //Kohm

#define RES 255

float k= (RES*(float)R1/100)/1000; 
float kr= ((float)P1*1000)/RES;

#define RO_MICS_5525 450 //Kohm segun tablas estadisticas del fabricante
#define RO_MICS_2710 3   //Kohm segun tablas estadisticas del fabricante

float RS_RO_MICS_5525[22] = {
  0.72, 0.68, 0.66, 0.64, 0.63, 0.62, 0.61, 0.6, 0.59, 0.58, 0.5, 0.45, 0.41, 0.38, 0.34, 0.31, 0.29, 0.275, 0.26, 0.17, 0.12, 0.088}; //Rs/Ro
float PPM_MICS_5525[22] =   {   
  1,    2,    3,    4,    5,    6,    7,   8,    9,   10,  20,   30,   40,   50,   60,   70,   80,    90,  100,  200,  300,   400}; //ppm

float RS_RO_MICS_2710[10] = { 
  25,  55,  90, 140, 190, 260, 330, 410, 500, 1000}; //Rs/Ro
float PPM_MICS_2710[10]   = {
  0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9,   1,  1.5}; //ppm

void writeMCP(byte deviceaddress, byte address, int data ) {
  if (data>RES) data=RES;
  Wire.beginTransmission(deviceaddress);
  address=(address<<4)|bitRead(data, 8) ;
  Wire.write(address);
  Wire.write(lowByte(data));
  Wire.endTransmission();
  delay(4);
}

int readMCP(int deviceaddress, byte address ) {
  byte rdata = 0xFF;
  int  data = 0x0000;
  Wire.beginTransmission(deviceaddress);
  address=(address<<4)|B00001100;
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,2);
  while (!Wire.available()); 
  rdata = Wire.read(); 
  data=rdata<<8;
  while (!Wire.available()); 
  rdata = Wire.read(); 
  data=data|rdata;
  return data;
}


void VH_MICS(byte device, long voltage ) {
  byte data=0;
  int temp = (int)(((voltage/1.2)-1000)*k);
  if (temp>255) data=255;
  else if (temp<0) data=0;
  else data = temp;
  if (device== MICS_5525) writeMCP(MCP2, 0x00, data);
  else if (device== MICS_2710) writeMCP(MCP2, 0x01, data);
}

float readVH(byte device) {
  int data;
  if (device== MICS_5525) data=readMCP(MCP2, 0x00);
  else if (device== MICS_2710) data=readMCP(MCP2, 0x01);
  float voltage = (data/k + 1000)*1.2;
  return(voltage);
}

void RL_MICS(byte device, long resistor) {
  byte data=0x00;
  data = (byte)(resistor/kr);
  if (device== MICS_5525) writeMCP(MCP1, 0x00, data);
  else if (device== MICS_2710) writeMCP(MCP1, 0x01, data);
}

float getMICS(byte device,unsigned long time){
  float current = 0;
  // float i=0;
  float Rs = 0;
  float VL = 0;
  float RL = 0;
  float Vc = 0;
  int   Rc = 10; //Ohm
  int   Vcc = 5000; //mV
  float Rh = 0;
  float Vh = 0;

  if (device == MICS_5525) 
  {   
    /*Correccion de la tension del Heather*/
    VH_MICS(MICS_5525, 2400); //VH_MICS5525 Inicial
    digitalWrite(IO0, HIGH); //VH_MICS5525
    Serial.println("*******************");
    Serial.println("MICS5525 VH a 2.4V");

    delay(200);
    Vc = (float)average(S2)*Vcc/1023; //mV 
    current = Vc/Rc; //mA 
    Rh = (readVH(MICS_5525)- Vc)/current;
    Vh = (Rh + Rc)*32;
    VH_MICS(MICS_5525, Vh); //VH_MICS5525 Corregido 
    Serial.print("MICS5525 correccion VH: ");
    Serial.print(Vh/1000.);
    Serial.println("V");
    Serial.println("Heating...");
    delay(5000); // Tiempo de heater!

    /*Lectura de datos*/
    RL_MICS(MICS_5525, 100000);
    digitalWrite(IO0, LOW);  //VH_MICS5525 OFF para lectura
    //VH_MICS(MICS_5525, 1200); //VH_MICS5525 a 1.2V para lectura
    Serial.println("MICS5525 VH a 0V ");
    delay(time); //Tiempo de enfriamiento para lectura

    //Vc = (float)average(S2)*Vcc/1023; //mV 
    //current = Vc/Rc; //mA 

    RL = kr*readMCP(MCP1, 0x00)/1000; //Kohm
    VL = ((float)average(S0)*Vcc)/1023; //mV
    Rs = ((Vcc-VL)/VL)*RL; //Kohm
    Serial.print("MICS5525 Rs: ");
    Serial.print(Rs);
    Serial.println("K");

    /*Correccion de impedancia de carga*/
    if (Rs < 100)
    {
      delay(100);
      RL_MICS(MICS_5525, Rs*1000);
      RL = kr*readMCP(MCP1, 0x00)/1000; //Kohm
      VL = ((float)average(S0)*Vcc)/1023; //mV
      Rs = ((Vcc-VL)/VL)*RL; //Kohm
    }

    //digitalWrite(IO0, LOW); //VH_MICS5525 OFF
    //Serial.println("MICS5525 VH OFF ");

    /*Calculo de los resultados*/
    //    if ((Rs/RO_MICS_5525)>RS_RO_MICS_5525[0]) //Valores inferiores a 1ppm
    //      {
    //        return(0);
    //      }
    //      else
    //      {
    //        int i=0;
    //        while (RS_RO_MICS_5525[i]>Rs/RO_MICS_5525) i++;
    //        float m = (PPM_MICS_5525[i] - PPM_MICS_5525[i-1])/(RS_RO_MICS_5525[i] - RS_RO_MICS_5525[i-1]); //Pendiente
    //        float b = PPM_MICS_5525[i]-m*RS_RO_MICS_5525[i]; //Ordenada al origen     
    //        return(m*(Rs/RO_MICS_5525)+b);  
    //      }
    return(Rs);
  }
  else if (device == MICS_2710) 
  {
    digitalWrite(IO2, LOW); //RADJ_MICS2710 PIN ALTA IMPEDANCIA
    digitalWrite(IO1, HIGH); //VH_MICS2710

    /*Correccion de la tension del Heather*/
    VH_MICS(MICS_2710, 1700); //VH_MICS2710 Inicial
    Serial.println("*******************");
    Serial.println("MICS2710 VH a 1.7V");

    delay(200);
    Vc = (float)average(S3)*Vcc/1023; //mV 
    current = Vc/Rc; //mA 
    Rh = (readVH(MICS_2710)- Vc)/current; //Ohm
    Vh = (Rh + Rc)*26; //mV 
    VH_MICS(MICS_2710, Vh); //VH_MICS2710 Corregido
    Serial.print("MICS2710 correccion VH: ");
    Serial.print(Vh/1000.);
    Serial.println("V");
    Serial.println("Heating...");
    delay(30000);   

    /*Lectura de datos*/
    //VH_MICS(MICS_2710, 1200); //VH_MICS2710 Inicial
    RL_MICS(MICS_2710, 10000);
    delay(time);
    Vc = (float)average(S3)*Vcc/1023; //mV 
    current = Vc/Rc; //mA 
    RL = kr*readMCP(MCP1, 0x01)/1000; //Kohm
    VL = ((float)average(S1)*Vcc)/1023; //mV
    Rs = ((2500-VL)/VL)*RL; //Kohm

    /*Correccion de impedancia de carga*/
    if (Rs > 100) RL_MICS(MICS_2710, 100000);
    else RL_MICS(MICS_2710, Rs*1000);
    delay(100);
    RL = kr*readMCP(MCP1, 0x01)/1000; //Kohm
    VL = ((float)average(S1)*Vcc)/1023; //mV
    Rs = ((2500-VL)/VL)*RL; //Kohm
    Serial.print("MICS2710 Rs: ");
    Serial.print(Rs);
    Serial.println("K");


    digitalWrite(IO1, LOW); //VH MICS2710 OFF
    Serial.println("MICS2710 VH OFF ");

    Serial.println("*******************");

    /*Calculo de los resultados*/
    //    if ((Rs/RO_MICS_2710)<RS_RO_MICS_2710[0]) //Valores inferiores a 0.2ppm
    //      {
    //        return(0);
    //      }
    //      else
    //      {
    //        int i=0;
    //        while (RS_RO_MICS_2710[i]<Rs/RO_MICS_2710) i++;
    //        float m = (PPM_MICS_2710[i] - PPM_MICS_2710[i-1])/(RS_RO_MICS_2710[i] - RS_RO_MICS_2710[i-1]); //Pendiente
    //        float b = PPM_MICS_2710[i]-m*RS_RO_MICS_2710[i]; //Ordenada al origen     
    //        return(m*(Rs/RO_MICS_2710)+b);  
    //      }  
    return(Rs);
  }  
}




