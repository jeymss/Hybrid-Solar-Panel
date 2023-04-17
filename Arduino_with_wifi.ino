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
char ssid[] = "TECNO POVA";
char wifiPassword[] = "ulolkadon";
char username[] = "78de96e0-c581-11ed-b0e7-e768b61d6137";
char password[] = "2cddbda53ae361fda4a12a7810f2d22f8057a89d";
char clientID[] = "a53f41c0-c582-11ed-b0e7-e768b61d6137";

#define EspSerial Serial1

ESP8266 wifi(&EspSerial);
const int voltageinputPIN = A8; //select analog input pin for voltage sensor //sets baud rate in bits per second for serial monitor
const int piezoinputPIN = A7; //select analog input pin for piezo voltage sensor //sets baud rate in bits per second for serial monitor
const int baudRate = 9600; //sets baud rate in bits per second for serial monitor
const int sensorreadDelay = 250; //sensor read delay in milliseconds
const int maxanalogValue = 1010; //highest integer given at max input voltage
const int sensormaxVoltage = 25; //highest input voltage of sensor being used

float analogVoltage = 0; //to store voltage value at analog pin
float analogPiezo = 0;
float Voltage = 0; 
float current = 0;
float power = 0;

float resistance = 2.5; // R = V^2/P == R = 5V/2W /// for solar panel
float piezoresistance = 2.5; // R = V^2/P == R = (5V)^2/2W /// for piezo

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
  //digitalWrite(3,LOW); 
  
}

void loop()
{
  topr = analogRead(A2);
  topl = analogRead(A3);
  botl = analogRead(A4);
  botr = analogRead(A5);
  Vout = (analogRead(A8) * 5.0) / 1023;
  int threshold_value=10; 
  analogVoltage = analogRead(voltageinputPIN); //reads analog voltage of incoming sensor
  analogPiezo = analogRead(piezoinputPIN); 

  Cayenne.loop();

  if (digitalRead(3) == HIGH) {
    Serial.println("Automatic-mode");

    servoh = servo_x.read();
    servov = servo_z.read();

    int avgtop = (topr + topl) / 2;
    int avgbot = (botr + botl) / 2;
    int avgright = (topr + botr) / 2;
    int avgleft = (topl + botl) / 2;
    int diffhori = avgtop - avgbot;
    int diffverti = avgleft - avgright;
    //int offset =20;// set the correction offset value
    
    /*tracking according to horizontal axis*/
    if (abs(diffhori) <= threshold_value) {
      servo_x.write(servoh); //stop the servo up-down
    }
    else {
      if (diffhori > threshold_value) {
        //Serial.println(" x -  2 ");
        servo_x.write(servoh - 2); //Clockwise rotation CW
        if (servoh > servohLimitHigh) {
          servoh = servohLimitHigh;
        }
        delay(10);
      }
      else {
        servo_x.write(servoh + 2); //CCW
        if (servoh < servohLimitLow) {
          servoh = servohLimitLow;
        }
        delay(10);
      }
    }

    /*tracking according to vertical axis*/
    if (abs(diffverti)  <= threshold_value) {
      servo_z.write(servov); //stop the servo left-right
    }
    else {
      if (diffverti > threshold_value) {
        servo_z.write(servov - 2); //CW - 2
        if (servov > servovLimitHigh) {
          servov = servovLimitHigh;
        }
        delay(10);
      }
      else {
        servo_z.write(servov + 2); //CCW + 2
        if (servov < servovLimitLow) {
          servov = servovLimitLow;
        }
        delay(10);
      }
    }
  }
  else {
    Serial.println("Manual-mode");
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
  Serial.println("v"); //prints label
  //delay(sensorreadDelay); //delay in milliseconds between read values
}
CAYENNE_OUT(0) { //Current
  float Voltage = (analogVoltage * (5.0 / 1023.0)); //conversion equation
  float current = Voltage/resistance;
  Cayenne.virtualWrite(0, current);
  Serial.print(" Current: ");
  Serial.print(current);
  Serial.println("a");
}
CAYENNE_OUT(10) { //Power
  float Voltage = (analogVoltage * (5.0 / 1023.0)); //conversion equation
  float current = Voltage/resistance;
  float power = Voltage * current ;
  Cayenne.virtualWrite(10, power);
  Serial.print(" Power: ");
  Serial.print(power);
  Serial.println("W");
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
  Serial.print(" Temperature: ");
  Serial.print(t);
  Serial.println("c");
}
CAYENNE_OUT(12){ //HuMidity
  double h = dht.readHumidity();
  //int chk = dht.read(DHT11PIN);
  Cayenne.virtualWrite(12, h); 
  Serial.print(" Humidity: ");
  Serial.println(h);
}
//Piezo sensors
CAYENNE_OUT(14){
  float piezoVoltage = (analogPiezo * (5.0 / 1023.0)); //conversion equation
  Cayenne.virtualWrite(14, piezoVoltage, TYPE_VOLTAGE, UNIT_VOLTS);
  Serial.print(" Piezo Voltage: "); 
  Serial.print(piezoVoltage); //prints value to serial monitor
  Serial.println("v"); //prints label
  //delay(sensorreadDelay); //delay in milliseconds between read values
}
CAYENNE_OUT(15) { //Current
  float piezoVoltage = (analogPiezo * (5.0 / 1023.0)); //conversion equation
  float piezocurrent = piezoVoltage/piezoresistance;
  Cayenne.virtualWrite(15, piezocurrent);
  Serial.print("Piezo Current: ");
  Serial.print(piezocurrent);
  Serial.println("a");
}
CAYENNE_OUT(16) { //Power
  float piezoVoltage = (analogVoltage * (5.0 / 1023.0)); //conversion equation
  float piezocurrent = piezoVoltage/piezoresistance;
  float piezopower = piezoVoltage * piezocurrent ;
  Cayenne.virtualWrite(16, piezopower);
  Serial.print("Piezo Power: ");
  Serial.print(piezopower);
  Serial.println("W");
}
