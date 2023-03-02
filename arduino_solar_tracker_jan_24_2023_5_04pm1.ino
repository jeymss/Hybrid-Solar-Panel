#include <Servo.h> // include Servo library 

Servo horizontal; // horizontal servo
int servoh = 90;     // stand horizontal servo

Servo vertical;   // vertical servo 
int servov = 90;     // stand vertical servo

// LDR pin connections
//  name  = analogpin;
int ldrlt = A0; //LDR top left
int ldrrt = A1; //LDR top rigt
int ldrld = A2; //LDR down left
int ldrrd = A3; //ldr down rigt
int spd = A4; //speed
int tole = A5; //tolerance


void setup()
{
  Serial.begin(9600);
// servo connections
// name.attacht(pin);
  horizontal.attach(9); 
  vertical.attach(10);
}

void loop() 
{
  int lt = analogRead(ldrlt); // top left
  int rt = analogRead(ldrrt); // top right
  int ld = analogRead(ldrld); // down left
  int rd = analogRead(ldrrd); // down rigt

  int dtime = analogRead(spd)/20; // read potentiometers  
  int tol = analogRead(tole)/4;

  int avt = (lt + rt) / 2; // average value top
  int avd = (ld + rd) / 2; // average value down
  int avl = (lt + ld) / 2; // average value left
  int avr = (rt + rd) / 2; // average value right

  int dvert = avt - avd; // check the difference of up and down
  int dhoriz = avl - avr;// check the diffirence of left and right

  if (-1*tol > dvert || dvert > tol) // check if the difference is in the tolerance else change vertical angle
  {
  if (avt > avd)
  {
    servov = ++servov;
     if (servov > 180) 
     { 
      servov = 180;
     }
  }
  else if (avt < avd)
  {
    servov= --servov;
    if (servov < 0)
  {
    servov = 0;
  }
  }
  vertical.write(servov);
  }

  if (-1*tol > dhoriz || dhoriz > tol) // check if the diffirence is in the tolerance else change horizontal angle
  {
  if (avl > avr)
  {
    servoh = --servoh;
    if (servoh < 0)
    {
    servoh = 0;
    }
  }
  else if (avl < avr)
  {
    servoh = ++servoh;
     if (servoh > 180)
     {
     servoh = 180;
     }
  }
  else if (avl = avr)
  {
    // nothing
  }
  horizontal.write(servoh);
  }
   delay(dtime); 
}