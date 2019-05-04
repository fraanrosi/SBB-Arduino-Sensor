#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
const int MPU=0x68;  // I2C address of the MPU-6050
int hitNumber;
float hitValue;
float AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
float weightBag;


void setup()
{
  Wire.begin();
  pinMode(5, OUTPUT);    
  pinMode(9, OUTPUT);  
  digitalWrite(9, HIGH); 
  Serial.begin(9600);
  Serial1.begin(9600);
  setupMPU();
  weightBag=0;
  hitNumber=1;
  hitValue=0; 
}

void loop()
{
  //Serial1 is the physical Serial Connections on TX and RX pins
  //No empieza a sensar hasta que el teléfono se conecte y mande el valor del peso de la      
  //bolsa en kilos
  if (Serial1.available() > 0 && weightBag == 0) {
    prendeLed(); 
    weightBag = Serial1.read();
    Serial1.print("peso de la bolsa"); Serial1.println(weightBag);
  }else{
  sensar(); 
  }
  if(AcZ>3.5){
    prendeLed();
    hitValue=abs(AcZ*weightBag);
    DynamicJsonDocument doc(1024);
    JsonArray root = doc.to<JsonArray>();
    if(hitValue != NULL){
    root.add(hitValue);
    serializeJson(doc, Serial1);
    }   
  }
  delay(200);
}

void setupMPU(){
  Wire.beginTransmission(MPU); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission(true);  

  Wire.beginTransmission(MPU); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00011000); //Setting the accel to +/- 16g
  Wire.endTransmission(true); 
}

float sensar(){
  //prendeLed();
  Wire.beginTransmission(MPU);
  Wire.write(0x3F);  // starting with register 0x3F (ACCEL_ZOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,7,true);  // request a total of 14 registers
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L) 
  AcZ=AcZ * (9.81/2048.0);// (lo dividimos por 2048 por que está seteado en rango +/-16g)lo transformamos en metros sobre segundo cuadrado(m/s^2)
  /*
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)   
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  */
  Serial.print("Datos del Acelerometro. ");
  Serial.print("AcZ = "); Serial.println(AcZ);
  /*Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | Tmp = "); Serial.println(Tmp/340.00+36.53);  //equation for temperature in grados C a partir de hoja de datos
  Serial.println("Datos del Giroscopo.");
  Serial.print("GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ);
  */
}

void prendeLed(){
    digitalWrite(5, HIGH);
    delay(100);
    digitalWrite(5, LOW);
}

