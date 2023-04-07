/***************************************************************
 PROJECT:  Warriors JRU
                                      Implementation of Internet of Things in Developing Hybrid Energy Harvesting System
                                                    College of Computer Studies and Engineering
                                                        Computer Engineering Department

 
 Arvenell Abad
 James Frederic Dulo
 Gelan Nicolan
 Adrian dela Cruz

Graduate na kami!!!
***************************************************************/

#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266Shield.h>    //CayenneMQTT library
#include <Servo.h>                  //Servo motor library 
#include <DHT.h>                    //DHT library 
#define DHTTYPE DHT22
#define DHTPIN 2 
DHT dht(DHTPIN,DHTTYPE);

// WiFi network info.
//MQTT credentials   
char ssid[] = "FiberLODIII";
char wifiPassword[] = "abadPAPANGARNEL0926@";
char username[] = "78de96e0-c581-11ed-b0e7-e768b61d6137";
char password[] = "2cddbda53ae361fda4a12a7810f2d22f8057a89d";
char clientID[] = "7e306fb0-c581-11ed-b72d-d9f6595c5b9d";

#define EspSerial Serial1

ESP8266 wifi(&EspSerial);
const int voltageinputPIN = A8; //select analog input pin for voltage sensor //sets baud rate in bits per second for serial monitor
const int baudRate = 9600; //sets baud rate in bits per second for serial monitor
const int sensorreadDelay = 250; //sensor read delay in milliseconds
const int maxanalogValue = 1010; //highest integer given at max input voltage
const int sensormaxVoltage = 25; //highest input voltage of sensor being used

float analogVoltage = 0; //to store voltage value at analog pin
float Voltage = 0; 
float current = 0;
float power = 0;

float resistance = 12.5; // R = V^2/P == R = (5V)^2/2W

Servo servo_x;                   //up-down servomotor  
int servoh = 0;
int servohLimitHigh  = 170;     
int servohLimitLow = 10;       

Servo servo_z;                   //left-right  servomotor 
int servov = 0; 
int servovLimitHigh = 170;
int servovLimitLow  = 10;

int topl,topr,botl,botr;
int threshold_value=10;        
float Vout;

void setup()
{ 
    
  Serial.begin(9600);
  delay(10);
  EspSerial.begin(9600); // Set ESP8266 baud rate
  delay(10);
  Serial.begin(baudRate); //initializes serial communication  
  Cayenne.begin(username, password, clientID, wifi, ssid, wifiPassword);
  servo_x.attach(5);
  servo_z.attach(6);
  dht.begin();
  pinMode(3,OUTPUT);
  
  
}

void loop()
{
  topr= analogRead(A2);       
  topl= analogRead(A3);         
  botl= analogRead(A4);      
  botr= analogRead(A5);        
  Vout=(analogRead(A7) * 5.0) / 1023;
  analogVoltage = analogRead(voltageinputPIN); //reads analog voltage of incoming sensor

  Serial.println(" Manual-mode");
  Cayenne.loop();
  
  if
  (digitalRead(3)==1){
    Serial.println(" Automatic-mode");
    servoh = servo_x.read();
    servov = servo_z.read();
    int avgtop = (topr + topl) / 2;     
    int avgbot = (botr + botl) / 2;   
    int avgright = (topr  + botr) / 2;   
    int avgleft = (topl + botl) / 2;    
    int diffhori=  avgtop - avgbot;      
    int diffverti= avgleft - avgright;    
    int offset =20;// set the correction offset value
    
    /*tracking according to horizontal axis*/ 
    if (abs(diffhori) <= threshold_value)
    {
     servo_x.write(servoh);            //stop the servo up-down
    }else {
       if (diffhori > threshold_value)
          { Serial.println(" x -  2 ");
          servo_x.write(servoh -2);    //Clockwise rotation CW
          if (servoh > servohLimitHigh)
          {
           servoh = servohLimitHigh;
          }
          delay(10);
          }else {
           servo_x.write(servoh +2);   //CCW
           if (servoh < servohLimitLow)
           {
           servoh = servohLimitLow;
           }
           delay(10);
           }
      }     
    /*tracking according to vertical axis*/ 
    if (abs(diffverti) <= threshold_value)
    {     
     servo_z.write(servov);       //stop the  servo left-right
    }else{
       if (diffverti > threshold_value)
       {
       servo_z.write(servov -2);  //CW
       if (servov > servovLimitHigh)
       { 
       servov = servovLimitHigh;
       }
       delay(10);
       }else{ 
        servo_z.write(servov +2);  //CCW
        if (servov < servovLimitLow) 
        {
        servov = servovLimitLow;
        }
        delay(10);
        }
     }
  }
}
// Cayenne Functions
CAYENNE_IN(8){
  int value = getValue.asInt();
  CAYENNE_LOG("Channel %d, pin %d, value %d", 8, 3, value);
  digitalWrite(3,value);
}
CAYENNE_IN(7){ //up-down servo  motor
  if (digitalRead(3)==HIGH){ //Automatic_mode
  }
  else{ //Manual_mode
  servo_x.write(getValue.asDouble() * 180);
  }
}
CAYENNE_IN(6){ //left-right  servo motor
  if (digitalRead(3)==HIGH){
  }  
  else{
  servo_z.write(getValue.asDouble()  * 180);
  }
}
CAYENNE_OUT(13){
  float Voltage = (analogVoltage * (5.0 / 1023.0)); //conversion equation
  Cayenne.virtualWrite(13, Voltage, TYPE_VOLTAGE, UNIT_VOLTS);
  Serial.print(" Voltage: "); 
  Serial.print(Voltage); //prints value to serial monitor
  Serial.println("V"); //prints label
  //delay(sensorreadDelay); //delay in milliseconds between read values
}
CAYENNE_OUT(0) { //Current
  float Voltage = (analogVoltage * (5.0 / 1023.0)); //conversion equation
  float current = Voltage/2;
  Cayenne.virtualWrite(0, current);
  Serial.print(" Current: ");
  Serial.println(current);
}
CAYENNE_OUT(10) { //Power
  float Voltage = (analogVoltage * (5.0 / 1023.0)); //conversion equation
  float power = (Voltage * Voltage)/resistance ;
  Cayenne.virtualWrite(10, power);
  Serial.print(" Power: ");
  Serial.println(power);
}
CAYENNE_OUT(2){  //LDR Top-right
  Cayenne.virtualWrite(2, topr);
}
CAYENNE_OUT(3){ //LDR  Top-left
  Cayenne.virtualWrite(3,topl);
}
CAYENNE_OUT(4){ //LDR Bot-left
  Cayenne.virtualWrite(4,botl);
}
CAYENNE_OUT(5){ //LDR Bot-right
  Cayenne.virtualWrite(5,botr);
}
CAYENNE_OUT(11){ //Temperature
  float t = dht.readTemperature();
  //int chk = dht.read(DHT11PIN);
  Cayenne.virtualWrite(11, t, TYPE_TEMPERATURE, UNIT_CELSIUS);
  Serial.print(" temperature:  ");
  Serial.println(t);
}
CAYENNE_OUT(12){ //HuMidity
  double h = dht.readHumidity();
  //int chk = dht.read(DHT11PIN);
  Cayenne.virtualWrite(12, h); 
  Serial.print(" humidity: ");
  Serial.println(h);
}
